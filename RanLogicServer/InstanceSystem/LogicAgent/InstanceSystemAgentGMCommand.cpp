#include "pch.h"

#include "../../../enginelib/GUInterface/UITextControl.h"
#include "../../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../Character/GLCharAG.h"
#include "../../AgentServer/GLAgentServer.h"
#include "InstanceSystemAgent.h"

#define notifyAgent(_format, ...) { \
	const std::string _notifyMessage(sc::string::format(_format, ##__VA_ARGS__)); \
	GLMSG::NET_INSTANCE_UI_CHATMSG _resultMessage(_notifyMessage.c_str(), D3DCOLOR_XRGB(239, 228, 176)); \
    msgpack::sbuffer SendBuffer; \
    msgpack::pack(SendBuffer, _resultMessage); \
    gpAgentServer->SENDTOCLIENT(_nClientSlot, NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);} \

namespace InstanceSystem
{
	const bool CALLBACK ManagerAgent::_messageGMCommand(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		GLCharAG* const _pGM(gpAgentServer->GetChar(_dwGaeaID));
		if ( _pGM == 0 )
			return false;

		if ( _pGM->UserType() < USER_GM3 )
			return false;

		const unsigned int _nClientSlot(_pGM->ClientSlot());

		const GLMSG::NET_INSTANCE_GM_COMMAND* const _pGMCommandMessage((const GLMSG::NET_INSTANCE_GM_COMMAND* const)_pMessage);
		const InstanceSystem::EMCOMMAND_TYPE _emCommand(_pGMCommandMessage->emCommandType);
		switch ( _emCommand )
		{
		case InstanceSystem::EMCOMMAND_WHO:
			{
				const InstanceMapID _instanceMapID(_pGMCommandMessage->nParameter0);
				InstanceAgentMapCIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
				if ( _iteratorInstance == this->instanceMap.end() )
				{
					notifyAgent("not exist instance(instanceMapID : %1%);", _instanceMapID);
					return false;
				}

				const InstanceAgent& _instanceAgent = _iteratorInstance->second;

				GLMSG::NET_INSTANCE_NOTIFY_PLAYER_LIST _playerListMessage(_instanceAgent.getKeyMapID(), _instanceMapID);

				const MemberAgentMap& _memberMap = _instanceAgent.getMemberMap();				
				for ( MemberAgentMapCIter _iteratorMember(_memberMap.begin()); _iteratorMember != _memberMap.end(); ++_iteratorMember )
				{
					const MemberAgent& _member(_iteratorMember->second);
					const DBNUM _dbNum(_member.getDbNum());

					GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(_dbNum));					
					const char* const _playerName(_pPlayer == 0 ? 0  : _pPlayer->GetChaName());
					const bool _bOffline(_pPlayer == 0);
					const bool _bInRequest(_member.isState(InstanceSystem::EMMEMBER_REQUEST_JOIN_FIELD));					
					if ( _playerListMessage.addPlayer(_dbNum, _playerName, _member.getAuthority()) == false )
					{
						gpAgentServer->SENDTOCLIENT(_nClientSlot, &_playerListMessage);
						_playerListMessage.reset();
					}
				}

				if ( _playerListMessage.isValid() == true )
					gpAgentServer->SENDTOCLIENT(_nClientSlot, &_playerListMessage);
			}
			break;



		case InstanceSystem::EMCOMMAND_WHERE:
			{
				const char* const _szName(_pGMCommandMessage->szParameter0);
				const DBNUM _dbNum(gpAgentServer->GetChaDbNumByChaName(_szName));
				GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(_dbNum));
				if ( _pPlayer == 0 )
				{
					notifyAgent("not exist [%1%]", _szName);
					return false;
				}

				GLAGLandMan* const _pLand(gpAgentServer->GetLand(_pPlayer->GetCurrentMap().getGaeaMapID()));
				if ( _pLand == 0 )
				{
					notifyAgent("[%1%] is in illegal location;", _szName);
					return false;
				}

				const SNATIVEID& _baseMapID(_pLand->GetBaseMapID());
				const D3DXVECTOR3& _position(_pPlayer->GetFieldMapPosition());

				MemberInstanceAgentMapCIter _iteratorMember(this->memberInstanceMap.find(_dbNum));
				if ( _iteratorMember == this->memberInstanceMap.end() )
				{
					GLMSG::NET_INSTANCE_NOTIFY_PLAYER_POSITION _notifyPositionMessage(_dbNum, _baseMapID, _position);
					::StringCchCopy(_notifyPositionMessage.targetPlayerName, CHAR_SZNAME, _szName);
					gpAgentServer->SENDTOCLIENT(_nClientSlot, &_notifyPositionMessage);
					
				}
				else
				{
					const InstanceIDSet& _instanceIDSet(_iteratorMember->second);
					const SNATIVEID _instanceMapID(WORD(_instanceIDSet.instanceMapID), WORD(_instanceIDSet.nStage));
					GLMSG::NET_INSTANCE_NOTIFY_PLAYER_POSITION _notifyPositionMessage(_dbNum, _baseMapID, _position, _instanceMapID, _instanceIDSet.keyMapID);
					::StringCchCopy(_notifyPositionMessage.targetPlayerName, CHAR_SZNAME, _szName);
					gpAgentServer->SENDTOCLIENT(_nClientSlot, &_notifyPositionMessage);
				}
			}
			break;



		case InstanceSystem::EMCOMMAND_LAND:
			{
				const InstanceMapID _instanceMapID(_pGMCommandMessage->nParameter0);
				InstanceAgentMapCIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
				if ( _iteratorInstance == this->instanceMap.end() )
				{
					notifyAgent("not exist instance(instanceMapID : %1%);", _instanceMapID);
					return false;
				}

				const InstanceAgent& _instanceAgent = _iteratorInstance->second;
				GLMSG::NET_INSTANCE_NOTIFY_CHILD_LAND _notifyChildMessage(_instanceAgent.getKeyMapID(), _instanceMapID);
				
				const unsigned int _nChildMap(_instanceAgent.getNChildInstance());
				for ( unsigned int _i(_nChildMap); _i--; )
				{
					const SNATIVEID& _childMapID(_instanceAgent.getChildMapID(_i));
					_notifyChildMessage.addChildMapID(_childMapID);
				}

				gpAgentServer->SENDTOCLIENT(_nClientSlot, &_notifyChildMessage);
			}
			break;



		case InstanceSystem::EMCOMMAND_LIST:
			{
				const unsigned int _nPage(_pGMCommandMessage->nParameter0);
				const unsigned int _nView(_pGMCommandMessage->nParameter1);
				const unsigned int _nInstanceSize(this->instanceMap.size());
				if ( _nView == 0 )
				{
					notifyAgent("not allow \"num = %1%\", amount %2%;", _nView, _nInstanceSize);
					return true;
				}

				const unsigned int _nRestPage((_nInstanceSize % _nView) == 0 ? 0 : 1);
				const unsigned int _nMaxPage((_nInstanceSize / _nView) + 1);				

				unsigned int _nElementCount(0);
				unsigned int _nPageCount(0);
				GLMSG::NET_INSTANCE_NOTIFY_INSTANCE_LIST _notifyListMessage(_nPage, _nMaxPage, _nInstanceSize);
				for ( InstanceAgentMapCIter _iteratorInstance(this->instanceMap.begin()); _iteratorInstance != this->instanceMap.end(); ++_iteratorInstance )
				{
					if ( _nPageCount == _nPage )
					{
						const InstanceAgent& _instanceAgent(_iteratorInstance->second);
						const InstanceMapID& _instanceMapID(_instanceAgent.getInstanceMapID());
						const SNATIVEID& _keyMapID(_instanceAgent.getKeyMapID());
						if ( _notifyListMessage.addInstance(InstanceList(_keyMapID, _instanceMapID)) == false )
						{
							gpAgentServer->SENDTOCLIENT(_nClientSlot, &_notifyListMessage);
							_notifyListMessage.reset();
						}
					}
					else if ( _nPageCount > _nPage )
						break;

					if ( _nElementCount == _nView )
						++_nPageCount;

					++_nElementCount;					
				}
				gpAgentServer->SENDTOCLIENT(_nClientSlot, &_notifyListMessage);
			}
			break;



		case InstanceSystem::EMCOMMAND_LOG:
			{
				const WORD _keyMapMainID(WORD(_pGMCommandMessage->nParameter0));
				const WORD _keyMapSubID(WORD(_pGMCommandMessage->nParameter1));
				const SNATIVEID _keyMapID(_keyMapMainID, _keyMapSubID);
				
				GLMSG::NET_INSTANCE_NOTIFY_INSTANCE_LOG _notifyLogMessage;
				if ( _keyMapID == SNATIVEID(false) )
				{
					for ( InstanceLogMapCIter _iteratorLog(this->instanceLogMap.begin()); _iteratorLog != this->instanceLogMap.end(); ++_iteratorLog )
					{
						const InstanceLog& _instanceLog(_iteratorLog->second);
						if ( _notifyLogMessage.addInstance(_instanceLog) == false )
						{
							gpAgentServer->SENDTOCLIENT(_nClientSlot, &_notifyLogMessage);
							_notifyLogMessage.reset();
						}
					}
					gpAgentServer->SENDTOCLIENT(_nClientSlot, &_notifyLogMessage);
				}
				else
				{
					InstanceLogMapCIter _iteratorLog(this->instanceLogMap.find(_keyMapID));
					if ( _iteratorLog == this->instanceLogMap.end() )
					{
						notifyAgent("illegal type of instance(keyMapID : %1%);", _keyMapID.wMainID, _keyMapID.wSubID);
						return true;
					}

					const InstanceLog& _instanceLog(_iteratorLog->second);
					_notifyLogMessage.addInstance(_instanceLog);
					gpAgentServer->SENDTOCLIENT(_nClientSlot, &_notifyLogMessage);
				}
			}
			break;



		case InstanceSystem::EMCOMMAND_CREATE:
			{
				const DBNUM _dbNum(_pGM->ChaDbNum());
				if ( this->memberInstanceMap.find(_dbNum) != this->memberInstanceMap.end() )
				{
					notifyAgent("can not create Instance in %1%;", "Instance");
					break;
				}
				const WORD _keyMapMainID(WORD(_pGMCommandMessage->nParameter0));
				const WORD _keyMapSubID(WORD(_pGMCommandMessage->nParameter1));
				const SNATIVEID _keyMapID(_keyMapMainID, _keyMapSubID);
				const InstanceMapID _instanceMapID(ManagerAgent::createInstance(_keyMapID));
				if ( _instanceMapID == InstanceMapID(-1) )
				{
					notifyAgent("failed to create instance(keyMapID : %1%/%2%);", _keyMapID.wMainID, _keyMapID.wSubID);
					notifyAgent("check [keyMapID], use MapID or Server %1%;", "Memory");
					break;
				}

				InstanceAgentMapIter _iteratorInstance( this->instanceMap.find( _instanceMapID ) );
				if ( _iteratorInstance == this->instanceMap.end() )
				{
					notifyAgent( "not exist instance(instanceMapID : %1%);", _instanceMapID );
					return false;
				}

				InstanceAgent& _instanceAgent = _iteratorInstance->second;

				const InformationPosition _informationPosition(
					Faction::GLActorID(CROW_PC, _dbNum),
					SNATIVEID(_instanceMapID, 0),
					Faction::GLActorID(),
					0.0f, 0.0f, InstanceSystem::EMMOVE_FIELD2INSTANCE);

				if ( ManagerAgent::reserveMember(_dbNum, _informationPosition, InstanceSystem::EMAUTHORITY_GM) == false )
				{
					notifyAgent("failed to reserve to join instance(keyMapID : %1%/%2%);", _keyMapID.wMainID, _keyMapID.wSubID);
					_instanceAgent.reserveDestroy();
					//gpAgentServer->DeleteInstantMap(_instanceMapID);
				}
			}
			break;
		case InstanceSystem::EMCOMMAND_CREATE_TEST:
			{				
				const WORD _keyMapMainID(WORD(_pGMCommandMessage->nParameter0));
				const WORD _keyMapSubID(WORD(_pGMCommandMessage->nParameter1));
				const DWORD sizeCreate(_pGMCommandMessage->nParameter2);
				const SNATIVEID _keyMapID(_keyMapMainID, _keyMapSubID);
				DWORD _count(0);
				for ( ; _count < sizeCreate; ++_count )
				{
					const InstanceMapID _instanceMapID(ManagerAgent::createInstance(_keyMapID));
					if ( _instanceMapID == InstanceMapID(-1) )
						break;
				}
				notifyAgent("create Instance(%1%/%2%) x %3%;", _keyMapID.wMainID, _keyMapID.wSubID, _count);
			}
			
			break;



		case InstanceSystem::EMCOMMAND_DESTROY:
			{
				const InstanceMapID _instanceMapID(_pGMCommandMessage->nParameter0);
				gpAgentServer->SENDTOMYSELF(&GLMSG::NET_INSTANCE_DESTROY(_instanceMapID));
				notifyAgent("reserve To Destroy instance(instanceMapID : %1%)", _instanceMapID);
			}
			break;



		case InstanceSystem::EMCOMMAND_DESTROY_TYPE:
			{
				const WORD _keyMapMainID(WORD(_pGMCommandMessage->nParameter0));
				const WORD _keyMapSubID(WORD(_pGMCommandMessage->nParameter1));
				const SNATIVEID _keyMapID(_keyMapMainID, _keyMapSubID);
				unsigned int _nSize(0);
				if ( _keyMapID == SNATIVEID(false) )
				{ // 모든 인던을 쌍그리 다 날려 버림;
					_nSize = this->instanceMap.size();
					for ( InstanceAgentMapCIter _iteratorInstance(this->instanceMap.begin()); _iteratorInstance != this->instanceMap.end(); ++_iteratorInstance )
					{
						const InstanceAgent& _instanceAgent(_iteratorInstance->second);
						const InstanceMapID _instanceMapID(_instanceAgent.getInstanceMapID());
						gpAgentServer->SENDTOMYSELF(&GLMSG::NET_INSTANCE_DESTROY(_instanceMapID));
					}
					notifyAgent("reserve To Destroy %1% instance", _nSize);
				}
				else
				{ // 특정 종류의 인던을 다 날려버림;					
					for ( InstanceAgentMapCIter _iteratorInstance(this->instanceMap.begin()); _iteratorInstance != this->instanceMap.end(); ++_iteratorInstance )
					{
						const InstanceAgent& _instanceAgent(_iteratorInstance->second);
						if ( _instanceAgent.getKeyMapID() != _keyMapID )
							continue;

						const InstanceMapID& _instanceMapID = _instanceAgent.getInstanceMapID();
						gpAgentServer->SENDTOMYSELF(&GLMSG::NET_INSTANCE_DESTROY(_instanceMapID));
						++_nSize;
					}
					notifyAgent("reserve To Destroy %1% instance(keyMapID : %2%/%3%)", _nSize, _keyMapMainID, _keyMapSubID);
				}
			}
			break;



		case InstanceSystem::EMCOMMAND_ENTER_TO_INSTANCE:
			{
				const char* const _szName(_pGMCommandMessage->szParameter0);
				const DBNUM _dbNum(gpAgentServer->GetChaDbNumByChaName(_szName));
				GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(_dbNum));
				if ( _pPlayer == 0 )
				{
					notifyAgent("not exist [%1%]", _szName);
					return false;
				}


				const InstanceSystem::EMAUTHORITY _emAuthority(_dbNum == _pGM->ChaDbNum() ? InstanceSystem::EMAUTHORITY_GM : InstanceSystem::EMAUTHORITY_NORMAL);
				
				const InstanceMapID _instanceMapID(_pGMCommandMessage->nParameter0);

				const InformationPosition _informationPosition(
					Faction::GLActorID(CROW_PC, _dbNum),
					SNATIVEID(_instanceMapID, 0),
					Faction::GLActorID(),
					0.0f, 0.0f, InstanceSystem::EMMOVE_FIELD2INSTANCE);

				const bool _bSuccess(ManagerAgent::reserveMember(_dbNum, _informationPosition, _emAuthority));
				notifyAgent("%1% to register into Instance(%2%);", _bSuccess ? "Success" : "Failed", _instanceMapID);
			}
			break;



		case InstanceSystem::EMCOMMAND_ON:
			{
				const WORD _keyMapMainID(WORD(_pGMCommandMessage->nParameter0));
				const WORD _keyMapSubID(WORD(_pGMCommandMessage->nParameter1));
				const SNATIVEID _keyMapID(_keyMapMainID, _keyMapSubID);
				const bool _bAvailable(this->pScriptManager->isState(_keyMapID, EMINSTANCE_SCRIPT_USE));
				if ( this->pScriptManager->setState(_keyMapID, EMINSTANCE_SCRIPT_USE) == true )
				{
					notifyAgent("can create instance(keyMapID : %1%/%2%) from now on;  previous state[%3%];", _keyMapMainID, _keyMapSubID, _bAvailable ? "On" : "Off");
				}				
				else
				{
					notifyAgent("can't find instanceType(keyMapID : %1%/%2%);", _keyMapMainID, _keyMapSubID);
				}
			}
			break;



		case InstanceSystem::EMCOMMAND_OFF:
			{
				const WORD _keyMapMainID(WORD(_pGMCommandMessage->nParameter0));
				const WORD _keyMapSubID(WORD(_pGMCommandMessage->nParameter1));
				const SNATIVEID _keyMapID(_keyMapMainID, _keyMapSubID);
				const bool _bAvailable(this->pScriptManager->isState(_keyMapID, EMINSTANCE_SCRIPT_USE));
				if ( this->pScriptManager->setState(_keyMapID, EMINSTANCE_SCRIPT_USE, false) == true )
				{
					notifyAgent("can't create instance(keyMapID : %1%/%2%) from now off;  previous state[%3%];", _keyMapMainID, _keyMapSubID, _bAvailable ? "On" : "Off");
				}
				else
				{
					notifyAgent("can't find instanceType(keyMapID : %1%/%2%);", _keyMapMainID, _keyMapSubID);
				}
			}
			break;


		case InstanceSystem::EMCOMMAND_JOIN_SUSPEND:
			{				
				const InstanceMapID _instanceMapID(_pGMCommandMessage->nParameter0);
				const bool _bLock(_pGMCommandMessage->nParameter1 == TRUE ? true : false);
				if ( _instanceMapID == -1 )
				{
					GLMSG::NET_INSTANCE_JOIN_SUSPEND_AF _joinLockMessage(_pGM->GetCurrentMap().getGaeaMapID().wMainID, SNATIVEID(false), _bLock);
					gpAgentServer->SENDTOMYSELF(&_joinLockMessage);

					notifyAgent("reserve %1% to Join instance(instanceMapID : %2%);", _bLock ? "suspend" : "resume", _pGM->GetCurrentMap().getGaeaMapID().wMainID);
				}
				else
				{
					GLMSG::NET_INSTANCE_JOIN_SUSPEND_AF _joinLockMessage(_instanceMapID, SNATIVEID(false), _bLock);
					gpAgentServer->SENDTOMYSELF(&_joinLockMessage);

					notifyAgent("reserve %1% to Join instance(instanceMapID : %2%);", _bLock ? "suspend" : "resume", _instanceMapID);
				}
			}
			break;



		case InstanceSystem::EMCOMMAND_JOIN_LOCK:
			{				
				const InstanceMapID _instanceMapID(_pGMCommandMessage->nParameter0);
				const bool _bLock(_pGMCommandMessage->nParameter1 == TRUE ? true : false);
				if ( _instanceMapID == -1 )
				{
					GLMSG::NET_INSTANCE_JOIN_LOCK_AF _joinLockMessage(_pGM->GetCurrentMap().getGaeaMapID().wMainID, SNATIVEID(false), _bLock);
					gpAgentServer->SENDTOMYSELF(&_joinLockMessage);

					notifyAgent("reserve %1% to Join instance(instanceMapID : %2%);", _bLock ? "lock" : "unlock", _pGM->GetCurrentMap().getGaeaMapID().wMainID);
				}
				else
				{
					GLMSG::NET_INSTANCE_JOIN_LOCK_AF _joinLockMessage(_instanceMapID, SNATIVEID(false), _bLock);
					gpAgentServer->SENDTOMYSELF(&_joinLockMessage);

					notifyAgent("reserve %1% to Join instance(instanceMapID : %2%);", _bLock ? "lock" : "unlock", _instanceMapID);
				}
			}
			break;



		case InstanceSystem::EMCOMMAND_JOIN_LOCK_TYPE:
			{				
				const unsigned int _nKeyMapMainID(_pGMCommandMessage->nParameter0);
				const unsigned int _nKeyMapSubID(_pGMCommandMessage->nParameter1);
				const SNATIVEID _keyMapID(_nKeyMapMainID, _nKeyMapSubID);
				const bool _bLock(_pGMCommandMessage->nParameter1 == TRUE ? true : false);

				GLMSG::NET_INSTANCE_JOIN_LOCK_AF _joinLockMessage(GAEAID_NULL, _keyMapID, _bLock);
				gpAgentServer->SENDTOMYSELF(&_joinLockMessage);

				notifyAgent("reserve %1% to Join instance(keyMapID : %2%/%3%);", _bLock ? "lock" : "unlock", _nKeyMapMainID, _nKeyMapSubID);
			}
			break;



		case InstanceSystem::EMCOMMAND_RELOAD:
			{
				const bool _bSuccess(ManagerAgent::reload(_dwGaeaID));
				notifyAgent("%1% to reload at Agent;", _bSuccess ? "Success" : "Failed");
				notifyAgent("wait a sec for %1%;", "Field");
			}
			break;



		case InstanceSystem::EMCOMMAND_DEBUG:
			{
                GLCharAG* _pPlayer(_pGM);
                if ( _pGMCommandMessage->nParameter0 == -1 )
                {
                    _pPlayer = gpAgentServer->GetChar(_pGMCommandMessage->szParameter0);
                    if ( _pPlayer == NULL )
                        return false;
                }

				const MapID _currentMapID(_pPlayer->GetCurrentMap());
				const MapID _previousMapID(_pPlayer->GetPreviousMap());
				const unsigned int _fieldServerID(_pPlayer->GetCurrentFieldServer());
				const unsigned int _serverChannel(_pPlayer->GetChannel());

				const SNATIVEID& _baseMapIDCurrent(_currentMapID.getBaseMapID());
				const SNATIVEID& _gaeaMapIDCurrent(_currentMapID.getGaeaMapID());				

				const SNATIVEID& _baseMapIDPrevious(_previousMapID.getBaseMapID());
				const SNATIVEID& _gaeaMapIDPrevious(_previousMapID.getGaeaMapID());	
				notifyAgent(
					"Agent_PC : CurrentMap[%1%/%2%, %3%/%4%], PreviousMap[%5%/%6%, %7%/%8%];", 
					_gaeaMapIDCurrent.Mid(), _gaeaMapIDCurrent.Sid(), _baseMapIDCurrent.Mid(), _baseMapIDCurrent.Sid(),
					_gaeaMapIDPrevious.Mid(), _gaeaMapIDPrevious.Sid(), _baseMapIDPrevious.Mid(), _baseMapIDPrevious.Sid());
				notifyAgent("Agent_PC : Field-ID[%1%], Channel[%2%];", _fieldServerID, _serverChannel);

				const DBNUM _dbNum(_pPlayer->ChaDbNum());
				MemberInstanceAgentMapCIter _iteratorMember(this->memberInstanceMap.find(_dbNum));
				if ( _iteratorMember != this->memberInstanceMap.end() )
				{
					const InstanceIDSet& _instanceIDSet(_iteratorMember->second);
					const SNATIVEID _instanceMapID(WORD(_instanceIDSet.instanceMapID), WORD(_instanceIDSet.nStage));
					notifyAgent("Agent_Instance : InstanceMap[%1%/%2%];", _instanceIDSet.instanceMapID, _instanceIDSet.nStage);
				}				
			}
			break;


		case InstanceSystem::EMCOMMAND_COUNT_PACKET:
			{
				const unsigned int _sizePacket(this->packetCounter.size());
				GLMSG::NET_INSTANCE_NOTIFY_COUNT_PACKET _notifyCountPacket;
				for ( unsigned int _index(0); _index < _sizePacket; ++_index )
				{
					const unsigned int _countPacket(this->packetCounter[_index]);
					if ( _notifyCountPacket.push_back(_index, _countPacket) == false )
					{
						gpAgentServer->SENDTOCLIENT(_nClientSlot, &_notifyCountPacket);
						_notifyCountPacket.clear();
					}
				}

				if ( _notifyCountPacket.isValid() == true )
					gpAgentServer->SENDTOCLIENT(_nClientSlot, &_notifyCountPacket);
			}
			break;
		}

		return true;
	}

}
