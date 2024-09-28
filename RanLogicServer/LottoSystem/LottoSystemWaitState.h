#pragma once

#include "./LottoSystemState.h"

namespace LottoSystem
{

	class CLottoSystemWaitState : public CLottoSystemState
	{
	public:
		CLottoSystemWaitState( LOTTO_SYSTEM_ID lottoSystemId, CLottoSystem* pLottoSystem );
		virtual ~CLottoSystemWaitState();

	public:
		virtual void Initialize();
		virtual void Destroy();

		virtual bool Update( float elapsedTime, const CTime& currentTime );
		virtual bool OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

		virtual void Update_ChangeState();

		virtual void SyncState( DWORD dwClientID, DWORD dwChaNum );

	private:
		virtual void SendReqLottoToBuyAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );
		virtual void SendWinningNumMsgAC( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID );

	private:
		// 패킷 처리;
		virtual void ChangeStateMsgProc();

	private:
		float m_fAllWaitTime;
	};

}