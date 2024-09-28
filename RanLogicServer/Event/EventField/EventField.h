#pragma once

#include "./EventFieldDefine.h"

namespace GLEvent
{
	class ManagerField
	{
	public:
		void frameMove(const float _fTime, const float _fTimeElapsed, const __time64_t& _timeCurrent);

		const bool messageProcedure(const unsigned int _clientID, const unsigned int _gaeaID, const NET_MSG_GENERIC* const _pMessage);

	private:
		const bool _messageProcedureItemTransferItem(const unsigned int _clientID, const unsigned int _gaeaID, const NET_MSG_GENERIC* const _pMessage);

		__forceinline void _removeInterface(GLEvent::InterfaceMapConstIterator _iteratorEventInterface);

	public:
		__forceinline const GLEvent::ManagerField& operator =(const GLEvent::ManagerField& _rhs);
		ManagerField(GLGaeaServer* const _pGaeaServer);
		~ManagerField(void);

	private:
		GLGaeaServer* const pGaeaServer;
		GLEvent::InterfaceMap mapEventInterface;
		__time64_t timeCurrent;
	};
}

#include "./EventField.hpp"
