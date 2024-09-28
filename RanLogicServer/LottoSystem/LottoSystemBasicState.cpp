#include "pch.h"

#include "../../RanLogic/LottoSystem/LottoDrawingMachine.h"

#include "./LottoSystemBasicState.h"

#include "./GLLottoSystemManAgent.h"

#include "./GLLottoGlobalData.h"

namespace LottoSystem
{

	CLottoSystemBasicState::CLottoSystemBasicState( LOTTO_SYSTEM_ID lottoSystemId
		, CLottoSystem* pLottoSystem )
		: CLottoSystemState( lottoSystemId, pLottoSystem ) { }
	CLottoSystemBasicState::~CLottoSystemBasicState() { }

	void CLottoSystemBasicState::Initialize()
	{
		CLottoSystemState::Initialize();
	}

	void CLottoSystemBasicState::Destroy()
	{
		CLottoSystemState::Destroy();
	}

	bool CLottoSystemBasicState::Update( float fElapsedTime, const CTime& currentTime )
	{
		if( !CLottoSystemState::Update( fElapsedTime, currentTime ) )
			return true;

		// 시스템의 Global Value;
		const LOTTO_SYSTEM_VALUE systemValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_lottoSystemId );

		const LOTTO_GLOBAL_VALUE gValue =
			GLLottoGlobalData::Instance()->GetLottoGlobalValue();

		if( m_pLottoSystem )
		{
			if( m_pLottoSystem->GetLottoSystemState() ==
				LottoSystem::LOTTO_STATE_GATHERING )
			{
				// 현재의 second;
				unsigned int curSecond =
					currentTime.GetHour()*3600 + currentTime.GetMinute()*60 + currentTime.GetSecond();

				// 집계 시작 시간 ( 추첨 종료 시간과 같다 );
				LOTTO_DRAWING_DATE gatheringDrawingDate = m_pLottoSystem->GetPreEndTime();
				gatheringDrawingDate.GenerateTime();

				// 집계 종료 시간 ( 집계 시작 시간에 집계시간을 초단위로 더한다 ); 
				CTime gatheringEndTime =
					gatheringDrawingDate.sTime +
					CTimeSpan( 0, 0, 0, gValue.nAggregateTime );

				// 집계 종료 시간이면;
				if( currentTime.GetDayOfWeek() == static_cast<int>( gatheringEndTime.GetDayOfWeek() ) )
				{
					unsigned int GatheringEndTimeSecond =
						gatheringEndTime.GetHour()*3600 +
						gatheringEndTime.GetMinute()*60 +
						gatheringEndTime.GetSecond();

					if( curSecond >= GatheringEndTimeSecond )
						m_pLottoSystem->ChangeState( LOTTO_STATE_BASIC );
				}
			}
		}

		// 턴 변경 상태가 잠겨있지않다면;
		if( !m_pLottoSystem->GetChangeTurnLockState() )
		{
			// 현재의 second;
			unsigned int curSecond =
				currentTime.GetHour()*3600 + currentTime.GetMinute()*60 + currentTime.GetSecond();

			// 추첨 대기 시간일 경우;
			LOTTO_DRAWING_DATE drawingDate = m_pLottoSystem->GetStartTime();
			if( currentTime.GetDayOfWeek() == static_cast<int>( drawingDate.emDrawingDayOfTheWeek ) )
			{
				if( curSecond >= drawingDate.nDrawingTime &&
					curSecond < drawingDate.nDrawingTime + systemValue.nDrawingReadyTime )
				{
					return GLLottoSystemManAgent::Instance()
						->ChangeLottoSystemState( m_lottoSystemId, LOTTO_STATE_WAIT );
				}
			}
		}

		return true;
	}

	bool CLottoSystemBasicState::OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		if( !CLottoSystemState::OnMessage( pMsg, dwClientID, dwGaeaID ) )
			return false;

		return true;
	}

	void CLottoSystemBasicState::Update_ChangeState()
	{
		LOTTO_SYSTEM_VALUE sValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue(
			m_lottoSystemId );

		CLottoSystem* pLottoSystem = GLLottoSystemManAgent::Instance()
			->GetLottoSystem( m_lottoSystemId );
		if( !pLottoSystem )
			return;

		/// 당첨자 구하기;
		// 만약 1등이 무조건 나와야 하는 경우 당첨 번호를 변경한다;
		// 로또 번호를 조작하지 않은 경우에는;
		// 최소 중복 로또 번호로서 당첨을 결정한다;
		if( sValue.nDrawingType == 1 &&
			!pLottoSystem->GetInventLottoNumState() )
		{
			GLLottoSystemManAgent::Instance()->ChangeInventWinNumAction(
				pLottoSystem->GetCurTurnNum(), m_lottoSystemId );
		}
		// 랜덤 당첨번호를 이용하거나;
		// 로또번호를 조작했을 경우 바로 경우의 수를 구한다;
		else
			pLottoSystem->GenerateWinNumList();
	}

	void CLottoSystemBasicState::SyncState( DWORD dwClientID, DWORD dwChaNum )
	{
		// 당첨 번호 동기화;
		GLMSG::NET_LOTTO_MESSAGE_WINNING_NUM_AC winningNumAC(
			m_lottoSystemId, m_pLottoSystem->GetCurTurnNum() );
		winningNumAC.m_emAC = EMLOTTO_REQ_WINNER_NUM_AC_OK;
		winningNumAC.m_winnerNum = m_pLottoSystem->GetPreWinnerNum();

		GLLottoSystemManAgent::Instance()->SendToClientFromAgent( dwChaNum, dwClientID, &winningNumAC );
	}

	void CLottoSystemBasicState::SendReqLottoToBuyAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		CLottoSystemState::SendReqLottoToBuyAC( pMsg, dwClientID, dwGaeaID );

		GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FA* pLottoMsg = 
			static_cast<GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_FA*>( pMsg );

		LOTTO_SYSTEM_ID id = pLottoMsg->m_lottoSystemId;
		DWORD dwDbNum = pLottoMsg->m_dwDbNum;

		DWORD dwServerGroup = 0;
		GLAgentServer* pServer = GLLottoSystemManAgent::Instance()->GetAgentServer();
		if ( pServer )
		{
			if ( pServer->IsAgentClientSlot( dwClientID ) )
				dwServerGroup = pServer->GetServerGroup_ClientSlotID( dwClientID );
			else
				dwServerGroup = pServer->GetServerGroup();
		}

		pLottoMsg->m_lottoBuy.dwServerGroup = dwServerGroup;

		LOTTO_SYSTEM_VALUE systemValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( id );

		if( m_pLottoSystem->GetBuyLockState() )
		{
			// 실패 패킷;
			GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_AF lottoMsgAF( id, dwDbNum );
			lottoMsgAF.m_emAC = LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_STATE_ERROR;
			GLLottoSystemManAgent::Instance()->SendToClientFromAgent(
				dwDbNum, dwClientID, &lottoMsgAF );

			return;
		}

		// 레벨 확인;
		if( pLottoMsg->m_level < systemValue.nBuyLevel )
		{
			// 실패 패킷;
			GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_AF lottoMsgAF( id, dwDbNum );
			lottoMsgAF.m_emAC = LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_LEVEL_FAIL;
			GLLottoSystemManAgent::Instance()->SendToClientFromAgent(
				dwDbNum, dwClientID, &lottoMsgAF );

			return;
		}

		// 수수료 계산;
		LONGLONG fee = static_cast<LONGLONG>(
			ceil( ( m_pLottoSystem->GetLottoTicketMoney() * m_pLottoSystem->GetTicketFee() ) / 100.f ) );

		// 구매 리스트에 저장;
		m_pLottoSystem->AddBuyList(
			pLottoMsg->m_lottoBuy,
			m_pLottoSystem->GetLottoTicketMoney()-fee,
			fee );
	}

	void CLottoSystemBasicState::SendWinningNumMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
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

		// 응답 패킷;
		if( m_pLottoSystem->GetCurTurnNum() == turnNum )
		{
			GLMSG::NET_LOTTO_MESSAGE_WINNING_NUM_AC lottoMsgAC(
				m_lottoSystemId, turnNum );
			lottoMsgAC.m_emAC = EMLOTTO_REQ_WINNER_NUM_AC_OK;
			lottoMsgAC.m_winnerNum = m_pLottoSystem->GetPreWinnerNum();

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

	void CLottoSystemBasicState::ChangeStateMsgProc()
	{
		// 턴 변경 상태가 잠겨있다면;
		if( m_pLottoSystem->GetChangeTurnLockState() )
			return;

		if( m_pLottoSystem &&
			m_pLottoSystem->GetLottoSystemState() ==
			LottoSystem::LOTTO_STATE_GATHERING )
		{
			GLLottoSystemManAgent::Instance()->ChangeLottoSystemState(
				m_lottoSystemId, LOTTO_STATE_BASIC );
		}
		else if( m_pLottoSystem &&
			m_pLottoSystem->GetLottoSystemState() ==
			LottoSystem::LOTTO_STATE_BASIC )
		{
			GLLottoSystemManAgent::Instance()->ChangeLottoSystemState(
				m_lottoSystemId, LOTTO_STATE_WAIT );
		}
	}

}