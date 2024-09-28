#pragma once

#include "./EventDefine.h"

namespace GLEvent
{
	class Interface
	{
	public:
		virtual void vf_online(const unsigned int _dbNum) = 0;
		virtual void vf_offline(const unsigned int _dbNum) = 0;
		virtual void vf_frameMove(const float _fTime, const float _fTimeElapsed, const __time64_t& _timeCurrent) = 0;
		virtual const bool vf_messageProcedure(const unsigned int _clientID, const unsigned int _gaeaID, const NET_MSG_GENERIC* const _pMessage) = 0;

		inline const GLEvent::TYPE getTypeEvent(void) const;
		inline const std::string& getStringScriptEvent(void) const;

	public:
		inline const GLEvent::Interface& operator =(const GLEvent::Interface& _rhs);		
		Interface(const GLEvent::TYPE _typeEvent, const std::string& _stringScriptEvent, const __time64_t _timeDuration, const __time64_t& _timeBegin);
		Interface(const GLEvent::Interface& _rhs);
		Interface(void);
		virtual ~Interface(void);

	protected:
		GLEvent::TYPE typeEvent;
		std::string stringScriptEvent;		
		unsigned int hashKeyScriptEvent;
		__time64_t timeDuration;
		__time64_t timeBegin;
	};
	typedef std::map<unsigned int, GLEvent::Interface*> InterfaceMap;
	typedef InterfaceMap::const_iterator InterfaceMapConstIterator;
	typedef InterfaceMap::iterator InterfaceMapIterator;
	typedef InterfaceMap::value_type InterfaceValue;
}

#include "./Event.hpp"