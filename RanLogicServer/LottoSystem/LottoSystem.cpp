#include "pch.h"

#include "../../RanLogic/LottoSystem/LottoDrawingMachine.h"

#include "../../SigmaCore/Json/MinJson.h"

#include "../Database/DBAction/DbActionGameLottoSystem.h"

#include "./LottoSystem.h"

#include "./GLLottoSystemManAgent.h"
#include "./GLLottoGlobalData.h"

#include "./LottoSystemBasicState.h"
#include "./LottoSystemWaitState.h"
#include "./LottoSystemDrawingState.h"

#include "../AgentServer/GLAgentServer.h"

namespace LottoSystem
{

	CLottoSystem::CLottoSystem( LOTTO_SYSTEM_ID id )
		: m_id( id )
		, m_nTotalNum( 0 )
		, m_nDrawingTotalNum( 0 )
		, m_nDrawingMainNum( 0 )
		, m_nDrawingBonusNum( 0 )
		, m_llLottoTicketMoney( 1000000 )
		, m_ticketFee( 40 )
		, m_fGiveWinTicketTime( 0.f )
		, m_moneyType( LOTTO_MONEY_TYPE_GAME_MONEY )
		, m_pCurState( NULL )
		, m_curState( LOTTO_STATE_BASIC )
		, m_pBasicState( NULL )
		, m_pWaitState( NULL )
		, m_pDrawingState( NULL )
		, m_pDrawingMachine( NULL )
		, m_accumulateMoney( 0 )
		, m_turnNum( 0 )
		, m_bInventLottoNum( false )
		, m_bBuyLock( false )
		, m_bChangeTurnLock( false )
	{
		m_pBasicState = new CLottoSystemBasicState( m_id, this );
		m_pWaitState = new CLottoSystemWaitState( m_id, this );
		m_pDrawingState = new CLottoSystemDrawingState( m_id, this );

		m_pCurState = m_pBasicState;

		m_pDrawingMachine = new CLottoDrawingMachine();

		m_preWinnerNum.Clear();
		m_winnerNum.Clear();
		m_winnerManList.Clear();
		m_rankingInfos.reserve( 5 );
	}

	CLottoSystem::~CLottoSystem() { }

	void CLottoSystem::Initialize()
	{
		if( !m_pCurState )
			return;

		m_pCurState->Initialize();

		// 추첨 시간 구하기;
		MakeDrawingDate();

		// 집계 시간일 경우;
		const LOTTO_GLOBAL_VALUE gValue =
			GLLottoGlobalData::Instance()->GetLottoGlobalValue();

		CTime currentTime = CTime::GetCurrentTime();
		unsigned int curSecond =
			currentTime.GetHour()*3600 + currentTime.GetMinute()*60 + currentTime.GetSecond();

		LOTTO_DRAWING_DATE gatheringDrawingDate = m_preEndTime;
		if( currentTime.GetDayOfWeek() == static_cast<int>( gatheringDrawingDate.emDrawingDayOfTheWeek ) )
		{
			if( curSecond >= gatheringDrawingDate.nDrawingTime &&
				curSecond < gatheringDrawingDate.nDrawingTime + gValue.nAggregateTime )
				m_curState = LOTTO_STATE_GATHERING;
		}
	}

	void CLottoSystem::Destroy()
	{
		if( !m_pCurState )
			return;

		m_pCurState->Destroy();

		SAFE_DELETE( m_pBasicState );
		SAFE_DELETE( m_pWaitState );
		SAFE_DELETE( m_pDrawingState );
		m_pCurState = NULL;

		SAFE_DELETE( m_pDrawingMachine );
	}

	bool CLottoSystem::Update( float fElapsedTime, const CTime& currentTime )
	{
		if( !m_pCurState )
			return false;

		if( !m_pCurState->Update( fElapsedTime, currentTime ) )
			return false;

		static LOTTO_GLOBAL_VALUE gValue =
			GLLottoGlobalData::Instance()->GetLottoGlobalValue();

		// 누적 당첨금 갱신 Broadcast;
		m_fUpdateAccumulateMoneyTime += fElapsedTime;
		if( m_fUpdateAccumulateMoneyTime >=
			static_cast<float>( gValue.nAccumulateMoneyTime ) )
		{
			GLMSG::NET_LOTTO_MESSAGE_ACCUMULATE_MONEY_AC accumulateMoneyAC(
				m_id, m_turnNum, m_accumulateMoney );
			accumulateMoneyAC.m_emAC = EMLOTTO_REQ_ACCUMULATE_MONEY_AC_OK;
			GLLottoSystemManAgent::Instance()->BroadcastFromAgent(
				&accumulateMoneyAC );

			m_fUpdateAccumulateMoneyTime = 0.f;
		}

		return true;
	}

	bool CLottoSystem::OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		if( !m_pCurState )
			return false;

		if( !m_pCurState->OnMessage( pMsg, dwClientID, dwGaeaID ) )
			return false;

		return true;
	}

	void CLottoSystem::LoadFromDB()
	{
		// 로또 시스템 로드;
		GLLottoSystemManAgent::Instance()->SystemLoad( m_id );
	}

	bool CLottoSystem::ChangeState( EMLOTTO_STATE state )
	{
		if( !m_pBasicState ||
			!m_pWaitState ||
			!m_pDrawingState )
		{
			sc::writeLogError( "LottoSystem의 각 상태가 만들어지지 않았습니다!");
			return false;
		}

		switch( state )
		{
		case LOTTO_STATE_GATHERING:
			if( m_pCurState != m_pBasicState )
				m_pCurState->Update_ChangeState();
			m_pCurState = m_pBasicState;
			break;
		case LOTTO_STATE_BASIC:
			/*if( m_pCurState != m_pBasicState )
				m_pCurState->Update_ChangeState();*/
			m_pCurState = m_pBasicState;
			break;
		case LOTTO_STATE_WAIT:
			if( m_pCurState != m_pWaitState )
				m_pCurState->Update_ChangeState();
			m_pCurState = m_pWaitState;
			break;
		case LOTTO_STATE_DRAWING:
			if( m_pCurState != m_pDrawingState )
				m_pCurState->Update_ChangeState();
			m_pCurState = m_pDrawingState;
			break;
		}

		m_curState = state;

		m_pCurState->Initialize();

		// Broadcast;
		BroadcastChangeState();

		return true;
	}

	// 다음 턴으로 넘어감;
	void CLottoSystem::GoNextTurn()
	{
		// 조작 가능 여부 상태 초기화;
		m_bInventLottoNum = false;

		// 턴 증가;
		m_turnNum++;

		// 누적 금액 초기화;
		m_accumulateMoney = 0;

		// 당첨번호를 이전회차 당첨번호로 넘김;
		m_preWinnerNum = m_winnerNum;

		// 당첨번호 초기화;
		m_winnerNum.Clear();

		// 현재 당첨자 리스트 초기화;
		m_winnerManList.Clear();

		// Lotto System의 현재 Turn DB Action;
		GLLottoSystemManAgent::Instance()
			->SystemNextTurnAction( m_id );

		BroadcastGoNextTurn();
	}

	void CLottoSystem::SyncState( DWORD dwClientID, DWORD dwChaNum )
	{
		DWORD dwServerGroup = 0;
		GLAgentServer* pServer = GLLottoSystemManAgent::Instance()->GetAgentServer();
		if ( pServer )
		{
			if ( pServer->IsAgentClientSlot( dwClientID ) )
				dwServerGroup = pServer->GetServerGroup_ClientSlotID( dwClientID );
			else
				dwServerGroup = pServer->GetServerGroup();
		}

		LOTTO_SYSTEM_VALUE systemValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_id );

		// System Value 클라이언트와 동기화;
		GLMSG::NET_LOTTO_MESSAGE_SYNC_STATUS_AC msgSync( m_id );
		msgSync.m_emAC = LottoSystem::EMLOTTO_REQ_SYNC_STATUS_AC_OK;
		msgSync.m_systemValue.ServerToClient( systemValue );
		msgSync.m_systemValue.nLottoTotal = m_nTotalNum;
		msgSync.m_systemValue.nDrawingTotal = m_nDrawingTotalNum;
		msgSync.m_systemValue.nDrawingMain = m_nDrawingMainNum;
		msgSync.m_systemValue.nDrawingBonus= m_nDrawingBonusNum;
		msgSync.m_systemValue.moneyType = m_moneyType;
		msgSync.m_systemValue.lottoTicketMoney = m_llLottoTicketMoney;
		msgSync.m_systemValue.winTypeCount = m_vecWinType.size();

		GLLottoSystemManAgent::Instance()->SendToClientFromAgent( dwChaNum, dwClientID, &msgSync );

		// 상태 동기화;
		GLMSG::NET_LOTTO_MESSAGE_CURRENT_STATE_AC curStateAC(
			m_id, m_curState );
		GLLottoSystemManAgent::Instance()->SendToClientFromAgent( dwChaNum, dwClientID, &curStateAC );

		// 회차 동기화;
		GLMSG::NET_LOTTO_MESSAGE_CURRENT_TURN_AC curTurnAC(
			m_id, m_turnNum );
		GLLottoSystemManAgent::Instance()->SendToClientFromAgent( dwChaNum, dwClientID, &curTurnAC );

		// 구매리스트 동기화;
		// 이번 회차 구매 리스트 동기화;
		GLLottoSystemManAgent::Instance()->BuyListSelectAction(
			dwChaNum, dwServerGroup, m_turnNum, m_id );

		// 마지막으로 확인안한 구매 리스트 동기화;
		// 마지막으로 확인안한 턴을 찾아서 당첨 번호도 동기화 시킨다;
		GLLottoSystemManAgent::Instance()->BuyListNonConfirmSelectAction(
			dwChaNum, dwServerGroup, m_id, 0 );

		// 누적 당첨금 동기화;
		GLMSG::NET_LOTTO_MESSAGE_ACCUMULATE_MONEY_AC accumulateMoneyAC(
			m_id, m_turnNum, m_accumulateMoney );
		accumulateMoneyAC.m_emAC = EMLOTTO_REQ_ACCUMULATE_MONEY_AC_OK;
		GLLottoSystemManAgent::Instance()->SendToClientFromAgent( dwChaNum, dwClientID, &accumulateMoneyAC );

		// 각 상태 별 동기화;
		if( m_pCurState )
			m_pCurState->SyncState( dwClientID, dwChaNum );

		// Lock 상태 동기화;
		GLMSG::NET_LOTTO_MESSAGE_LOCK_BUY_LOTTO_AC msgLockBuyAC(
			m_id,
			m_bBuyLock );
		GLLottoSystemManAgent::Instance()->SendToClientFromAgent( dwChaNum, dwClientID, &msgLockBuyAC );

		GLMSG::NET_LOTTO_MESSAGE_LOCK_CHANGE_TURN_LOTTO_AC msgLockTurnAC(
			m_id,
			m_bChangeTurnLock );
		GLLottoSystemManAgent::Instance()->SendToClientFromAgent( dwChaNum, dwClientID, &msgLockTurnAC );

		// 시간 상태 동기화;
		GLMSG::NET_LOTTO_MESSAGE_NEXT_DRAWING_DATE_AC nextDrawingAC(
			m_id, m_startTime, m_endTime );
		GLLottoSystemManAgent::Instance()->SendToClientFromAgent( dwChaNum, dwClientID, &nextDrawingAC );
	}

	void CLottoSystem::BroadcastSyncSystemValue()
	{
		LOTTO_SYSTEM_VALUE systemValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_id );

		// System Value 클라이언트와 동기화;
		GLMSG::NET_LOTTO_MESSAGE_SYNC_STATUS_AC msgSync( m_id );
		msgSync.m_emAC = LottoSystem::EMLOTTO_REQ_SYNC_STATUS_AC_OK;
		msgSync.m_systemValue.ServerToClient( systemValue );
		msgSync.m_systemValue.nLottoTotal = m_nTotalNum;
		msgSync.m_systemValue.nDrawingTotal = m_nDrawingTotalNum;
		msgSync.m_systemValue.nDrawingMain = m_nDrawingMainNum;
		msgSync.m_systemValue.nDrawingBonus= m_nDrawingBonusNum;
		msgSync.m_systemValue.moneyType = m_moneyType;
		msgSync.m_systemValue.lottoTicketMoney = m_llLottoTicketMoney;
		msgSync.m_systemValue.winTypeCount = m_vecWinType.size();

		GLLottoSystemManAgent::Instance()->BroadcastFromAgent( &msgSync );
	}

	void CLottoSystem::BroadcastChangeState()
	{
		GLMSG::NET_LOTTO_MESSAGE_CHANGE_STATE_AB lottoMsgAB(
			m_id, m_curState );
		GLLottoSystemManAgent::Instance()->BroadcastFromAgent( &lottoMsgAB );
	}

	void CLottoSystem::BroadcastGoNextTurn()
	{
		GLMSG::NET_LOTTO_MESSAGE_GO_NEXT_TURN_AB lottoMsgAB(
			m_id, m_turnNum );
		GLLottoSystemManAgent::Instance()->BroadcastFromAgent( &lottoMsgAB );
	}

	void CLottoSystem::AccumulateMoney( LONGLONG deltaMoney )
	{
		m_accumulateMoney += deltaMoney;

		// DB Action;
		GLLottoSystemManAgent::Instance()->AccumulateMoneyUpdateAction(
			m_accumulateMoney, m_turnNum, m_id );
	}

	bool CLottoSystem::AddBuyList(
		LOTTO_SYSTEM_BUY& buyLotto,
		LONGLONG accumulateMoney,
		LONGLONG charge )
	{
		LOTTO_SYSTEM_VALUE sValue = 
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_id );
	
		// DrawingNum String 화;
		std::string drawingNum = "";
		LONGLONG drawingNumInt = 0;
		for( int i=0; i<m_nDrawingMainNum; ++i )
		{
			if( buyLotto.drawingNum[ i ] <= 0 )
				return false;

			drawingNum += sc::string::format( "%d",
				buyLotto.drawingNum[ i ] );
			drawingNum += "|";

			// Integer 화;
			drawingNumInt += static_cast<LONGLONG>( pow( 2.f, static_cast<int>( buyLotto.drawingNum[ i ]-1 ) ) );
		}

		// 구매 DBAction;
		GLLottoSystemManAgent::Instance()->BuyListInsertAction(
			accumulateMoney,
			charge,
			buyLotto.lottoBuyType,
			drawingNum,
			drawingNumInt,
			buyLotto.bConfirm ? 1 : 0,
			buyLotto.dwUserNum,
			buyLotto.dwDbNum,
			buyLotto.dwServerGroup,
			buyLotto.szChaName,
			m_turnNum,
			m_id );

		// 숫자 누적 ( 조작 가능 상태가 아닐 때만 누적시킨다 );
		if( sValue.nDrawingType == 0 )
		{
			unsigned int nDrawingMain = m_nDrawingMainNum;
			for( unsigned int i=0; i<nDrawingMain; ++i )
			{
				unsigned int num = buyLotto.drawingNum[ i ];
				m_pDrawingMachine->AddAccumulateNum( num, 1 );

				LOTTO_SYSTEM_NUM lottoSystemNum =
					m_pDrawingMachine->GetNumInfo( num );

				// 숫자 누적 DBAction;
				GLLottoSystemManAgent::Instance()->DrawingMachineNumSaveAction(
					num,
					lottoSystemNum.drawingCount,
					lottoSystemNum.addCount,
					m_id );
			}
		}

		return true;
	}

	// 당첨 번호 갱신;
	void CLottoSystem::UpdateWinNum( std::vector<unsigned int>& winNum )
	{
		LOTTO_SYSTEM_VALUE sValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_id );

		if( winNum.size() != m_nDrawingTotalNum )
		{
			sc::writeLogError( "WinNum이 만들어지지 않았습니다!");
			return;
		}

		// String 화;
		std::string strWinNum = "";
		LONGLONG winNumInt = 0;
		LONGLONG bonusNumInt = 0;
		for( unsigned int i=0; i<winNum.size(); ++i )
		{
			if( winNum.at( i ) == 0 )
			{
				sc::writeLogError( "잘못된 WinNum이 생성되었습니다!");
				return;
			}

			strWinNum += sc::string::format( "%d", winNum.at( i ) );
			strWinNum += "|";

			if( i < static_cast< unsigned int >( m_nDrawingMainNum ) )
			{
				winNumInt += static_cast<LONGLONG>(
					pow( 2.f, static_cast<int>( winNum.at( i ) - 1 ) ) );
			}
			else
			{
				bonusNumInt += static_cast<LONGLONG>(
					pow( 2.f, static_cast<int>( winNum.at( i ) - 1 ) ) );
			}
		}

		// 현재 당첨번호를 갱신한다 ( Setting );
		for( unsigned int i=0; i<winNum.size(); ++i )
			m_winnerNum.lottoNum[ i ] = winNum.at( i );
		m_winnerNum.SetLottoSetState( true );

		// DBAction;
		GLLottoSystemManAgent::Instance()->WinNumUpdateAction(
			strWinNum,
			winNumInt,
			bonusNumInt,
			m_turnNum,
			m_id );
	}

	void CLottoSystem::UpdatePreWinNum( std::vector<unsigned int>& winNum )
	{
		if( m_turnNum <= 0 )
			return;

		LOTTO_SYSTEM_VALUE sValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_id );

		if( winNum.size() != m_nDrawingTotalNum )
		{
			sc::writeLogError( "WinNum이 만들어지지 않았습니다!");
			return;
		}

		// String 화;
		std::string strWinNum = "";
		LONGLONG winNumInt = 0;
		LONGLONG bonusNumInt = 0;
		for( unsigned int i=0; i<winNum.size(); ++i )
		{
			if( winNum.at( i ) == 0 )
			{
				sc::writeLogError( "잘못된 WinNum이 생성되었습니다!");
				return;
			}

			strWinNum += sc::string::format( "%d", winNum.at( i ) );
			strWinNum += "|";

			if( i < static_cast< unsigned int >( m_nDrawingMainNum ) )
			{
				winNumInt += static_cast<LONGLONG>(
					pow( 2.f, static_cast<int>( winNum.at( i ) - 1 ) ) );
			}
			else
			{
				bonusNumInt += static_cast<LONGLONG>(
					pow( 2.f, static_cast<int>( winNum.at( i ) - 1 ) ) );
			}
		}

		// 현재 당첨번호를 갱신한다 ( Setting );
		for( unsigned int i=0; i<winNum.size(); ++i )
			m_preWinnerNum.lottoNum[ i ] = winNum.at( i );
		m_preWinnerNum.SetLottoSetState( true );

		// DBAction;
		GLLottoSystemManAgent::Instance()->WinNumUpdateAction(
			strWinNum,
			winNumInt,
			bonusNumInt,
			m_turnNum-1,
			m_id );
	}

	void CLottoSystem::AddRankingInfo( const LOTTO_SYSTEM_WINNER_RANKING_INFO& rankingInfo )
	{
		m_rankingInfos.push_back( rankingInfo );
	}

	void CLottoSystem::ClearRankingInfo()
	{
		m_rankingInfos.clear();
	}

	LONGLONG CLottoSystem::CalculateNextMoney()
	{
		LONGLONG nextMoney = 0;

		// 누적 금액 이월;
		LOTTO_SYSTEM_VALUE sValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_id );

		for( unsigned int i=0; i<m_rankingInfos.size(); ++i )
		{
			if( i >= 0 && i < m_vecWinType.size() )
			{
				if( m_vecWinType.at( i ).emGiveMoneyType
					== ACCUMULATE_MONEY )
				{
					if( m_rankingInfos.at( i ).count == 0 )
					{
						nextMoney += m_rankingInfos.at( i ).winMoney;
					}
				}
			}
		}

		return nextMoney;
	}

	// 당첨 번호 리스트;
	void CLottoSystem::ClearWinNumList()
	{
		m_winNumList.clear();
	}

	// 랭킹 정보 갱신;
	void CLottoSystem::UpdateRankingInfo()
	{
		LOTTO_SYSTEM_VALUE sValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_id );

		// 전체 당첨 금액 갱신;
		// WinType에 설정된 만큼의 당첨자 랭킹정보 설정;
		for( unsigned int i=0; i<m_vecWinType.size(); ++i )
		{
			LOTTO_SYSTEM_WINNER_RANKING_INFO rankingInfo;
			rankingInfo.ranking = i+1;

			for( unsigned int j=0; j<m_winnerManList.winnerManList.size(); ++j )
			{
				LOTTO_SYSTEM_WINNER_MAN winnerMan =
					m_winnerManList.winnerManList.at( j );

				// 당첨자 리스트는 랭킹당 하나가 존재한다;
				// 반복문으로 표현했지만 실제로는 각 랭킹 별로 한번만;
				// 호출된다;
				if( winnerMan.ranking == rankingInfo.ranking )
				{
					rankingInfo.count = winnerMan.winCount;

					// 당첨금 설정;
					LOTTO_WIN_TYPE winType = m_vecWinType.at( i );
					switch( winType.emGiveMoneyType )
					{
					case ACCUMULATE_MONEY:
						rankingInfo.winMoney = static_cast<LONGLONG>(
							( m_accumulateMoney * winType.nGiveMoneyFct ) / 100 );
						break;
					case FIXED_MONEY:
						rankingInfo.winMoney = winType.llGiveMoney;
						break;
					}

					rankingInfo.sRewardItemID = winType.sRewardItemID;
					rankingInfo.dwRewardItemCount = winType.dwRewardItemCount;

					// 랭킹 정보를 저장한다;
					AddRankingInfo( rankingInfo );

					break;
				}
			}
		}
	}

	void CLottoSystem::InitializeDrawingMachine()
	{
		// 추첨기 초기화;
		if( m_pDrawingMachine )
		{
			m_pDrawingMachine->Initialize(
				m_nTotalNum,
				m_nDrawingTotalNum,
				m_nDrawingMainNum,
				m_nDrawingBonusNum );
		}
	}

	void CLottoSystem::UpdateDrawingMachine()
	{
		// 추첨기 로드;
		if( m_pDrawingMachine )
		{
			/*m_pDrawingMachine->Initialize(
				m_nTotalNum,
				m_nDrawingTotalNum,
				m_nDrawingMainNum,
				m_nDrawingBonusNum );*/

			m_pDrawingMachine->ChangeDrawingNums();
		}
	}

	void CLottoSystem::AddTempWinNum( const LOTTO_WIN_NUM_INT& tempWinNum )
	{
		m_winNumList.push_back( tempWinNum );
	}

	void CLottoSystem::GenerateWinNumList()
	{
		LOTTO_SYSTEM_VALUE sValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue(
			m_id );

		ClearWinNumList();
		for( unsigned int i=0; i<m_vecWinType.size(); ++i )
		{
			// 당첨 번호 정보;
			LOTTO_WIN_NUM_INT winNumInt;

			// 당첨 정보;
			LOTTO_WIN_TYPE winType = m_vecWinType.at( i );

			// Main Num 상태;
			std::vector<int> originMainWinState;
			std::vector<int> tempMainWinState;
			for( int j=0; j<m_nDrawingMainNum; ++j )
			{
				if( j < static_cast< int >( winType.nMainNum ) )
					tempMainWinState.push_back( 1 );
				else
					tempMainWinState.push_back( 0 );
			}

			// Bonus Num 상태;
			std::vector<int> originBonusWinState;
			std::vector<int> tempBonusWinState;
			for( int j=0; j<m_nDrawingBonusNum; ++j )
			{
				if( j < static_cast< int >( winType.nBonusNum ) )
					tempBonusWinState.push_back( 1 );
				else
					tempBonusWinState.push_back( 0 );
			}

			// Origin 상태 저장;
			originMainWinState = tempMainWinState;
			originBonusWinState = tempBonusWinState;

			// Main 경우의 수 찾기;
			do
			{
				winNumInt.winNumInt.push_back( MakeNumInt( tempMainWinState ) );
				next_permutation( tempMainWinState.begin(), tempMainWinState.end() );
			} while( originMainWinState != tempMainWinState );

			// Bonus 경우의 수 찾기;
			do
			{
				winNumInt.bonusNumInt.push_back( MakeNumInt( tempBonusWinState, false ) );
				next_permutation( tempBonusWinState.begin(), tempBonusWinState.end() );
			} while( originBonusWinState != tempBonusWinState );

			// 랭킹 저장;
			winNumInt.ranking = i+1;

			// 전체 당첨 번호 리스트에 추가;
			AddTempWinNum( winNumInt );
		}

		// 전체 당첨리스트를 이용하여 당첨을 확인한다;
		GLLottoSystemManAgent::Instance()->ConfirmWinManAction( m_id );
	}

	LONGLONG CLottoSystem::MakeNumInt( std::vector<int>& numState, bool bMain )
	{
		LONGLONG rValue = 0;

		LOTTO_SYSTEM_VALUE sValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_id );

		for( unsigned int i=0; i<numState.size(); ++i )
		{
			if( numState.at( i ) == 1 )
			{
				unsigned int idx = i;
				if( !bMain )
					idx += ( sValue.nDrawingMain );

				if( idx >= 0 && idx < MAX_DRAWING_NUM_LIST )
				{
					rValue += static_cast<LONGLONG>(
						pow( 2.f, static_cast<int>( m_winnerNum.lottoNum[ idx ]-1 ) ) );
				}
			}
		}

		return rValue;
	}

	// 함수가 호출된 시점의 시간을 기준으로 추첨시간을 갱신시킨다;
	void CLottoSystem::MakeDrawingDate()
	{
		LOTTO_SYSTEM_VALUE sValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_id );
		const std::vector<LOTTO_DRAWING_DATE>& drawingDates
			= sValue.vecDrawingDate;

		// 현재 시각;
		CTime today = CTime::GetCurrentTime();
		unsigned int currentTime =
			today.GetHour()*3600 +
			today.GetMinute()*60 +
			today.GetSecond();

		// 추첨 일자 목록;
		LOTTO_DRAWING_DATE nextDrawingDate;
		int nextDayInterval = MAXLONG;
		int nextTimeInterval = MAXLONG;
		int drawingDateIdx = -1;
		for( unsigned int i=0; i<drawingDates.size(); ++i )
		{
			LOTTO_DRAWING_DATE tempDrawingDate =
				drawingDates.at( i );
			tempDrawingDate.GenerateTime();

			// 시간 간격;
			int timeInterval =
				tempDrawingDate.nDrawingTime - currentTime;

			// 요일 간격;
			int dayInterval =
				tempDrawingDate.emDrawingDayOfTheWeek -
				today.GetDayOfWeek();

			// 오늘 이전의 요일이면;
			if( dayInterval < 0 )
				dayInterval += DAY_OF_THE_WEEK_SIZE;

			// 오늘이 추첨일이면;
			if( dayInterval == 0 )
			{
				// 시간이 지나지 않았으면;
				if( timeInterval >= 0 )
				{
					// 요일이 바뀌었으면;
					if( 0 < nextDayInterval )
					{
						nextDayInterval = 0;

						// 요일이 바뀌었으면 시간간격을 무조건 갱신;
						nextTimeInterval = timeInterval;

						// 다음 추첨 날짜 갱신;
						nextDrawingDate = tempDrawingDate;

						drawingDateIdx = i;
					}
					// 요일이 바뀌지 않았으면;
					else
					{
						// 요일이 바뀌지 않았을 경우 시간간격을 체크;
						if( timeInterval < nextTimeInterval ||
							nextTimeInterval < 0 )
						{
							nextTimeInterval = timeInterval;
							nextDrawingDate = tempDrawingDate;

							drawingDateIdx = i;
						}
					}
				}
				// 시간이 지났으면;
				else
				{
					if( 7 < nextDayInterval )
					{
						nextDayInterval = 7;
						nextTimeInterval = timeInterval;
						nextDrawingDate = tempDrawingDate;

						drawingDateIdx = i;
					}
					else
					{
						if( timeInterval < nextTimeInterval )
						{
							nextTimeInterval = timeInterval;
							nextDrawingDate = tempDrawingDate;

							drawingDateIdx = i;
						}
					}
				}
			}
			// 오늘 이후의 요일이면;
			else if( dayInterval > 0 )
			{
				// 요일이 바뀌었으면;
				if( dayInterval < nextDayInterval )
				{
					nextDayInterval = dayInterval;

					// 요일이 바뀌었으면 시간간격을 무조건 갱신;
					nextTimeInterval = timeInterval;

					// 다음 추첨 날짜 갱신;
					nextDrawingDate = tempDrawingDate;

					drawingDateIdx = i;
				}
				// 요일이 바뀌지 않았으면;
				else
				{
					// 요일이 바뀌지 않았을 경우 시간간격을 체크;
					if( timeInterval < nextTimeInterval ||
						( nextTimeInterval < 0 && timeInterval >= 0 ) )
					{
						nextTimeInterval = timeInterval;
						nextDrawingDate = tempDrawingDate;

						drawingDateIdx = i;
					}
				}
			}
		}

		int preDrawingDateIdx = -1;
		if( drawingDateIdx - 1 < 0 )
			preDrawingDateIdx = drawingDates.size() - 1;
		else
			preDrawingDateIdx = drawingDateIdx - 1;

		if( preDrawingDateIdx >= 0 && preDrawingDateIdx < (int)drawingDates.size() )
		{
			LOTTO_DRAWING_DATE tempDrawingDate =
				drawingDates.at( preDrawingDateIdx );

			m_preStartTime = tempDrawingDate;
			tempDrawingDate.GenerateTime();
			m_preStartTime.nDrawingTime += sValue.nDrawingReadyTime;
			m_preEndTime = m_preStartTime;
			m_preEndTime.nDrawingTime += sValue.nDrawingTime;

			m_preStartTime.GenerateTime();
			m_preEndTime.GenerateTime();
		}			

		// 추첨 시작 시간, 추첨 종료 시간 갱신;
		m_startTime = nextDrawingDate;
		nextDrawingDate.GenerateTime();
		m_startTime.nDrawingTime += sValue.nDrawingReadyTime;
		m_endTime = m_startTime;
		m_endTime.nDrawingTime += sValue.nDrawingTime;

		m_startTime.GenerateTime();
		m_endTime.GenerateTime();
	}

	void CLottoSystem::SetWinType( const std::string& strWinTypes )
	{
		m_vecWinType.clear();

		boost::char_separator<char> sep("|");
		tokenizer tok( strWinTypes, sep );

		for( tokenizer::iterator it=tok.begin(); it!=tok.end(); ++it )
		{
			std::string strWinType = (*it).c_str();

			json_spirit::mValue outputValue;

			if( !json_spirit::read( strWinType, outputValue ) )
				return;

			const json_spirit::mObject outputObj = outputValue.get_obj();
			
			int mainNum = 0;
			int bonusNum = 0;
			int giveMoneyType = 0;
			int giveMoney = 0;
			int nGiveItemMainID = 0;
			int nGiveItemSubID = 0;
			int nGiveItemCount = 0;

			if( !sc::Json::find_value( outputObj, "mainNum", mainNum ) )
				continue;

			if( !sc::Json::find_value( outputObj, "bonusNum", bonusNum ) )
				continue;

			if( !sc::Json::find_value( outputObj, "giveMoneyType", giveMoneyType ) )
				continue;

			if( !sc::Json::find_value( outputObj, "giveMoney", giveMoney ) )
				continue;

			if ( !sc::Json::find_value( outputObj, "giveItemMainID", nGiveItemMainID ) )
				nGiveItemMainID = 0;

			if ( !sc::Json::find_value( outputObj, "giveItemSubID", nGiveItemSubID ) )
				nGiveItemSubID = 0;

			if ( !sc::Json::find_value( outputObj, "giveItemCount", nGiveItemCount ) )
				nGiveItemCount = 0;

			LOTTO_WIN_TYPE winType;
			winType.nMainNum = static_cast<unsigned int>( mainNum );
			winType.nBonusNum = static_cast<unsigned int>( bonusNum );
			winType.emGiveMoneyType = static_cast<EMGIVE_MONEY_TYPE>( giveMoneyType );
			switch( winType.emGiveMoneyType )
			{
			case LottoSystem::ACCUMULATE_MONEY:
				winType.nGiveMoneyFct = giveMoney;
				break;
			case LottoSystem::FIXED_MONEY:
				winType.llGiveMoney = static_cast<LONGLONG>( giveMoney );
				break;
			}
			winType.sRewardItemID = SNATIVEID( nGiveItemMainID, nGiveItemSubID );
			winType.dwRewardItemCount = nGiveItemCount;

			m_vecWinType.push_back( winType );
		}
	}
}