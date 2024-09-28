#pragma once

#include "./ILottoSystemState.h"

namespace LottoSystem
{
	class CLottoSystem;

	// 각 상태의 공통 로직;
	class CLottoSystemState : public ILottoSystemState
	{
	public:
		CLottoSystemState( LOTTO_SYSTEM_ID lottoSystemId, CLottoSystem* pLottoSystem );
		virtual ~CLottoSystemState();

	public:
		virtual void Initialize();
		virtual void Destroy();

		virtual bool Update( float fElapsedTime, const CTime& currentTime );
		virtual bool OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

		// 상태가 변하기 전에 실행되어야 할 로직;
		virtual void Update_ChangeState() { }

		// 상태 별 Client와의 동기화;
		virtual void SyncState( DWORD dwClientID, DWORD dwChaNum ) { }

	protected:
		/// Agent -> Client;
		// 상태마다 동작이 다른 패킷;
		virtual void SendReqLottoToBuyAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		virtual void SendWinningNumMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

		// 모든 상태에서 같은 동작을 하는 패킷;
		//void SendCurrentStateMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );
		//void SendCurrentTurnMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg );
		void SendAccumulateMoneyMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void SendBuyListMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void SendWinningManListMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void SendConfirmBuyListMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

		// GM Commander;
		void SendViewNumDSPAC( GLMSG::NET_LOTTO_MESSAGE_NUM_DSP_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void SendViewNumAC( GLMSG::NET_LOTTO_MESSAGE_NUM_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void SendNumInitAC( GLMSG::NET_LOTTO_MESSAGE_NUM_INIT_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void SendNumAddAC( GLMSG::NET_LOTTO_MESSAGE_NUM_ADD_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void SendNumDelAC( GLMSG::NET_LOTTO_MESSAGE_NUM_DEL_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void SendLockBuyAC( GLMSG::NET_LOTTO_MESSAGE_LOCK_BUY_LOTTO_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void SendLockChangeTurnAC( GLMSG::NET_LOTTO_MESSAGE_LOCK_CHANGE_TURN_LOTTO_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID );

	protected:
		/// 패킷 처리;
		virtual void ChangeStateMsgProc();

		void SetInventWinNumProc( GLMSG::NET_LOTTO_MESSAGE_SET_INVENT_WIN_NUM_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void ResetInventWinNumProc( GLMSG::NET_LOTTO_MESSAGE_RESET_INVENT_WIN_NUM_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID );

	protected:
		LOTTO_SYSTEM_ID m_lottoSystemId;

		CLottoSystem* m_pLottoSystem;
	};

}