#include "pch.h"

#include "../../../enginelib/GUInterface/UITextControl.h"
#include "../../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../AgentServer/GLAgentServer.h"
#include "../../Character/GLCharAG.h"
#include "../InstanceSystem.h"
#include "InstanceSystemAgent.h"

namespace InstanceSystem
{
	/*
	 * ��Ī �ý����� ����� ���� ��ɵ��� ��� ��Ī �ý��������� �̰� ��;
	*/
	const bool ManagerAgent::messageProcedure(NET_MSG_GENERIC* _pMessage, const DWORD _dwGaeaID)
	{
		if ( GLUseFeatures::GetInstance().IsUsingNewInstance() == false )
			return true;

		GLMSG::NET_INSTANCE_MESSAGE* _pInstanceMessage((GLMSG::NET_INSTANCE_MESSAGE*)_pMessage);	

		const InstanceSystem::EMMESSAGE_TYPE _emType(_pInstanceMessage->emType);

		switch ( _emType )
		{
		case InstanceSystem::EMMESSAGE_ERROR:
			return ManagerAgent::_messageError(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_RELOAD_FA:
			return ManagerAgent::_messageReloadFA(_pMessage, _dwGaeaID);
        case InstanceSystem::EMMESSAGE_GM_COMMAND:
            return ManagerAgent::_messageGMCommand(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_MOVE_MAP:
			return ManagerAgent::_messageMoveMap(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_CREATE_COMPLETE:
			return ManagerAgent::_messageCreateComplete(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_DESTROY:
			return ManagerAgent::_messageDestroy(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_ANSWER_JOIN_FA:
			return ManagerAgent::_messageAnswerJoinFA(_pMessage, _dwGaeaID);
        case InstanceSystem::EMMESSAGE_SET_POSITION:
            return ManagerAgent::_messageSetPositionInstance(_pMessage, _dwGaeaID);
        case InstanceSystem::EMMESSAGE_SET_POSITION_FA:
            return ManagerAgent::_messageSetPositionInstanceFA(_pMessage, _dwGaeaID);
        case InstanceSystem::EMMESSAGE_JOIN_LOCK_AF:
            return ManagerAgent::_messageJoinLockAF(_pMessage, _dwGaeaID);
        case InstanceSystem::EMMESSAGE_JOIN_LOCK_FA:
            return ManagerAgent::_messageJoinLockFA(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_JOIN_SUSPEND_FA:
			return ManagerAgent::_messageJoinSuspendFA(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_REQUEST_OUT:
			return ManagerAgent::_messageRequestOut(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_REQUEST_JOIN:
			return ManagerAgent::_messageRequestJoin(_pMessage, _dwGaeaID);
        case InstanceSystem::EMMESSAGE_NEW_CUSTOM_MESSAGE:
            return ManagerAgent::_messageNewCustomMessage(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_MOVE_DONE:
			return ManagerAgent::_messageMoveDone(_pMessage, _dwGaeaID);
        case InstanceSystem::EMMESSAGE_JOIN_PARTY_FA:
            return ManagerAgent::_messageJoinParty(_pMessage, _dwGaeaID);
        case InstanceSystem::EMMESSAGE_OUT_PARTY_FA:
            return ManagerAgent::_messageOutParty(_pMessage, _dwGaeaID);
        case InstanceSystem::EMMESSAGE_CLEAR_PARTY_FA:
            return ManagerAgent::_messageClearParty(_pMessage, _dwGaeaID);
		case InstanceSystem::EMMESSAGE_REQUEST_OBSERVE:
			return ManagerAgent::_messageRequestObserve(_pMessage, _dwGaeaID);
		}

		return false;
	}	

    const bool ManagerAgent::messageProcedureMsgPack(NET_MSG_GENERIC* _pMessage, const DWORD _dwGaeaID)
    {
        if ( GLUseFeatures::GetInstance().IsUsingNewInstance() == false )
            return true;

        //GLMSG::NET_INSTANCE_MESSAGE* _pInstanceMessage((GLMSG::NET_INSTANCE_MESSAGE*)_pMessage);	
        GLMSG::NET_INSTANCE_MSGPACK_MESSAGE _InstanceMessage;

        if ( CastNetMsg(_pMessage, _InstanceMessage) == false )
            return false;

        const InstanceSystem::EMMESSAGE_TYPE _emType((InstanceSystem::EMMESSAGE_TYPE)_InstanceMessage.emType);
        switch ( _emType )
        {
        case InstanceSystem::EMMESSAGE_UI_TIMER_MSGBOX:
            return ManagerAgent::_messageUITimerMsgBox(_pMessage, _dwGaeaID);
        }
        return false;
    }		

	const bool CALLBACK ManagerAgent::_messageRequestOut(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		if ( _dwGaeaID != GAEAID_NULL )
			return false;

		const GLMSG::NET_INSTANCE_REQUEST_OUT* const _pOutMessage((const GLMSG::NET_INSTANCE_REQUEST_OUT* const)_pMessage);

		const InstanceMapID _instanceMapID(_pOutMessage->instanceMapID);
		const DBNUM _dbNum(_pOutMessage->dbNum);

		InstanceAgentMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
		if ( _iteratorInstance == this->instanceMap.end() )
			return false;

		InstanceAgent& _instanceAgent = _iteratorInstance->second;
		return _instanceAgent.removeMember(_dbNum);
	}
	const bool CALLBACK ManagerAgent::_messageRequestJoin(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		if ( _dwGaeaID != GAEAID_NULL )
			return false;

		const GLMSG::NET_INSTANCE_REQUEST_JOIN* const _pJoinMessage((const GLMSG::NET_INSTANCE_REQUEST_JOIN* const)_pMessage);
		
		const InformationPosition& informationSetPosition = _pJoinMessage->informationSetPosition;

		const DBNUM _dbNum(informationSetPosition.actorID.actorID_Num);
		const InstanceSystem::EMAUTHORITY _emAuthority(_pJoinMessage->emAuthority);		

		const bool _bResult(ManagerAgent::reserveMember(_dbNum, informationSetPosition, _emAuthority));

		return _bResult;
	}

	const bool CALLBACK ManagerAgent::_messageReloadFA(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		if ( _dwGaeaID != GAEAID_NULL )
			return false;
		
		const GLMSG::NET_INSTANCE_RELOAD_FA* const _pReloadResultMessage((GLMSG::NET_INSTANCE_RELOAD_FA* const)_pMessage);

		const unsigned int _nFieldServer(_pReloadResultMessage->nFieldServer);
		const unsigned int _nFieldChannel(_pReloadResultMessage->nFieldChannel);
		const bool _bSuccess(_pReloadResultMessage->bSuccess);

		const InstanceSystem::ReloadState _reloadState(_nFieldServer, _nFieldChannel, _bSuccess);
		this->reloadStateVector.push_back(_reloadState);

		if ( this->nReloadServerGroup == this->reloadStateVector.size() )
		{
			this->pScriptManager->setState(InstanceSystem::EMINSTANCE_SCRIPT_RELOAD, false);

			GLCharAG* const _pGM(gpAgentServer->GetChar(_pReloadResultMessage->dwGaeaID));
			if ( _pGM != 0 )
			{
				const unsigned int _nClientSlot(_pGM->ClientSlot());
				for ( ReloadStateVectorCIter _iteratorState(this->reloadStateVector.begin()); _iteratorState != this->reloadStateVector.end(); ++_iteratorState )
				{
					const InstanceSystem::ReloadState& _reloadState(*_iteratorState);

					const std::string _notifyMessage(sc::string::format("%1% Channel, %2% Field reload - %3%;", _reloadState.nFieldChannel, _reloadState.nFieldServer, _reloadState.bSuccess == true ? "OK" : "Fail"));
                    GLMSG::NET_INSTANCE_UI_CHATMSG _resultMessage(_notifyMessage.c_str(), NS_UITEXTCOLOR::RED);
                    msgpack::sbuffer SendBuffer;
                    msgpack::pack(SendBuffer, _resultMessage);
					gpAgentServer->SENDTOCLIENT(_nClientSlot, NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);
				}
			}
		}
        return true;
	}

	const bool CALLBACK ManagerAgent::_messageJoinSuspendFA(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		if ( _dwGaeaID != GAEAID_NULL )
			return false;

		const GLMSG::NET_INSTANCE_JOIN_SUSPEND_FA* const _pJoinSuspendMessage((const GLMSG::NET_INSTANCE_JOIN_SUSPEND_FA* const)_pMessage);
		const InstanceMapID _instanceMapID(_pJoinSuspendMessage->instanceMapID);
		const SNATIVEID& _keyMapID(_pJoinSuspendMessage->keyMapID);
		const bool _bSuspend(_pJoinSuspendMessage->bSuspend);

		const bool _bResultInstanceSuspend(ManagerAgent::_joinSuspend(_instanceMapID, _bSuspend));
		const bool _bResultTypeSuspend(false);
		if ( (_bResultInstanceSuspend == false) && (_bResultTypeSuspend == false) )
			return false;

		return true;
	}

	const bool CALLBACK ManagerAgent::_messageJoinSuspendAF(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		if ( _dwGaeaID != GAEAID_NULL )
			return false;

		const GLMSG::NET_INSTANCE_JOIN_SUSPEND_AF* const _pJoinSuspendMessage((const GLMSG::NET_INSTANCE_JOIN_SUSPEND_AF* const)_pMessage);
		const InstanceMapID& _instanceMapID(_pJoinSuspendMessage->instanceMapID);
		const SNATIVEID& _keyMapID(_pJoinSuspendMessage->keyMapID);
		const bool _bSuspend(_pJoinSuspendMessage->bSuspend);

		const bool _bResultInstanceSuspend(ManagerAgent::_joinSuspend(_instanceMapID, _bSuspend));
		const bool _bResultTypeSuspend(false);
		if ( (_bResultInstanceSuspend == false) && (_bResultTypeSuspend == false) )
			return false;

		if ( _bResultInstanceSuspend == true )
		{
			InstanceAgentMapIter& _iteratorInstance(this->instanceMap.find(_instanceMapID));
			InstanceAgent& _instanceAgent = _iteratorInstance->second;
			const unsigned int _nFieldChannel(_instanceAgent.getFieldChannel());
			const unsigned int _nFieldServer(_instanceAgent.getFieldServer());
			GLMSG::NET_INSTANCE_JOIN_SUSPEND_AF _joinSuspendMessage(*_pJoinSuspendMessage);
			gpAgentServer->SENDTOFIELDSVR(_nFieldChannel, _nFieldServer, &_joinSuspendMessage);
			return true;
		}
		else if ( _bResultTypeSuspend == true )
		{
			GLMSG::NET_INSTANCE_JOIN_SUSPEND_AF _joinSuspendMessage(*_pJoinSuspendMessage);
			gpAgentServer->SENDTOALLCHANNEL(&_joinSuspendMessage);
			return true;
		}

		return false;

	}

    const bool CALLBACK ManagerAgent::_messageJoinLockFA(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
    {
        if ( _dwGaeaID != GAEAID_NULL )
            return false;

        const GLMSG::NET_INSTANCE_JOIN_LOCK_FA* const _pJoinLockMessage((const GLMSG::NET_INSTANCE_JOIN_LOCK_FA*)_pMessage);
        const InstanceMapID _instanceMapID(_pJoinLockMessage->instanceMapID);
		const SNATIVEID& _keyMapID(_pJoinLockMessage->keyMapID);
        const bool _bLock(_pJoinLockMessage->bLock);

        const bool _bResultInstanceLock(ManagerAgent::_joinLock(_instanceMapID, _bLock));
		const bool _bResultTypeLock(ManagerAgent::_joinLock(_keyMapID, _bLock));
		if ( (_bResultInstanceLock == false) && (_bResultTypeLock == false) )
			return false;

        return true;
    }	

    const bool CALLBACK ManagerAgent::_messageJoinLockAF(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
    {
        if ( _dwGaeaID != GAEAID_NULL )
            return false;

        const GLMSG::NET_INSTANCE_JOIN_LOCK_AF* const _pJoinLockMessage((const GLMSG::NET_INSTANCE_JOIN_LOCK_AF* const)_pMessage);
        const InstanceMapID _instanceMapID(_pJoinLockMessage->instanceMapID);
		const SNATIVEID& _keyMapID(_pJoinLockMessage->keyMapID);
        const bool _bLock(_pJoinLockMessage->bLock);

        const bool _bResultInstanceLock(ManagerAgent::_joinLock(_instanceMapID, _bLock));
		const bool _bResultTypeLock(ManagerAgent::_joinLock(_keyMapID, _bLock));		
		
		if ( _bResultInstanceLock == true )
		{
			InstanceAgentMapIter& _iteratorInstance(this->instanceMap.find(_instanceMapID));
			InstanceAgent& _instanceAgent = _iteratorInstance->second;
			const unsigned int _nFieldChannel(_instanceAgent.getFieldChannel());
			const unsigned int _nFieldServer(_instanceAgent.getFieldServer());
			GLMSG::NET_INSTANCE_JOIN_LOCK_AF _joinLockMessage(*_pJoinLockMessage);
			gpAgentServer->SENDTOFIELDSVR(_nFieldChannel, _nFieldServer, &_joinLockMessage);
			return true;
		}
		else if ( _bResultTypeLock == true )
		{
			GLMSG::NET_INSTANCE_JOIN_LOCK_AF _joinLockMessage(*_pJoinLockMessage);
			gpAgentServer->SENDTOALLCHANNEL(&_joinLockMessage);
			return true;
		}

        return false;
    }


    const bool CALLBACK ManagerAgent::_messageSetPositionInstanceFA(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
    {
        if ( _dwGaeaID != GAEAID_NULL )
            return false;

        const GLMSG::NET_INSTANCE_SET_POSITION_FA* const _pResultMessage((const GLMSG::NET_INSTANCE_SET_POSITION_FA* const)_pMessage);		
        const Faction::GLActorID& _actorID(_pResultMessage->actorID);
        const MapID& _targetMapID(_pResultMessage->targetMapID);
        const bool _bFail(_pResultMessage->bFail);

		if ( _actorID.actorType != CROW_PC )
		{ // ����ν� PC�ܿ��� �̵��� �Ұ��ϴ�;
			return false;
		}

		const SNATIVEID _gaeaMapID(_targetMapID.getGaeaMapID());
        InstanceAgentMapIter _iteratorInstance(this->instanceMap.find(_gaeaMapID.Mid()));
        if ( _iteratorInstance == this->instanceMap.end() )
            return false;

		const DBNUM _dbNum(_actorID.actorID_Num);
		if ( _bFail == true )
		{   
			// � ���������� �̵��� ���� �� ��� �δ��� ���� �Ѵ�;
			// ����ν� ���¶����� ���� ���� ���� ��Ȳ�� ���� �� �� �ִ� ����� ����;
			// Ư���� �� �� �̵��� �����ؼ��� ������;
			sc::writeLogFatal("Instance Force Destroyed, it Cannot be make fix");
			GLMSG::NET_INSTANCE_DESTROY _destroyMessage(_gaeaMapID.Mid());
			gpAgentServer->SENDTOMYSELF(&_destroyMessage);
			return true;
		}

		GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(_dbNum));
		if ( _pPlayer == 0 )
		{
			ManagerAgent::_removeMember(_dbNum);
			return true;
		}		

		MemberInstanceAgentMapIter _iteratorMember(this->memberInstanceMap.find(_dbNum));
		if ( _iteratorMember != this->memberInstanceMap.end() )
		{
			InstanceIDSet& _instanceIDSet(_iteratorMember->second);
			_instanceIDSet.nStage = _gaeaMapID.Sid();
			_pPlayer->SetCurrentMap(_targetMapID);
		}
		else
			ManagerAgent::_removeMember(_dbNum);

        return true;
    }

    const bool CALLBACK ManagerAgent::_messageSetPositionInstance(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
    {
        if ( _dwGaeaID != GAEAID_NULL )
            return false;
        // _targetMapID ������ �δ� �ٱ��������� ���� �̵����� ���� �ϱ⵵ ����Ӵ���,
        // ���� �̵��� ��� �������� land�� ���� �� ��� �̵��� ���� �� ����,
        // ó�� �帧�� �δ� �ٱ������� �̵��� ���� ���� �޶� �޼����� ���� ����;

        const GLMSG::NET_INSTANCE_SET_POSITION* const _pSetPositionMessage((const GLMSG::NET_INSTANCE_SET_POSITION* const)_pMessage);

        const InformationPosition& _reserveSetPosition(_pSetPositionMessage->informationSetPosition);
        const InstanceMapID _instanceMapID(_pSetPositionMessage->instanceMapID);
        const SNATIVEID& _targetMapID(_reserveSetPosition.targetMapID);
		const Faction::GLActorID& _actorID(_reserveSetPosition.actorID);
		if ( _actorID.actorType != CROW_PC )
			return false;
		
		const DBNUM _dbNum(_actorID.actorID_Num);

        InstanceAgentMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
        if ( _iteratorInstance == this->instanceMap.end() )
        { // �� ���������� field Num/channel �ƹ��͵� �� �� �����Ƿ� ��� field channel��,
            // �ش� instanceMapID�� ���� �δ��� �ı��϶�� �޽����� ������;
            gpAgentServer->SENDTOALLCHANNEL(&GLMSG::NET_INSTANCE_DESTROY(_instanceMapID));
            return false;
        }
		
        InstanceAgent& _instanceAgent(_iteratorInstance->second);
		_instanceAgent.setPositionMember(_dbNum, _reserveSetPosition);
        
        return true;
    }

	const bool CALLBACK ManagerAgent::_messageMoveMap(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
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
		GLCharAG* pChar = gpAgentServer->GetCharByDbNum( dwChaDBNum );
		if ( NULL == pChar )
			return false;

		MapID sCurrentMapID = pChar->GetCurrentMap();

		// ��߸ʰ� �������� ���Ѵ�;
		const MapID& sFromMapID = pMoveMsg->fromMapID;
		const MapID& sToMapID = pMoveMsg->toMapID;

		/// �������� �δ��� ��� ���´�;
		// �������� �δ��̸� ���� ĳ���Ͱ� �����ʿ� �����ߴ��� �˾ƺ���;
		const InstanceMapID nToInstanceMapID = InstanceMapID( sToMapID.getGaeaMapID().wMainID );
		InstanceAgentMapIter iterToInstanceMapID = this->instanceMap.find( nToInstanceMapID );
		if ( iterToInstanceMapID != this->instanceMap.end() )
		{
			// ���� Instnace Map �� ���Ѵ�;
			InstanceAgent& sToInstanceAgent = iterToInstanceMapID->second;

			// Instance -> Instance �̵��� ��� ó��;
			const InstanceMapID nFromInstanceMapID = InstanceMapID( sFromMapID.getGaeaMapID().wMainID );
			InstanceAgentMapIter iterFromInstanceMapID = this->instanceMap.find( nFromInstanceMapID );
			if ( iterFromInstanceMapID != this->instanceMap.end() )
			{
				// �ٸ� �δ����� �̵��� ó���Ѵ�;
				if ( sFromMapID.getGaeaMapID().Mid() != sToMapID.getGaeaMapID().Mid() )
				{
					sc::writeLogInfo( "[ Instance Log ] [ Move Other Instance Map. ]" );

					InstanceAgent& sFromInstanceAgent = iterFromInstanceMapID->second;

					const MapInformation* const pMapInformation =
						sFromInstanceAgent.getPreviousMapPosition( dwChaDBNum );

					MapInformation sPreviousMapInformation;
					if ( NULL == pMapInformation )
					{
						const SNATIVEID sStartMapID = pChar->m_sStartMapID;
						const DWORD dwStartGateID = pChar->m_dwStartGate;
						const D3DXVECTOR3& vStartPosition = pChar->m_vStartPos;
						sPreviousMapInformation = MapInformation(
							sStartMapID,
							vStartPosition,
							Faction::GLActorID( CROW_GATE, dwStartGateID ) );
					}
					else
						sPreviousMapInformation = *pMapInformation;

					// ����� �����;
					ManagerAgent::_removeMember( dwChaDBNum );

					if ( sToInstanceAgent.setPreviousMapPosition(
						dwChaDBNum, sPreviousMapInformation ) == true )
					{
						this->memberInstanceMap.insert(
							MemberInstanceAgentValue( dwChaDBNum,
							InstanceIDSet( sToInstanceAgent.getKeyMapID(), sToMapID.getGaeaMapID() ) ) );
					}

				}
				// Child Map ������ �̵��� ��쿡 ó���Ѵ�;
				else
				{
					/// �ƹ��� ó���� ���� �ʴ´�;
					/// ���Ŀ� �ʿ������� ó������;
					sc::writeLogInfo( "[ Instance Log ] [ Move Same Instance Map. ]" );
				}
			}
			// Field -> Instance �̵��� ��� ó��;
			else
			{
				// �δ��� �����߱� ������ ĳ���͸� �߰��Ѵ�;
				if ( sToInstanceAgent.setPreviousMapPosition(
					dwChaDBNum,
					MapInformation( sFromMapID.getGaeaMapID(), pMoveMsg->fromMapPosition, Faction::GLActorID() ) )
					== true )
				{
					this->memberInstanceMap.insert(
						MemberInstanceAgentValue( dwChaDBNum,
						InstanceIDSet( sToInstanceAgent.getKeyMapID(), sToMapID.getGaeaMapID() ) ) );
				}
			}

			return true;
		}

		/// �������� �δ��� �ƴҰ�� ���´�;
		// ��߸��� �δ��̸� ������� �������� �ٸ��� ĳ���Ͱ� ���� ��߸ʿ� �ִ��� �˾ƺ���;
		const InstanceMapID nFromInstanceMapID = InstanceMapID( sFromMapID.getGaeaMapID().wMainID );
		InstanceAgentMapIter iterFromInstanceMapID = this->instanceMap.find( nFromInstanceMapID );
		if ( ( iterFromInstanceMapID != this->instanceMap.end() ) &&
			( sFromMapID != sToMapID ) )
		{
			// ��߸��� �δ��̸� ���� �δ����� �������� �ϰ� �ֱ� ������ ĳ���͸� �����Ѵ�;
			ManagerAgent::_removeMember( dwChaDBNum );
		}

		return true;

		/*// ���� �� Ȥ�� �̵� �� ���� �δ����� ���ε��� ó�� �� �־�� ��;
        const DBNUM _dbNum(pMoveMsg->dbNum);
		GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(_dbNum));
		if ( _pPlayer == 0 )
			return false;

        const SNATIVEID& _mapIDFrom(pMoveMsg->fromMapID.getGaeaMapID());
        const SNATIVEID& _mapIDTo(pMoveMsg->toMapID.getGaeaMapID());

        InstanceAgentMapIter _iteratorInstanceFrom(this->instanceMap.find(_mapIDFrom.wMainID));		
        const bool _bInstanceFromMap(_iteratorInstanceFrom != this->instanceMap.end());

        InstanceAgentMapIter _iteratorInstanceTo(this->instanceMap.find(_mapIDTo.wMainID));
        const bool _bInstanceTo(_iteratorInstanceTo != this->instanceMap.end());
        if ( _bInstanceTo == true )
        { // �������� �δ��� ���;
            InstanceAgent& _instanceToAgent(_iteratorInstanceTo->second);

			// �δ����� �δ������� �̵��� ��� ���� �� ������ ���� �� �´�;
			// �� �δ��� �ٸ� �δ��� ��쿡�� ó���Ѵ�;
			// Child Map ������ �̵��� ��쿡�� ���� �������� �ʱ� ������ ó���� �ʿ䰡 ����;
			// ���� ������ �� �ֵ��� �ϴ°��� ������ ������ ���߿� �Ѵ�;
			if ( ( _bInstanceFromMap == true ) &&
				( _mapIDFrom.Mid() != _mapIDTo.Mid() ) )
			{
				InstanceAgent& _instanceFromAgent(_iteratorInstanceFrom->second);
				const MapInformation* const _pMapInformation(_instanceFromAgent.getPreviousMapPosition(_dbNum));
				MapInformation _previousMapInformation;
				if ( _pMapInformation == 0 )
				{
					const SNATIVEID _returnMapID(_pPlayer->m_sStartMapID);
					const DWORD _returnGateID(_pPlayer->m_dwStartGate);
					const D3DXVECTOR3& _returnPosition(_pPlayer->m_vStartPos);
					const unsigned int _nFieldServer(gpAgentServer->GetFieldServer(_returnMapID));
					_previousMapInformation = MapInformation(_returnMapID, _returnPosition, Faction::GLActorID(CROW_GATE, _returnGateID), _nFieldServer);
				}
				else
					_previousMapInformation = *_pMapInformation;

				// ����� �����;
				ManagerAgent::_removeMember(_dbNum);

				if ( _instanceToAgent.setPreviousMapPosition(_dbNum, _previousMapInformation) == true )
					this->memberInstanceMap.insert(MemberInstanceAgentValue(_dbNum, 	InstanceIDSet(_instanceToAgent.getKeyMapID(), _mapIDTo.wMainID)));
			}
			else
			{ // �ʵ� -> �δ������� �̵��� ��� ������ ��ġ�� ���� �Ѵ�;
				const D3DXVECTOR3& _previousPosition(pMoveMsg->fromMapPosition);
				const unsigned int _previousFieldServer(pMoveMsg->fromFieldServer);

				if ( _instanceToAgent.setPreviousMapPosition(_dbNum, MapInformation(_mapIDFrom, _previousPosition, Faction::GLActorID(), _previousFieldServer)) == true )
					this->memberInstanceMap.insert(MemberInstanceAgentValue(_dbNum, 	InstanceIDSet(_instanceToAgent.getKeyMapID(), _mapIDTo)));
			}
				
        }
		else
			ManagerAgent::_removeMember(_dbNum);

        enum
        {
            DO_JOIN = 0x1,
            DO_OUT = 0x2,
            DO_ENTER = DO_JOIN + DO_OUT, // = 0x3;
        };
        const unsigned int _nType((_bInstanceTo ? DO_JOIN : 0) + (_bInstanceFromMap ? DO_OUT : 0));

        switch ( _nType )
        {
        case DO_ENTER:
            { // instance to instance;
                // todo : ��Ī��ũ��Ʈ EventEnter;
            }
            break;
        case DO_JOIN:
            { // field to instance;
                // todo : ��Ī��ũ��Ʈ EventJoin;
            }
            break;
        case DO_OUT:
            { // instance to field;
                // todo : ��Ī��ũ��Ʈ EventOut;
            }
            break;
        }

		return true;*/
	}
	

	const bool CALLBACK ManagerAgent::_messageCreateComplete(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		if ( _dwGaeaID != GAEAID_NULL )
			return false; // field server�κ��� �� �޼������ GAEAID_NULL �̴�;

		const GLMSG::NET_INSTANCE_CREATE_COMPLETE* const _pCompleteMessage((const GLMSG::NET_INSTANCE_CREATE_COMPLETE* const)_pMessage);
		const InstanceMapID _instanceMapID(_pCompleteMessage->instanceMapID);
		const bool _bCreateComplete(_pCompleteMessage->bSuccess);

		InstanceAgentMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
		if ( _iteratorInstance == this->instanceMap.end() )
		{
			GLMSG::NET_INSTANCE_DESTROY _destroyMessage(_instanceMapID);
			gpAgentServer->SENDTOALLCHANNEL(&_destroyMessage);
			return false;
		}		

		InstanceAgent& _instanceAgent(_iteratorInstance->second);
		const SNATIVEID& _keyMapID(_instanceAgent.getKeyMapID());

		_instanceAgent.createComplete();

		//�����ص� �˱Ǹ��� �ִ�.
		CallTrigger(_keyMapID, _instanceMapID, _bCreateComplete);

		

		if ( _bCreateComplete == false )
		{ // field server ������ ������ ���� �� ��� agent server ���� �̸� ����;
			GLMSG::NET_INSTANCE_DESTROY _destroyMessage(_instanceMapID);
			gpAgentServer->SENDTOMYSELF(&_destroyMessage);			
			return true;
		}		

		//�δ� ���� �α�;
		const bool _bFirstMake(_pCompleteMessage->bFirstMake);
		InstanceLogMapIter _iteratorLog(this->instanceLogMap.find(_keyMapID));
		if ( (_bFirstMake == true) && (_iteratorLog != this->instanceLogMap.end()) )
		{
			InstanceLog& _instanceLog(_iteratorLog->second);
			++_instanceLog.nUsedCount;
			++_instanceLog.nRunningCount;
		}		

		return true;
	}

	const bool CALLBACK ManagerAgent::_messageDestroy(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		if ( _dwGaeaID != GAEAID_NULL )
			return false; // �ϴ� �÷��̾�κ��� �� �޼����� ���Ƶд�;
		// ���� GM ������� �δ� ���� ���� ��û�� ���� ���� �ϵ���;
		// �÷��̾���� ��� �������� �Ʒ� ���� ���� �ؾ� ��;

		const GLMSG::NET_INSTANCE_DESTROY* const _pDestroyMessage((const GLMSG::NET_INSTANCE_DESTROY* const)_pMessage);
		const InstanceMapID _instanceMapID(_pDestroyMessage->instanceMapID);
		const DWORD _dwFieldChannel(_pDestroyMessage->dwFieldChannel);
		const DWORD _dwFieldID(_pDestroyMessage->dwFieldID);

		if ( _instanceMapID == GAEAID_NULL )
		{ // �ʵ� �������� ������ ���� ��� instanceMapID == GAEAID_NULL�̴�;
			// �� ��� �ش� �ʵ� ������ ���� �δ����� ��� �����Ͽ� ID�� ���������� ��ȯ �� �� �ֵ��� �Ѵ�;
			for ( InstanceAgentMapIter _iteratorInstance(this->instanceMap.begin()); _iteratorInstance != this->instanceMap.end(); ++_iteratorInstance )
			{
				InstanceAgent& _instanceAgent(_iteratorInstance->second);
				const DWORD _dwInstanceFieldChannel(_instanceAgent.getFieldChannel());
				const DWORD _dwInstanceFieldID(_instanceAgent.getFieldServer());

				const bool _bValidChannel((_dwFieldChannel == net::INVALID_SLOT) || (_dwFieldChannel == _dwInstanceFieldChannel));
				const bool _bValidID((_dwFieldID == net::INVALID_SLOT) || (_dwFieldID == _dwInstanceFieldID));
				if ( (_bValidChannel == false) || (_bValidID == false) )
					continue;

				_instanceAgent.reserveDestroy();
			}

			return true;
		}

		InstanceAgentMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
        if ( _iteratorInstance == this->instanceMap.end() )
		{
            gpAgentServer->DeleteInstantMap(_instanceMapID);
            return false;
        }

        InstanceAgent& _instanceAgent(_iteratorInstance->second);		
		_instanceAgent.reserveDestroy();

		if ( _instanceAgent.isValidToDestroy() == false )
			return true;		
		
		{ // log�� ����;
			const SNATIVEID& _keyMapID(_instanceAgent.getKeyMapID());
			InstanceLogMapIter _iteratorLog(this->instanceLogMap.find(_keyMapID));
			if ( _iteratorLog != this->instanceLogMap.end() )
			{
				InstanceLog& _instanceLog = _iteratorLog->second;
				if ( _instanceLog.nRunningCount != 0 )
					--_instanceLog.nRunningCount;
			}
		}

		// ���������� ���� �� ���� ������ ����;
		_instanceAgent.destroy();

        gpAgentServer->DeleteInstantMap(_instanceMapID);

		// ��ü ����;
		this->instanceMap.erase(_iteratorInstance);

		 return true;
	}

	const bool CALLBACK ManagerAgent::_messageAnswerJoinFA(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		if ( _dwGaeaID != GAEAID_NULL )
		{
			sc::writeLogError( "[ Instance Log ] [ Failed Join ... ]" );

			return false;
		}

		const GLMSG::NET_INSTANCE_ANSWER_JOIN_FA* const _pAnswerMessage(
			(const GLMSG::NET_INSTANCE_ANSWER_JOIN_FA* const)_pMessage );
		const InstanceMapID _instanceMapID( _pAnswerMessage->instanceMapID );
		const SNATIVEID& _keyMapID( _pAnswerMessage->keyMapID );
		const DBNUM _dbNum( _pAnswerMessage->dbNum );
		const bool _bReject( _pAnswerMessage->bReject );

		// Join ��� Ȯ�� -> Matching System Callback ���;
		MatchingSystem::JoinTrigger* jointrigger = NULL;

		MatchingSystem::JOINTRIGGER_ITER biter = m_pMatchingCallBackTriggerManager->m_JoinTriggerList.begin();
		MatchingSystem::JOINTRIGGER_ITER eiter = m_pMatchingCallBackTriggerManager->m_JoinTriggerList.end();

		for ( ; biter!= eiter; ++biter )
		{
			jointrigger = *biter;
			if ( jointrigger->_ClientDBID == _dbNum &&
				jointrigger->_instanceMapID ==_instanceMapID )
			{
				m_pMatchingCallBackTriggerManager->
					m_JoinTriggerList.erase(biter);

				MatchingSystem::JoinReceipt receipt;
				receipt.isFail = _bReject;
				receipt._instanceMapID = _instanceMapID;
				receipt.ClientDB = _dbNum;
				jointrigger->Call( &receipt );

				// ��� �� Ʈ���Ŵ� ���� �Ѵ�;
				if( jointrigger != NULL )
					SAFE_DELETE( jointrigger );
				
				break;
			}
		}

		// Map ���� ���� Ȯ��;
		InstanceAgentMapIter _iteratorInstance(
			this->instanceMap.find( _instanceMapID ) );
		if ( _iteratorInstance == this->instanceMap.end() )
		{
			sc::writeLogError( sc::string::format(
				"[ Instance Log ] [ Failed Join ... %1% / Failed Create Map ]",
				_dbNum ) );

			return false;
		}

		InstanceAgent& _instanceAgent( _iteratorInstance->second );
		if ( _instanceAgent.getKeyMapID() != _keyMapID )
		{
			sc::writeLogError( sc::string::format(
				"[ Instance Log ] [ Failed Join ... %1% / Failed Create Map ]",
				_dbNum ) );

			_instanceAgent.setStateMember(
				_dbNum,
				InstanceSystem::EMMEMBER_SET_TO_REMOVE );

			return false;
		}

		// ĳ���� ��ȿ�� Ȯ��;
		GLCharAG* const _pPlayer( gpAgentServer->GetCharByDbNum( _dbNum ) );
		if ( _pPlayer == 0 )
		{
			sc::writeLogError( sc::string::format(
				"[ Instance Log ] [ Failed Join ... %1% / Invalid Character ]",
				_dbNum ) );

			_instanceAgent.setStateMember(
				_dbNum,
				InstanceSystem::EMMEMBER_SET_TO_REMOVE );

			return false;
		}

		// ������ �ź� ���� ���;
		if ( _bReject == true )
		{
			sc::writeLogError( sc::string::format(
				"[ Instance Log ] [ Failed Join ... %1% / Reject Join ]",
				_dbNum ) );

			_instanceAgent.setStateMember(
				_dbNum,
				InstanceSystem::EMMEMBER_SET_TO_REMOVE );

			GLMSG::NET_INSTANCE_ERROR _errorMessage(
				EMERROR_REJECT_JOIN,
				_keyMapID,
				_instanceMapID );

			if ( gpAgentServer )
			{
				gpAgentServer->SENDTOCLIENT(
					_pPlayer->ClientSlot(),
					&_errorMessage );
			}

			return true;
		}            

		// �ʵ尡 �¶��� ���°� �ƴ� ���;
		const unsigned int _nInstanceFieldServer( _instanceAgent.getFieldServer() );
		const unsigned int _nInstanceFieldChannel( _instanceAgent.getFieldChannel() );
		if ( gpAgentServer->IsFieldOnline(
			_nInstanceFieldChannel,
			_nInstanceFieldServer ) == false )
		{
			sc::writeLogError( sc::string::format(
				"[ Instance Log ] [ Failed Join ... %1% / Field is Offline ]",
				_dbNum ) );

			_instanceAgent.setStateMember(
				_dbNum,
				InstanceSystem::EMMEMBER_SET_TO_REMOVE );

			GLMSG::NET_INSTANCE_ERROR _errorMessage(
				EMERROR_FIELD_SERVER_OUT,
				_keyMapID,
				_instanceMapID );

			gpAgentServer->SENDTOCLIENT(
				_pPlayer->ClientSlot(),
				&_errorMessage );

			return false;
		}

		_instanceAgent.setStateMember(
			_dbNum,
			InstanceSystem::EMMEMBER_CONFIRM_JOIN_FILED );

		return true;
	}
	
	const bool CALLBACK ManagerAgent::_messageError(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		if ( _dwGaeaID != GAEAID_NULL )
			return false;

		const GLMSG::NET_INSTANCE_ERROR* const _pErrorMessage((const GLMSG::NET_INSTANCE_ERROR* const)_pMessage);
		EMERROR_TYPE _emType(_pErrorMessage->emError);
		switch ( _emType )
		{
		case EMERROR_NOT_EXIST_INSTANCE:
			const InstanceMapID _instanceMapID(_pErrorMessage->instanceMapID);
            gpAgentServer->SENDTOMYSELF(&GLMSG::NET_INSTANCE_DESTROY(_instanceMapID));
			break;
		}
        return true;
	}

	const bool ManagerAgent::_messageMoveDone(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		if ( _dwGaeaID != GAEAID_NULL )
			return false;

		const GLMSG::NET_INSTANCE_MOVE_DONE* const _pMessageMoveDone((const GLMSG::NET_INSTANCE_MOVE_DONE* const)_pMessage);
		const MapID& _mapIDCurrent(_pMessageMoveDone->mapIDCurrent);
		const DWORD _dbNum(_pMessageMoveDone->dbNum);
		
		const DWORD _baseMapIDMain(_mapIDCurrent.getBaseMapID().Mid());
		const InstanceMapID _instanceMapID(_mapIDCurrent.getGaeaMapID().Mid());
		if ( _instanceMapID == _baseMapIDMain )
			return false;

		if ( _instanceMapID == GAEAID_NULL )
		{
			const std::string _stringErrorMessage(sc::string::format(
				"ManagerAgent::_messageMoveDone() : Player[dbNum:%1%] is in invalid-place. instantMapID[%2%];"
				, _dbNum, _instanceMapID));
			sc::writeLogError(_stringErrorMessage);
			return false;
		}

		InstanceAgentMapIter _iteratorInstance(this->instanceMap.find(_instanceMapID));
		if ( _iteratorInstance == this->instanceMap.end() )
		{ // ���� �δ� �ý���(���δ�, ������ ���(PVE)�� ��� �ű� �δ� �ʿ��� ���� ���� �ʱ� ������,
			// �������� ��� �´�, ���� ��� �δ��� �ű� �δ� �ý������� ���� �� �Ŀ�,
			// �Ʒ� �ּ��� ���� �ϵ��� �Ѵ�;
			// ps. instanceMapID�� ��� �ű��δ��ʿ��� �߱��� ���� ID�� ��� �ϹǷ� �ߺ� �� ������ ����;
// 			const std::string _stringErrorMessage(	sc::string::format(
// 				"ManagerAgent::_messageMoveDone() : can't find instant-Map[%1%]. Player[dbNum:%2%];"
// 				, _instanceMapID, _dbNum));
// 			sc::writeLogError(_stringErrorMessage);
			return false;
		}

		InstanceAgent& _instanceAgent(_iteratorInstance->second);
		if ( _instanceAgent.moveDone(_dbNum) == false )
		{
			const std::string _stringErrorMessage(sc::string::format(
				"ManagerAgent::_messageMoveDone() : Player[dbNum:%1%] is not an instant-member. instantMapID[%2%];"
				, _dbNum, _instanceMapID));
			sc::writeLogError(_stringErrorMessage);
			return false;
		}

		return true;
	}

    const bool CALLBACK ManagerAgent::_messageJoinParty(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
    {
        if ( _dwGaeaID != GAEAID_NULL )
            return false;

        const GLMSG::NET_INSTANCE_JOIN_PARTY_FA* const _pMsg((const GLMSG::NET_INSTANCE_JOIN_PARTY_FA* const)_pMessage);
        gpAgentServer->JoinParty(_pMsg->dwMasterGaeaID, _pMsg->dwMemberGaeaID, _pMsg->partyOption, false);
        return true;
    }

    const bool CALLBACK ManagerAgent::_messageOutParty(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
    {
        if ( _dwGaeaID != GAEAID_NULL )
            return false;

        const GLMSG::NET_INSTANCE_OUT_PARTY_FA* const _pMsg((const GLMSG::NET_INSTANCE_OUT_PARTY_FA* const)_pMessage);
        GLCharAG* _pChar = gpAgentServer->GetChar(_pMsg->dwGaeaID);
        gpAgentServer->QuitParty(_pChar, false);
        return true;
    }

    const bool CALLBACK ManagerAgent::_messageClearParty(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
    {
        if ( _dwGaeaID != GAEAID_NULL )
            return false;

        const GLMSG::NET_INSTANCE_CLEAR_PARTY_FA* const _pMsg((const GLMSG::NET_INSTANCE_CLEAR_PARTY_FA* const)_pMessage);
        gpAgentServer->DeleteParty(_pMsg->dwPartyID);

        return true;
    }

	const bool CALLBACK ManagerAgent::_messageRequestObserve(const NET_MSG_GENERIC* const _pMessage, const DWORD _dwGaeaID)
	{
		const GLMSG::NET_INSTANCE_REQUEST_OBSERVE* const _pMsg((const GLMSG::NET_INSTANCE_REQUEST_OBSERVE* const)_pMessage);

		GLCharAG* const _pCharObserver(gpAgentServer->GetChar(_dwGaeaID));
		if ( _pCharObserver == NULL )
			return false;

		const DBNUM _dbNumObserver(_pCharObserver->ChaDbNum());

		const InstanceMapID _instanceMapIDTarget(_pMsg->instanceMapIDTargetObserve);
		if ( _instanceMapIDTarget == InstanceMapID(-1) )
		{
			const char* const _namePlayerTarget(_pMsg->namePlayerTargetObserve);
			const DBNUM _dbNumTarget(_pMsg->dbNumTargetObserve == DWORD(-1) ? gpAgentServer->GetChaDbNumByChaName(_namePlayerTarget) : _pMsg->dbNumTargetObserve);

			GLCharAG* const _pCharTargetObserve(gpAgentServer->GetCharByDbNum(_dbNumTarget));
			if ( _pCharTargetObserve == NULL )
				return false;

			MemberInstanceAgentMapCIter _iteratorMember(this->memberInstanceMap.find(_dbNumTarget));
			if ( _iteratorMember == this->memberInstanceMap.end() )
				return false;

			const InstanceSystem::InstanceIDSet& _instanceIDSet(_iteratorMember->second);
			const InstanceMapID _instanceMapID(_instanceIDSet.instanceMapID);
			const DWORD _indexStage(_instanceIDSet.nStage);

			const InformationPosition _informationPosition(
				Faction::GLActorID(CROW_PC, _dbNumObserver),
				SNATIVEID(_instanceMapID, (WORD)_indexStage),
				Faction::GLActorID(),
				0.0f, 0.0f, InstanceSystem::EMMOVE_FIELD2INSTANCE);

			if ( ManagerAgent::reserveMember(_dbNumObserver, _informationPosition, InstanceSystem::EMAUTHORITY_OBSERVER) == false )
				return false;
		}
		else
		{
			InstanceAgentMapIter _iteratorInstance(this->instanceMap.find(_instanceMapIDTarget));
			if ( _iteratorInstance == this->instanceMap.end() )
				return false;

			const InformationPosition _informationPosition(
				Faction::GLActorID(CROW_PC, _dbNumObserver),
				SNATIVEID(_instanceMapIDTarget, 0),
				Faction::GLActorID(),
				0.0f, 0.0f, InstanceSystem::EMMOVE_FIELD2INSTANCE);

			if ( ManagerAgent::reserveMember(_dbNumObserver, _informationPosition, InstanceSystem::EMAUTHORITY_OBSERVER) == false )
				return false;			
		}		

		return true;
	}

    const bool CALLBACK ManagerAgent::_messageNewCustomMessage(NET_MSG_GENERIC* _pMessage, const DWORD _dwGaeaID)
    {
        GLMSG::NET_INSTANCE_NEW_CUSTOM_MESSAGE* NetMsg((GLMSG::NET_INSTANCE_NEW_CUSTOM_MESSAGE* )_pMessage);

        if (( NetMsg->emSrcType != InstanceSystem::EM_MESSAGE_USER ) && ( _dwGaeaID != GAEAID_NULL ))
        {
            // ���� ������ ���� �� ��Ŷ�� �ƴ϶�� _dwGaeaID�� GAEAID_NULL�̿��� �Ѵ�. ( ��Ŷ���� �ǽ� );
            return false;
        }

        switch ( NetMsg->emDestType ) 
        {
        case InstanceSystem::EM_MESSAGE_USER:           // Ư�� �������� ����. (�ʵ弭������ �ö�� ��Ŷ. Relay���� �ش�);
            {
                GLCharAG* _pChar = gpAgentServer->GetCharByDbNum(NetMsg->dwDestID);
                if ( _pChar == NULL )
                    return false;

                gpAgentServer->SENDTOCLIENT(_pChar->ClientSlot(), _pMessage);
            }
            break;

        //case InstanceSystem::EM_MESSAGE_MY_INSTANCE:    // ���� �ڽ��� ���� �ʵ弭���� LevelField ��ü�̱⶧���� ����(agent)�� ��Ŷ�� ���� ���� ����.;
            //break;
        case InstanceSystem::EM_MESSAGE_FIELD_BASE:     // Ư�� �������� PublicField�̱� ������ �ڽ��� �ʵ弭���θ� ��Ŷ ����, ����(agent)�� ��Ŷ�� ���� ���� ����.;
            {
                gpAgentServer->SENDTOALLCHANNEL(_pMessage);
            }
            break;
        case InstanceSystem::EM_MESSAGE_FIELD_INSTANCE: // Ư�� �ʵ弭���� LevelField��ü. Agent�������� �ش� ��ü�� ���� �ʵ弭���� ã�Ƴ� �������ش�.;
            {
                GLAGLandManager* _pLandMan = gpAgentServer->GetLandManager(NetMsg->dwDestID);
                if ( _pLandMan == NULL )
                    return false;

                gpAgentServer->SENDTOFIELDSVR(_pLandMan->getFieldChannel(), _pLandMan->getFieldID(), _pMessage);
            }
            break;

        case InstanceSystem::EM_MESSAGE_AGENT_BASE:     // Public Agent
            {
                sc::LuaInstance* const _pLuaInstance(InstanceSystem::getScript(NetMsg->dwDestID));
                if ( _pLuaInstance == 0 )
                    return false;

                if ( _pLuaInstance->PushFunctionEx("luaAgentEventCustomMessage") == true )
                {   
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

        case InstanceSystem::EM_MESSAGE_AGENT_INSTANCE: // Level Agent
            {
                InstanceAgentMapIter _iterator = this->instanceMap.find(NetMsg->dwDestID);
                if ( _iterator == this->instanceMap.end() )
                    return false;

                InstanceAgent& _instanceAgent = _iterator->second;
                _instanceAgent.eventCustomMessage((EM_CUSTOM_MESSAGE_TYPE)NetMsg->emSrcType, NetMsg->dwSrcID, NetMsg->param1, NetMsg->param2, NetMsg->param3, NetMsg->param4);
                return true;
            }
            break;

        default:
            return false;
        }

        return true;
    }

    const bool ManagerAgent::_messageUITimerMsgBox(NET_MSG_GENERIC* _pMessage, const DWORD _dwGaeaID)
    {
        GLMSG::NET_INSTANCE_UI_TIMER_MSGBOX NetMsg;

        if ( CastNetMsg(_pMessage, NetMsg) == false )
        {
            return false;
        }

        GLCharAG* const _pChar = gpAgentServer->GetCharByDbNum(NetMsg.dwDbNum);
        if ( _pChar == NULL )
            return 0;

        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, NetMsg);
        gpAgentServer->SENDTOCLIENT(_pChar->ClientSlot(), NET_MSG_GCTRL_INSTANCE_SYSTEM_MSGPACK, SendBuffer);
        return true;
    }

	
	const bool ManagerAgent::OnMessageProc ( GLMSG::NET_INSTANCE_MESSAGE_HEADER* _pMessage, const DWORD _dwGaeaID )
	{
		InstanceAgentMapIter iter = instanceMap.find( _pMessage->nInstanceID );
		if ( iter == instanceMap.end() )
			return false;

		InstanceAgent& refInstanceAgent = iter->second;

		return refInstanceAgent.OnMessageProc ( _pMessage, _dwGaeaID );
	}



	const bool InstanceAgent::OnMessageProc ( GLMSG::NET_INSTANCE_MESSAGE_HEADER* _pMessage, const DWORD _dwGaeaID )
	{
		switch ( _pMessage->Type() )
		{
		case NET_MSG_GCTRL_INSTANCE_SET_RETURNMAP_INFORMATION_FA:
			return MsgSetReturnMapInformationFA(
				static_cast< GLMSG::NET_SET_RETURNMAP_INFORMATION_FA* >( _pMessage ),
				_dwGaeaID );
		}

		return false;
	}

	const bool CALLBACK InstanceAgent::MsgSetReturnMapInformationFA (
		GLMSG::NET_SET_RETURNMAP_INFORMATION_FA* _pMsg,
		const DWORD _dwGaeaID )
	{
		if ( NULL == _pMsg )
			return false;

		setPreviousMapPosition( _pMsg->dwChaDBNum, _pMsg->sReturnMapInformation );

		return true;
	}

}
