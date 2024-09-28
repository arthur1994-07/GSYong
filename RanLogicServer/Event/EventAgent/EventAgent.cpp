#include "pch.h"

#include "../../AgentServer/GLAgentServer.h"
#include "../../Character/GLCharAG.h"
#include "./EventAgent.h"

namespace GLEvent
{
	ManagerAgent::ManagerAgent(GLAgentServer* const _pAgentServer)
		: pAgentServer(_pAgentServer)
		, timeCurrent(0)
	{
	}
	ManagerAgent::~ManagerAgent(void)
	{
		GLEvent::InterfaceMap& _mapEventInterface(this->mapEventInterface);
		for ( GLEvent::InterfaceMapIterator _iteratorEventInterface(_mapEventInterface.begin()); _iteratorEventInterface != _mapEventInterface.end(); ++_iteratorEventInterface )
		{
			GLEvent::Interface* const _pEventInterface(_iteratorEventInterface->second);
			delete _pEventInterface;
		}
		_mapEventInterface.clear();
	}
	void ManagerAgent::frameMove(const float _fTime, const float _fTimeElapsed, const __time64_t& _timeCurrent)
	{
		this->timeCurrent = _timeCurrent;

		GLEvent::InterfaceMap& _mapEventInterface(this->mapEventInterface);
		for ( GLEvent::InterfaceMapIterator _iteratorEventInterface(_mapEventInterface.begin()); _iteratorEventInterface != _mapEventInterface.end(); ++_iteratorEventInterface )
		{
			GLEvent::Interface* const _pEventInterface(_iteratorEventInterface->second);
			_pEventInterface->vf_frameMove(_fTime, _fTimeElapsed, _timeCurrent);
		}
	}	
	void ManagerAgent::online(const unsigned int _dbNum)
	{
		GLEvent::InterfaceMap& _mapEventInterface(this->mapEventInterface);
		for ( GLEvent::InterfaceMapIterator _iteratorEventInterface(_mapEventInterface.begin()); _iteratorEventInterface != _mapEventInterface.end(); ++_iteratorEventInterface )
		{
			GLEvent::Interface* const _pEventInterface(_iteratorEventInterface->second);
			_pEventInterface->vf_online(_dbNum);
		}		
	}	
	void ManagerAgent::offline(const unsigned int _dbNum)
	{
		GLCharAG* const _pCharacter(this->pAgentServer->GetCharByDbNum(_dbNum));
		if ( _pCharacter == 0 )
			return;

		GLEvent::InterfaceMap& _mapEventInterface(this->mapEventInterface);
		for ( GLEvent::InterfaceMapIterator _iteratorEventInterface(_mapEventInterface.begin()); _iteratorEventInterface != _mapEventInterface.end(); ++_iteratorEventInterface )
		{
			GLEvent::Interface* const _pEventInterface(_iteratorEventInterface->second);
			_pEventInterface->vf_online(_dbNum);
		}		
	}
}