#include "pch.h"

#include "../../../enginelib/GUInterface/UITextControl.h"
#include "../../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../Character/GLChar.h"
#include "../../FieldServer/GLGaeaServer.h"
#include "InstanceSystemField.h"

#define notifyField(_format, ...) { \
	std::string _notifyMessage(sc::string::format(_format, ##__VA_ARGS__)); \
	GLMSG::NET_INSTANCE_UI_CHATMSG _resultMessage(_notifyMessage.c_str(), D3DCOLOR_XRGB(239, 228, 176)); \
    msgpack::sbuffer SendBuffer; \
    msgpack::pack(SendBuffer, _resultMessage); \
	gpGaeaServer->SENDTOCLIENT(_nClientSlot, NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);} \

namespace InstanceSystem
{
	enum
	{
		VALUE_TYPE_NULL,
		VALUE_TYPE_NUMBER,
		VALUE_TYPE_INTEGER,
		VALUE_TYPE_BOOLEAN,
		VALUE_TYPE_STRING,
		VALUE_TYPE_TABLE,
	};

	const unsigned int getValueType(const LuaPlus::LuaObject& _objectGlobal);
	void sendTableValue(LuaPlus::LuaObject& _objectTable, const char* const _strGlobalName, const InstanceMapID _instanceMapID, const SNATIVEID& _keyMapID, const unsigned int _nClientSlot);	

	const bool CALLBACK ManagerField::_messageGMCommand(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		GLChar* const _pGM(gpGaeaServer->GetChar(_dwGaeaID));
		if ( _pGM == 0 )
			return false;

		if ( _pGM->UserLevel() < USER_GM3 )
			return false;

		GLLandMan* const _pLand(_pGM->GetLandMan());
		if ( _pLand == 0 )
			return false;
		
		const SNATIVEID& _gaeaMapID(_pLand->GetGaeaMapID());
		const unsigned int _nClientSlot(_pGM->ClientSlot());

		const GLMSG::NET_INSTANCE_GM_COMMAND* const _pGMCommandMessage((const GLMSG::NET_INSTANCE_GM_COMMAND* const)_pMessage);
		const InstanceSystem::EMCOMMAND_TYPE _emCommand(_pGMCommandMessage->emCommandType);
		switch ( _emCommand )
		{
		case InstanceSystem::EMCOMMAND_MOVE_TO_FIELD:
		case InstanceSystem::EMCOMMAND_MOVE_TO_INSTANCE:
			{
				const char* const _szName(_pGMCommandMessage->szParameter0);
				const DBNUM _dbNum(gpGaeaServer->GetChaDbNumByChaName(_szName));
				GLChar* const _pPlayer(gpGaeaServer->GetCharByDbNum(_dbNum));
				if ( _pPlayer == 0 )
				{
					notifyField("not exist [%1%]", _szName);
					break;
				}

				const unsigned int _targetMapMainID(_pGMCommandMessage->nParameter0);
				const unsigned int _targetMapSubID(_pGMCommandMessage->nParameter1);
				const float _targetPositionX(_pGMCommandMessage->fParameter2);
				const float _targetPositionZ(_pGMCommandMessage->fParameter3);
				const EMMOVE_TYPE _emMoveType(_emCommand == InstanceSystem::EMCOMMAND_MOVE_TO_FIELD ? InstanceSystem::EMMOVE_INSTANCE2FIELD : InstanceSystem::EMMOVE_INSTANCE2INSTANCE);
				const InformationPosition _reserveSetPosition(
					Faction::GLActorID(CROW_PC, _dbNum),
					SNATIVEID(_targetMapMainID, _targetMapSubID),
					Faction::GLActorID(CROW_NULL, GAEAID_NULL), 
					_targetPositionX, _targetPositionZ,
					_emMoveType);
				ManagerField::requestMoveToPosition(_gaeaMapID.Mid(), _reserveSetPosition);
			}
			break;
		case InstanceSystem::EMCOMMAND_MOVE_TO_GATE:
			{
				const unsigned int _targetMapMainID(_pGMCommandMessage->nParameter0);
				const unsigned int _targetMapSubID(_pGMCommandMessage->nParameter1);
				const unsigned int _gateID(_pGMCommandMessage->nParameter2);
				const InformationPosition _reserveSetPosition(
					Faction::GLActorID(CROW_PC, _pGM->GetCharID()),
					SNATIVEID(_targetMapMainID, _targetMapSubID), 					
					Faction::GLActorID(CROW_GATE, _gateID), 
					0.0f, 0.0f, InstanceSystem::EMMOVE_INSTANCE2INSTANCE);
				ManagerField::requestMoveToPosition(_gaeaMapID.Mid(), _reserveSetPosition);
			}			
			break;
		case InstanceSystem::EMCOMMAND_MOVE_TO:
			{
				const char* const _szName(_pGMCommandMessage->szParameter0);
				const DBNUM _dbNum(gpGaeaServer->GetChaDbNumByChaName(_szName));
				GLChar* const _pPlayer(gpGaeaServer->GetCharByDbNum(_dbNum));
				if ( _pPlayer == 0 )
				{
					notifyField("not exist [%1%]", _szName);
					break;
				}				
				const Faction::GLActorID _actorID(CROW_PC, _dbNum);
				const D3DXVECTOR3 _targetPosition(_pGMCommandMessage->fParameter0, _pPlayer->GetPositionActor().y, _pGMCommandMessage->fParameter1);
				GLMSG::NET_INSTANCE_DO_MOVE_TO _moveToMessage(_gaeaMapID.Mid(), _actorID, Faction::GLActorID(), _targetPosition);
				gpGaeaServer->SENDTOMYSELF(&_moveToMessage);
			}
			break;
		case InstanceSystem::EMCOMMAND_DO_JOIN:
			{
				const char* const _szName(_pGMCommandMessage->szParameter0);
				const DBNUM _dbNum(gpGaeaServer->GetChaDbNumByChaName(_szName));
				GLChar* const _pPlayer(gpGaeaServer->GetCharByDbNum(_dbNum));
				if ( _pPlayer == 0 )
				{
					notifyField("not exist [%1%]", _szName);
					return false;
				}

				const InstanceMapID _targetInstanceMapID(_pGMCommandMessage->nParameter0 == -1 ? _gaeaMapID.Mid() : _pGMCommandMessage->nParameter0);
				if ( _targetInstanceMapID == InstanceMapID(-1) )
				{
					notifyField("invalid instanceMapID [%1%]", _targetInstanceMapID);
					return false;
			}				
				const InformationPosition _informationPosition(
					Faction::GLActorID(CROW_PC, _dbNum),
					SNATIVEID(_targetInstanceMapID, 0),
					Faction::GLActorID(),
					0.0f, 0.0f, InstanceSystem::EMMOVE_FIELD2INSTANCE);

				GLMSG::NET_INSTANCE_REQUEST_JOIN _outMessage(_informationPosition, InstanceSystem::EMAUTHORITY_NORMAL_FORCED);
				gpGaeaServer->SENDTOAGENT(&_outMessage);
			}
			break;
		case InstanceSystem::EMCOMMAND_DO_OUT:
			{
				const char* const _szName(_pGMCommandMessage->szParameter0);
				const DBNUM _dbNum(gpGaeaServer->GetChaDbNumByChaName(_szName));
				GLChar* const _pPlayer(gpGaeaServer->GetCharByDbNum(_dbNum));
				if ( _pPlayer == 0 )
				{
					notifyField("not exist [%1%]", _szName);
					return false;
				}

				const InstanceMapID _targetInstanceMapID(_pGMCommandMessage->nParameter0 == -1 ? _gaeaMapID.Mid() : _pGMCommandMessage->nParameter0);
				if ( _targetInstanceMapID == InstanceMapID(-1) )
				{
					notifyField("invalid instanceMapID [%1%]", _targetInstanceMapID);
					return false;
				}

				GLMSG::NET_INSTANCE_REQUEST_OUT _outMessage(_targetInstanceMapID, _dbNum);
				gpGaeaServer->SENDTOAGENT(&_outMessage);
			}
			break;
		case InstanceSystem::EMCOMMAND_SET_GLOBAL:
			{
				GLLandMan* const _pLand(_pGM->GetLandMan());
				if ( _pLand == 0 )
				{
					notifyField("can't find %1%;", "LandMan");
					break;
				}
				const InstanceMapID _instanceMapID(_pGMCommandMessage->nParameter0 != -1 ? _pGMCommandMessage->nParameter0 : _pLand->GetGaeaMapID().wMainID);
				const char* const _strGlobalName(_pGMCommandMessage->szParameter0);
				InstanceFieldMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
				if ( _iteratorInstance == this->instanceMap.end() )
				{
					notifyField("not in instance or illegal instanceMapID(instanceMapID : %1%);", _instanceMapID);
				}
				else
				{
					InstanceField& _instanceField(_iteratorInstance->second);
					const SNATIVEID& _keyMapID(_instanceField.getKeyMapID());
					LuaPlus::LuaObject _objectGlobal(_instanceField.luaInstance.GetGlobalValue_Lua(_strGlobalName));
					const unsigned int _nValueType(InstanceSystem::getValueType(_objectGlobal));
					switch ( _nValueType )
					{
					case InstanceSystem::VALUE_TYPE_NULL:
						{
							notifyField("not exist global : [%1%] in instance(instanceMapID : %2%);", _strGlobalName, _instanceMapID);
						}
						break;
					case InstanceSystem::VALUE_TYPE_NUMBER:
						{							
						}
						break;
					case InstanceSystem::VALUE_TYPE_INTEGER:
						{							
						}
						break;
					case InstanceSystem::VALUE_TYPE_BOOLEAN:
						{							
						}
						break;
					}		
				}
			}
			break;
		case InstanceSystem::EMCOMMAND_GET_GLOBAL:
			{
				GLLandMan* const _pLand(_pGM->GetLandMan());
				if ( _pLand == 0 )
				{
					notifyField("can't find %1%;", "LandMan");
					break;
				}
				const InstanceMapID _instanceMapID(_pGMCommandMessage->nParameter0 != -1 ? _pGMCommandMessage->nParameter0 : _pLand->GetGaeaMapID().wMainID);
				const char* const _strGlobalName(_pGMCommandMessage->szParameter0);
				InstanceFieldMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
				if ( _iteratorInstance == this->instanceMap.end() )
				{
					notifyField("not in instance or illegal instanceMapID(instanceMapID : %1%);", _instanceMapID);
				}
				else
				{
					
					InstanceField& _instanceField(_iteratorInstance->second);
					const SNATIVEID& _keyMapID(_instanceField.getKeyMapID());
					LuaPlus::LuaObject _objectGlobal(_instanceField.luaInstance.GetGlobalValue_Lua(_strGlobalName));
					const unsigned int _nValueType(InstanceSystem::getValueType(_objectGlobal));
					switch ( _nValueType )
					{
					case InstanceSystem::VALUE_TYPE_NULL:
						{
							notifyField("not exist global : [%1%] in instance(instanceMapID : %2%);", _strGlobalName, _instanceMapID);
						}
						break;
					case InstanceSystem::VALUE_TYPE_TABLE:
						{
							InstanceSystem::sendTableValue(_objectGlobal, _strGlobalName, _instanceMapID, _keyMapID, _nClientSlot);							
						}
						break;
					case InstanceSystem::VALUE_TYPE_NUMBER:
						{
							GLMSG::NET_INSTANCE_NOTIFY_GLOBAL_FLOAT _notifyGlobalMessage(_strGlobalName, _instanceMapID, _keyMapID);
							_notifyGlobalMessage.addGlobal(0, _objectGlobal.GetNumber());
							gpGaeaServer->SENDTOCLIENT(_nClientSlot, &_notifyGlobalMessage);
						}
						break;
					case InstanceSystem::VALUE_TYPE_INTEGER:
						{
							GLMSG::NET_INSTANCE_NOTIFY_GLOBAL_INTEGER _notifyGlobalMessage(_strGlobalName, _instanceMapID, _keyMapID);
							_notifyGlobalMessage.addGlobal(0, _objectGlobal.GetInteger());
							gpGaeaServer->SENDTOCLIENT(_nClientSlot, &_notifyGlobalMessage);
						}
						break;
					case InstanceSystem::VALUE_TYPE_BOOLEAN:
						{
							GLMSG::NET_INSTANCE_NOTIFY_GLOBAL_BOOLEAN _notifyGlobalMessage(_strGlobalName, _instanceMapID, _keyMapID);
							_notifyGlobalMessage.addGlobal(0, _objectGlobal.GetBoolean());
							gpGaeaServer->SENDTOCLIENT(_nClientSlot, &_notifyGlobalMessage);
						}
						break;
					case InstanceSystem::VALUE_TYPE_STRING:
						{
							GLMSG::NET_INSTANCE_NOTIFY_GLOBAL_STRING _notifyGlobalMessage(_strGlobalName, _instanceMapID, _keyMapID);
							_notifyGlobalMessage.addGlobal(0, _objectGlobal.GetString());
							gpGaeaServer->SENDTOCLIENT(_nClientSlot, &_notifyGlobalMessage);						
						}
						break;
					}
				}
			}
			break;
		case InstanceSystem::EMCOMMAND_DEBUG:
			{
                GLChar* _pPlayer(_pGM);
                if ( _pGMCommandMessage->nParameter0 == -1 )
                {
                    const DBNUM _dbNum(gpGaeaServer->GetChaDbNumByChaName(_pGMCommandMessage->szParameter0));
                    _pPlayer = gpGaeaServer->GetCharByDbNum(_dbNum);
                    if ( _pPlayer == NULL )
                        return false;
                }

				GLLandMan* const _pLand(_pPlayer->GetLandMan());
				const MapID& _currentMapID(_pPlayer->GetCurrentMap());
				const SNATIVEID& _baseMapIDPC(_currentMapID.getBaseMapID());
				const SNATIVEID& _gaeaMapIDPC(_currentMapID.getGaeaMapID());

				const SNATIVEID& _baseMapIDLand(_pLand->GetBaseMapID());
				const SNATIVEID& _gaeaMapIDLand(_pLand->GetGaeaMapID());

				const unsigned int _fieldServerID(gpGaeaServer->GetFieldSvrID());
				const unsigned int _serverChannel(gpGaeaServer->GetServerChannel());
				notifyField("Field-PC : baseID[%1%/%2%], gaeaID[%3%/%4%];", _baseMapIDPC.Mid(), _baseMapIDPC.Sid(), _gaeaMapIDPC.Mid(), _gaeaMapIDPC.Sid());
				notifyField("Field-Land : baseID[%1%/%2%], gaeaID[%3%/%4%];", _baseMapIDLand.Mid(), _baseMapIDLand.Sid(), _gaeaMapIDLand.Mid(), _gaeaMapIDLand.Sid());
				notifyField("Field : Field-ID[%1%], Channel[%2%];", _fieldServerID, _serverChannel);
			}
			break;
		}

		return true;
	}



	
	const unsigned int getValueType(const LuaPlus::LuaObject& _objectGlobal)
	{
		if ( _objectGlobal.IsNil() == true )
			return InstanceSystem::VALUE_TYPE_NULL;
		else if ( _objectGlobal.IsTable() == true )
			return InstanceSystem::VALUE_TYPE_TABLE;
		else if ( _objectGlobal.IsBoolean() == true )
			return InstanceSystem::VALUE_TYPE_BOOLEAN;
		else if ( _objectGlobal.IsInteger() == true )
			return InstanceSystem::VALUE_TYPE_INTEGER;
		else if ( _objectGlobal.IsNumber() == true )
			return InstanceSystem::VALUE_TYPE_NUMBER;
		else if ( _objectGlobal.IsString() == true )
			return InstanceSystem::VALUE_TYPE_STRING;		

		return InstanceSystem::VALUE_TYPE_NULL;
	}

	void sendTableValue(LuaPlus::LuaObject& _objectTable, const char* const _strGlobalName, const InstanceMapID _instanceMapID, const SNATIVEID& _keyMapID, const unsigned int _nClientSlot)
	{
		LuaPlus::LuaTableIterator _iterator(_objectTable);
        if ( _iterator.IsValid() == false )
        {
            notifyField("empty global : [%1%] in instance(instanceMapID : %2%);", _strGlobalName, _instanceMapID);
            return;
        }

		const LuaPlus::LuaObject _keyObject(_iterator.GetKey());
		if ( _keyObject.IsInteger() == false )
		{
			notifyField("global : [%1%] is table, but key-value is not integer;(instanceMapID : %2%);", _strGlobalName, _instanceMapID);
			return;
		}

		LuaPlus::LuaObject _valueObject(_iterator.GetValue());
		const unsigned int _nValueType(getValueType(_valueObject));
		switch ( _nValueType )
		{
		case InstanceSystem::VALUE_TYPE_NULL:
			notifyField("not exist global : [%1%] in instance(instanceMapID : %2%);", _strGlobalName, _instanceMapID);
			break;
		case InstanceSystem::VALUE_TYPE_TABLE:
			InstanceSystem::sendTableValue(_valueObject, _strGlobalName, _instanceMapID, _keyMapID, _nClientSlot);
			break;
		case InstanceSystem::VALUE_TYPE_NUMBER:
			{
				GLMSG::NET_INSTANCE_NOTIFY_GLOBAL_FLOAT _notifyGlobalMessage(_strGlobalName, _instanceMapID, _keyMapID, true);
				for ( ; _iterator; _iterator.Next() )
				{
					const LuaPlus::LuaObject _keyObject(_iterator.GetKey());
					const LuaPlus::LuaObject _valueObject(_iterator.GetValue());

					const unsigned int _nKey(_keyObject.GetInteger());					
					if ( _notifyGlobalMessage.addGlobal(_nKey, _valueObject.GetNumber()) == false )
					{
						gpGaeaServer->SENDTOCLIENT(_nClientSlot, &_notifyGlobalMessage);
						_notifyGlobalMessage.reset();
					}
				}				
				gpGaeaServer->SENDTOCLIENT(_nClientSlot, &_notifyGlobalMessage);
			}
			break;
		case InstanceSystem::VALUE_TYPE_INTEGER:
			{
				GLMSG::NET_INSTANCE_NOTIFY_GLOBAL_INTEGER _notifyGlobalMessage(_strGlobalName, _instanceMapID, _keyMapID, true);
				for ( ; _iterator; _iterator.Next() )
				{
					const LuaPlus::LuaObject _keyObject(_iterator.GetKey());
					const LuaPlus::LuaObject _valueObject(_iterator.GetValue());

					const unsigned int _nKey(_keyObject.GetInteger());
					if ( _notifyGlobalMessage.addGlobal(_nKey, _valueObject.GetInteger()) == false )
					{
						gpGaeaServer->SENDTOCLIENT(_nClientSlot, &_notifyGlobalMessage);
						_notifyGlobalMessage.reset();
					}
				}				
				gpGaeaServer->SENDTOCLIENT(_nClientSlot, &_notifyGlobalMessage);
			}
			break;
		case InstanceSystem::VALUE_TYPE_BOOLEAN:
			{
				GLMSG::NET_INSTANCE_NOTIFY_GLOBAL_BOOLEAN _notifyGlobalMessage(_strGlobalName, _instanceMapID, _keyMapID, true);
				for ( ; _iterator; _iterator.Next() )
				{
					const LuaPlus::LuaObject _keyObject(_iterator.GetKey());
					const LuaPlus::LuaObject _valueObject(_iterator.GetValue());

					const unsigned int _nKey(_keyObject.GetInteger());
					if ( _notifyGlobalMessage.addGlobal(_nKey, _valueObject.GetBoolean()) == false )
					{
						gpGaeaServer->SENDTOCLIENT(_nClientSlot, &_notifyGlobalMessage);
						_notifyGlobalMessage.reset();
					}					
				}
				gpGaeaServer->SENDTOCLIENT(_nClientSlot, &_notifyGlobalMessage);
			}
			break;
		case InstanceSystem::VALUE_TYPE_STRING:
			{
				GLMSG::NET_INSTANCE_NOTIFY_GLOBAL_STRING _notifyGlobalMessage(_strGlobalName, _instanceMapID, _keyMapID, true);
				for ( ; _iterator; _iterator.Next() )
				{
					const LuaPlus::LuaObject _keyObject(_iterator.GetKey());
					const LuaPlus::LuaObject _valueObject(_iterator.GetValue());

					const unsigned int _nKey(_keyObject.GetInteger());
					if ( _notifyGlobalMessage.addGlobal(_nKey, _valueObject.GetString()) == false )
					{
						gpGaeaServer->SENDTOCLIENT(_nClientSlot, &_notifyGlobalMessage);
						_notifyGlobalMessage.reset();
					}					
				}				
				gpGaeaServer->SENDTOCLIENT(_nClientSlot, &_notifyGlobalMessage);						
			}
			break;
		}			
	}
}
