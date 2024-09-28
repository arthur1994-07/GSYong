#pragma once

#include "./PingManagerClientDefine.h"

namespace Ping
{
	class ManagerClient
	{
		friend class GLGaeaClient;
	public:
		__forceinline const unsigned int getPing(void) const;

	private:
		void _frameMove(const float _fTime, const float _fTimeElapsed);
		const bool CALLBACK _messageProcedure(const NET_MSG_GENERIC* const _pMessage);

	private:
		const bool CALLBACK _messageProcedureAnswer(const NET_MSG_GENERIC* const _pMessage);
		const bool CALLBACK _messageProcedureAnswerInformationDetail(const NET_MSG_GENERIC* const _pMessage);

	private:
		__forceinline const Ping::ManagerClient& operator =(const Ping::ManagerClient&){}
		ManagerClient(GLGaeaClient* const _pGaeaClient);
		~ManagerClient(void);		

	private:
		GLGaeaClient* const pGaeaClient;

		float fTimeElapsed;
		unsigned int pingCurrent;
	};
}

#include "./PingManagerClient.hpp"

