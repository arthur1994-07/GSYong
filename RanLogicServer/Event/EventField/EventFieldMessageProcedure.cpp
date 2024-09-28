#include "pch.h"

#include "../../../RanLogic/Msg/GLContrlEventMsg.h"
#include "../../../SigmaCore/Hash/CHash.h"
#include "../../FieldServer/GLGaeaServer.h"
#include "../../Character/GLChar.h"
#include "./EventField.h"

namespace GLEvent
{
	const bool ManagerField::messageProcedure(const unsigned int _clientID, const unsigned int _gaeaID, const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_EVENT_MESSAGE* const _pMessageEvent((const GLMSG::NET_EVENT_MESSAGE* const)_pMessage);

		const char* const _cNameFileScript(_pMessageEvent->cNameFileScript);
		const unsigned int _hashKey(HASH::generateHashKey(_cNameFileScript, GLEvent::LENGTH_NAME));

		GLEvent::InterfaceMap& _mapEventInterface(this->mapEventInterface);
		GLEvent::InterfaceMapIterator _iteratorEventInterface(_mapEventInterface.find(_hashKey));
		GLEvent::Interface* const _pEventInterface(_iteratorEventInterface == _mapEventInterface.end() ? 0 : _iteratorEventInterface->second);

		const GLEvent::MESSAGE _typeMessage(_pMessageEvent->typeMessage);
		switch ( _typeMessage )
		{
		case GLEvent::MESSAGE_ITEM_TRANSFER_ITEM:
			return ManagerField::_messageProcedureItemTransferItem(_clientID, _gaeaID, _pMessage);
		}

		return false;
	}
	const bool ManagerField::_messageProcedureItemTransferItem(const unsigned int _clientID, const unsigned int _gaeaID, const NET_MSG_GENERIC* const _pMessage)
	{
		const GLMSG::NET_EVENT_ITEM_TRANSFER_ITEM* const _pMessageTransferItem((const GLMSG::NET_EVENT_ITEM_TRANSFER_ITEM* const)_pMessage);

		const unsigned int _dbNum(_pMessageTransferItem->dbNum);
		GLChar* const _pCharacter(this->pGaeaServer->GetCharByDbNum(_dbNum));
		if ( _pCharacter == 0 )
			return false;

		const unsigned int _sizeItem(_pMessageTransferItem->sizeItem);
		for ( unsigned int _index(_sizeItem); _index--; )
		{
			const SNATIVEID& _itemID(_pMessageTransferItem->itemID[_index]);
			if ( this->pGaeaServer->GmMakeItem(_pCharacter, _itemID, 1) == FALSE )
				continue;

// 			const std::string stringGMCmd(sc::string::format("Character(%1%) Item(%2%/%3%) Add by GM-Command\0", pTargetChar->CharDbNum(), _itemID.Mid(), _itemID.Sid()));
// 			m_pAdoMan->AddUserJob(db::DbActionPtr(new UserWriteGMLog(pPacket->dwMasterUserNum, stringGMCmd, pPacket->szMasterIP)));			
		}

		return true;
	}
}