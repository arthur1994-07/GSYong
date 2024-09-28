#include "pch.h"

#include "../../../RanLogic/Msg/GLContrlEventMsg.h"
#include "../../../SigmaCore/Hash/CHash.h"
#include "../../AgentServer/GLAgentServer.h"
#include "./List/EventAgentItem.h"
#include "./EventAgent.h"

namespace GLEvent
{
	const bool ManagerAgent::messageProcedure(const unsigned int _clientID, const unsigned int _gaeaID, const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_EVENT_MESSAGE* const _pMessageEvent((const GLMSG::NET_EVENT_MESSAGE* const)_pMessage);

		const std::string _stringNameFileScript(_pMessageEvent->cNameFileScript);
		const unsigned int _hashKey(HASH::generateHashKey(_stringNameFileScript.c_str(), _stringNameFileScript.length()));

		GLEvent::InterfaceMap& _mapEventInterface(this->mapEventInterface);
		GLEvent::InterfaceMapIterator _iteratorEventInterface(_mapEventInterface.find(_hashKey));
		GLEvent::Interface* const _pEventInterface(_iteratorEventInterface == _mapEventInterface.end() ? 0 : _iteratorEventInterface->second);

		const GLEvent::MESSAGE _typeMessage(_pMessageEvent->typeMessage);
		switch ( _typeMessage )
		{
		case GLEvent::MESSAGE_LIST:
			return ManagerAgent::_messageProcedureList(_clientID, _gaeaID, _pMessage);
		case GLEvent::MESSAGE_END:
			return ManagerAgent::_messageProcedureEnd(_clientID, _gaeaID, _pMessage);
		case GLEvent::MESSAGE_ITEM_INFORMATION_BASIC:
			return ManagerAgent::_messageProcedureItemInformationBasic(_clientID, _gaeaID, _pMessage);
		case GLEvent::MESSAGE_ITEM_INFORMATION_SET:
		case GLEvent::MESSAGE_ITEM_INFORMATION_DONE:
			if ( _pEventInterface == 0 )
				return false;

			if ( _pEventInterface->vf_messageProcedure(_clientID, _gaeaID, _pMessage) == false )
				ManagerAgent::_removeInterface(_iteratorEventInterface);
			return true;
		}

		return false;
	}	
	const bool ManagerAgent::_messageProcedureEnd(const unsigned int _clientID, const unsigned int _gaeaID, const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_EVENT_END* const _pMessageEnd((const GLMSG::NET_EVENT_END* const)_pMessage);

		const std::string _stringNameFileScript(_pMessageEnd->cNameFileScript);
		const unsigned int _hashKey(HASH::generateHashKey(_stringNameFileScript.c_str(), _stringNameFileScript.length()));

		GLEvent::InterfaceMap& _mapEventInterface(this->mapEventInterface);
		GLEvent::InterfaceMapConstIterator _iteratorEventInterface(_mapEventInterface.find(_hashKey));
		if ( _iteratorEventInterface == _mapEventInterface.end() )
			return false;

		_mapEventInterface.erase(_iteratorEventInterface);		

		GLMSG::NET_EVENT_END_FB _messageEnd;
		this->pAgentServer->SENDTOCLIENT(_clientID, &_messageEnd);

		return true;
	}
	const bool ManagerAgent::_messageProcedureList(const unsigned int _clientID, const unsigned int _gaeaID, const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_EVENT_LIST* const _pMessageList((const GLMSG::NET_EVENT_LIST* const)_pMessage);

		GLEvent::InterfaceMap& _mapEventInterface(this->mapEventInterface);

		GLMSG::NET_EVENT_LIST_FB _messageList;
		for ( GLEvent::InterfaceMapConstIterator _iteratorEventInterface(_mapEventInterface.begin()); _iteratorEventInterface != _mapEventInterface.end(); ++_iteratorEventInterface )
		{
			const GLEvent::Interface* const _pEventInterface(_iteratorEventInterface->second);
			const std::string& _stringScriptEvent(_pEventInterface->getStringScriptEvent());
			if ( _messageList.addItem(_stringScriptEvent) == false )
			{
				this->pAgentServer->SENDTOCLIENT(_clientID, &_messageList);
				_messageList.reset();
			}
		}

		if ( _messageList.isValid() == true )
			this->pAgentServer->SENDTOCLIENT(_clientID, &_messageList);

		return true;
	}
	const bool ManagerAgent::_messageProcedureItemInformationBasic(const unsigned int _clientID, const unsigned int _gaeaID, const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_EVENT_ITEM_INFORMATION_BASIC* const _pMessageInformationBasic((const GLMSG::NET_EVENT_ITEM_INFORMATION_BASIC* const)_pMessage);

		const std::string _stringNameFileScript(_pMessageInformationBasic->cNameFileScript);
		const __time64_t _timeDuration(_pMessageInformationBasic->timeDuration);
		const unsigned int _hashKey(HASH::generateHashKey(_stringNameFileScript.c_str(), _stringNameFileScript.length()));
		const bool _bLoopBack(_pMessageInformationBasic->bLoopBack);
		const bool _bShowInUI(_pMessageInformationBasic->bShowInUI);

		GLEvent::InterfaceMap& _mapEventInterface(this->mapEventInterface);
		GLEvent::InterfaceMapConstIterator _iteratorEventInterface(_mapEventInterface.find(_hashKey));
		if ( _iteratorEventInterface != _mapEventInterface.end() )
			ManagerAgent::_removeInterface(_iteratorEventInterface);		

		GLEvent::Interface* const _pEventInterface((GLEvent::Interface* const)new GLEvent::Item(this->pAgentServer, _stringNameFileScript, _timeDuration, this->timeCurrent, _bLoopBack, _bShowInUI));
		_mapEventInterface.insert(GLEvent::InterfaceValue(_hashKey, _pEventInterface));

		return true;
	}	
}