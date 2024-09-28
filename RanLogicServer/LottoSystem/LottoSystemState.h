#pragma once

#include "./ILottoSystemState.h"

namespace LottoSystem
{
	class CLottoSystem;

	// �� ������ ���� ����;
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

		// ���°� ���ϱ� ���� ����Ǿ�� �� ����;
		virtual void Update_ChangeState() { }

		// ���� �� Client���� ����ȭ;
		virtual void SyncState( DWORD dwClientID, DWORD dwChaNum ) { }

	protected:
		/// Agent -> Client;
		// ���¸��� ������ �ٸ� ��Ŷ;
		virtual void SendReqLottoToBuyAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		virtual void SendWinningNumMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

		// ��� ���¿��� ���� ������ �ϴ� ��Ŷ;
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
		/// ��Ŷ ó��;
		virtual void ChangeStateMsgProc();

		void SetInventWinNumProc( GLMSG::NET_LOTTO_MESSAGE_SET_INVENT_WIN_NUM_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		void ResetInventWinNumProc( GLMSG::NET_LOTTO_MESSAGE_RESET_INVENT_WIN_NUM_CA* pMsg, DWORD dwClientID, DWORD dwGaeaID );

	protected:
		LOTTO_SYSTEM_ID m_lottoSystemId;

		CLottoSystem* m_pLottoSystem;
	};

}