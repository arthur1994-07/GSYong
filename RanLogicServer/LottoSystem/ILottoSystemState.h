#pragma once

#include "../../RanLogic/Msg/GLContrlLottoSystemMsg.h"
#include "./GLLottoDataTypeServer.h"

namespace LottoSystem
{

	// 로또 시스템의 각 상태 별 Interface;
	class ILottoSystemState
	{
	public:
		ILottoSystemState() { }
		~ILottoSystemState() { }

		virtual void Initialize() = 0;
		virtual void Destroy() = 0;

		virtual bool Update( float elapsedTime, const CTime& currentTime ) = 0;
		virtual bool OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID ) = 0;

		virtual void Update_ChangeState() = 0;

		virtual void SyncState( DWORD dwClientID, DWORD dwChaNum ) = 0;
	};

}