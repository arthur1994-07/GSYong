#pragma once

#include "./PingManagerFieldDefine.h"

namespace Ping
{	
	class ManagerField
	{
		friend class GLGaeaServer;
	private:
		void _frameMove(const float _fTime, const float _fTimeElapsed);
		const bool CALLBACK _messageProcedure(const DWORD _dwClientID, const DWORD _dwGaeaID, const NET_MSG_GENERIC* const _pMessage);

	private:
		const bool CALLBACK _messageProcedureRequest(const DWORD _dwClientID, const DWORD _dwGaeaID, const NET_MSG_GENERIC* const _pMessage);
		const bool CALLBACK _messageProcedureRequestInformationDetail(const DWORD _dwClientID, const DWORD _dwGaeaID, const NET_MSG_GENERIC* const _pMessage);

	private:
		__forceinline const Ping::ManagerField& operator =(const Ping::ManagerField& _rhs);
		ManagerField(GLGaeaServer* const _pGaeaServer);
		~ManagerField(void);

	private:
		GLGaeaServer* const pGaeaServer;
		Ping::EntityMap mapPingEntity;
	};
}

#include "./PingManagerField.hpp"
