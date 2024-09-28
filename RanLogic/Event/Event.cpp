#include "pch.h"

#include "../../SigmaCore/Hash/CHash.h"
#include "./Event.h"

namespace GLEvent
{
	Interface::Interface(void)
		: typeEvent(GLEvent::TYPE_NSIZE)
		, hashKeyScriptEvent(0)
		, timeDuration(0)
		, timeBegin(0)
	{
	}
	Interface::Interface(const GLEvent::TYPE _typeEvent, const std::string& _stringScriptEvent, const __time64_t _timeDuration, const __time64_t& _timeBegin)
		: typeEvent(_typeEvent)
		, stringScriptEvent(_stringScriptEvent)
		, hashKeyScriptEvent(HASH::generateHashKey(_stringScriptEvent.c_str(), _stringScriptEvent.length()))
		, timeDuration(_timeDuration)
		, timeBegin(_timeBegin)
	{
	}
	Interface::Interface(const GLEvent::Interface& _rhs)
		: typeEvent(_rhs.typeEvent)
		, stringScriptEvent(_rhs.stringScriptEvent)
		, hashKeyScriptEvent(_rhs.hashKeyScriptEvent)
		, timeDuration(_rhs.timeDuration)
		, timeBegin(_rhs.timeBegin)
	{
	}
	Interface::~Interface(void)
	{
	}
}