#include "pch.h"

#include "../../RanLogic/LottoSystem/LottoDrawingMachine.h"

#include "../Character/GLCharAG.h"
#include "../AgentServer/GLAgentServer.h"

#include "./LottoSystemDrawingState.h"

#include "./GLLottoSystemManAgent.h"

#include "./GLLottoGlobalData.h"

namespace LottoSystem
{

	/// Agent Server Message Procedure;
	void CLottoSystemState::SendReqLottoToBuyAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		// �� ���º��� �ٸ� ������ ��;
	}

	void CLottoSystemState::SendWinningNumMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		// �� ���º��� �ٸ� ������ ��;
	}

	void CLottoSystemState::SendAccumulateMoneyMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		GLMSG::NET_LOTTO_MESSAGE_ACCUMULATE_MONEY_CA* pLottoMsg = 
			static_cast<GLMSG::NET_LOTTO_MESSAGE_ACCUMULATE_MONEY_CA*>( pMsg );

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

		// ���� �ݾ� ��������;
		LONGLONG accumulateMoney = 0;
		if( m_pLottoSystem->GetCurTurnNum() == turnNum )
			accumulateMoney = m_pLottoSystem->GetAccumulateMoney();
		else
		{
			// �ٸ� ȸ���� ��� DbAction�� �Ѵ�;
			GLLottoSystemManAgent::Instance()->AccumulateMoneySelectAction(
				dwDbNum, dwServerGroup, turnNum, m_lottoSystemId );
		}

		// ���� ��Ŷ;
		GLMSG::NET_LOTTO_MESSAGE_ACCUMULATE_MONEY_AC lottoMsgAC(
			m_lottoSystemId, turnNum, accumulateMoney );
		lottoMsgAC.m_emAC = LottoSystem::EMLOTTO_REQ_ACCUMULATE_MONEY_AC_OK;

		// ����;
		GLLottoSystemManAgent::Instance()->SendToClientFromAgent( dwDbNum, dwClientID, &lottoMsgAC );
	}

	void CLottoSystemState::SendBuyListMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_CA* pLottoMsg = 
			static_cast<GLMSG::NET_LOTTO_MESSAGE_BUY_LIST_CA*>( pMsg );

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

		// ������ DbAction ����;
		// �ٸ� ȸ���� ��� DbAction�� �Ѵ�;
		GLLottoSystemManAgent::Instance()->BuyListSelectAction(
			dwDbNum, dwServerGroup, turnNum, m_lottoSystemId );
	}

	void CLottoSystemState::SendWinningManListMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		GLMSG::NET_LOTTO_MESSAGE_WINNING_MAN_LIST_CA* pLottoMsg = 
			static_cast<GLMSG::NET_LOTTO_MESSAGE_WINNING_MAN_LIST_CA*>( pMsg );

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

		// �ٸ� ȸ���� ��� DbAction�� �Ѵ�;
		GLLottoSystemManAgent::Instance()->WinManListSelectAction(
			dwDbNum, dwServerGroup, turnNum, m_lottoSystemId );
	}

	void CLottoSystemState::SendConfirmBuyListMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		GLMSG::NET_LOTTO_MESSAGE_CONFIRM_BUY_LIST_FA* pLottoMsg = 
			static_cast<GLMSG::NET_LOTTO_MESSAGE_CONFIRM_BUY_LIST_FA*>( pMsg );

		LOTTO_SYSTEM_ID id = pLottoMsg->m_lottoSystemId;
		DWORD dwDbNum = pLottoMsg->m_dwDbNum;
		LONGLONG money = 0;

		DWORD dwServerGroup = 0;
		GLAgentServer* pServer = GLLottoSystemManAgent::Instance()->GetAgentServer();
		if ( pServer )
		{
			if ( pServer->IsAgentClientSlot( dwClientID ) )
				dwServerGroup = pServer->GetServerGroup_ClientSlotID( dwClientID );
			else
				dwServerGroup = pServer->GetServerGroup();
		}

		LOTTO_SYSTEM_VALUE sValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( id );

		switch( m_pLottoSystem->GetMoneyType() )
		{
		case LOTTO_MONEY_TYPE_GAME_MONEY:
			money = pLottoMsg->m_invenMoney;
			break;
		case LOTTO_MONEY_TYPE_POINT:
			money = pLottoMsg->m_point;
			break;
		}

		GLLottoSystemManAgent::Instance()->BuyListNonConfirmUpdateAction(
			dwDbNum, dwServerGroup, money, m_lottoSystemId );
	}

	// GM Commander;
	void CLottoSystemState::SendViewNumDSPAC( GLMSG::NET_LOTTO_MESSAGE_NUM_DSP_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID )
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

		LOTTO_SYSTEM_VALUE sValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_lottoSystemId );

		CLottoDrawingMachine* pDrawingMachine = m_pLottoSystem->GetDrawingMachine();
		if( !pDrawingMachine )
			return;

		DWORD dwDbNum = pMsg->m_dwDbNum;
		unsigned int num = pMsg->m_num;

		if( num > 0 && num <= (unsigned int)m_pLottoSystem->GetTotalNum() )
		{
			// ���� ���� �����̸� ������� �ʴ´�;
			if( sValue.nDrawingType == 1 )
				return;

			LOTTO_SYSTEM_NUM numInfo = pDrawingMachine->GetNumInfo( num );

			GLMSG::NET_LOTTO_MESSAGE_NUM_DSP_AC numDSPAC(
				m_lottoSystemId, num, numInfo.drawingCount );
			GLLottoSystemManAgent::Instance()->SendToClientFromAgent(
				dwDbNum, dwClientID, &numDSPAC );
		}
		else
		{
			// ���� ���� �����̸� �ٸ� ó���� �Ѵ�;
			if( sValue.nDrawingType == 1 )
			{
				CLottoSystem* pLottoSystem = GLLottoSystemManAgent::Instance()
					->GetLottoSystem( m_lottoSystemId );
				if( pLottoSystem )
				{
					GLLottoSystemManAgent::Instance()->InventWinNumSelectAction(
						dwDbNum, dwServerGroup, pLottoSystem->GetCurTurnNum(), m_lottoSystemId );
				}

				return;
			}

			for( unsigned int i=0; i< (unsigned int)m_pLottoSystem->GetTotalNum(); ++i )
			{
				LOTTO_SYSTEM_NUM numInfo = pDrawingMachine->GetNumInfo( i+1 );

				GLMSG::NET_LOTTO_MESSAGE_NUM_DSP_AC numDSPAC(
					m_lottoSystemId, i+1, numInfo.drawingCount );
				GLLottoSystemManAgent::Instance()->SendToClientFromAgent(
					dwDbNum, dwClientID, &numDSPAC );
			}
		}
	}

	void CLottoSystemState::SendViewNumAC( GLMSG::NET_LOTTO_MESSAGE_NUM_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		// ���� ���� �����̸� ������� �ʴ´�;
		LOTTO_SYSTEM_VALUE sValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_lottoSystemId );
		if( sValue.nDrawingType == 1 )
			return;

		CLottoDrawingMachine* pDrawingMachine = m_pLottoSystem->GetDrawingMachine();
		if( !pDrawingMachine )
			return;

		DWORD dwDbNum = pMsg->m_dwDbNum;
		unsigned int num = pMsg->m_num;

		if( num > 0 && num <= (unsigned int)m_pLottoSystem->GetTotalNum() )
		{
			LOTTO_SYSTEM_NUM numInfo = pDrawingMachine->GetNumInfo( num );

			GLMSG::NET_LOTTO_MESSAGE_NUM_AC numAC(
				m_lottoSystemId, num, numInfo.addCount );
			GLLottoSystemManAgent::Instance()->SendToClientFromAgent( dwDbNum, dwClientID, &numAC );
		}
		else
		{
			for( unsigned int i=0; i< (unsigned int)m_pLottoSystem->GetTotalNum(); ++i )
			{
				LOTTO_SYSTEM_NUM numInfo = pDrawingMachine->GetNumInfo( i+1 );

				GLMSG::NET_LOTTO_MESSAGE_NUM_AC numAC(
					m_lottoSystemId, i+1, numInfo.addCount );
				GLLottoSystemManAgent::Instance()->SendToClientFromAgent( dwDbNum, dwClientID, &numAC );
			}
		}
	}

	void CLottoSystemState::SendNumInitAC( GLMSG::NET_LOTTO_MESSAGE_NUM_INIT_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		// ���� ���� �����̸� ������� �ʴ´�;
		LOTTO_SYSTEM_VALUE sValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_lottoSystemId );
		if( sValue.nDrawingType == 1 )
			return;

		CLottoDrawingMachine* pDrawingMachine = m_pLottoSystem->GetDrawingMachine();
		if( !pDrawingMachine )
			return;

		DWORD dwDbNum = pMsg->m_dwDbNum;

		// �ʱ�ȭ;
		pDrawingMachine->InitializeNumCount();

		// DB ����;
		for( unsigned int i=0; i< (unsigned int)m_pLottoSystem->GetTotalNum(); ++i )
		{
			LOTTO_SYSTEM_NUM lottoSystemNum =
				pDrawingMachine->GetNumInfo( i+1 );

			GLLottoSystemManAgent::Instance()->DrawingMachineNumSaveAction(
				i+1,
				lottoSystemNum.drawingCount,
				lottoSystemNum.addCount,
				m_lottoSystemId );
		}

		GLMSG::NET_LOTTO_MESSAGE_NUM_INIT_AC msgAC( m_lottoSystemId );
		GLLottoSystemManAgent::Instance()->SendToClientFromAgent( dwDbNum, dwClientID, &msgAC );
	}

	void CLottoSystemState::SendNumAddAC( GLMSG::NET_LOTTO_MESSAGE_NUM_ADD_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		// ���� ���� �����̸� ������� �ʴ´�;
		LOTTO_SYSTEM_VALUE sValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_lottoSystemId );
		if( sValue.nDrawingType == 1 )
			return;

		CLottoDrawingMachine* pDrawingMachine = m_pLottoSystem->GetDrawingMachine();
		if( !pDrawingMachine )
			return;

		DWORD dwDbNum = pMsg->m_dwDbNum;

		unsigned int num = pMsg->m_num;
		unsigned int count = pMsg->m_count;

		// �߰�;
		pDrawingMachine->ChangeNumCountAndDrawingNums( num, count );

		// DB ����;
		LOTTO_SYSTEM_NUM lottoSystemNum =
			pDrawingMachine->GetNumInfo( num );

		GLLottoSystemManAgent::Instance()->DrawingMachineNumSaveAction(
			num,
			lottoSystemNum.drawingCount,
			lottoSystemNum.addCount,
			m_lottoSystemId );

		GLMSG::NET_LOTTO_MESSAGE_NUM_ADD_AC msgAC( m_lottoSystemId );
		GLLottoSystemManAgent::Instance()->SendToClientFromAgent( dwDbNum, dwClientID, &msgAC );
	}

	void CLottoSystemState::SendNumDelAC( GLMSG::NET_LOTTO_MESSAGE_NUM_DEL_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		// ���� ���� �����̸� ������� �ʴ´�;
		LOTTO_SYSTEM_VALUE sValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_lottoSystemId );
		if( sValue.nDrawingType == 1 )
			return;

		CLottoDrawingMachine* pDrawingMachine = m_pLottoSystem->GetDrawingMachine();
		if( !pDrawingMachine )
			return;

		DWORD dwDbNum = pMsg->m_dwDbNum;

		unsigned int num = pMsg->m_num;
		int count = pMsg->m_count;

		// ����;
		pDrawingMachine->ChangeNumCountAndDrawingNums( num, -count );

		// DB ����;
		LOTTO_SYSTEM_NUM lottoSystemNum =
			pDrawingMachine->GetNumInfo( num );

		GLLottoSystemManAgent::Instance()->DrawingMachineNumSaveAction(
			num,
			lottoSystemNum.drawingCount,
			lottoSystemNum.addCount,
			m_lottoSystemId );

		GLMSG::NET_LOTTO_MESSAGE_NUM_DEL_AC msgAC( m_lottoSystemId );
		GLLottoSystemManAgent::Instance()->SendToClientFromAgent( dwDbNum, dwClientID, &msgAC );
	}

	void CLottoSystemState::SendLockBuyAC( GLMSG::NET_LOTTO_MESSAGE_LOCK_BUY_LOTTO_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		// ���� ���� ��� ���� ����;
		m_pLottoSystem->SetBuyLockState( pMsg->m_bBuyLock );

		GLMSG::NET_LOTTO_MESSAGE_LOCK_BUY_LOTTO_AC msgAC(
			m_lottoSystemId,
			pMsg->m_bBuyLock );
		GLLottoSystemManAgent::Instance()->BroadcastFromAgent( &msgAC );
	}

	void CLottoSystemState::SendLockChangeTurnAC( GLMSG::NET_LOTTO_MESSAGE_LOCK_CHANGE_TURN_LOTTO_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		// ���� Turn ��� ���� ����;
		m_pLottoSystem->SetChangeTurnLockState( pMsg->m_bChangeTurnLock );

		GLMSG::NET_LOTTO_MESSAGE_LOCK_CHANGE_TURN_LOTTO_AC msgAC(
			m_lottoSystemId,
			pMsg->m_bChangeTurnLock );
		GLLottoSystemManAgent::Instance()->BroadcastFromAgent( &msgAC );
	}

	void CLottoSystemState::ChangeStateMsgProc()
	{
		// �� ���º��� �ٸ� ������ ��;
	}

	void CLottoSystemState::SetInventWinNumProc( GLMSG::NET_LOTTO_MESSAGE_SET_INVENT_WIN_NUM_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		CLottoSystem* pLottoSystem =
			GLLottoSystemManAgent::Instance()->GetLottoSystem( m_lottoSystemId );
		if( pLottoSystem )
		{
			pLottoSystem->SetInventLottoState( true );

			LOTTO_SYSTEM_LOTTO_NUM winNum = pMsg->m_winnerNum;
			std::vector< unsigned int > winNumList;

			LOTTO_SYSTEM_VALUE sValue =
				GLLottoGlobalData::Instance()->GetLottoSystemValue( m_lottoSystemId );
			
			for( unsigned int i=0; i< (unsigned int)m_pLottoSystem->GetDrawingTotalNum(); ++i )
			{
				if( winNum.lottoNum[ i ] == 0 )
					return;

				winNumList.push_back( winNum.lottoNum[ i ] );
			}

			pLottoSystem->UpdateWinNum( winNumList );
		}
	}

	void CLottoSystemState::ResetInventWinNumProc( GLMSG::NET_LOTTO_MESSAGE_RESET_INVENT_WIN_NUM_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		CLottoSystem* pLottoSystem =
			GLLottoSystemManAgent::Instance()->GetLottoSystem( m_lottoSystemId );
		if( pLottoSystem )
			pLottoSystem->SetInventLottoState( false );
	}
	
}