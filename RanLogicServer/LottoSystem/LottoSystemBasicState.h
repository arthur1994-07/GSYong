#pragma once

#include "./LottoSystemState.h"

namespace LottoSystem
{

	// �ζ� �ý����� ���� ���� ����;
	class CLottoSystemBasicState : public CLottoSystemState
	{
	public:
		CLottoSystemBasicState( LOTTO_SYSTEM_ID lottoSystemId, CLottoSystem* pLottoSystem );
		virtual ~CLottoSystemBasicState();

	public:
		virtual void Initialize();
		virtual void Destroy();

		virtual bool Update( float fElapsedTime, const CTime& currentTime );
		virtual bool OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

		virtual void Update_ChangeState();

		virtual void SyncState( DWORD dwClientID, DWORD dwChaNum );

	private:
		virtual void SendReqLottoToBuyAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		virtual void SendWinningNumMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

	private:
		// ��Ŷ ó��;
		virtual void ChangeStateMsgProc();
	};

}