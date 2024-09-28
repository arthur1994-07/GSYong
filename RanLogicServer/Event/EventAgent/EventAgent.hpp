#pragma once

namespace GLEvent
{
	GLEvent::Interface* const ManagerAgent::findInterface(const unsigned int _keyInterface)
	{
		GLEvent::InterfaceMap& _mapEventInterface(this->mapEventInterface);
		GLEvent::InterfaceMapIterator _iteratorEventInterface(_mapEventInterface.find(_keyInterface));
		if ( _iteratorEventInterface == _mapEventInterface.end() )
			return 0;

		GLEvent::Interface* const _pEventInterface(_iteratorEventInterface->second);
		return _pEventInterface;
	}
	inline void ManagerAgent::_removeInterface(GLEvent::InterfaceMapConstIterator _iteratorEventInterface)
	{
		GLEvent::Interface* const _pEventInterface(_iteratorEventInterface->second);
		delete _pEventInterface;

		this->mapEventInterface.erase(_iteratorEventInterface);
	}
}