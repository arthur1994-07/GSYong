#include "pch.h"

#include "./EventField.h"

namespace GLEvent
{
	ManagerField::ManagerField(GLGaeaServer* const _pGaeaServer)
		: pGaeaServer(_pGaeaServer)
		, timeCurrent(0)
	{
	}
	ManagerField::~ManagerField(void)
	{
		GLEvent::InterfaceMap& _mapEventInterface(this->mapEventInterface);
		for ( GLEvent::InterfaceMapIterator _iteratorEventInterface(_mapEventInterface.begin()); _iteratorEventInterface != _mapEventInterface.end(); ++_iteratorEventInterface )
		{
			GLEvent::Interface* const _pEventInterface(_iteratorEventInterface->second);
			delete _pEventInterface;
		}
		_mapEventInterface.clear();
	}
	void ManagerField::frameMove(const float _fTime, const float _fTimeElapsed, const __time64_t& _timeCurrent)
	{
		this->timeCurrent = _timeCurrent;

		GLEvent::InterfaceMap& _mapEventInterface(this->mapEventInterface);
		for ( GLEvent::InterfaceMapIterator _iteratorEventInterface(_mapEventInterface.begin()); _iteratorEventInterface != _mapEventInterface.end(); ++_iteratorEventInterface )
		{
			GLEvent::Interface* const _pEventInterface(_iteratorEventInterface->second);
			_pEventInterface->vf_frameMove(_fTime, _fTimeElapsed, _timeCurrent);
		}
	}
}
