#include "pch.h"

#include "../../RanLogic/LottoSystem/LottoDrawingMachine.h"

#include "../AgentServer/GLAgentServer.h"

#include "./LottoSystemDrawingState.h"

#include "./GLLottoSystemManAgent.h"

#include "./GLLottoGlobalData.h"

#include "./GLLottoDataTypeServer.h"

namespace LottoSystem
{

	CLottoSystemDrawingState::CLottoSystemDrawingState( LOTTO_SYSTEM_ID lottoSystemId
		, CLottoSystem* pLottoSystem )
		: CLottoSystemState( lottoSystemId, pLottoSystem )
		, m_fAllDrawingTime( 0.f )
		, m_fEachOtherDrawingTime( 0.f )
		, m_curDrawingCount( 0 ) { }
	CLottoSystemDrawingState::~CLottoSystemDrawingState() { }

	void CLottoSystemDrawingState::Initialize()
	{
		CLottoSystemState::Initialize();

		BroadcastDrawingNum();
	}

	void CLottoSystemDrawingState::Destroy()
	{
		CLottoSystemState::Destroy();
	}

	bool CLottoSystemDrawingState::Update( float fElapsedTime, const CTime& currentTime )
	{
		if( !CLottoSystemState::Update( fElapsedTime, currentTime ) )
			return false;

		// 시스템의 Global Value;
		const LOTTO_SYSTEM_VALUE systemValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_lottoSystemId );

		// 시스템의 대기 시간;
		float fWaitTime = static_cast<float>( systemValue.nDrawingTime );
		float fEachOtherDrawingTime = systemValue.fEachOtherDrawingTime;

		// 시간이 흐름;
		m_fAllDrawingTime += ( fElapsedTime );
		m_fEachOtherDrawingTime += ( fElapsedTime );

		// 추첨 시간이 오면 추첨;
		if( m_fEachOtherDrawingTime >= fEachOtherDrawingTime )
		{
			m_fEachOtherDrawingTime = 0.f;
			BroadcastDrawingNum();
		}

		// 대기 시간이 모두 지나가면;
		if( m_fAllDrawingTime >= fWaitTime )
		{
			return GLLottoSystemManAgent::Instance()->
				ChangeLottoSystemState( m_lottoSystemId, LOTTO_STATE_GATHERING );
		}

		return true;
	}

	bool CLottoSystemDrawingState::OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		if( !CLottoSystemState::OnMessage( pMsg, dwClientID, dwGaeaID ) )
			return false;

		return true;
	}

	void CLottoSystemDrawingState::Update_ChangeState()
	{
		m_fEachOtherDrawingTime = 0.f;
		m_fAllDrawingTime = 0.f;

		// 당첨자 확인 및 당첨금 지급;
		ExitDrawingState();

		if( m_pLottoSystem )
		{
			// 추첨 시간 구하기;
			m_pLottoSystem->MakeDrawingDate();

			// 추첨 시간 동기화;
			LOTTO_DRAWING_DATE startTime = m_pLottoSystem->GetStartTime();
			LOTTO_DRAWING_DATE endTime = m_pLottoSystem->GetEndTime();

			GLMSG::NET_LOTTO_MESSAGE_NEXT_DRAWING_DATE_AC nextDrawingAC(
				m_lottoSystemId, startTime, endTime );
			GLLottoSystemManAgent::Instance()->BroadcastFromAgent( &nextDrawingAC );
		}
	}

	void CLottoSystemDrawingState::SyncState( DWORD dwClientID, DWORD dwChaNum )
	{
		// 당첨 번호 동기화;
		GLMSG::NET_LOTTO_MESSAGE_WINNING_NUM_AC winningNumAC(
			m_lottoSystemId, m_pLottoSystem->GetCurTurnNum() );
		winningNumAC.m_emAC = EMLOTTO_REQ_WINNER_NUM_AC_OK;
		winningNumAC.m_winnerNum = m_pLottoSystem->GetWinnerNum();
		winningNumAC.m_winnerNumCount = m_curDrawingCount;

		GLLottoSystemManAgent::Instance()->SendToClientFromAgent( dwChaNum, dwClientID, &winningNumAC );
	}

	void CLottoSystemDrawingState::SendReqLottoToBuyAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		CLottoSystemState::SendReqLottoToBuyAC( pMsg, dwClientID, dwGaeaID );

		GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FA* pLottoMsg = 
			static_cast<GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FA*>( pMsg );

		DWORD dwDbNum = pLottoMsg->m_dwDbNum;

		// 응답 패킷;
		GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_AF lottoMsgAF(
			m_lottoSystemId, dwDbNum );

		// 추첨 상태에서는 구매가 불가능 하다;
		lottoMsgAF.m_emAC = LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_STATE_ERROR;

		// 응답;
		GLLottoSystemManAgent::Instance()->SendToFieldFromAgent( dwClientID, &lottoMsgAF );
	}

	void CLottoSystemDrawingState::SendWinningNumMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		GLMSG::NET_LOTTO_MESSAGE_WINNING_NUM_CA* pLottoMsg = 
			static_cast<GLMSG::NET_LOTTO_MESSAGE_WINNING_NUM_CA*>( pMsg );

		DWORD dwDbNum = pLottoMsg->m_dwDbNum;
		unsigned int turnNum = pLottoMsg->m_turnNum;

		DWORD dwServerGroup = 0;
		GLAgentServer* pServer = GLLottoSystemManAgent::Instance()->GetAgentServer();
		if ( pServer )
		{
			if ( pServer->IsAgentClientSlot( dwClientID ) )
				dwServerGroup = pServer->GetServerGroup_ClientSlotID( dwClientID );
			else
				dwServerGroup = pServer->GetServerGroup();
		}

		CLottoDrawingMachine* pDrawingMachine =
			m_pLottoSystem->GetDrawingMachine();
		if( !pDrawingMachine )
			return;

		// 응답 패킷;
		if( m_pLottoSystem->GetCurTurnNum() == turnNum )
		{
			GLMSG::NET_LOTTO_MESSAGE_WINNING_NUM_AC lottoMsgAC(
				m_lottoSystemId, turnNum );
			lottoMsgAC.m_emAC = EMLOTTO_REQ_WINNER_NUM_AC_OK;
			lottoMsgAC.m_winnerNum = m_pLottoSystem->GetWinnerNum();

			if( m_pLottoSystem->GetLottoSystemState() == LOTTO_STATE_DRAWING )
				lottoMsgAC.m_winnerNumCount = m_curDrawingCount;
			else
				lottoMsgAC.m_winnerNumCount = pDrawingMachine->GetWinNumCount();

			// 응답;
			GLLottoSystemManAgent::Instance()->SendToClientFromAgent( dwDbNum, dwClientID, &lottoMsgAC );
		}
		else
		{
			// 다른 회차의 경우 DbAction을 한다;
			GLLottoSystemManAgent::Instance()->WinNumSelectAction(
				dwDbNum, dwServerGroup, turnNum, m_lottoSystemId );
		}
	}

	void CLottoSystemDrawingState::ChangeStateMsgProc()
	{
		GLLottoSystemManAgent::Instance()->ChangeLottoSystemState( m_lottoSystemId, LOTTO_STATE_GATHERING );
	}

	void CLottoSystemDrawingState::BroadcastDrawingNum()
	{
		CLottoDrawingMachine* pDrawingMachine =
			m_pLottoSystem->GetDrawingMachine();
		if( !pDrawingMachine )
			return;

		LOTTO_SYSTEM_LOTTO_NUM lottoNum = m_pLottoSystem->GetWinnerNum();
		unsigned int drawingNum = 0;
		if( m_curDrawingCount >= 0 && m_curDrawingCount < MAX_DRAWING_NUM_LIST )
			drawingNum = lottoNum.lottoNum[ m_curDrawingCount++ ];

		// 추첨 번호의 총 개수가 넘어가지 않도록 체크;
		GLMSG::NET_LOTTO_MESSAGE_DRAWING_NUM_AB lottoMsgAB(
			m_lottoSystemId,
			m_curDrawingCount,
			drawingNum );
		GLLottoSystemManAgent::Instance()->BroadcastFromAgent( &lottoMsgAB );
	}

	void CLottoSystemDrawingState::ExitDrawingState()
	{
		{
			// GM 명령어 등으로 강제로 변경 될 경우가 있기 때문에;
			// 공이 하나씩 나오는 NET_LOTTO_MESSAGE_DRAWING_NUM_AB;
			// 패킷을 못받을 경우가 생긴다;
			// 그러므로, 추첨되어진 번호를 상태 변경 시에 한번 더 보낸다;
			// ( 브로드캐스트 );
			GLMSG::NET_LOTTO_MESSAGE_WINNING_NUM_AC lottoMsgAC(
				m_lottoSystemId, m_pLottoSystem->GetCurTurnNum() );
			lottoMsgAC.m_emAC = EMLOTTO_REQ_WINNER_NUM_AC_OK;
			lottoMsgAC.m_winnerNum = m_pLottoSystem->GetWinnerNum();
			GLLottoSystemManAgent::Instance()->BroadcastFromAgent( &lottoMsgAC );
		}

		LOTTO_SYSTEM_VALUE sValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_lottoSystemId );

		{
			// 현재 나온 로또 번호의 개수 초기화;
			m_curDrawingCount = 0;
		}
		
		{
			// 로그;
			LOTTO_SYSTEM_TURN_LOG logInfo;
			logInfo.moneyType = m_pLottoSystem->GetMoneyType();
			logInfo.systemId = m_lottoSystemId;
			logInfo.turnNum = m_pLottoSystem->GetCurTurnNum();

			CTime curTime = CTime::GetCurrentTime();
			logInfo.drawingEndTime =
				curTime.Format("%Y-%m-%d %H:%M:%S").GetString();

			int startTimeInterval = static_cast<int>(
				sValue.nDrawingReadyTime + sValue.nDrawingTime );
			CTime startTime = curTime +
				CTimeSpan( 0, 0, 0,
				-startTimeInterval );
			logInfo.drawingStartTime = 
				startTime.Format("%Y-%m-%d %H:%M:%S").GetString();
			
			for( unsigned int i=0; i<m_pLottoSystem->GetWinTypeCount(); ++i )
			{
				LOTTO_WIN_TYPE winType = m_pLottoSystem->GetWinType( i );
				switch( winType.emGiveMoneyType )
				{
				case ACCUMULATE_MONEY:
					logInfo.winInfo += sc::string::format(
						"No. %d : %d Pct, %d/%d ( %d )",
						i+1,
						winType.nGiveMoneyFct,
						winType.sRewardItemID.Mid(),
						winType.sRewardItemID.Sid(),
						winType.dwRewardItemCount );
					if( i != m_pLottoSystem->GetWinTypeCount()-1 )
						logInfo.winInfo += "|";
					break;
				case FIXED_MONEY:
					logInfo.winInfo += sc::string::format(
						"No. %d : %d, %d/%d ( %d )",
						i+1,
						winType.llGiveMoney,
						winType.sRewardItemID.Mid(),
						winType.sRewardItemID.Sid(),
						winType.dwRewardItemCount );
					if( i != m_pLottoSystem->GetWinTypeCount()-1 )
						logInfo.winInfo += "|";
					break;
				}
			}

			logInfo.ticketMoney = m_pLottoSystem->GetLottoTicketMoney();
			logInfo.ticketMaxCount = sValue.nBuyMultiNum;
			LONGLONG accumulateMoney = m_pLottoSystem->GetAccumulateMoney();
			logInfo.accumulateMoney = accumulateMoney;
			logInfo.chargePct = m_pLottoSystem->GetTicketFee();

			std::vector<LOTTO_SYSTEM_WINNER_RANKING_INFO> rankingInfos
				= m_pLottoSystem->GetLottoRankingInfoList();
			LONGLONG forwardMoney = 0;
			for( unsigned int i=0; i<rankingInfos.size(); ++i )
			{
				LOTTO_SYSTEM_WINNER_RANKING_INFO info = rankingInfos.at( i );
				logInfo.rankingAllMoney += sc::string::format(
					"No. %d : %d", info.ranking, info.winMoney );
				if( i != rankingInfos.size()-1 )
					logInfo.rankingAllMoney += "|";

				logInfo.rankingAllCount += sc::string::format(
					"No. %d : %d", info.ranking, info.count );
				if( i != rankingInfos.size()-1 )
					logInfo.rankingAllCount += "|";


				int rankingIdx = info.ranking-1;
				EMGIVE_MONEY_TYPE giveMoneyType = ACCUMULATE_MONEY;
				if( rankingIdx >= 0 && rankingIdx < static_cast<int>( m_pLottoSystem->GetWinTypeCount() ) )
				{
					LOTTO_WIN_TYPE winType =
						m_pLottoSystem->GetWinType( rankingIdx );

					giveMoneyType = winType.emGiveMoneyType;
				}

				LONGLONG rankingMoney = 0;
				switch( giveMoneyType )
				{
				case ACCUMULATE_MONEY:
					if( info.count > 0 )
						rankingMoney = info.winMoney/info.count; 
					else if( info.count == 0 )
						forwardMoney += info.winMoney;
					break;
				case FIXED_MONEY:
					rankingMoney = info.winMoney;
					break;
				}

				logInfo.rankingMoney += sc::string::format(
					"No. %d : %d", info.ranking, rankingMoney );
				if( i != rankingInfos.size()-1 )
					logInfo.rankingMoney += "|";
			}

			logInfo.forwardMoney = forwardMoney;

			GLLottoSystemManAgent::Instance()
				->SystemTurnInfoLogAction( logInfo );
		}

		{
			// 다음 회차로 넘어감;
			m_pLottoSystem->GoNextTurn();
		}
	}

}