#include "pch.h"

#include "../../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../../RanLogic/InstanceSystem/GLInstanceSystem.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../InstantMap/InstantMapTypes.h"
#include "../../FieldServer/GLGaeaServer.h"
#include "InstanceSystemField.h"

namespace InstanceSystem
{
	const bool CALLBACK ManagerField::OnMessageProc ( GLMSG::NET_INSTANCE_MESSAGE_HEADER* _pMessage, const DWORD _dwGaeaID )
	{
		GLLandManager* pLandManager = gpGaeaServer->GetLandManager( _pMessage->nInstanceID );
		if ( NULL == pLandManager )
			return false;

		InstanceSystem::InstanceField* pInstance =
			pLandManager->GetScriptInstance();
		if ( NULL == pInstance )
			return false;

		return pInstance->OnMessageProc ( _pMessage, _dwGaeaID );
	}

	const bool CALLBACK ManagerField::_messageProcedure(NET_MSG_GENERIC* _pMessage, const DWORD _dwGaeaID)
	{ // �ڽ��� ���� �޼��� Ȥ�� Agent���� �� �޼����ΰ�� dwGaeaID�� �ǹ̰� ����;
		// ���� ���� ��� �κп� ���ؼ� ������ �����ؾ� ��;
		if ( GLUseFeatures::GetInstance().IsUsingNewInstance() == false )
			return true;

		const GLMSG::NET_INSTANCE_MESSAGE* const _pInstanceMessage((const GLMSG::NET_INSTANCE_MESSAGE* const)_pMessage);

		const InstanceSystem::EMMESSAGE_TYPE _emType(_pInstanceMessage->emType);
		switch ( _emType )
		{
		case InstanceSystem::EMMESSAGE_MOVE_MAP:
			return ManagerField::_messageMoveMap(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_CREATE:
			return ManagerField::_messageCreate(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_DESTROY:
			return ManagerField::_messageDestroy(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_JOIN:
			return ManagerField::_messageJoin(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_REQUEST_JOIN_AF:
			return ManagerField::_messageRequestJoinAF(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_REQUEST_OUT:
			return ManagerField::_messageReuqestOut(_pMessage, _dwGaeaID);
        case InstanceSystem::EMMESSAGE_SET_POSITION:
            return ManagerField::_messageSetPosition(_pMessage, _dwGaeaID);
        case InstanceSystem::EMMESSAGE_DO_MOVE_TO:
            return ManagerField::_messageDoMoveTo(_pMessage, _dwGaeaID);
        case InstanceSystem::EMMESSAGE_JOIN_LOCK_AF:
            return ManagerField::_messageJoinLockAF(_pMessage, _dwGaeaID);
        case InstanceSystem::EMMESSAGE_JOIN_LOCK_FA:
            return ManagerField::_messageJoinLockFA(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_JOIN_SUSPEND_AF:
			return ManagerField::_messageJoinSuspendAF(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_JOIN_SUSPEND_FA:
			return ManagerField::_messageJoinSuspendFA(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_GM_COMMAND:
			return ManagerField::_messageGMCommand(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_RELOAD_AF:
			return ManagerField::_messageReloadAF(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_REQ_FACTOIN_SET_FACTION:
			return ManagerField::_messageFactionsSet(_pMessage, _dwGaeaID);        
        case InstanceSystem::EMMESSAGE_CLICK_TRIGGER:
            return ManagerField::_messageClickTrigger(_pMessage, _dwGaeaID);
        case InstanceSystem::EMMESSAGE_NEW_CUSTOM_MESSAGE:
            return ManagerField::_messageNewCustomMessage(_pMessage, _dwGaeaID);
		}
		return false;
	}

	const bool CALLBACK ManagerField::_messageFactionsSet(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		if ( _dwGaeaID != GAEAID_NULL )
			return false;


		const GLMSG::NET_FACTION_RELATION_SET_FACTOIN* const _pfactionMemberMessage((const GLMSG::NET_FACTION_RELATION_SET_FACTOIN* const)_pMessage);
	

		const InstanceMapID _instanceMapID(_pfactionMemberMessage->instanceMapID);

		InstanceFieldMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
		if ( _iteratorInstance == this->instanceMap.end() )
		{
			GLMSG::NET_INSTANCE_ERROR _errorMessage(InstanceSystem::EMERROR_NOT_EXIST_INSTANCE, SNATIVEID(false), _instanceMapID);
			gpGaeaServer->SENDTOAGENT(&_errorMessage);
			return false;
		}

		InstanceField& _instanceField = _iteratorInstance->second;
		if ( _instanceField.IsReserveDestroy() )
		{
			sc::writeLogInfo ( sc::string::format(
				"[ Instance Log ] [ Destroing Instance Field : %1% ]",
				_instanceField.getInstanceMapID() ) );

			return false;
		}
		
		GLLandManager* const _pLanManager(gpGaeaServer->GetLandManager(_instanceMapID));
		if ( _pLanManager == 0 )
			return 0;

		const Faction::GLActorID _actorID(CROW_PC, _pfactionMemberMessage->dwDbNum);
		const Faction::FactionID _factionID((Faction::EMFACTION_TYPE)_pfactionMemberMessage->factiontype, _pfactionMemberMessage->dwFaction);
		_pLanManager->setFactionID(_actorID, _factionID);
		

		return true;
	}	

	const bool CALLBACK ManagerField::_messageReloadAF(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		if ( _dwGaeaID != GAEAID_NULL )
			return false;

		const GLMSG::NET_INSTANCE_RELOAD_AF* const _pReloadMessage((const GLMSG::NET_INSTANCE_RELOAD_AF* const)_pMessage);
        const unsigned int nGMGaeaID(_pReloadMessage->dwGaeaID);
		GLChar* const _pGM(gpGaeaServer->GetChar(nGMGaeaID));

		const unsigned int _nFieldServer(gpGaeaServer->GetFieldSvrID());
		const unsigned int _nFieldChannel(gpGaeaServer->GetServerChannel());
		const bool _bSuccess(ManagerField::_reload());
		GLMSG::NET_INSTANCE_RELOAD_FA _reloadMessage(_nFieldServer, _nFieldChannel, _bSuccess, nGMGaeaID);

		gpGaeaServer->SENDTOAGENT(&_reloadMessage);	

		return true;
	}

	// joinSuspend�� ��� ��ü�� �����Ŀ� ���� 
	// agent Suspend -> field Suspend / field Suspend -> agent Suspend �ΰ��� �帧�� ������ ����;
	const bool CALLBACK ManagerField::_messageJoinSuspendAF(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		if ( _dwGaeaID != GAEAID_NULL )
			return false;

		const GLMSG::NET_INSTANCE_JOIN_SUSPEND_AF* const _pJoinSuspendMessage((const GLMSG::NET_INSTANCE_JOIN_SUSPEND_AF* const)_pMessage);
		const InstanceMapID _instanceMapID(_pJoinSuspendMessage->instanceMapID);
		const SNATIVEID& _keyMapID(_pJoinSuspendMessage->keyMapID);
		const bool _bSuspend(_pJoinSuspendMessage->bSuspend);

		const bool _bResultInstanceSuspend(ManagerField::_joinSuspend(_instanceMapID, _bSuspend));
		const bool _bResultTypeSuspend(false);
		if ( (_bResultInstanceSuspend == false) && (_bResultTypeSuspend == false) )
			return false;

		return true;
	}	
	const bool CALLBACK ManagerField::_messageJoinSuspendFA(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		if ( _dwGaeaID != GAEAID_NULL )
			return false;

		const GLMSG::NET_INSTANCE_JOIN_SUSPEND_FA* const _pJoinSuspendMessage((const GLMSG::NET_INSTANCE_JOIN_SUSPEND_FA* const)_pMessage);
		const InstanceMapID _instanceMapID(_pJoinSuspendMessage->instanceMapID);
		const SNATIVEID& _keyMapID(_pJoinSuspendMessage->keyMapID);
		const bool _bSuspend(_pJoinSuspendMessage->bSuspend);

		const bool _bResultInstanceSuspend(ManagerField::_joinSuspend(_instanceMapID, _bSuspend));
		const bool _bResultTypeSuspend(false);
		if ( (_bResultInstanceSuspend == false) && (_bResultTypeSuspend == false) )
			return false;

		if ( _bResultInstanceSuspend == true )
		{
			GLMSG::NET_INSTANCE_JOIN_SUSPEND_FA _joinSuspendMessage(*_pJoinSuspendMessage);
			gpGaeaServer->SENDTOAGENT(&_joinSuspendMessage);
		}
		else if ( _bResultTypeSuspend == true )
		{
			GLMSG::NET_INSTANCE_JOIN_SUSPEND_FA _joinLockMessage(*_pJoinSuspendMessage);
			gpGaeaServer->SENDTOAGENT(&_joinLockMessage);
			return true;
		}

		return false;
	}

    // joinLock�� ��� ��ü�� �����Ŀ� ���� 
    // agent Lock -> field Lock / field Lock -> agent Lock �ΰ��� �帧�� ������ ����;
    const bool CALLBACK ManagerField::_messageJoinLockFA(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
    {
        if ( _dwGaeaID != GAEAID_NULL )
            return false;

        const GLMSG::NET_INSTANCE_JOIN_LOCK_FA* const _pJoinLockMessage((const GLMSG::NET_INSTANCE_JOIN_LOCK_FA* const)_pMessage);
        const InstanceMapID _instanceMapID(_pJoinLockMessage->instanceMapID);
		const SNATIVEID& _keyMapID(_pJoinLockMessage->keyMapID);
        const bool _bLock(_pJoinLockMessage->bLock);

        const bool _bResultInstanceLock(ManagerField::_joinLock(_instanceMapID, _bLock));
		const bool _bResultTypeLock(ManagerField::_joinLock(_keyMapID, _bLock));
		if ( (_bResultInstanceLock == false) && (_bResultTypeLock == false) )
			return false;

		if ( _bResultInstanceLock == true )
		{
			GLMSG::NET_INSTANCE_JOIN_LOCK_FA _joinLockMessage(*_pJoinLockMessage);
			gpGaeaServer->SENDTOAGENT(&_joinLockMessage);
			return true;
		}
		else if ( _bResultTypeLock == true )
		{
			GLMSG::NET_INSTANCE_JOIN_LOCK_FA _joinLockMessage(*_pJoinLockMessage);
			gpGaeaServer->SENDTOAGENT(&_joinLockMessage);
			return true;
		}

		return false;
    }
    const bool CALLBACK ManagerField::_messageJoinLockAF(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
    {
        if ( _dwGaeaID != GAEAID_NULL )
            return false;

        const GLMSG::NET_INSTANCE_JOIN_LOCK_AF* const _pJoinLockMessage((const GLMSG::NET_INSTANCE_JOIN_LOCK_AF* const)_pMessage);
        const InstanceMapID _instanceMapID(_pJoinLockMessage->instanceMapID);
		const SNATIVEID& _keyMapID(_pJoinLockMessage->keyMapID);
        const bool _bLock(_pJoinLockMessage->bLock);

        const bool _bResultInstanceLock(ManagerField::_joinLock(_instanceMapID, _bLock));
		const bool _bResultTypeLock(ManagerField::_joinLock(_keyMapID, _bLock));
		if ( (_bResultInstanceLock == false) && (_bResultTypeLock == false) )
			return false;

        return true;
    }

    const bool CALLBACK ManagerField::_messageSetPosition(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
    {
        if ( _dwGaeaID != GAEAID_NULL )
            return false;

        const GLMSG::NET_INSTANCE_SET_POSITION* const _pSetPositionMessage((const GLMSG::NET_INSTANCE_SET_POSITION* const)_pMessage);
        const InformationPosition& _setPosition(_pSetPositionMessage->informationSetPosition);
        const InstanceMapID _instanceMapID(_pSetPositionMessage->instanceMapID);
        const Faction::GLActorID& _actorID(_setPosition.actorID);
		const SNATIVEID& _targetBaseMapID(_setPosition.targetMapID);

        InstanceFieldMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
        if ( _iteratorInstance == this->instanceMap.end() )
        { // agent�� ���� �޼��� �۽�;
            GLMSG::NET_INSTANCE_SET_POSITION_FA _setPositionResult(_actorID);
            gpGaeaServer->SENDTOAGENT(&_setPositionResult);
            return false;
        }

        GLLandManager* const _pLandManager(gpGaeaServer->GetLandManager(_instanceMapID));
        if ( _pLandManager == 0 )
        { // agent�� ���� �޼��� �۽�;
            GLMSG::NET_INSTANCE_SET_POSITION_FA _setPositionResult(_actorID);
            gpGaeaServer->SENDTOAGENT(&_setPositionResult);
            return false;
        }

        InstanceField& _instanceField(_iteratorInstance->second);
        const Faction::GLActorID& _targetActorID(_setPosition.targetActorID);
        const SNATIVEID& _targetGaeaMapID(_instanceField.getGaeaMapID(_targetBaseMapID));
        if ( _targetGaeaMapID == SNATIVEID(false) )
        { // agent�� ���� �޼��� �۽�;
            GLMSG::NET_INSTANCE_SET_POSITION_FA _setPositionResult(_actorID);
            gpGaeaServer->SENDTOAGENT(&_setPositionResult);
            return false;
        }

		const MapID _targetMapID(_targetBaseMapID, _targetGaeaMapID);
        const D3DXVECTOR3 _targetPosition(_setPosition.fPositionX, 0.0f, _setPosition.fPositionZ);
        if ( _pLandManager->setPositionInstance(_actorID, _targetActorID, _targetGaeaMapID, _targetPosition, _setPosition.bResurrect) == false )
        { // agent�� ���� �޼��� �۽�;
            GLMSG::NET_INSTANCE_SET_POSITION_FA _setPositionResult(_actorID, _targetMapID);
            gpGaeaServer->SENDTOAGENT(&_setPositionResult);
            return false;
        }		

        // agent�� ���� �޼��� �۽�;		
        GLMSG::NET_INSTANCE_SET_POSITION_FA _setPositionResult(_actorID, _targetMapID, false);
        gpGaeaServer->SENDTOAGENT(&_setPositionResult);

        return true;
    }   

    const bool CALLBACK ManagerField::_messageDoMoveTo(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
    {
        if ( _dwGaeaID != GAEAID_NULL )
            return false;

        const GLMSG::NET_INSTANCE_DO_MOVE_TO* const _pMoveToMessage((const GLMSG::NET_INSTANCE_DO_MOVE_TO* const)_pMessage);
        const InstanceMapID _instanceMapID(_pMoveToMessage->instanceMapID);

        GLLandManager* const _pLandManager(gpGaeaServer->GetLandManager(_instanceMapID));
        if ( _pLandManager == 0 )
            return false;

		const Faction::GLActorID& _actorID(_pMoveToMessage->actorID);
        ServerActor* const _pActor(_pLandManager->getActor(_actorID));
		if ( _pActor == 0 )
			return false;

        D3DXVECTOR3 _targetPosition(_pMoveToMessage->targetPosition);
        const Faction::GLActorID& _targetActorID(_pMoveToMessage->targetActorID);
        if ( _targetActorID != Faction::GLActorID() )
        {
            ServerActor* const _pTargetActor(_pLandManager->getActor(_targetActorID));
            if ( _pTargetActor == 0 )
                return false;

            _targetPosition = _pTargetActor->GetPositionActor();
        }
		
		switch ( _actorID.actorType )
		{
		case CROW_MOB:
		case CROW_NPC:
			_pActor->MoveTo(&_targetPosition, false, 0);
			break;
		case CROW_PC:
			{
				GLMSG::SNETPC_GOTO _gotoMessage;
				_gotoMessage.dwGaeaID = _pActor->GetGaeaID();
				_gotoMessage.dwActState = _pActor->IsActState( EM_ACT_RUN ) ? EM_ACT_RUN : 0;
				_gotoMessage.vCurPos = _pActor->GetPositionActor();
				_gotoMessage.vTarPos = _targetPosition;
				gpGaeaServer->SENDTOMYSELF(&_gotoMessage);
				_pActor->SendToClient(&_gotoMessage);
			}
			break;
		}
        

        return true;

    }

	const bool CALLBACK ManagerField::_messageMoveMap ( const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID )
	{
		if ( _dwGaeaID != GAEAID_NULL )
			return false;

#ifdef _INSTANCE_SCRIPT_SYSTEM_LOG_
		sc::writeLogInfo ( "[ Instance Log ] [ Move Map ( _messageMoveMap ) ]" );
#endif

		const GLMSG::NET_INSTANCE_MOVE_MAP* const pMoveMsg =
			static_cast< const GLMSG::NET_INSTANCE_MOVE_MAP* const >( _pMessage );
		if ( NULL == pMoveMsg )
			return false;

        const DBNUM dwChaDBNum = pMoveMsg->dbNum;
		GLChar* pChar = gpGaeaServer->GetCharByDbNum( dwChaDBNum );
		if ( NULL == pChar )
			return false;

		GLLandMan* pLand = pChar->GetLandMan();
		if ( NULL == pLand )
			return false;

		// ��߸ʰ� �������� ���Ѵ�;
        const MapID& sFromMapID = pMoveMsg->fromMapID;
        const MapID& sToMapID = pMoveMsg->toMapID;

		/// �������� �δ��� ��� ���´�;
		// �������� �δ��̸� ���� ĳ���Ͱ� �����ʿ� �����ߴ��� �˾ƺ���;
        const InstanceMapID nToInstanceMapID = InstanceMapID( sToMapID.getGaeaMapID().wMainID );
        InstanceFieldMapIter iterToInstanceMapID = this->instanceMap.find( nToInstanceMapID );
		if ( ( iterToInstanceMapID != this->instanceMap.end() ) &&
			( pMoveMsg->bAfterMove ) )
		{
			// Instance -> Instance �̵��� ��� ó��;
			const InstanceMapID nFromInstanceMapID = InstanceMapID( sFromMapID.getGaeaMapID().wMainID );
			InstanceFieldMapIter iterFromInstanceMapID = this->instanceMap.find( nFromInstanceMapID );
			if ( iterFromInstanceMapID != this->instanceMap.end() )
			{
				// �ٸ� �δ����� �̵��� ó���Ѵ�;
				if ( sFromMapID.getGaeaMapID().Mid() != sToMapID.getGaeaMapID().Mid() )
				{
					sc::writeLogInfo( "[ Instance Log ] [ Move Other Instance Map. ]" );
				}
				// Child Map ������ �̵��� ��쿡 ó���Ѵ�;
				else
				{
					sc::writeLogInfo( "[ Instance Log ] [ Move Same Instance Map. ]" );
				}
			}
			// Field -> Instance �̵��� ��� ó��;
			else
			{
				// �δ��� �����߱� ������ ĳ���͸� �߰��Ѵ�;
				ManagerField::_addMember( dwChaDBNum, sToMapID.getGaeaMapID() );

				// �δ� ���� �̺�Ʈ �߻�;
				/*D3DXVECTOR3 vAdsjustPosition = pMoveMsg->fromMapPosition;
				InstanceField& sInstanceField = iterToInstanceMapID->second;
				if ( sInstanceField.EventJoin( dwChaDBNum, pLand, vAdsjustPosition ) )
				{
					// ����� ��ġ ������ �ִٸ� �ش��ϴ� ��ġ�� �ٲ۴�.;
					pChar->SetPosition( vAdsjustPosition );
				}*/
			}

			return true;
		}

		/// �������� �δ��� �ƴҰ�� ���´�;
		// ��߸��� �δ��̸� ������� �������� �ٸ��� ĳ���Ͱ� ���� ��߸ʿ� �ִ��� �˾ƺ���;
        const InstanceMapID nFromInstanceMapID = InstanceMapID( sFromMapID.getGaeaMapID().wMainID );
        InstanceFieldMapIter iterFromInstanceMapID = this->instanceMap.find( nFromInstanceMapID );
        if ( ( iterFromInstanceMapID != this->instanceMap.end() ) &&
			( sFromMapID != sToMapID ) &&
			( false == pMoveMsg->bAfterMove ) )
		{
			// ��߸��� �δ��̸� ���� �δ����� �������� �ϰ� �ֱ� ������ ĳ���͸� �����Ѵ�;
			ManagerField::_removeMember( dwChaDBNum );

			/*// �δ� ���� �̺�Ʈ �߻�;
			InstanceField& sInstanceField = iterFromInstanceMapID->second;
			sInstanceField.EventOut( dwChaDBNum );*/
		}

        return true;
	}
	const bool CALLBACK ManagerField::_messageCreate(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		if ( _dwGaeaID != GAEAID_NULL )
			return false; // Ŭ���̾�Ʈ���� ���Ƿ� ��Ŷ�� ���� �� �ִ�;
		// �������� ���� Ŭ���̾�Ʈ�� ��Ŷ�� ��� ó�� �ǰ� �ִ��� �ݵ�� Ȯ�� �� ��;

		const GLMSG::NET_INSTANCE_CREATE* const _pCreateMessage((const GLMSG::NET_INSTANCE_CREATE* const)_pMessage);

        const SNATIVEID& _baseMapID(_pCreateMessage->baseMapID);
		const SNATIVEID& _keyMapID(_pCreateMessage->keyMapID);
        const InstanceMapID _instanceMapID(_pCreateMessage->instanceMapID);
		const DWORD _instanceType(DWORD(_pCreateMessage->emInstanceType));

		const InstanceMapInformation _instanceMapInformation(_baseMapID, _instanceMapID, _keyMapID, _instanceType);
		gpGaeaServer->reserveCreateInstance(_instanceMapInformation);

		return true;
	}

	const bool CALLBACK ManagerField::_messageDestroy(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		if ( _dwGaeaID != GAEAID_NULL )
			return false; // Ŭ���̾�Ʈ���� ���Ƿ� ��Ŷ�� ���� �� �ִ�;
		// �������� ���� Ŭ���̾�Ʈ�� ��Ŷ�� ��� ó�� �ǰ� �ִ��� �ݵ�� Ȯ�� �� ��;

		const GLMSG::NET_INSTANCE_DESTROY* const _pDestroyMessage((const GLMSG::NET_INSTANCE_DESTROY* const)_pMessage);

		const InstanceMapID _instanceMapID(_pDestroyMessage->instanceMapID);
		InstanceFieldMapIter iter(this->instanceMap.find(_instanceMapID));
		if ( iter == this->instanceMap.end() )
			return false;	

		// �δ� �ı� ����;
		InstanceField& refInstanceField = iter->second;
		refInstanceField.ReserveDestroy();

		// �� ������ �÷��̾���� �̹� ��� ������ ���� �־�� ��;
		if ( gpGaeaServer->reserveDestroyInstance(_instanceMapID) == false )
		{ // ���� �� ��� ���� ���� �� �� _ �̷����� ����� �Ѵ�;
			ManagerField::_destroy(_instanceMapID);
			return false;
		}

		return true;
	}

	const bool CALLBACK ManagerField::_messageBegin(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		return true;
	}

	const bool CALLBACK ManagerField::_messageJoin(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		return true;
	}

	const bool CALLBACK ManagerField::_messageRequestJoinAF(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		if ( _dwGaeaID != GAEAID_NULL )
			return false;

		const GLMSG::NET_INSTANCE_REQUEST_JOIN_AF* const _pJoinMessage((const GLMSG::NET_INSTANCE_REQUEST_JOIN_AF* const)_pMessage);
		const InstanceMapID _instanceMapID(_pJoinMessage->instanceMapID);
		const SNATIVEID& _keyMapID(_pJoinMessage->keyMapID);
		const DBNUM _dbNum(_pJoinMessage->dbNum);
		const EMAUTHORITY _emAuthority(_pJoinMessage->emAuthority);

		InstanceFieldMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
		if ( _iteratorInstance == this->instanceMap.end() )
		{
			ManagerField::requestDestroy(_instanceMapID);
			return false;
		}
		
		InstanceField& _instanceField = _iteratorInstance->second;
		if ( _instanceField.getKeyMapID() != _keyMapID )
		{
			ManagerField::requestDestroy(_instanceMapID);
			return false;
		}

		const bool _bReject(_instanceField.EventRequestJoin(_dbNum, _emAuthority));

		GLMSG::NET_INSTANCE_ANSWER_JOIN_FA _answerMessage(_instanceMapID, _keyMapID, _dbNum, _bReject);		
		gpGaeaServer->SENDTOAGENT(&_answerMessage);

		return true;
	}

	const bool CALLBACK ManagerField::_messageReuqestOut(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		GLChar* _pChar(gpGaeaServer->GetChar(_dwGaeaID));
		if ( _pChar == NULL )
			return false;

		const MapID& _mapID(_pChar->GetCurrentMap());
		const InstanceMapID _instanceMapID(_mapID.getGaeaMapID().Mid());
		const DBNUM _dbNum(_pChar->CharDbNum());

		const GLMSG::NET_INSTANCE_REQUEST_OUT* const _pMessageRequestOut((const GLMSG::NET_INSTANCE_REQUEST_OUT* const)_pMessage);

		if ( _dbNum != _pMessageRequestOut->dbNum || _instanceMapID != _pMessageRequestOut->instanceMapID )
		{
			const std::string _stringErrorMessage(sc::string::format(
				"ManagerAgent::_messageReuqestOut() : Player[dbNum:%1%] in [%2%]is cracking dbNum[%3%] or instanceMapID[%4%];"
				, _dbNum, _instanceMapID, _pMessageRequestOut->dbNum, _pMessageRequestOut->instanceMapID));
			sc::writeLogError(_stringErrorMessage);
			return false;
		}

		GLMSG::NET_INSTANCE_REQUEST_OUT _messageRequestOut(_instanceMapID, _dbNum);
		gpGaeaServer->SENDTOAGENT(&_messageRequestOut);

		return true;
	}

    const bool CALLBACK ManagerField::_messageClickTrigger(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
    {
        GLChar* pChar(gpGaeaServer->GetChar(_dwGaeaID));
        if ( pChar == NULL )
            return false;

		const MapID& _mapID(pChar->GetCurrentMap());
		const SNATIVEID _gaeaMapID(_mapID.getGaeaMapID());
		GLLandMan* const _pLand(gpGaeaServer->GetLand(_gaeaMapID));
        if ( _pLand == NULL )
            return false;

        const GLMSG::NET_INSTANCE_CLICK_TRIGGER* const NetMsg((const GLMSG::NET_INSTANCE_CLICK_TRIGGER* const)_pMessage);
        GLCrow* pCrow = (_pLand->GetCrow(NetMsg->sTarget.GaeaId));
        if ( pCrow == NULL )
            return false;

        if ( pCrow->m_pCrowData->m_emNPCType != NPC_TYPE_CLICK_TRIGGER )
            return false;

        InstanceSystem::InstanceField* const pScript(_pLand->GetScriptInstance());
        if ( pScript == NULL )
            return false;

        pScript->EventClickTrigger(pCrow->GetCrow(),pCrow->GetCharID(), CROW_PC, pChar->GetCharID());

        return true;
    }

    const bool CALLBACK ManagerField::_messageNewCustomMessage(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
    {
        GLMSG::NET_INSTANCE_NEW_CUSTOM_MESSAGE* NetMsg((GLMSG::NET_INSTANCE_NEW_CUSTOM_MESSAGE* )_pMessage);

        if (( NetMsg->emSrcType != InstanceSystem::EM_MESSAGE_USER ) && ( _dwGaeaID != GAEAID_NULL ))
        {
            // ���� ������ ���� �� ��Ŷ�� �ƴ϶�� _dwGaeaID�� GAEAID_NULL�̿��� �Ѵ�. ( ��Ŷ���� �ǽ� );
            return false;
        }

        switch ( NetMsg->emDestType ) 
        {
        //case InstanceSystem::EM_MESSAGE_USER:           // Ư�� ������ �������� �ʵ� ������ �� ���� ����. Agent������ �̿�, ����(Field)�� ��Ŷ�� ���� ���� ����.;
            //break;
        case InstanceSystem::EM_MESSAGE_MY_INSTANCE:    // ���� �ڽ��� ���� LevelField ��ü;
            {
                switch ( NetMsg->emSrcType )
                {
                case InstanceSystem::EM_MESSAGE_USER:
                    {
                        GLChar* _pChar = gpGaeaServer->GetCharByDbNum(NetMsg->dwSrcID);
                        if ( _pChar == NULL )
                            return false;

                        GLLandMan* _pLand = _pChar->GetLandMan();
                        if ( _pLand == NULL )
                            return false;

                        InstanceField* _pInstanceField = _pLand->GetScriptInstance();
						if ( _pInstanceField == NULL )
							return false;

                        _pInstanceField->EventCustomMessage((EM_CUSTOM_MESSAGE_TYPE)NetMsg->emSrcType, NetMsg->dwSrcID, NetMsg->param1, NetMsg->param2, NetMsg->param3, NetMsg->param4);
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        case InstanceSystem::EM_MESSAGE_FIELD_BASE:     // Ư�� �������� PublicField;
            {
                sc::LuaInstance* const _pLuaInstance(InstanceSystem::getScript(NetMsg->dwDestID));
                if ( _pLuaInstance == 0 )
                    return false;

                if ( _pLuaInstance->IsExist("luaFieldEventCustomMessage") == true )
                {   
                    _pLuaInstance->PushFunction("luaFieldEventCustomMessage");
                    _pLuaInstance->PushInteger(NetMsg->emSrcType);
                    _pLuaInstance->PushInteger(NetMsg->dwSrcID);
                    _pLuaInstance->PushInteger(NetMsg->param1);
                    _pLuaInstance->PushInteger(NetMsg->param2);
                    _pLuaInstance->PushInteger(NetMsg->param3);
                    _pLuaInstance->PushInteger(NetMsg->param4);
                    _pLuaInstance->CallFunction(6, 0);
                }
            }
            break;
        case InstanceSystem::EM_MESSAGE_FIELD_INSTANCE: // Ư�� �ʵ弭���� LevelField��ü. �������۽ô� �ٸ������� ������ �����Ƿ� Agent������ �̿�, ����(Field)�� ��Ŷ�� ���� ���� �� �ʵ弭���� �ش� �δ���ü�� �ִٴ� ��;
            {
                InstanceFieldMapIter _iterator = this->instanceMap.find(NetMsg->dwDestID);
                if ( _iterator == this->instanceMap.end() )
                    return false;

                InstanceField& _instanceField = _iterator->second;
                _instanceField.EventCustomMessage((EM_CUSTOM_MESSAGE_TYPE)NetMsg->emSrcType, NetMsg->dwSrcID, NetMsg->param1, NetMsg->param2, NetMsg->param3, NetMsg->param4);
                return true;
            }
            break;
        //case InstanceSystem::EM_MESSAGE_AGENT_BASE:     // Agent���̹Ƿ� ����(Field)�� ��Ŷ�� ���� ���� ����.;
            //break;
        //case InstanceSystem::EM_MESSAGE_AGENT_INSTANCE: // Agent���̹Ƿ� ����(Field)�� ��Ŷ�� ���� ���� ����.;
            //break;
        default:
            return false;
        }
        return true;
    }
}
