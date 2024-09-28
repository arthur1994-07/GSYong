#pragma once

namespace GLEvent
{
	inline const GLEvent::Interface& Interface::operator =(const GLEvent::Interface& _rhs)
	{
		this->typeEvent = _rhs.typeEvent;
		this->stringScriptEvent = _rhs.stringScriptEvent;
		this->hashKeyScriptEvent = _rhs.hashKeyScriptEvent;
		this->timeDuration = _rhs.timeDuration;
		this->timeBegin = _rhs.timeBegin;

		return *this;
	}
	inline const GLEvent::TYPE Interface::getTypeEvent(void) const
	{
		return this->typeEvent;
	}
	inline const std::string& Interface::getStringScriptEvent(void) const
	{
		return this->stringScriptEvent;
	}
}