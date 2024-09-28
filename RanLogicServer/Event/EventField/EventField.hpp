#pragma once

namespace GLEvent
{
	__forceinline void ManagerField::_removeInterface(GLEvent::InterfaceMapConstIterator _iteratorEventInterface)
	{
		GLEvent::Interface* const _pEventInterface(_iteratorEventInterface->second);
		delete _pEventInterface;

		this->mapEventInterface.erase(_iteratorEventInterface);
	}
}