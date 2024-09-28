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
		// 시스템의 Global Value;
		const LOTTO_SYSTEM_VALUE systemValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_lottoSystemId );

		if( !systemValue.bUse )
			return false;

		return true;
	}

	bool CLottoSystemState::OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID )
	{
		// 시스템의 Global Value;
		const LOTTO_SYSTEM_VALUE systemValue =
			GLLottoGlobalData::Instance()->GetLottoSystemValue( m_lottoSystemId );

		if( !systemValue.bUse )
			return true;

		switch( pMsg->Type() )
		{
		case NET_MSG_GCTRL_LOTTO_SYSTEM_REQ_LOTTO_TO_BUY_FA:	// 로또 구매 요청;
			SendReqLottoToBuyAC( pMsg, dwClientID, dwGaeaID );
			break;
		/*case NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_STATE_CA:		// 현재 상태 확인;
			SendCurrentStateMsgAC( pMsg );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_CURRENT_TURN_CA:		// 현재 회차 확인;
			SendCurrentTurnMsgAC( pMsg );
			break;*/
		case NET_MSG_GCTRL_LOTTO_SYSTEM_ACCUMULATE_MONEY_CA:	// 누적 당첨금;
			SendAccumulateMoneyMsgAC( pMsg, dwClientID, dwGaeaID );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_BUY_LIST_CA:			// 구매 목록;
			SendBuyListMsgAC( pMsg, dwClientID, dwGaeaID );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_NUM_CA:			// 당첨 번호;
			SendWinningNumMsgAC( pMsg, dwClientID, dwGaeaID );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_WINNING_MAN_LIST_CA:	// 당첨자;
			SendWinningManListMsgAC( pMsg, dwClientID, dwGaeaID );
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_CONFIRM_BUY_LIST_FA:	// 구매리스트 확인;
			SendConfirmBuyListMsgAC( pMsg, dwClientID, dwGaeaID );
			break;

		case NET_MSG_GCTRL_LOTTO_SYSTEM_CHANGE_STATE_CA:
			ChangeStateMsgProc();								// 상태 변경;
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DSP_CA:
			SendViewNumDSPAC( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_DSP_CA*>( pMsg ), dwClientID, dwGaeaID );							// 번호 누적 횟수;
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_CA:
			SendViewNumAC( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_CA*>( pMsg ), dwClientID, dwGaeaID );								// 번호 확률을 위한 추가 개수;
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_INIT_CA:
			SendNumInitAC( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_INIT_CA*>( pMsg ), dwClientID, dwGaeaID );								// 번호를 추가 횟수 초기화;
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_ADD_CA:
			SendNumAddAC( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_ADD_CA*>( pMsg ), dwClientID, dwGaeaID );									// 번호 추가 횟수 추가;
			break;
		case NET_MSG_GCTRL_LOTTO_SYSTEM_NUM_DEL_CA:
			SendNumDelAC( static_cast<GLMSG::NET_LOTTO_MESSAGE_NUM_DEL_CA*>( pMsg ), dwClientID, dwGaeaID );									// 번호 추가 횟수 삭제;
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