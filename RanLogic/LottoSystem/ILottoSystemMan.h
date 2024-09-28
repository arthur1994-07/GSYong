#pragma once

#include "../Msg/GLContrlLottoSystemMsg.h"

namespace LottoSystem
{

	// Lotto System의 모든 Manager의 Interface;
	class ILottoSystemMan
	{
	public:
		ILottoSystemMan() { }
		virtual ~ILottoSystemMan() { }

		virtual void Initialize() = 0;
		virtual void Destroy() = 0;

		virtual bool Update( float elapsedTime, const CTime& currentTime ) = 0;
		virtual bool OnMessage( GLMSG::NET_LOTTO_MESSAGE_HEADER* pMsg, DWORD dwClientID, DWORD dwGaeaID ) = 0;
	};

}