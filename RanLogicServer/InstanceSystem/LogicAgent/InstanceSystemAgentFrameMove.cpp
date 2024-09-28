#include "pch.h"

#include "../../../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "../../../RanLogic/Msg/LoginMsg.h"
#include "../../InstantMap/InstantMapTypes.h"
#include "../../Character/GLCharAG.h"
#include "../../AgentServer/GLAgentServer.h"

#include "../../Move/MoveManagerServer.h"

#include "./InstanceSystemAgent.h"

namespace InstanceSystem
{
	void MemberAgent::_frameMove_field2instance ( const float _fElapsedTime, InstanceAgent& _instanceAgent )
	{
		// Member 가 삭제 예정이거나 인던이 삭제예정일 경우 이동을 처리하지 않는다;
		const bool bRemoveMember = isState( InstanceSystem::EMMEMBER_SET_TO_REMOVE );
		const bool bReserveDestroyInstanceLand = _instanceAgent.isState( InstanceSystem::EMINSTANCE_RESERVE_DESTROY );
		if ( bRemoveMember || bReserveDestroyInstanceLand )
		{
			queueInformationPosition.pop();

			return;
		}
		
		// 인던 입장이 제한될 경우 이동을 처리하지 않는다;
		const bool bJoinSuspend = _instanceAgent.isState( InstanceSystem::EMINSTANCE_JOIN_SUSPEND );
		if ( bJoinSuspend )
		{
			queueInformationPosition.pop();

			return;
		}

		const InstanceMapID _instanceMapID(_instanceAgent.getInstanceMapID());
		const SNATIVEID& _baseMapID(_instanceAgent.getChildMapID(0));
		const unsigned int _nFieldChannel(_instanceAgent.getFieldChannel());
		const unsigned int _nFieldServer(_instanceAgent.getFieldServer());

		// 필드의 입장 가능 여부를 확인하지 않았다면 확인한다;
		if ( false == isState( InstanceSystem::EMMEMBER_REQUEST_JOIN_FIELD ) )
		{	
			const SNATIVEID& sKeyMapID = _instanceAgent.getKeyMapID();

			GLMSG::NET_INSTANCE_REQUEST_JOIN_AF sRequestJoinMsg(
				_instanceMapID,
				sKeyMapID,
				dbNum,
				emAuthority );

			gpAgentServer->SENDTOFIELDSVR(
				_nFieldChannel,
				_nFieldServer,
				&sRequestJoinMsg );

			// 필드 입장 가능 여부를 확인한 상태로 전환한다;
			MemberAgent::setState( InstanceSystem::EMMEMBER_REQUEST_JOIN_FIELD ); 
			MemberAgent::setState( InstanceSystem::EMMEMBER_NOT_INITIALIZE, false );
		}
		else if ( MemberAgent::isState(InstanceSystem::EMMEMBER_CONFIRM_JOIN_FILED) == true )
		{
			GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(this->dbNum));
			if ( _pPlayer == 0 )
			{
				MemberAgent::setState(InstanceSystem::EMMEMBER_SET_TO_REMOVE);
				return;
			}

			const InformationPosition& _informationPosition(this->queueInformationPosition.front());
			const Faction::GLActorID& _targetActorID(_informationPosition.targetActorID);
			if ( _targetActorID.actorType != CROW_GATE && _targetActorID.actorType != CROW_NULL )
			{ // 현재로썬 게이트 이동만 가능하다;
				this->queueInformationPosition.pop();
				return;
			}

			const SNATIVEID _targetMapID(SNATIVEID(WORD(_instanceMapID), 0));

			sc::writeLogDebug( sc::string::format(
				"[ Move Map ] [ Move ( Field -> Instance ) : %1% / %2% ]",
				_pPlayer->ChaDbNum(),
				_targetMapID.Mid() ) );

			MoveSystem::SMOVE_INFO_AGENT sTempMoveInfo;
			sTempMoveInfo.dwChaDBNum = _pPlayer->ChaDbNum();
			sTempMoveInfo.sMapID = MapID( _baseMapID, _targetMapID );
			sTempMoveInfo.dwGateID = _targetActorID.actorID_Num;
			sTempMoveInfo.vPosition = D3DXVECTOR3( _informationPosition.fPositionX, 0.0f, _informationPosition.fPositionZ );
			sTempMoveInfo.dwFieldNum = _nFieldServer;
			sTempMoveInfo.dwChannelNum = _nFieldChannel;

			if ( MoveSystem::CMoveManagerServer::Instance()->MoveMap( sTempMoveInfo ) )
			{
				MemberAgent::setState(InstanceSystem::EMMEMBER_IN_GUARD);
				MemberAgent::setState(InstanceSystem::EMMEMBER_CONFIRM_JOIN_FILED, false);
			}

			/*gpAgentServer->ReActionMoveFieldSvr(_pPlayer->ClientSlot(), _pPlayer->GaeaId());

			const SNATIVEID _targetMapID(SNATIVEID(WORD(_instanceMapID), 0));

			MemberAgent::setState(InstanceSystem::EMMEMBER_IN_GUARD);
			MemberAgent::setState(InstanceSystem::EMMEMBER_CONFIRM_JOIN_FILED, false);

			// 현재 필드 서버에 자신이 떠나갈 예정임을 통보;
			InstanceSystem::doMessageFieldServerOut(
				_pPlayer,
				_baseMapID,
				_targetMapID,
				_targetActorID.actorID_Num,
				D3DXVECTOR3(_informationPosition.fPositionX, 0.0f, _informationPosition.fPositionZ),
				_nFieldServer,
				_nFieldChannel );*/
		}
	}

	void MemberAgent::_frameMove_instance2instance(const float _fElapsedTime, InstanceAgent& _instanceAgent)
	{
		if ( (_instanceAgent.isState(InstanceSystem::EMINSTANCE_RESERVE_DESTROY) == true) || (MemberAgent::isState(InstanceSystem::EMMEMBER_SET_TO_REMOVE) == true) )
		{
			this->queueInformationPosition.pop();
			return;
		}

		if ( _instanceAgent.isValidToMove() == false )
			return;		

		const InformationPosition& _informationPosition(this->queueInformationPosition.front());
		const Faction::GLActorID& _targetActorID(_informationPosition.targetActorID);
		const SNATIVEID& _targetMapID(_informationPosition.targetMapID);

		if ( _targetActorID.actorType != CROW_GATE && _targetActorID.actorType != CROW_NULL )
		{ // 현재로썬 게이트 이동만 가능하다;
			this->queueInformationPosition.pop();
			return;
		}

		if ( gpAgentServer->FindMapNode(_targetMapID) == 0 )
		{ // 존재 하지 않는 맵이면 삭제 한다;
			this->queueInformationPosition.pop();		

			const MapInformation& _mapInformationPrevious(this->previousMapInformation);

			const Faction::GLActorID _actorID(CROW_PC, this->dbNum);
			const SNATIVEID& _targetGaeaMapID(_mapInformationPrevious.targetMapID);
			const Faction::GLActorID& _targetActorID(_mapInformationPrevious.targetActorID);
			const float _fPositionX(_mapInformationPrevious.targetPosition.x);
			const float _fPositionZ(_mapInformationPrevious.targetPosition.z);
			const InformationPosition _informationPosition(_actorID, _targetGaeaMapID, _targetActorID, _fPositionX, _fPositionZ, InstanceSystem::EMMOVE_INSTANCE2FIELD);

			this->queueInformationPosition.push(_informationPosition);
			MemberAgent::setState(InstanceSystem::EMMEMBER_SET_TO_REMOVE);
			return;
		}

		const InstanceMapID _instanceMapID(_instanceAgent.getInstanceMapID());
		const unsigned int _nFieldServer(_instanceAgent.getFieldServer());
		const unsigned int _nFieldChannel(_instanceAgent.getFieldChannel());

		if ( _instanceAgent.addChildInstance(_targetMapID) == true )
		{
			const SNATIVEID& _keyMapID(_instanceAgent.getKeyMapID());
			const EMINSTANCE_TYPE _emInstanceType(_instanceAgent.getInstanceType());
			GLMSG::NET_INSTANCE_CREATE _createMessage(_keyMapID, _instanceMapID, _targetMapID, _emInstanceType);
			gpAgentServer->SENDTOFIELDSVR(_nFieldChannel, _nFieldServer, &_createMessage);
			return;
		}				
		
		GLMSG::NET_INSTANCE_SET_POSITION _setPositionMessage(_instanceMapID, _informationPosition);
		gpAgentServer->SENDTOFIELDSVR(_nFieldChannel, _nFieldServer, &_setPositionMessage);

		MemberAgent::setState(InstanceSystem::EMMEMBER_IN_GUARD);
	}

	void MemberAgent::_frameMove_instance2field(const float _fElapsedTime, InstanceAgent& _instanceAgent)
	{
		const DBNUM _dbNum(this->dbNum);
		GLCharAG* const _pPlayer(gpAgentServer->GetCharByDbNum(_dbNum));
		if ( _pPlayer == 0 )
		{
			this->queueInformationPosition.pop();
			MemberAgent::setState(InstanceSystem::EMMEMBER_SET_TO_REMOVE);
			return;
		}

		const InformationPosition& _informationPosition(this->queueInformationPosition.front());
		const SNATIVEID& _targetMapID(_informationPosition.targetMapID);
		const DWORD _targetGateID(_informationPosition.targetActorID.actorType == CROW_GATE ? _informationPosition.targetActorID.actorID_Num : GAEAID_NULL);
		const D3DXVECTOR3 _targetPosition(_informationPosition.fPositionX, 0.0f, _informationPosition.fPositionZ);

		const GLAGLandManager* const pLandManager(gpAgentServer->GetLandManager(_targetMapID));
		const SNATIVEID& _baseMapID(pLandManager == 0 ? SNATIVEID(false) : pLandManager->getBaseMapID());
		const SMAPNODE* const pMapNode(gpAgentServer->FindMapNode(_baseMapID));
		const bool _bValidReturnMap((pLandManager != 0) && (pMapNode != 0) && (_baseMapID == _targetMapID));

		const SNATIVEID _returnMapID(_bValidReturnMap == true ? _targetMapID : _pPlayer->m_sStartMapID);
		const DWORD _dwGateID(_bValidReturnMap == true ? _targetGateID : _pPlayer->m_dwStartGate);		
		const D3DXVECTOR3& _returnPosition(_bValidReturnMap == true ? _targetPosition : _pPlayer->m_vStartPos);		
		const unsigned int _nFieldServer(_bValidReturnMap == true ? pMapNode->GetFieldServerNum() : gpAgentServer->GetFieldServer(_pPlayer->m_sStartMapID));
		const unsigned int _nFieldChannel(_pPlayer->GetOriginChannel());

		sc::writeLogDebug( sc::string::format(
			"[ Move Map ] [ Move ( Instance -> Field ) : %1% / %2% ]",
			_pPlayer->ChaDbNum(),
			_targetMapID.Mid() ) );

		MoveSystem::SMOVE_INFO_AGENT sTempMoveInfo;
		sTempMoveInfo.dwChaDBNum = _pPlayer->ChaDbNum();
		sTempMoveInfo.sMapID = _returnMapID;
		sTempMoveInfo.dwGateID = _dwGateID;
		sTempMoveInfo.vPosition = D3DXVECTOR3( _returnPosition.x, 0.0f, _returnPosition.z );
		sTempMoveInfo.dwFieldNum = _nFieldServer;
		sTempMoveInfo.dwChannelNum = _nFieldChannel;

		if ( MoveSystem::CMoveManagerServer::Instance()->MoveMap( sTempMoveInfo ) )
		{
			MemberAgent::setState(InstanceSystem::EMMEMBER_SET_TO_REMOVE);
			this->queueInformationPosition.pop();
		}

		MemberAgent::setState(InstanceSystem::EMMEMBER_SET_TO_REMOVE);
		this->queueInformationPosition.pop();

		
		/*if ( gpAgentServer->IsFieldOnline(_nFieldChannel, _nFieldServer) == false )
		{			
			const std::string _stringError(sc::string::format("Invalid Field#%1%, Channel#%2%, targetMapID#%3%/%4%", _nFieldServer, _nFieldChannel, _returnMapID.Mid(), _returnMapID.Sid()));
			sc::writeLogError(_stringError);
			NET_CLOSE_CLIENT _closeClient(_dbNum);
			gpAgentServer->SENDTOMYSELF(&_closeClient);
			return;			
		}		

		gpAgentServer->ReActionMoveFieldSvr(_pPlayer->ClientSlot(), _pPlayer->GaeaId());
		_pPlayer->SetNextFieldSvr(_returnMapID, _dwGateID, _returnPosition, _nFieldServer);			
		

		const unsigned int _nPlayerFieldServer(_pPlayer->GetCurrentFieldServer());

		// 현재 필드 서버에 자신이 떠나갈 예정임을 통보.
		GLMSG::SNETPC_FIELDSVR_OUT NetMsg;
		NetMsg.dwChaNum = _pPlayer->ChaDbNum();
		NetMsg.dwSvrGroupNum = gpAgentServer->GetServerGroup();	
		NetMsg.targetMapID = _returnMapID;
		NetMsg.bSameFieldServer = (_nPlayerFieldServer == _nFieldServer);
		gpAgentServer->SENDTOFIELD(_pPlayer->ClientSlot(), &NetMsg);

		MemberAgent::setState(InstanceSystem::EMMEMBER_SET_TO_REMOVE);
		this->queueInformationPosition.pop();*/
	}

	const bool MemberAgent::frameMove(const float _fElapsedTime, InstanceAgent& _instanceAgent)
	{
		if ( this->queueInformationPosition.size() == 0 )
		{
			if ( MemberAgent::isState(InstanceSystem::EMMEMBER_SET_TO_REMOVE) == true )
				return false;

			return true;
		}

		if ( MemberAgent::isState(InstanceSystem::EMMEMBER_IN_GUARD) == true )
			return true;
		
		InformationPosition& _informationPosition(this->queueInformationPosition.front());
		switch ( _informationPosition.emMoveType )
		{
		case InstanceSystem::EMMOVE_FIELD2INSTANCE:
			MemberAgent::_frameMove_field2instance(_fElapsedTime, _instanceAgent);
			break;
		case InstanceSystem::EMMOVE_INSTANCE2INSTANCE:
			MemberAgent::_frameMove_instance2instance(_fElapsedTime, _instanceAgent);
			break;
		case InstanceSystem::EMMOVE_INSTANCE2FIELD:
			MemberAgent::_frameMove_instance2field(_fElapsedTime, _instanceAgent);
			break;
		}
		return true;
	}

	void InstanceAgent::_frameMoveMember(const float _fElapsedTime)
	{
		for ( MemberAgentMapIter _iteratorMember(this->memberMap.begin()); _iteratorMember != this->memberMap.end(); )
		{
			MemberAgent& _member(_iteratorMember->second);
			if ( _member.frameMove(_fElapsedTime, *this) == true )
				++_iteratorMember;
			else
			{
				sc::writeLogInfo( sc::string::format(
					"[ Move Map ] [ Remove Member ( Instance ) : %1% / %2% ]",
					_member.getDbNum(),
					instanceMapID ) );

				_iteratorMember = this->memberMap.erase(_iteratorMember);

				nCurrentPlayer--;
			}
		}
	}

	void InstanceAgent::frameMove(const float _fTime, const float _fElapsedTime, const CTime& _currentTime)
	{
		if ( InstanceAgent::isState(InstanceSystem::EMINSTANCE_CREATE_COMPLETE) == false )
			return; // 생성 중이라면 아무 행위도 하지 않는다;

		if ( InstanceAgent::isValidToDestroy(_fElapsedTime) == true )
		{ // 인던안의 모든 인원들이 정상적으로 필드로 돌아간 것이 확인 되면,
			// 인던 파괴 메세지를 다시 한번 보낸다;
			GLMSG::NET_INSTANCE_DESTROY _destroyMessage(this->instanceMapID);
			gpAgentServer->SENDTOMYSELF(&_destroyMessage);
		}	

		if ( InstanceAgent::isState(InstanceSystem::EMINSTANCE_JOIN_LOCK) == true )
		{ // 입장 불가 옵션이 켜져 있는 경우 입장 요청을 하지 않고,
			// 입장 요청 중인 플레이어들을 삭제;
			InstanceAgent::_clearRequestMember();
		}        

		InstanceAgent::_frameMoveMember(_fElapsedTime);
	}	
}