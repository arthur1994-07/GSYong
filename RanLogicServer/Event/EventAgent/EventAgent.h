#pragma once

#include "./EventAgentDefine.h"

namespace GLEvent
{
	class ManagerAgent
	{
	public:
		void online(const unsigned int _dbNum);
		void offline(const unsigned int _dbNum);
		void frameMove(const float _fTime, const float _fTimeElapsed, const __time64_t& _timeCurrent);
		const bool messageProcedure(const unsigned int _clientID, const unsigned int _garaID, const NET_MSG_GENERIC* const _pMessage);

		inline GLEvent::Interface* const findInterface(const unsigned int _keyInterface);

	private:
		const bool _messageProcedureList(const unsigned int _clientID, const unsigned int _gaeaID, const NET_MSG_GENERIC* const _pMessage);
		const bool _messageProcedureEnd(const unsigned int _clientID, const unsigned int _gaeaID, const NET_MSG_GENERIC* const _pMessage);
		const bool _messageProcedureItemInformationBasic(const unsigned int _clientID, const unsigned int _gaeaID, const NET_MSG_GENERIC* const _pMessage);

		inline void _removeInterface(GLEvent::InterfaceMapConstIterator _iteratorEventInterface);

	public:
		inline const GLEvent::ManagerAgent& operator =(const GLEvent::ManagerAgent& _rhs){}
		ManagerAgent(GLAgentServer* const _pAgentServer);
		~ManagerAgent(void);

	private:
		GLAgentServer* const pAgentServer;
		GLEvent::InterfaceMap mapEventInterface;
		__time64_t timeCurrent;
	};
}

#include "./EventAgent.hpp"