#pragma once

#include "./LottoSystemState.h"

namespace LottoSystem
{

	// 로또 시스템의 구입 가능 상태;
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
		// 패킷 처리;
		virtual void ChangeStateMsgProc();
	};

}