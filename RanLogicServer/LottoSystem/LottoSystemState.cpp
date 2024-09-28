#include "pch.h"

#include "./LottoSystemState.h"

#include "./GLLottoSystemManAgent.h"

#include "./LottoSystem.h"

#include "./GLLottoGlobalData.h"

namespace LottoSystem
{

	CLottoSystemState::CLottoSystemState( LOTTO_SYSTEM_ID lottoSystemId
		, CLottoSystem* pLottoSystem )
		: m_lottoSystemId( lottoSystemId )
		, m_pLottoSystem( pLottoSystem ) { }
	CLottoSystemState::~CLottoSystemState() { }

	void CLottoSystemState::Initialize()
	{

	}

	void CLottoSystemState::Destroy()
	{

	}

	bool CLottoSystemState::Update( float fElapsedTime, const CTime& currentTime )
	{
		// �ý����� Global Value;
		const LOTTO_SYSTEM_VALUE systemValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_lottoSystemId );

		if( !systemValue.bUse )
			return false;

		return true;
	}

	bool CLottoSystemState::OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		// �ý����� Global Value;
		const LOTTO_SYSTEM_VALUE systemValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_lottoSystemId );

		if( !systemValue.bUse )
			return true;

		switch( pMsg->Type() )
		{
		case NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_FA:	// �ζ� ���� ��û;
			SendReqLottoToBuyAC( pMsg, dwClientID, dwGaeaID );
			break;
		/*case NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_STATE_CA:		// ���� ���� Ȯ��;
			SendCurrentStateMsgAC( pMsg );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_TURN_CA:		// ���� ȸ�� Ȯ��;
			SendCurrentTurnMsgAC( pMsg );
			break;*/
		case NET_MSG_GCTRL_LOTTO_SYSTEM_ACCUMULATE_MONEY_CA:	// ���� ��÷��;
			SendAccumulateMoneyMsgAC( pMsg, dwClientID, dwGaeaID );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_BUY_LIST_CA:			// ���� ���;
			SendBuyListMsgAC( pMsg, dwClientID, dwGaeaID );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_NUM_CA:			// ��÷ ��ȣ;
			SendWinningNumMsgAC( pMsg, dwClientID, dwGaeaID );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_MAN_LIST_CA:	// ��÷��;
			SendWinningManListMsgAC( pMsg, dwClientID, dwGaeaID );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_CONFIRM_BUY_LIST_FA:	// ���Ÿ���Ʈ Ȯ��;
			SendConfirmBuyListMsgAC( pMsg, dwClientID, dwGaeaID );
			break;

		case NET_MSG_GCTRL_LOTTO_SYSTEM_CHANGE_STATE_CA:
			ChangeStateMsgProc();								// ���� ����;
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DSP_CA:
			SendViewNumDSPAC( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_DSP_CA*>( pMsg ), dwClientID, dwGaeaID );							// ��ȣ ���� Ƚ��;
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_CA:
			SendViewNumAC( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_CA*>( pMsg ), dwClientID, dwGaeaID );								// ��ȣ Ȯ���� ���� �߰� ����;
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_INIT_CA:
			SendNumInitAC( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_INIT_CA*>( pMsg ), dwClientID, dwGaeaID );								// ��ȣ�� �߰� Ƚ�� �ʱ�ȭ;
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_ADD_CA:
			SendNumAddAC( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_ADD_CA*>( pMsg ), dwClientID, dwGaeaID );									// ��ȣ �߰� Ƚ�� �߰�;
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DEL_CA:
			SendNumDelAC( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_DEL_CA*>( pMsg ), dwClientID, dwGaeaID );									// ��ȣ �߰� Ƚ�� ����;
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_SET_INVENT_WIN_NUM_CA:
			SetInventWinNumProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_SET_INVENT_WIN_NUM_CA*>( pMsg ), dwClientID, dwGaeaID );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_RESET_INVENT_WIN_NUM_CA:
			ResetInventWinNumProc( static_cast<GLMSG::NET_LOTTO_MESSAGE_RESET_INVENT_WIN_NUM_CA*>( pMsg ), dwClientID, dwGaeaID );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_BUY_LOTTO_CA:
			SendLockBuyAC( static_cast<GLMSG::NET_LOTTO_MESSAGE_LOCK_BUY_LOTTO_CA*>( pMsg ), dwClientID, dwGaeaID );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_LOCK_CHANGE_TURN_CA:
			SendLockChangeTurnAC( static_cast<GLMSG::NET_LOTTO_MESSAGE_LOCK_CHANGE_TURN_LOTTO_CA*>( pMsg ), dwClientID, dwGaeaID );
			break;
		}

		return true;
	}

}