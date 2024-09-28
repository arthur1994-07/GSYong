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

		// �ý����� Global Value;
		const LOTTO_SYSTEM_VALUE systemValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_lottoSystemId );

		const LOTTO_GLOBAL_VALUE gValue =
			GLLottoGlobalData::Instance()->GetLottoGlobalValue();

		if( m_pLottoSystem )
		{
			if( m_pLottoSystem->GetLottoSystemState() ==
				LottoSystem::LOTTO_STATE_GATHERING )
			{
				// ������ second;
				unsigned int curSecond =
					currentTime.GetHour()*3600 + currentTime.GetMinute()*60 + currentTime.GetSecond();

				// ���� ���� �ð� ( ��÷ ���� �ð��� ���� );
				LOTTO_DRAWING_DATE gatheringDrawingDate = m_pLottoSystem->GetPreEndTime();
				gatheringDrawingDate.GenerateTime();

				// ���� ���� �ð� ( ���� ���� �ð��� ����ð��� �ʴ����� ���Ѵ� ); 
				CTime gatheringEndTime =
					gatheringDrawingDate.sTime +
					CTimeSpan( 0, 0, 0, gValue.nAggregateTime );

				// ���� ���� �ð��̸�;
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

		// �� ���� ���°� ��������ʴٸ�;
		if( !m_pLottoSystem->GetChangeTurnLockState() )
		{
			// ������ second;
			unsigned int curSecond =
				currentTime.GetHour()*3600 + currentTime.GetMinute()*60 + currentTime.GetSecond();

			// ��÷ ��� �ð��� ���;
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

		/// ��÷�� ���ϱ�;
		// ���� 1���� ������ ���;� �ϴ� ��� ��÷ ��ȣ�� �����Ѵ�;
		// �ζ� ��ȣ�� �������� ���� ��쿡��;
		// �ּ� �ߺ� �ζ� ��ȣ�μ� ��÷�� �����Ѵ�;
		if( sValue.nDrawingType == 1 &&
			!pLottoSystem->GetInventLottoNumState() )
		{
			GLLottoSystemManAgent::Instance()->ChangeInventWinNumAction(
				pLottoSystem->GetCurTurnNum(), m_lottoSystemId );
		}
		// ���� ��÷��ȣ�� �̿��ϰų�;
		// �ζǹ�ȣ�� �������� ��� �ٷ� ����� ���� ���Ѵ�;
		else
			pLottoSystem->GenerateWinNumList();
	}

	void CLottoSystemBasicState::SyncState( DWORD dwClientID, DWORD dwChaNum )
	{
		// ��÷ ��ȣ ����ȭ;
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
			// ���� ��Ŷ;
			GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_AF lottoMsgAF( id, dwDbNum );
			lottoMsgAF.m_emAC = LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_STATE_ERROR;
			GLLottoSystemManAgent::Instance()->SendToClientFromAgent(
				dwDbNum, dwClientID, &lottoMsgAF );

			return;
		}

		// ���� Ȯ��;
		if( pLottoMsg->m_level < systemValue.nBuyLevel )
		{
			// ���� ��Ŷ;
			GLMSG::NET_LOTTO_MESSAGE_REQ_LOTTO_TO_BUY_AF lottoMsgAF( id, dwDbNum );
			lottoMsgAF.m_emAC = LottoSystem::EMLOTTO_REQ_LOTTO_TO_BUY_AC_LEVEL_FAIL;
			GLLottoSystemManAgent::Instance()->SendToClientFromAgent(
				dwDbNum, dwClientID, &lottoMsgAF );

			return;
		}

		// ������ ���;
		LONGLONG fee = static_cast<LONGLONG>(
			ceil( ( m_pLottoSystem->GetLottoTicketMoney() * m_pLottoSystem->GetTicketFee() ) / 100.f ) );

		// ���� ����Ʈ�� ����;
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

		// ���� ��Ŷ;
		if( m_pLottoSystem->GetCurTurnNum() == turnNum )
		{
			GLMSG::NET_LOTTO_MESSAGE_WINNING_NUM_AC lottoMsgAC(
				m_lottoSystemId, turnNum );
			lottoMsgAC.m_emAC = EMLOTTO_REQ_WINNER_NUM_AC_OK;
			lottoMsgAC.m_winnerNum = m_pLottoSystem->GetPreWinnerNum();

			// ����;
			GLLottoSystemManAgent::Instance()->SendToClientFromAgent( dwDbNum, dwClientID, &lottoMsgAC );
		}
		else
		{
			// �ٸ� ȸ���� ��� DbAction�� �Ѵ�;
			GLLottoSystemManAgent::Instance()->WinNumSelectAction(
				dwDbNum, dwServerGroup, turnNum, m_lottoSystemId );
		}
	}

	void CLottoSystemBasicState::ChangeStateMsgProc()
	{
		// �� ���� ���°� ����ִٸ�;
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