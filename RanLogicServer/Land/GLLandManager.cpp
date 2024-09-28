#include "pch.h"
#include "GLLandMan.h"

#include "../../SigmaCore/Math/SeqUniqueGuid.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../InstanceSystem/LogicField/InstanceSystemField.h"
#include "../Move/MoveManagerField.h"
#include "../../RanLogic/Msg/GLContrlFactionMsg.h"
#include "../../RanLogic/Msg/GLContrlInstanceMsg.h"

GLLandManager::GLLandManager(GLGaeaServer* const _pGaeaServer, const unsigned int nInstanceMapID, const unsigned int _nBaseMapID)
	: pGaeaServer(_pGaeaServer)
	, nInstanceMapMainID(nInstanceMapID)
	, nBaseMapID(_nBaseMapID)
	, bRawDataMap(false)
	, nStage(0)
	, nUsedMemory(0)
	, pScriptInstance(NULL)
	, pFactionManager(NULL)
	, freeEventID(MAXLANDEVENT)
	, freeSummonID(MAXSUMMON)
	, freeMaterialID(MAXCROW)
	, freeCrowID(MAXCROW)
	, freeItemID(MAXITEM)
	, freeMoneyID(MAXMONEY)

	, itemMemoryPool(_T("CItemDrop"))
	, moneyMemoryPool(_T("CMoneyDrop"))
	, m_bActiveFactionChatting( false )
{	
	for ( unsigned int _i(MAX_LAND_SID); _i--; )
		this->pLand[_i] = 0;

	for ( unsigned int _i(MAXLANDEVENT); _i--; )
		this->freeEventID.push_back(_i);	
	for ( unsigned int _i(MAXSUMMON); _i--; )
		this->freeSummonID.push_back(_i);
	for ( unsigned int _i(MAXCROW); _i--; )
		this->freeMaterialID.push_back(_i);

	for ( unsigned int _i(MAXCROW); _i--; )
		this->freeCrowID.push_back(_i);
	for ( unsigned int _i(MAXITEM); _i--; )
		this->freeItemID.push_back(_i);
	for ( unsigned int _i(MAXMONEY); _i--; )
		this->freeMoneyID.push_back(_i);

    sc::SeqUniqueGUID::Instance()->getGUID( landGUID );

    // �Ѽ� �Ŵ��� ����;
    pFactionManager = new Faction::ManagerField(pGaeaServer, this);
}

GLLandManager::~GLLandManager(void)
{
	for ( unsigned int _i(MAX_LAND_SID); _i--; )
		GLLandManager::removeLand(_i);	

	SAFE_DELETE(pFactionManager);
}

void GLLandManager::frameMove(const float _fTime, const float _fElapsedTime, __time64_t& _currentTime)
{
	if ( this->nInstanceMapMainID == GAEAID_NULL )
		return;

	unsigned int _nLand(0);	
	for ( unsigned int _i(0); _i < MAX_LAND_SID; ++_i )
	{
		GLLandMan* const _pLand(this->pLand[_i]);
		if ( _pLand == 0 )
			continue;

		++_nLand;
		_pLand->FrameMove(_fTime, _fElapsedTime, _currentTime);		

		if ( this->nStage == _nLand )
			break;
	}

	if ( GLLandManager::isOldInstantMap() == true && GLLandManager::isEnableDelete(_fElapsedTime) == true )
	{
		this->pGaeaServer->reserveDestroyInstance(this->nInstanceMapMainID);
	}	
}
void GLLandManager::sendMsgPC(NET_MSG_GENERIC* _pMessage)
{
	if ( this->nInstanceMapMainID == GAEAID_NULL )
		return;

	unsigned int _nLand(0);
	for ( unsigned int _i(0); _i < MAX_LAND_SID; ++_i )
	{
		if ( this->pLand[_i] == 0 )
			continue;

		++_nLand;
		this->pLand[_i]->SendMsgPC(_pMessage);

		if ( this->nStage == _nLand )
			break;
	}
}
void GLLandManager::sendMsgPC(EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc /*= false*/)
{
    if ( this->nInstanceMapMainID == GAEAID_NULL )
        return;

    unsigned int _nLand(0);
    for ( unsigned int _i(0); _i < MAX_LAND_SID; ++_i )
    {
        if ( this->pLand[_i] == 0 )
            continue;

        ++_nLand;
        this->pLand[_i]->SendMsgPC(MsgType, Buffer, UseCrc);

        if ( this->nStage == _nLand )
            break;
    }
}

void GLLandManager::sendMsgFaction(const Faction::FactionID _factionID, NET_MSG_GENERIC* _pMessage)
{
    const Faction::FactionMemberVector* pFactionMember = pFactionManager->getFactionMembers(_factionID);
	if(pFactionMember == NULL)
	{
		sc::writeLogError(
			sc::string::format("Can NOT find FactionID %1%, FactionTYPE %2%", _factionID.factionID_Num, _factionID.factionType)
			);
		return;
	}

    const unsigned int nMember = pFactionMember->size();
    for ( unsigned int i = 0; i < nMember; i++)
    {
        const Faction::GLActorID& _actor = (*pFactionMember)[i];
        if ( _actor.actorType == CROW_PC )
        {
            const GLChar* const pChar = pGaeaServer->GetCharByDbNum(_actor.actorID_Num);
            if ( pChar )
                pGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), _pMessage);
        }
    }
}

void GLLandManager::sendMsgFaction(const Faction::FactionID _factionID, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc /*= false*/)
{
    const Faction::FactionMemberVector* pFactionMember = pFactionManager->getFactionMembers(_factionID);
	if(pFactionMember == NULL)
	{
		sc::writeLogError(
		sc::string::format("Can NOT find FactionID %1%, FactionTYPE %2%", _factionID.factionID_Num, _factionID.factionType)
		);
		return;
	}


    const unsigned int nMember = pFactionMember->size();
    for ( unsigned int i = 0; i < nMember; i++)
    {
        const Faction::GLActorID& _actor = (*pFactionMember)[i];
        if ( _actor.actorType == CROW_PC )
        {
            const GLChar* const pChar = pGaeaServer->GetCharByDbNum(_actor.actorID_Num);
            if ( pChar )
                pGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), MsgType, Buffer, UseCrc);
        }
    }
}

const bool GLLandManager::loadLevelFile(const DWORD _nSubMapID, const char* _szFile, MapAxisInfoMan* _pAxisMan)
{
	GASSERT(_nSubMapID < MAX_LAND_SID);
	if ( _nSubMapID >= MAX_LAND_SID )
		return false;

	const bool _bSuccess(this->pLand[_nSubMapID]->LoadFile(_szFile, _pAxisMan));

	return _bSuccess;
}
const bool GLLandManager::addLand(GLLandMan* const _pBaseLand)
{	
	GASSERT(this->nStage < MAX_LAND_SID);
	if ( this->nStage >= MAX_LAND_SID )
		return false;
	
	const unsigned int _nSubMapID(this->nStage);
	const SNATIVEID _instanceMapID(this->nInstanceMapMainID, _nSubMapID);

	LandPoolField* const _pLandPool(LandPoolField::Instance());
	GLLandMan* const _pLand(_pLandPool->newLand(_pBaseLand, this, _instanceMapID));
	if ( _pLand == 0 )
		return false;
	
	const SNATIVEID& _baseMapID = _pLand->GetBaseMapID(); //(this->nMapID, _nSubMapID);	
	for ( unsigned int _index(0); _index < MAX_LAND_SID; ++_index )
	{
		if ( this->pLand[_index] != 0 )
			continue;

		this->pLand[_index] = _pLand;
		if ( this->pScriptInstance != 0 )
			this->pScriptInstance->addChildInstance(_index, _baseMapID);

		++this->nStage;
		return true;
	}

	return false;
}

/*
 * actorID�� _targetMap�� _targetActorID ��ġ�� ������;
*/
const bool GLLandManager::setPositionInstance(const Faction::GLActorID& _actorID, const Faction::GLActorID& _targetActorID, const SNATIVEID& _targetGaeaMapID, const D3DXVECTOR3& _targetPosition, const bool bResurrect /* = false */)
{	
	GLLandMan* const _pTargetLand(getLand(_targetGaeaMapID.Sid()));
	if ( _pTargetLand == 0 )
		return false;	

	// �ϴ��� �÷��̾ ���ؼ��� �̵��� �����ϵ��� �Ѵ�;
	// �� �ܿ��� �����췯�� ������ ���� ������ �ʿ� �ϴ�;
	// ������ �̷������ ���� getActor()�� ���� ó�� �ϵ��� �Ѵ�;
	if ( _actorID.actorType != CROW_PC )
		return false;

	const MapID& _targetMapID(_pTargetLand->GetMapID());

	ServerActor* const _pTargetActor(GLLandManager::getActor(_targetActorID));
	const D3DXVECTOR3& _position = _pTargetActor == 0 ? _targetPosition : _pTargetActor->GetPosition();
	const DWORD _nGateID(_targetActorID.actorType ==  CROW_GATE ? _targetActorID.actorID_Num : GAEAID_NULL);
	// _targetActorID.actorType�� ��� GAEAID_NULL�� �ƴ� CROW_NULL�� null ������ ���� ������,
	// 	CROW_GATE �� �ƴ� ��� ���� GAEAID_NULL�� �־� �ش�;

	const DWORD _dbNum(_actorID.actorID_Num);	
	ServerActor* const _pActor(getActor(_actorID));
	if ( _pActor == NULL )
	{
		GLMSG::NET_INSTANCE_SET_POSITION_FA _setPositionResult(_actorID, _targetMapID);
		this->pGaeaServer->SENDTOAGENT(&_setPositionResult);
		return false;
	}

	GLLandMan* const _pCurrentLand(_pActor->GetLandMan());
	if ( _pCurrentLand == NULL )
	{
		GLMSG::NET_INSTANCE_SET_POSITION_FA _setPositionResult(_actorID, _targetMapID);
		this->pGaeaServer->SENDTOAGENT(&_setPositionResult);
		return false;
	}

	const MapID& _currentMapID(_pCurrentLand->GetMapID());
	const SNATIVEID& _currentGaeaMapID(_currentMapID.getGaeaMapID());

	if ( _currentGaeaMapID.Mid() != _targetGaeaMapID.Mid() )
	{
		GLMSG::NET_INSTANCE_SET_POSITION_FA _setPositionResult(_actorID, _targetMapID);
		this->pGaeaServer->SENDTOAGENT(&_setPositionResult);
		return false; // �񵿱�� ó�� �ǹǷ� �δ� ���� �� �ִ�;
	}

	sc::writeLogDebug( sc::string::format(
		"[ Move Map ] [ Move ( Instance -> Instance ) : %1% / %2% ]",
		_dbNum,
		_targetGaeaMapID.Mid() ) );

	/// �̵������� �����Ѵ�;
	MoveSystem::SMOVE_INFO sTempMoveInfo;
	sTempMoveInfo.dwChaDBNum = _dbNum;
	sTempMoveInfo.sMapID = _targetMapID;
	sTempMoveInfo.dwGateID = _nGateID;
	sTempMoveInfo.vPosition = _position;
	sTempMoveInfo.bCompleteProc_InstanceSystem = true;

	/// �̵��Ѵ�;
	if ( bResurrect )
	{
		if ( false == MoveSystem::CMoveManagerField::Instance()->Resurrection( sTempMoveInfo ) )
		{
			GLMSG::NET_INSTANCE_SET_POSITION_FA _setPositionResult(_actorID, _targetGaeaMapID);
			this->pGaeaServer->SENDTOAGENT(&_setPositionResult);
			return false;
		}
	}
	else
	{
		if ( false == MoveSystem::CMoveManagerField::Instance()->MoveMap( sTempMoveInfo ) )
		{
			GLMSG::NET_INSTANCE_SET_POSITION_FA _setPositionResult(_actorID, _targetGaeaMapID);
			this->pGaeaServer->SENDTOAGENT(&_setPositionResult);
			return false;
		}
	}

	/*if ( this->pGaeaServer->EnterLand(_dbNum, _targetGaeaMapID, _nGateID, -1, _position) == FALSE )
	{
		GLMSG::NET_INSTANCE_SET_POSITION_FA _setPositionResult(_actorID, _targetGaeaMapID);
		this->pGaeaServer->SENDTOAGENT(&_setPositionResult);
		return false;
	}*/

	// ��Ȱ ó�� ������ �ӽ÷� ����ó�� [8/13/2014 hsshin];
	// �ݵ�� ��Ȱ,���̵� ������ ���� �ش� ����ó���� �����ؾ���;
	// ���� �̵����μ����� ��Ȱ�� ��� ���������� ó���� �Ǿ��־;
	// �ű��δ� �̵� ���μ����� ���� �ʴ� �κ��� �־ ����ó����.;
	// ��Ȱ�� ����� ��� �̵���Ȳ���� ���ϵ� ������� ������ ����;
	/*if ( bResurrect )
	{
		GLChar* const _pPlayer(this->pGaeaServer->GetCharByDbNum(_dbNum));
		GLMSG::NET_CHAR_RESURRECT_FAFC NetMsg(
			_targetMapID,
			_pPlayer->GetPosition(),
			_pPlayer->GETHP(),
			_pPlayer->GETMP(),
			_pPlayer->GETSP(),
			_pTargetLand->IsCullByObjectMap(),
			_pPlayer->CharDbNum()
		);
		//this->pGaeaServer->SENDTOAGENT(&NetMsg);
		this->pGaeaServer->SENDTOMYSELF(&NetMsg);
	}*/
	/*else
	{
		// bCullByObjectMap�� ���� ���� ���� ��ũ��Ʈ�� �ñ����,
		// �̰����� pScriptInstance�� ���� ȣ�� �ϸ� �ȴ�;
		GLChar* const _pPlayer(this->pGaeaServer->GetCharByDbNum(_dbNum));
		GLMSG::SNETPC_REQ_RECALL_FB _teleportMessage;
		_teleportMessage.sMAPID = _targetMapID;
		_teleportMessage.vPOS = _pPlayer->GetPosition();
		_teleportMessage.bCullByObjectMap = _pTargetLand->IsCullByObjectMap();		
		this->pGaeaServer->SENDTOCLIENT(_pPlayer->ClientSlot(), &_teleportMessage);
	}*/
	
	return true;
}

const bool GLLandManager::dropCrow(const DWORD _nSubMapID, const SNATIVEID& _nativeCrowID, const WORD _nPositionX, const WORD _nPositionZ)
{
    GASSERT(_nSubMapID < MAX_LAND_SID);
    if ( _nSubMapID >= MAX_LAND_SID )
        return false;

    if ( this->pLand[_nSubMapID] == 0 )
        return false;

    if ( this->pLand[_nSubMapID]->DropCrow(_nativeCrowID, _nPositionX, _nPositionZ) == UINT_MAX )
        return false;

    return true;
}

const bool GLLandManager::dropCrow(const DWORD _nSubMapID, const SNATIVEID& _nativeCrowID, const float _fPositionX, const float _fPositionZ)
{
	GASSERT(_nSubMapID < MAX_LAND_SID);
	if ( _nSubMapID >= MAX_LAND_SID )
		return false;

	if ( this->pLand[_nSubMapID] == 0 )
		return false;

	if ( this->pLand[_nSubMapID]->DropCrow(_nativeCrowID, _fPositionX, _fPositionZ) == UINT_MAX )
		return false;

	return true;
}

GLChar* const GLLandManager::getCharacter(const DWORD _dwGaeaID) const
{
	return this->pGaeaServer->GetCharByDbNum(_dwGaeaID);
}
GLCrow* const GLLandManager::getCrow(const DWORD _dwGaeaID) const
{
	CROW_DROP_MAP_CITER _iteratorCrow(this->dropCrowReferenceMap.find(_dwGaeaID));
	if ( _iteratorCrow == this->dropCrowReferenceMap.end() )
		return NULL;

	return _iteratorCrow->second;
}
CItemDrop* const GLLandManager::getItem(const DWORD _dwGaeaID) const
{
	ITEM_DROP_MAP_CITER _iteratorItem(this->dropItemReferenceMap.find(_dwGaeaID));
	if ( _iteratorItem == this->dropItemReferenceMap.end() )
		return NULL;

	return _iteratorItem->second;
}
CMoneyDrop* const GLLandManager::getMoney(const DWORD _dwGaeaID) const
{
	MONEY_DROP_MAP_CITER _iteratorMoney(this->dropMoneyReferenceMap.find(_dwGaeaID));
	if ( _iteratorMoney == this->dropMoneyReferenceMap.end() )
		return NULL;

	return _iteratorMoney->second;
}
GLSummonField* const GLLandManager::getSummon(const DWORD _dwGaeaID) const
{
	SUMMON_DROP_MAP_CITER _iteratorSummon(this->dropSummonReferenceMap.find(_dwGaeaID));
	if ( _iteratorSummon == this->dropSummonReferenceMap.end() )
		return NULL;

	return _iteratorSummon->second;
}
GLMaterial* const GLLandManager::getMaterial(const DWORD _dwGaeaID) const
{
	MATERIAL_DROP_MAP_CITER _iteratorMaterial(this->dropMaterialReferenceMap.find(_dwGaeaID));
	if ( _iteratorMaterial == this->dropMaterialReferenceMap.end() )
		return NULL;

	return _iteratorMaterial->second;
}

ServerActor* const GLLandManager::getActor(const Faction::GLActorID& _actorID) const
{
	const EMCROW emCrow(_actorID.actorType);
	const DWORD dwActorID(_actorID.actorID_Num);
	switch ( emCrow )
	{
	case CROW_PC:
		return (ServerActor*)GLLandManager::getCharacter(dwActorID);
	case CROW_NPC:
	case CROW_MOB:	
		return (ServerActor*)GLLandManager::getCrow(dwActorID);
	case CROW_ITEM:
		return (ServerActor*)GLLandManager::getItem(dwActorID);
	case CROW_MONEY:
		return (ServerActor*)GLLandManager::getMoney(dwActorID);	
	case CROW_SUMMON:
		return (ServerActor*)GLLandManager::getSummon(dwActorID);
	case CROW_MATERIAL:
		return (ServerActor*)GLLandManager::getMaterial(dwActorID);
	case CROW_PET:
	case CROW_ZONENAME:
	case CROW_GATENAME:
		break;
	}
	return NULL;
}

const unsigned int GLLandManager::getBaseMapMainID(void) const
{
	return this->nBaseMapID;
}
const unsigned int GLLandManager::getInstanceMapMainID(void) const
{
	return this->nInstanceMapMainID;
}
const unsigned int GLLandManager::getNStage(void) const
{
	return this->nStage;
}
const int GLLandManager::getUsedMemory(void) const
{
	return this->nUsedMemory;
}

void GLLandManager::removeLand(const unsigned int _nSubMapID)
{
	GASSERT(_nSubMapID < MAX_LAND_SID);
	if ( _nSubMapID >= MAX_LAND_SID )
		return;

	if ( this->pLand[_nSubMapID] == 0 )
		return;

	--this->nStage;
	LandPoolField::Instance()->release(this->pLand[_nSubMapID]);
	this->pLand[_nSubMapID] = 0;
}

void GLLandManager::addUsedMemory(const int _nSize)
{
	this->nUsedMemory += _nSize;
}
void GLLandManager::setScriptInstance(InstanceSystem::InstanceField* _pScriptInstance)
{
	this->pScriptInstance = _pScriptInstance;
}

void GLLandManager::createFaction(const Faction::FactionID& _factionID)
{
	// ������ �ʿ䰡 ���� �κ�;
	/*const Faction::ManagerField::EMFACTION_RETURN emReturn =
		pFactionManager->createFaction( _factionID );
	if ( Faction::ManagerField::EMFACTION_SUCCESS == emReturn )
	{

	}*/
}

void GLLandManager::setFactionID(const Faction::GLActorID& _actorID, const Faction::FactionID& _factionID)
{
	Faction::FactionID preFactionID;
	const Faction::ManagerField::EMFACTION_RETURN emReturn =
		pFactionManager->setFaction( _actorID, _factionID, preFactionID );

	if ( Faction::ManagerField::EMFACTION_SUCCESS == emReturn )
	{
		if ( pLand[0] )
		{
			if ( preFactionID != Faction::FactionID() )
			{
				pLand[0]->DelCurFactionPlayerCount( preFactionID );
			}

			pLand[0]->AddCurFactionPlayerCount( _factionID );
		}

		GLMSG::NET_FACTION_CHANGE _factionChangeMessage(_factionID, _actorID);
		GLLandManager::sendMsgPC(&_factionChangeMessage);
	}	
}

void GLLandManager::setRelation(const Faction::FactionID& _factionID, const Faction::FactionID& _targetFactionID, const Faction::EMRELATION_TYPE _emRelation)
{
	const Faction::ManagerField::EMFACTION_RETURN emReturn =
		pFactionManager->setRelation(_factionID, _targetFactionID, _emRelation);

	if ( Faction::ManagerField::EMFACTION_SUCCESS == emReturn )
	{   
        GLMSG::NET_FACTION_RELATION_CHANGE _relationChangeMessage(GLMSG::NET_FACTION_RELATION_CHANGE::CHANGE_FACTION_TO_FACTION, _factionID, _targetFactionID, _emRelation);
		GLLandManager::sendMsgPC(&_relationChangeMessage);
	}
}

void GLLandManager::setRelationToAll(const Faction::FactionID& _factionID, const Faction::EMRELATION_TYPE _emRelation)
{
    const Faction::ManagerField::EMFACTION_RETURN emReturn =
		pFactionManager->setRelationToAll(_factionID, _emRelation);

    if ( Faction::ManagerField::EMFACTION_SUCCESS == emReturn )
    {   
        GLMSG::NET_FACTION_RELATION_CHANGE _relationChangeMessage(GLMSG::NET_FACTION_RELATION_CHANGE::CHANGE_FACTION_TO_ALL, _factionID, _factionID, _emRelation);
        GLLandManager::sendMsgPC(&_relationChangeMessage);
    }
}

void GLLandManager::setRelationFromAll(const Faction::FactionID& _factionID, const Faction::EMRELATION_TYPE _emRelation)
{
    const Faction::ManagerField::EMFACTION_RETURN emReturn =
		pFactionManager->setRelationFromAll(_factionID, _emRelation);

    if ( Faction::ManagerField::EMFACTION_SUCCESS == emReturn )
    {   
        GLMSG::NET_FACTION_RELATION_CHANGE _relationChangeMessage(GLMSG::NET_FACTION_RELATION_CHANGE::CHANGE_FACTION_FROM_ALL, _factionID, _factionID, _emRelation);
        GLLandManager::sendMsgPC(&_relationChangeMessage);
    }
}

void GLLandManager::setHpVisible(const Faction::EMRELATION_FLAG_TYPE _emHpVisible)
{
    const bool bChanged = pFactionManager->setHPVisible(_emHpVisible);
    if ( bChanged )
    {
        GLMSG::NET_FACTION_HP_VISIBLE_INFO _HpVisibleMessage(_emHpVisible);
        GLLandManager::sendMsgPC(&_HpVisibleMessage);
    }
}
void GLLandManager::setRawDataMap(void)
{
	bRawDataMap = true;
}

InstanceSystem::InstanceField* GLLandManager::GetScriptInstance(void)
{
	return pScriptInstance;
}








///// ���������� ȣȯ���� ���� ���ܵ� /////
const bool GLLandManager::addLand(const unsigned int _nSubMapID, const bool _bPeaceZone, const bool _bPKZone, GLLandMan* const _pBaseLand)
{	
	GASSERT(_nSubMapID < MAX_LAND_SID);
	if ( _nSubMapID >= MAX_LAND_SID )
		return false;

	if ( this->pLand[_nSubMapID] != 0 )
		return false;

	const SNATIVEID _baseMapID(this->nBaseMapID, _nSubMapID);	
    const SNATIVEID _instanceMapID(this->nInstanceMapMainID, this->nInstanceMapMainID == unsigned int(-1) ? USHRT_MAX : _nSubMapID);
	GLLandMan* _pLand(NULL);

	LandPoolField* const _pLandPool(LandPoolField::Instance());
	if ( _pBaseLand == 0 )
		_pLand = _pLandPool->newLand(this->pGaeaServer, this, _baseMapID, _instanceMapID);
	else
		_pLand = _pLandPool->newLand(_pBaseLand, this, _instanceMapID);

	if ( _pLand == 0 )
		return false;

	_pLand->SetBaseMapID(_baseMapID);
	_pLand->SetPeaceZone(_bPeaceZone);
	_pLand->SetPKZone(_bPKZone);

	if ( this->pScriptInstance != 0 )
		this->pScriptInstance->addChildInstance(_nSubMapID, _baseMapID);
	this->pLand[_nSubMapID] = _pLand;
	++this->nStage;

	return true;
}



const bool GLLandManager::isCopied(void) const
{
	if ( this->pLand[0] == 0 )
		return false;

	return this->pLand[0]->HasCopied();
}

const bool GLLandManager::isOldInstantMap(void) const
{
	if ( this->pLand[0] == 0 )
		return false;

	return this->pLand[0]->GetInstantType() == SMAPNODE::EMINSTANCE_OLD_INSTANCE;
}

const bool GLLandManager::isNewInstantMap(void) const
{
    if ( this->pLand[0] == 0 )
        return false;

    return this->pLand[0]->GetInstantType() == SMAPNODE::EMINSTANCE_NEW_INSTANCE;
}

const bool GLLandManager::isEnableDelete(const float _fElapsedTime)
{
	unsigned int _nLand(0);
	for ( unsigned int _i(0); _i < MAX_LAND_SID; ++_i )
	{
		if ( this->pLand[_i] == 0 )
			continue;

		++_nLand;
		if ( this->pLand[_i]->CanBeDeleted(_fElapsedTime) == FALSE )
			return false;

		if ( this->nStage == _nLand )
			break;
	}
    return true;
}
void GLLandManager::extraFrameMoveInstantMap(const float _fElapsedTime)
{
	unsigned int _nLand(0);
	for ( unsigned int _i(0); _i < MAX_LAND_SID; ++_i )
	{
		if ( this->pLand[_i] == 0 )
			continue;

		++_nLand;
		this->pLand[_i]->ExtraFrameMoveInstantMap(_fElapsedTime);

		if ( this->nStage == _nLand )
			break;
	}	
}

void GLLandManager::setInstantMapData(const unsigned int _nGaeaID, const GLPartyID& _partyID)
{
	if ( this->pLand[0] == 0 )
		return;

	this->pLand[0]->SetInstantMapData(_nGaeaID, _partyID);
}
void GLLandManager::setInformation(LPDIRECT3DDEVICEQ pd3dDevice, const bool _bEmulator)
{
	if ( this->pLand[0] == 0 )
		return;

	this->pLand[0]->SetD3dDevice(pd3dDevice);
	this->pLand[0]->SetEmulator(_bEmulator);
}
void GLLandManager::setMapState(void)
{
	unsigned int _nLand(0);
	for ( unsigned int _i(0); _i < MAX_LAND_SID; ++_i )
	{
		GLLandMan* const pLand(this->pLand[_i]);
		if ( pLand == 0 )
			continue;

		pLand->m_bGuidBattleMap = false;
		pLand->m_bGuidBattleMapHall = false;
		pLand->m_bClubDeathMatchMap = false;
		pLand->m_bClubDeathMatchMapHall = false;

		pLand->SetCommissionRate(GLCONST_CHAR::fDEFAULT_COMMISSION);
		
		++_nLand;		

		if ( this->nStage == _nLand )
			break;
	}
}
void GLLandManager::StartTriggerFinger(int nStage, int nLevel, const std::string& strGuid)
{
	if ( this->pLand[0] == 0 )
		return;

	this->pLand[0]->StartTriggerFinger(nStage, nLevel, strGuid);
}
void GLLandManager::RemoveTriggerFinger()
{
	if ( this->pLand[0] == 0 )
		return;

	this->pLand[0]->RemoveTriggerFinger();
}
void GLLandManager::SendTriggerMessage(TriggerSystem::SMessage* msg)
{
	if ( this->pLand[0] == 0 )
		return;

	this->pLand[0]->SendTriggerMessage(msg);
}
void GLLandManager::PostTriggerMessage(TriggerSystem::SpMessage msg)
{
	if ( this->pLand[0] == 0 )
		return;

	this->pLand[0]->PostTriggerMessage(msg);
}

void GLLandManager::clearObjectExceptCharacter(void)
{
	unsigned int _nLand(0);
	for ( unsigned int _i(0); _i < MAX_LAND_SID; ++_i )
	{
		if ( this->pLand[_i] == 0 )
			continue;

		++_nLand;
		this->pLand[_i]->ClearExptChaObj();

		if ( this->nStage == _nLand )
			break;
	}
}

void GLLandManager::clearDropObject(void)
{
	unsigned int _nLand(0);
	for ( unsigned int _i(0); _i < MAX_LAND_SID; ++_i )
	{
		if ( this->pLand[_i] == 0 )
			continue;

		++_nLand;
		this->pLand[_i]->ClearDropObj();

		if ( this->nStage == _nLand )
			break;
	}
}

void GLLandManager::render(LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME &cv)
{
	unsigned int _nLand(0);
	for ( unsigned int _i(0); _i < MAX_LAND_SID; ++_i )
	{
		if ( this->pLand[_i] == 0 )
			continue;

		++_nLand;
		this->pLand[_i]->Render(pd3dDevice, cv);

		if ( this->nStage == _nLand )
			break;
	}
}

const DWORD GLLandManager::GetCurPlayerCount ( void ) const
{
	DWORD dwPlayerCount = 0;

	unsigned int nLandCount = 0;
	for ( unsigned int i=0; i<MAX_LAND_SID; ++i )
	{
		if ( NULL == pLand[i] )
			continue;

		++nLandCount;

		dwPlayerCount += pLand[i]->GetCurPlayerCount();

		if ( nStage == nLandCount )
			break;
	}

	return dwPlayerCount;
}

const bool GLLandManager::GetCurPartyPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const
{
	unsigned int nLandCount = 0;
	for ( unsigned int i=0; i<MAX_LAND_SID; ++i )
	{
		if ( NULL == pLand[i] )
			continue;

		++nLandCount;

		GLCountRecorder::MAP_COUNTER mapPlayerCounter;
		pLand[i]->GetCurPartyPlayerCount( mapPlayerCounter );

		GLCountRecorder::MAP_COUNTER_ITR iter = mapPlayerCounter.begin();
		GLCountRecorder::MAP_COUNTER_ITR eiter = mapPlayerCounter.end();
		for ( ;iter != eiter; ++iter )
		{
			LONGLONG llCount = iter->second;

			GLCountRecorder::MAP_COUNTER_ITR tempIter = _refPlayerCounter.find( iter->first );
			if ( tempIter != _refPlayerCounter.end() )
			{
				LONGLONG& llRefBasicCount = tempIter->second;
				llRefBasicCount += llCount;
			}
			else
			{
				_refPlayerCounter.insert(
					GLCountRecorder::MAP_COUNTER_VALUE(
					iter->first,
					llCount ) );
			}
		}

		if ( nStage == nLandCount )
			break;
	}

	return true;
}

const bool GLLandManager::GetCurClubPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const
{
	unsigned int nLandCount = 0;
	for ( unsigned int i=0; i<MAX_LAND_SID; ++i )
	{
		if ( NULL == pLand[i] )
			continue;

		++nLandCount;

		GLCountRecorder::MAP_COUNTER mapPlayerCounter;
		pLand[i]->GetCurClubPlayerCount( mapPlayerCounter );

		GLCountRecorder::MAP_COUNTER_ITR iter = mapPlayerCounter.begin();
		GLCountRecorder::MAP_COUNTER_ITR eiter = mapPlayerCounter.end();
		for ( ;iter != eiter; ++iter )
		{
			LONGLONG llCount = iter->second;

			GLCountRecorder::MAP_COUNTER_ITR tempIter = _refPlayerCounter.find( iter->first );
			if ( tempIter != _refPlayerCounter.end() )
			{
				LONGLONG& llRefBasicCount = tempIter->second;
				llRefBasicCount += llCount;
			}
			else
			{
				_refPlayerCounter.insert(
					GLCountRecorder::MAP_COUNTER_VALUE(
					iter->first,
					llCount ) );
			}
		}

		if ( nStage == nLandCount )
			break;
	}

	return true;
}

const bool GLLandManager::GetCurCountryPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const
{
	unsigned int nLandCount = 0;
	for ( unsigned int i=0; i<MAX_LAND_SID; ++i )
	{
		if ( NULL == pLand[i] )
			continue;

		++nLandCount;

		GLCountRecorder::MAP_COUNTER mapPlayerCounter;
		pLand[i]->GetCurCountryPlayerCount( mapPlayerCounter );

		GLCountRecorder::MAP_COUNTER_ITR iter = mapPlayerCounter.begin();
		GLCountRecorder::MAP_COUNTER_ITR eiter = mapPlayerCounter.end();
		for ( ;iter != eiter; ++iter )
		{
			LONGLONG llCount = iter->second;

			GLCountRecorder::MAP_COUNTER_ITR tempIter = _refPlayerCounter.find( iter->first );
			if ( tempIter != _refPlayerCounter.end() )
			{
				LONGLONG& llRefBasicCount = tempIter->second;
				llRefBasicCount += llCount;
			}
			else
			{
				_refPlayerCounter.insert(
					GLCountRecorder::MAP_COUNTER_VALUE(
					iter->first,
					llCount ) );
			}
		}

		if ( nStage == nLandCount )
			break;
	}

	return true;
}

const bool GLLandManager::GetCurSchoolPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const
{
	unsigned int nLandCount = 0;
	for ( unsigned int i=0; i<MAX_LAND_SID; ++i )
	{
		if ( NULL == pLand[i] )
			continue;

		++nLandCount;

		GLCountRecorder::MAP_COUNTER mapPlayerCounter;
		pLand[i]->GetCurSchoolPlayerCount( mapPlayerCounter );

		GLCountRecorder::MAP_COUNTER_ITR iter = mapPlayerCounter.begin();
		GLCountRecorder::MAP_COUNTER_ITR eiter = mapPlayerCounter.end();
		for ( ;iter != eiter; ++iter )
		{
			LONGLONG llCount = iter->second;

			GLCountRecorder::MAP_COUNTER_ITR tempIter = _refPlayerCounter.find( iter->first );
			if ( tempIter != _refPlayerCounter.end() )
			{
				LONGLONG& llRefBasicCount = tempIter->second;
				llRefBasicCount += llCount;
			}
			else
			{
				_refPlayerCounter.insert(
					GLCountRecorder::MAP_COUNTER_VALUE(
					iter->first,
					llCount ) );
			}
		}

		if ( nStage == nLandCount )
			break;
	}

	return true;
}

const bool GLLandManager::GetCurFactionPlayerCount ( GLCountRecorder::MAP_FACTION_COUNTER& _refPlayerCounter ) const
{
	if ( NULL == pLand[0] )
		return false;

	pLand[0]->GetCurFactionPlayerCount( _refPlayerCounter );

	return true;
}