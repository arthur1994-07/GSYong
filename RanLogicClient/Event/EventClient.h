#pragma once

#include "./EventClientDefine.h"

namespace GLEvent
{
	class ManagerClient
	{
	public:
		void frameMove(const float _fTime, const float _fTimeElapsed);
		const bool messageProcedure(NET_MSG_GENERIC* const _pMessage);

		void reset(void);

	private:
		const bool _messageProcedureEndFB(NET_MSG_GENERIC* const _pMessage);
		const bool _messageProcedureListFB(NET_MSG_GENERIC* const _pMessage);
		const bool _messageProcedureBegin(NET_MSG_GENERIC* const _pMessage);
		const bool _messageProcedureTransferMessage(NET_MSG_GENERIC* const _pMessage);

	public:
		__forceinline const GLEvent::ManagerClient& operator =(const GLEvent::ManagerClient& _rhs) {}
		ManagerClient(GLGaeaClient* const _pGaeaClient);
		~ManagerClient(void);

	private:
		GLGaeaClient* const pGaeaClient;
	};
}

#include "./EventClient.hpp"
