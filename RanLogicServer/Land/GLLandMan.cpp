#include "../pch.h"
#include <process.h>
#include "../../SigmaCore/Math/Random.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/Common/profile.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Item/GLITEMLMT.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/HideSet/GLHideSet.h"
#include "../../RanLogic/TriggerSystem/TriggerFinger.h"
#include "../../RanLogic/Msg/GLContrlFactionMsg.h"
#include "../../RanLogic/Skill/GLCountAttackSkill.h"

#include "../AroundSlot/GLARoundSlot.h"
#include "../Club/GLClubManField.h"
#include "../Club/GLClubField.h"
#include "../Character/GLChar.h"
#include "../Crow/GLCrow.h"
#include "../Character/GLChar.h"
#include "../FieldServer/GLGaeaServer.h"
#include "../Guidance/GLGuidance.h"
#include "../LandEvent/GLLandEventServer.h"
#include "../Summon/GLSummonField.h"
#include "../Util/StlFuncServer.h"
#include "../Util/GLItemLimit.h"
#include "../Util/StlFuncServer.h"
#include "../TriggerSystem/ServerTriggerSystem.h"
#include "../InstanceSystem/LogicField/InstanceSystemField.h"
#include "../AI/GLFSMContainer.h"
#include "../AI/GLScriptedFSM.h"
#include "../CountRecorder/CountRecorder.h"
#include "../Party/GLPartyTenderManagerField.h"
#include "../Move/MoveManagerField.h"
#include "../Database/DBAction/DbActionGameItem.h"

#include "../../RanLogic/Prison/LimitPrison.h"

#include "./GLLandMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


GLLandMan::GLLandMan (GLGaeaServer* pServer, GLLandManager* pLandManager, const SNATIVEID& baseMapID)
    : m_bInit(NULL)
	, m_bCopied(FALSE)	//mjeon.InstantMap의 경우 SrcLandMan으로부터 복사가 정상적으로 되었는가?

	, m_vMax(0,0,0)
	, m_vMin(0,0,0)

	, m_dwClubMapID(0)
	, m_pGuidance(NULL)
	, m_bGuidBattleMap(false)
	, m_bGuidBattleMapHall(false)
	, m_dwGuidClubID(CLUB_NULL)
	, m_fCommissionRate(0.0f)
    , m_fCommissionLogTimer(0.0f)
    , m_llBuyCommissionMoney(0)
    , m_llSellCommissionMoney(0)

	, m_bClubDeathMatchMap(false)
	, m_bClubDeathMatchMapHall(false)

	, m_bNonConfrontMode(false)
	
	, m_bServerStop(false)

	, m_dwOldWeather(NULL)

	, m_pd3dDevice(NULL)

	, m_fInstantMapDeleteTime(0.0f)

	, m_fElapsedLimitTime(0.0f)
	, m_bExcessiveLimitTime(FALSE)
	, m_nLastSendRemainTime(0)
	, m_bEmulator(false)
    , m_pGLGaeaServer(pServer)
	, m_pLandManager(pLandManager)

    , m_fCrowMultipleHP(1.f)
    , m_fCrowMultipleAttack(1.f)
    , m_fCrowMultipleDefense(1.f)
    , m_fCrowMultipleExp(1.f)
	, m_fCrowMultipleDrop(1.f)    	
{
    InitializeCriticalSectionAndSpinCount(&m_GlobPCListLock, sc::CRITICAL_SECTION_SPIN_COUNT);

	SetBaseMapID(baseMapID);

	m_vTempPC.reserve(160);
	m_vDetectPC.reserve(160);

	m_vTempCROW.reserve(160);
	m_vDetectCROW.reserve(160);

	m_vecMapOldWeather.clear();

    m_pLandEvent = new GLLandEventServer(m_pGLGaeaServer, &m_pLandManager->freeEventID, m_LandTree);
    m_pCountRecorder = new GLCountRecorder(this);

    BOOST_FOREACH( const SpMobScheduleManServer& spMobSchMan, m_MobSchManContainer )
        spMobSchMan->SetLandMan( this );	

	CopyDone();
}

GLLandMan::~GLLandMan()
{
    //CleanUp();//단순히 하는 일은 ResetMap()을 호출하는 것 뿐이므로, ResetMap() 그 자체로 대체가 된다.

    ReSetMap();	

    this->m_DropCrowList.clear();	

	SAFE_DELETE(m_pLandEvent);
	SAFE_DELETE(m_pCountRecorder);

    DeleteCriticalSection(&m_GlobPCListLock);
}

GLLandMan::GLLandMan(const GLLandMan& _rhs, GLLandManager* pLandManager)
	: m_bInit(NULL)
	, m_bCopied(FALSE)	//mjeon.InstantMap의 경우 SrcLandMan으로부터 복사가 정상적으로 되었는가?

	, m_vMax(0,0,0)
	, m_vMin(0,0,0)

	, m_dwClubMapID(0)
	, m_pGuidance(NULL)
	, m_bGuidBattleMap(false)
	, m_bGuidBattleMapHall(false)
	, m_dwGuidClubID(CLUB_NULL)
	, m_fCommissionRate(0.0f)
	, m_fCommissionLogTimer(0.0f)
	, m_llBuyCommissionMoney(0)
	, m_llSellCommissionMoney(0)

	, m_bClubDeathMatchMap(false)
	, m_bClubDeathMatchMapHall(false)

	, m_bNonConfrontMode(false)

	, m_bServerStop(false)

	, m_dwOldWeather(NULL)

	, m_pd3dDevice(NULL)

	, m_fInstantMapDeleteTime(0.0f)

	, m_fElapsedLimitTime(0.0f)
	, m_bExcessiveLimitTime(FALSE)
	, m_nLastSendRemainTime(0)
	, m_bEmulator(false)
	, m_pGLGaeaServer(_rhs.m_pGLGaeaServer)
	, m_pLandManager(pLandManager)
	, m_nUsedMemory(_rhs.m_nUsedMemory)

	, m_fCrowMultipleHP(1.f)
	, m_fCrowMultipleAttack(1.f)
	, m_fCrowMultipleDefense(1.f)
	, m_fCrowMultipleExp(1.f)
	, m_fCrowMultipleDrop(1.f)   
{
	InitializeCriticalSectionAndSpinCount(&m_GlobPCListLock, sc::CRITICAL_SECTION_SPIN_COUNT);

	m_vTempPC.reserve(160);
	m_vDetectPC.reserve(160);

	m_vTempCROW.reserve(160);
	m_vDetectCROW.reserve(160);

	m_vecMapOldWeather.clear();

	m_pLandEvent = new GLLandEventServer(m_pGLGaeaServer, &m_pLandManager->freeEventID, m_LandTree);	    
	m_pCountRecorder = new GLCountRecorder(this);

	//CopyLevelData( rhsSrc, this );	//mjeon.아래의 operator=로 대체
	GLLevelFileServer::operator=(_rhs);


	BOOST_FOREACH( const SpMobScheduleManServer& spMobSchMan, m_MobSchManContainer )
		spMobSchMan->SetLandMan( this );		

	if ( this == &_rhs )
		return;

	m_spNaviMeshInWld = _rhs.m_spNaviMeshInWld;
	m_spNaviMeshBypass = _rhs.m_spNaviMeshBypass;

    //mjeon.tbb.인던 병렬화 시에 NavigationMesh::m_NavHeap에서 동시접근에 의한 총돌이 발생하므로 DeepCopy하도록 한다.
    if ( _rhs.m_spNaviMeshInWld.get() == NULL )
    {
        sc::writeLogError(std::string("GLLandMan::NavigationMesh is NULL!!"));
        return;
    }
    else
    {
		// 변경된 Navigation이 있다면 그것을 사용하도록 한다.
		if ( _rhs.m_spNaviMeshBypass.get() )
		{
			m_spNaviMesh = boost::shared_ptr<NavigationMesh>( new NavigationMesh(*_rhs.m_spNaviMeshBypass.get()) );
		}
		else
		{
			m_spNaviMesh = boost::shared_ptr<NavigationMesh>( new NavigationMesh(*_rhs.m_spNaviMeshInWld.get()) );
		}
    }

    //날씨 설정 복사
    m_sWeatherProp = _rhs.m_sWeatherProp;

    //	Note : Map 데이터 초기화.
    HRESULT	hr = SetMap();
    if (FAILED(hr))
    {
        sc::writeLogError(std::string("GLLandMan::SetMap failure."));
        return;
    }

    if (m_pd3dDevice)
    {
        hr = GLLevelFileServer::InitDeviceObjects(m_pd3dDevice);
        if (FAILED(hr))
        {
            sc::writeLogError(std::string("GLLevelFileServer::InitDeviceObjects failure."));
            return;
        }

        hr = GLLevelFileServer::RestoreDeviceObjects(m_pd3dDevice);
        if (FAILED(hr))
        {
            sc::writeLogError(std::string("GLLevelFileServer::RestoreDeviceObjects failure."));
            return;
        }
    }

    SMAPNODE* pMAP_NODE = m_pGLGaeaServer->FindMapNode(GetBaseMapID());
    if (pMAP_NODE)
    {
        m_bPKZone				= pMAP_NODE->IsPkZone();
        m_bFreePK				= pMAP_NODE->IsFreePk();
        m_bItemDrop			= pMAP_NODE->IsItemDrop();
        m_bPetActivity		= pMAP_NODE->IsPetActivity();
        m_bDECEXP				= pMAP_NODE->IsDecExp();
        m_bVehicleActivity	= pMAP_NODE->IsVehicleActivity();
        m_bClubBattleZone	= pMAP_NODE->IsClubBattleZone();
        m_bLunchBoxForbid	= pMAP_NODE->IsLunchBoxForbid();
        m_bCPReset			= pMAP_NODE->IsCombatPointReset();		
        m_bPKMap				= pMAP_NODE->IsPkMap();
        m_bClubPkRecord     = pMAP_NODE->IsClubPkRecord();
        m_emInstanceType	= GLLevelFile::EMINSTANCE_TYPE(pMAP_NODE->GetInstanceType());
		m_bChangeSchool	= pMAP_NODE->IsChangeSchool();
    }

    m_fCommissionRate = _rhs.m_fCommissionRate;
    m_fCommissionLogTimer = _rhs.m_fCommissionLogTimer;
    m_llBuyCommissionMoney = _rhs.m_llBuyCommissionMoney;
    m_llSellCommissionMoney = _rhs.m_llSellCommissionMoney;

//    m_sPVEIndunID.Init();

    CopyDone();
}
//
//void GLLandMan::ResetLandMan()
//{
//	m_bInit = NULL;
//
//	m_bCopied = FALSE;
//
//	m_vMax = D3DXVECTOR3( 0,0,0 );
//	m_vMin = D3DXVECTOR3( 0,0,0 );
//
//	m_dwClubMapID = 0;
//	m_pGuidance = NULL;
//	m_bGuidBattleMap = false;
//	m_bGuidBattleMapHall = false;
//	m_dwGuidClubID = CLUB_NULL;
//	m_fCommissionRate = 0.0f;
//    m_fCommissionLogTimer = 0.0f;
//    m_llBuyCommissionMoney = 0;
//    m_llSellCommissionMoney = 0;
//
//	m_bClubDeathMatchMap = false;
//	m_bClubDeathMatchMapHall = false;
//
//	m_bServerStop = false;
//
//	m_dwOldWeather = NULL;
//
//	m_spNaviMesh = NULL;
//	m_pd3dDevice = NULL;
//
//	m_fInstantMapDeleteTime = 0.0f;
//
//	m_fElapsedLimitTime		= 0.0f;
//	m_nLastSendRemainTime	= 0;
//
//	m_bExcessiveLimitTime   = FALSE;
//
//    m_ItemDropData.clear();
//    m_MoneyDropData.clear();
//    m_CrowDrop.clear();
//    m_MaterialDrop.clear();
//    m_SummonDrop.clear();
//
//	//m_MobSchMan->SetLandMan ( this );
//    DelAllMobSchManRegenLayer();
//    DelAllMobSchManOnceLayer();
//
//	m_vTempPC.clear();
//	m_vDetectPC.clear();
//
//	m_vTempCROW.clear();
//	m_vDetectCROW.clear();
//
//	m_vTempPC.reserve(160);
//	m_vDetectPC.reserve(160);
//
//	m_vTempCROW.reserve(160);
//	m_vDetectCROW.reserve(160);
//	m_bEmulator = false;
//
//	// LevelFile 부분 초기화
//	GLLevelFileServer::DeleteDeviceObjects();
//	GLLevelFileServer::ResetData();
//
//    m_GlobPCListDelList.clear();
//
//    m_fCrowMultipleHP = 1.f;
//    m_fCrowMultipleAttack = 1.f;
//    m_fCrowMultipleDefense = 1.f;
//    m_fCrowMultipleExp = 1.f;
//	m_fCrowMultipleDrop = 1.f;
//
//	m_sPVEIndunID.Init();
//}


HRESULT GLLandMan::ReSetMap()
{
	m_bInit = FALSE;

	m_bCopied = FALSE;

	ClearDropObj();

	// [shhan][2016.04.20] boost::shared_ptr 로 되면서, 자동 삭제 됨.
	//mjeon.이제 모든 인던 LandMan들은 각각의 NavigationMesh deep복사본을 가지므로 모두 삭제해야 한다.	
    //SAFE_DELETE(m_spNaviMesh);

    m_LandTree.DeleteQuadTree();

	ClearMobSchMan();
	clearGateMan();

	m_CollisionMap.CleanUp ();
	m_listDelGlobIDEx.clear();
	m_listDelGlobID.clear();
	m_listDelGlobIDExMat.clear();
	m_listDelGlobIDMat.clear();
    m_GlobPCListDelList.clear();

    m_fCrowMultipleHP = 1.f;
    m_fCrowMultipleAttack = 1.f;
    m_fCrowMultipleDefense = 1.f;
    m_fCrowMultipleExp = 1.f;
	m_fCrowMultipleDrop = 1.f;

	return S_OK;
}

unsigned int GLLandMan::GetCrowListCount() const
{
    return m_CrowDrop.size();
}

GLCrow* GLLandMan::GetCrow(DWORD dwID) const
{
    CROW_DROP_MAP_CITER iter = m_CrowDrop.find(dwID);
    if (iter != m_CrowDrop.end())
        return iter->second;
    else
        return NULL;
}

void GLLandMan::SetCrow(DWORD dwID, GLCrow* pCrow)
{
    if ( pCrow == NULL )
        return;

    ResetCrow(dwID);    

    CROW_DROP_MAP_VALUE dropCrowValue(dwID, pCrow);
    m_CrowDrop.insert(dropCrowValue);
    m_pLandManager->dropCrowReferenceMap.insert(dropCrowValue);
}

void GLLandMan::ResetCrow(DWORD dwID)
{
    {
        CROW_DROP_MAP_CITER iter(m_CrowDrop.find(dwID));
        if (iter != m_CrowDrop.end())
            m_CrowDrop.erase(iter);
    }
    {
        CROW_DROP_MAP& _dropCrowMap = m_pLandManager->dropCrowReferenceMap;
        CROW_DROP_MAP_CITER iter(_dropCrowMap.find(dwID));
        if (iter != _dropCrowMap.end())
            _dropCrowMap.erase(iter);
    }
}

GLCrow* GLLandMan::GetCrowByID(const SNATIVEID& sID) const
{
    if (sID == NATIVEID_NULL())
        return NULL;

    for (CROW_DROP_MAP_CITER iter=m_CrowDrop.begin(); iter!=m_CrowDrop.end(); ++iter)
    {
        GLCrow* pCrow(iter->second);
        if ( pCrow == NULL )
            continue;

        if ( pCrow->m_sNativeID == sID)
            return pCrow;
    }
    return NULL;
}

//	특정 좌표의 Crow를 찾는다.
GLCrow* GLLandMan::GetCrowByID(const SNATIVEID& sID, WORD wPosX, WORD wPosY) const
{
    if (sID == NATIVEID_NULL())
        return NULL;

    for (CROW_DROP_MAP_CITER iter=m_CrowDrop.begin(); iter!=m_CrowDrop.end(); ++iter)
    {
        GLCrow* pCrow = iter->second;
        if (pCrow && sID == pCrow->m_sNativeID)
        {
            int nPosX, nPosY;
            const D3DXVECTOR3& vPos = pCrow->GetPosition();
            m_sLevelAxisInfo.Convert2MapPos(vPos.x, vPos.z, nPosX, nPosY);
            if (wPosX == nPosX && wPosY == nPosY)
                return pCrow;
        }
    }
    return NULL;
}


GLSummonField* GLLandMan::GetSummon(DWORD dwID) const
{
    SUMMON_DROP_MAP_CITER iter = m_SummonDrop.find(dwID);
    if (iter != m_SummonDrop.end())
        return iter->second;
    else
        return NULL;
}

void GLLandMan::SetSummon(DWORD dwID, GLSummonField* pSummon)
{
    if (!pSummon)
        return;

    SUMMON_DROP_MAP_ITER iter = m_SummonDrop.find(dwID);
    if (iter != m_SummonDrop.end())
        m_SummonDrop.erase(iter);

    SUMMON_DROP_MAP_VALUE dropSummonValue(dwID, pSummon);
    m_SummonDrop.insert(dropSummonValue);
    m_pLandManager->dropSummonReferenceMap.insert(dropSummonValue);
}

void GLLandMan::ResetSummon(DWORD dwID)
{
    {
        SUMMON_DROP_MAP_CITER iter = m_SummonDrop.find(dwID);
        if (iter != m_SummonDrop.end())
            m_SummonDrop.erase(iter);
    }
    {
        SUMMON_DROP_MAP& _dropSummonMap = m_pLandManager->dropSummonReferenceMap;
        SUMMON_DROP_MAP_CITER iter(_dropSummonMap.find(dwID));
        if (iter != _dropSummonMap.end())
            _dropSummonMap.erase(iter);
    }
}

GLMaterial* GLLandMan::GetMaterial(DWORD dwID) const
{
    MATERIAL_DROP_MAP_CITER iter = m_MaterialDrop.find(dwID);
    if (iter != m_MaterialDrop.end())
        return iter->second;
    else
        return NULL;
}

void GLLandMan::SetMaterial(DWORD dwID, GLMaterial* pMaterial)
{
    MATERIAL_DROP_MAP_ITER iter = m_MaterialDrop.find(dwID);
    if (iter != m_MaterialDrop.end())
        m_MaterialDrop.erase(iter);
    
    MATERIAL_DROP_MAP_VALUE dropMaterialValue(dwID, pMaterial);
    m_MaterialDrop.insert(dropMaterialValue);
    m_pLandManager->dropMaterialReferenceMap.insert(dropMaterialValue);
}

void GLLandMan::ResetMaterial(DWORD dwID)
{
    {
        MATERIAL_DROP_MAP_CITER iter = m_MaterialDrop.find(dwID);
        if (iter != m_MaterialDrop.end())
            m_MaterialDrop.erase(iter);
    }
    {
        MATERIAL_DROP_MAP& _dropMaterialMap = m_pLandManager->dropMaterialReferenceMap;
        MATERIAL_DROP_MAP_CITER iter(_dropMaterialMap.find(dwID));
        if (iter != _dropMaterialMap.end())
            _dropMaterialMap.erase(iter);
    }
}

CItemDrop* GLLandMan::GetDropItem(DWORD dwGlobID) const
{
    ITEM_DROP_MAP_CITER iter = m_ItemDropData.find(dwGlobID);
    if (iter != m_ItemDropData.end())
        return iter->second;
    else
        return NULL;
}

void GLLandMan::SetDropItem(DWORD dwGlobID, CItemDrop* pDropItem)
{
    if (!pDropItem)
        return;

    ITEM_DROP_MAP_ITER iter = m_ItemDropData.find(dwGlobID);
    if (iter != m_ItemDropData.end())
        m_ItemDropData.erase(iter);
    
    ITEM_DROP_MAP_VALUE dropItemValue(dwGlobID, pDropItem);
    m_ItemDropData.insert(dropItemValue);
    m_pLandManager->dropItemReferenceMap.insert(dropItemValue);
}

void GLLandMan::ResetDropItem(DWORD dwGlobID)
{
    {
        ITEM_DROP_MAP_CITER iter = m_ItemDropData.find(dwGlobID);
        if (iter != m_ItemDropData.end())
            m_ItemDropData.erase(iter);
    }
    {
        ITEM_DROP_MAP& _dropItemMap = m_pLandManager->dropItemReferenceMap;
        ITEM_DROP_MAP_CITER iter(_dropItemMap.find(dwGlobID));
        if (iter != _dropItemMap.end())
            _dropItemMap.erase(iter);
    }
}

CMoneyDrop* GLLandMan::GetDropMoney(DWORD dwGlobID) const
{
    MONEY_DROP_MAP_CITER iter = m_MoneyDropData.find(dwGlobID);
    if (iter != m_MoneyDropData.end())
        return iter->second;
    else
        return NULL;
}

void GLLandMan::SetDropMoney(DWORD dwGlobID, CMoneyDrop* pDropMoney)
{
    if (!pDropMoney)
        return;

    MONEY_DROP_MAP_ITER iter = m_MoneyDropData.find(dwGlobID);
    if (iter != m_MoneyDropData.end())
        m_MoneyDropData.erase(iter);
    
    MONEY_DROP_MAP_VALUE dropMoneyValue(dwGlobID, pDropMoney);
    m_MoneyDropData.insert(dropMoneyValue);
    m_pLandManager->dropMoneyReferenceMap.insert(dropMoneyValue);
}

void GLLandMan::ResetDropMoney(DWORD dwGlobID)
{
    {
        MONEY_DROP_MAP_CITER iter = m_MoneyDropData.find(dwGlobID);
        if (iter != m_MoneyDropData.end())
            m_MoneyDropData.erase(iter);
    }
    {
        MONEY_DROP_MAP& _dropMoneyMap = m_pLandManager->dropMoneyReferenceMap;
        MONEY_DROP_MAP_CITER iter(_dropMoneyMap.find(dwGlobID));
        if (iter != _dropMoneyMap.end())
            _dropMoneyMap.erase(iter);
    }
}

ServerActor* const GLLandMan::GetActor(const Faction::GLActorID& _actorID)
{
    const EMCROW emCrow(_actorID.actorType);
    const DWORD dwActorID(_actorID.actorID_Num);
    switch ( emCrow )
    {
    case CROW_PC:
        return (ServerActor*)GetCharByDbNum(dwActorID);
    case CROW_NPC:
    case CROW_MOB:	
        return (ServerActor*)GetCrow(dwActorID);
    case CROW_ITEM:
        return (ServerActor*)GetDropItem(dwActorID);
    case CROW_MONEY:
        return (ServerActor*)GetDropMoney(dwActorID);	
    case CROW_SUMMON:
        return (ServerActor*)GetSummon(dwActorID);
    case CROW_MATERIAL:
        return (ServerActor*)GetMaterial(dwActorID);
    case CROW_PET:
    case CROW_ZONENAME:
    case CROW_GATENAME:
        break;
    }
    return NULL;
}


ServerActor* const GLLandMan::GetActor( const STARGETID &sTargetID )
{
	if ( m_pGLGaeaServer )
	{
		return m_pGLGaeaServer->GetTarget( this, sTargetID );
	}
	return NULL;
}

BOOL GLLandMan::IsCollision(const D3DXVECTOR3 &vP1, /*const*/ D3DXVECTOR3 &vP2, EMCOLLMODE emCullMode )
{
    return m_CollisionMap.IsCollision_NoneConst( vP1, vP2, NULL, FALSE, NULL, NULL, emCullMode );
}

CEVENT_NODE_SERVER* GLLandMan::GetLandEvent(DWORD dwGlobID)const
{
    if (dwGlobID < MAXLANDEVENT)
        return m_pLandEvent->GetEvent(dwGlobID);
    else
        return NULL;        
}

const BOOL GLLandMan::DropOutLandEvent(DWORD dwGUID)
{
    return m_pLandEvent->RemoveEvent(dwGUID);
}

HRESULT GLLandMan::SetMap( BOOL bLevelEdit )
{
	HRESULT hr;

	ClearDropObj();

	//ResetMobSchMan(this);
    SetMobSchManRegenLayer( 0 );

	hr = CreateLandTree();

	if (FAILED(hr))
		return hr;

	// 몹 스케쥴을 한번 공회전 시켜줌. ( 몹 GEN )
	if(bLevelEdit)
	{
		//m_MobSchMan->InitMobScheduleState();
        BOOST_FOREACH( MOB_SCH_MAN_MAP_VALUE& mapValue, m_mapMobSchManRegen )
            mapValue.second->InitMobScheduleState();
	}

	//m_MobSchMan->FrameMove(0.0f, 0.0f, IsInstantMap());
    BOOST_FOREACH( MOB_SCH_MAN_MAP_VALUE& mapValue, m_mapMobSchManRegen )
        mapValue.second->FrameMove(0.0f, 0.0f, IsInstantMap());

	m_bInit = TRUE;
	return S_OK;
}

HRESULT GLLandMan::CreateLandTree()
{
	HRESULT hr;
	if (!m_spNaviMesh)
		return E_FAIL;

	m_spNaviMesh->GetAABB(m_vMax, m_vMin);

	hr = m_LandTree.SetState(m_vMax.x, m_vMax.z, m_vMin.x, m_vMin.z, m_sLevelHead.m_eDivision);
	if (FAILED(hr))
		return hr;

	hr = m_LandTree.MakeTree();
	if (FAILED(hr))
		return hr;

    m_ItemDropData.clear();
    m_MoneyDropData.clear();
    m_CrowDrop.clear();
    m_MaterialDrop.clear();
    m_SummonDrop.clear();

	return S_OK;
}

BOOL GLLandMan::RequestGenItemFieldFB ( GLMSG::SNET_REQ_GENITEM_FLD_FB * pNetMsg )
{
	const SNATIVEID &sNID = pNetMsg->sNID;

	const SITEM* pITEM = GLogicData::GetInstance().GetItem( sNID );
	if (!pITEM)
        return FALSE;

	//	아이템 발생.
	SITEMCUSTOM sITEM_NEW( sNID );
	CTime cTIME = CTime::GetCurrentTime();
	sITEM_NEW.tBORNTIME = cTIME.GetTime();

	//	아이템의 사용 횟수 표시. ( 소모품일 경우 x 값, 일반 물품 1 )
	sITEM_NEW.wTurnNum = pITEM->GETAPPLYNUM();

	//	발생된 정보 등록.
	sITEM_NEW.cGenType = pNetMsg->emGENTYPE;
	sITEM_NEW.guid = pNetMsg->guid;

	// 아이템 에디트에서 입력한 개조 등급 적용 ( RequestGenItem, 아직은 때가 아닌듯 )
	//sITEM_NEW.cDAMAGE = (BYTE)pITEM->sBasicOp.wGradeAttack;
	//sITEM_NEW.cDEFENSE = (BYTE)pITEM->sBasicOp.wGradeDefense;

	sITEM_NEW.GenerateBasicStat( true );
	sITEM_NEW.GenerateLinkSkill();
	sITEM_NEW.GenerateAddOption( true );

	m_pGLGaeaServer->GetItemLimit()->LogItemBasicStatOption( sITEM_NEW );
	m_pGLGaeaServer->GetItemLimit()->LogItemLinkSkillOption( sITEM_NEW );
	m_pGLGaeaServer->GetItemLimit()->LogItemAddonOption( sITEM_NEW );

	//	랜덤 옵션 생성.
	if( sITEM_NEW.GENERATE_RANDOM_OPT() )
	{
        GLItemLimit* pItemLimit = m_pGLGaeaServer->GetItemLimit();
		pItemLimit->LogItemRandomOption( sITEM_NEW );
	}

	DropItem( pNetMsg->vPos, &sITEM_NEW, pNetMsg->emHoldGroup, pNetMsg->dwHoldGID );

	return TRUE;
}

BOOL GLLandMan::RequestNonConfrontMode ( GLMSG::SNET_GM_NONCONFRONT_MODE * pNetMsg )
{
	if ( GetBaseMapID() != pNetMsg->sMapID )
	{
		return FALSE;
	}

	m_bNonConfrontMode = (0!=pNetMsg->bMode);

	SendMsgPC ( (NET_MSG_GENERIC*)pNetMsg );

	return TRUE;
}

void GLLandMan::ADD_GROUPMEMBER ( const std::string &strGroupName, const DWORD dwGlobID )
{
	if( strGroupName.empty() ) 
		return;

	//SETGROUPMEM setGroupMem;	
	//setGroupMem = m_mapMobGroup[strGroupName];
	//setGroupMem.insert( dwGlobID );
	//m_mapMobGroup[strGroupName] = setGroupMem;

	// [shhan][2015.09.22] 만약 이곳에서 또 hang 문제가 발생하면, 속도는 느리더라도 set 을 vector 로 변경시켜야겠다.
	//
	m_mapMobGroup[strGroupName].insert( dwGlobID );
}

void GLLandMan::DEL_GROUPMEMBER ( const std::string &strGroupName, const DWORD dwGlobID )
{
	if( strGroupName.empty() ) 
		return;

	//SETGROUPMEM setGroupMem;
	//setGroupMem = m_mapMobGroup[strGroupName];
	//setGroupMem.erase( setGroupMem.find(dwGlobID) );
	//m_mapMobGroup[strGroupName] = setGroupMem;

	m_mapMobGroup[strGroupName].erase( dwGlobID );
}

const SETGROUPMEM& GLLandMan::GET_GROUPMEMBER ( const std::string &strGroupName ) const
{
	if( strGroupName.empty() ) 
	{
		static SETGROUPMEM setGroupMem_STATIC;
		return setGroupMem_STATIC;
	}

	MAPMOBGROUP_CITER iter = m_mapMobGroup.find( strGroupName );
	if ( iter == m_mapMobGroup.end() )
	{
		static SETGROUPMEM setGroupMem_STATIC;
		return setGroupMem_STATIC;
	}

    return (*iter).second;
}

BOOL GLLandMan::IS_GROUPMEMBER ( const std::string &strGroupName, const DWORD dwGlobID )
{
	if( strGroupName.empty() ) return FALSE;
	return ( m_mapMobGroup[strGroupName].find(dwGlobID) == m_mapMobGroup[strGroupName].end() ) ? FALSE : TRUE;
}

void GLLandMan::ADD_GROUPLEADER ( const std::string &strGroupName, const DWORD dwGlobID )
{
	if( strGroupName.empty() ) return;
	m_mapGroupInfo[strGroupName].dwLeaderID = dwGlobID;
}


void GLLandMan::DEL_GROUPLEADER ( const std::string &strGroupName )
{
	if( strGroupName.empty() ) return;
	m_mapGroupInfo[strGroupName].dwLeaderID = 0;
}


PGLCROW GLLandMan::GET_GROUPLEADER( const std::string &strGroupName )
{
	if( strGroupName.empty() ) return NULL;
	DWORD   dwLeaderID = m_mapGroupInfo[strGroupName].dwLeaderID;

	if( !IS_GROUPMEMBER( strGroupName, dwLeaderID ) ) return NULL;
	PGLCROW pGLCrow = GetCrow ( dwLeaderID );
	if( !pGLCrow || !pGLCrow->IsGroupMember() || !pGLCrow->IsGroupLeader() ) return NULL;

	return pGLCrow;
}

STARGETID GLLandMan::GET_GROUPTARGET ( const std::string &strGroupName )
{
	if( strGroupName.empty() )
	{
		STARGETID sTargetID;
		sTargetID.RESET();
		return sTargetID;
	}

	ServerActor* pTarget = m_pGLGaeaServer->GetTarget ( this, m_mapGroupInfo[strGroupName].sTargetID );
	if ( !pTarget || pTarget->isAction(GLAT_FALLING) || pTarget->isAction(GLAT_DIE) )
        m_mapGroupInfo[strGroupName].sTargetID.RESET();

	return m_mapGroupInfo[strGroupName].sTargetID;
}
void GLLandMan::SET_GROUPTARGET ( const std::string &strGroupName, const STARGETID &sTargetID )
{
	if( strGroupName.empty() )
        return;

	ServerActor* pTarget = m_pGLGaeaServer->GetTarget ( this, sTargetID );
	if( !pTarget )
        return;

	m_mapGroupInfo[strGroupName].sTargetID = sTargetID;
}

void GLLandMan::RESET_GROUPTARGET( const std::string &strGroupName )
{
	if( strGroupName.empty() )
        return;

	STARGETID sTargetID = STARGETID(CROW_PC,EMTARGET_NULL);
	m_mapGroupInfo[strGroupName].sTargetID = sTargetID;
}

void GLLandMan::ReserveDropCrow(
									  GLMobSchedule* pMobSchedule, const SNATIVEID sMobLinkID, 
									  const float fPositionX, const float fPositionZ, const float fMobLinkScale, const float fMobLinkDelay)
{
	// DropCrow()는 호출 즉시 해당 위치에 몹을 생성;
	// ReserveDropCrow()는 호출 후 설정 시간이 지난 후에 몹을 생성;
	//
	// 변경 전;
	// GLCrow::FrameMove() { case GLAT_FALLING: } 에서 설정 시간 만큼 대기후 DropCrow()를 호출;
	// 문제점 : 
	// GLAT_FALLING 상태에서 4초 대기 후(죽는 애니메이션이 4초) GLAT_DIE로 상태를 바꿔
	// 몬스터 리스트에서 삭제를 해야 하는데 설정 시간이 길 경우 삭제가 되지 못해 죽는 애니메이션이 반복 됐었음;
	//
	// 변경 후;
	// GLAT_FALLING 상태에서 최대 대기 시간은 4초를 넘기지 못함;
	// GLAT_FALLING -> GLAT_DIE로 넘어가기 직전 예약 몬스터가 있다면 GLLandMan::ReserveDropCrow() 함수를 통해
	// 예약 후 GLAT_DIE로 상태가 바뀜;

	// 문제점;
	// 비단 이 효과뿐만이 아닌 다른 많은 기능들이 액션(스킬) 효과로 통일 해 일괄적으로 관리 해야 할듯;
	// 이 효과는 패시브 스킬 형태로 죽으면 발동 되는 방식으로 만들면 좋을 듯;
	SDropCrow sDropCrow(pMobSchedule, sMobLinkID, fPositionX, fPositionZ, fMobLinkScale, fMobLinkDelay);
	this->m_DropCrowList.push_back(sDropCrow);
}

CItemDrop* GLLandMan::DropGenItem ( D3DXVECTOR3 &vPos, const SNATIVEID &sNID, EMITEMGEN emGENTYPE, EMGROUP emGroup, DWORD dwHoldGID )
{
	// 이벤트 아이템일 경우 에이젼트를 거쳐서 생성.
	// 실제 코드 확인해보면 에이전트서버에서 수량 제한 확인하는 부분이 있어야 하는데 지금은 없는 상태이다.
	// 에이전트까지 다녀올 이유가 현재는 없는 상태
	SITEMCUSTOM sItemCustom( sNID );
	if ( sItemCustom.IsEvent() != NULL )
	{
		GLMSG::SNET_REQ_GENITEM_AGT NetMsg;
		NetMsg.nCHANNEL = m_pGLGaeaServer->GetServerChannel();
		NetMsg.sNID = sNID;
		NetMsg.sMAPID = GetBaseMapID();
		NetMsg.emGENTYPE = emGENTYPE;
		NetMsg.emHoldGroup = emGroup;
		NetMsg.dwHoldGID = dwHoldGID;
		NetMsg.vPos = vPos;

		m_pGLGaeaServer->SENDTOAGENT(&NetMsg);
		return NULL;
	}   
    
    // 아이템 발생
	SITEMCUSTOM sITEM_NEW;
    if ( false == m_pGLGaeaServer->NewItemMake( sITEM_NEW, sNID, emGENTYPE ) )
    {
		return NULL;
    }

	sITEM_NEW.GenerateBasicStat( true );
	sITEM_NEW.GenerateLinkSkill();
	sITEM_NEW.GenerateAddOption( true );
	
	m_pGLGaeaServer->GetItemLimit()->LogItemBasicStatOption( sITEM_NEW );
	m_pGLGaeaServer->GetItemLimit()->LogItemLinkSkillOption( sITEM_NEW );
	m_pGLGaeaServer->GetItemLimit()->LogItemAddonOption( sITEM_NEW );

    // 아이템 에디트에서 입력한 개조 등급 적용 ( DropGetItem, 아직은 때가 아닌듯 )
	//sITEM_NEW.cDAMAGE = (BYTE)pITEM->sBasicOp.wGradeAttack;
	//sITEM_NEW.cDEFENSE = (BYTE)pITEM->sBasicOp.wGradeDefense;
	//	랜덤 옵션 생성.
	if ( sITEM_NEW.GENERATE_RANDOM_OPT() )
    {
        GLItemLimit* pItemLimit = m_pGLGaeaServer->GetItemLimit();
		pItemLimit->LogItemRandomOption( sITEM_NEW );
    }

	return DropItem( vPos, &sITEM_NEW, emGroup, dwHoldGID );	
}

CItemDrop* GLLandMan::DropItem(
    const D3DXVECTOR3& vPos,
    const SITEMCUSTOM* pItemCustom,
    EMGROUP emGroup,
    DWORD dwHoldGID)
{
	// 떨어질 위치 노드를 찾는다.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if (!pLandNode)
        return NULL;

	const SITEM* pItem = GLogicData::GetInstance().GetItem(pItemCustom->GetNativeID());
	if ( !pItem )
		return NULL;

	// Global ID를 할당한다.
	if ( m_pLandManager->freeItemID.empty() == true )
        return NULL;

	const DWORD dwGlobalID(m_pLandManager->freeItemID.front());

	//CItemDrop *pNewItem = m_pLandManager->itemMemoryPool.getMem();
	CItemDrop *pNewItem = m_pLandManager->itemMemoryPool.construct();

	// 아이템 정보 ( 종류, 위치 ).
	//
	pNewItem->sItemCustom = *pItemCustom;
	pNewItem->vPos = vPos;

	pNewItem->fAge = 0.0f;
	pNewItem->emGroup = emGroup;
	pNewItem->dwHoldGID = dwHoldGID;
	pNewItem->bLock = false;

	// 관리 ID 부여.
	//
	pNewItem->sMapID = GetGaeaMapID();
	pNewItem->dwGlobID = dwGlobalID;
	pNewItem->dwCeID = pLandNode->dwCID;
	
	//////////////////////////////////////////////////////////////////////////
	

	// 이쪽의 아이템 정보를 클라이언트에 송신;
	// 관리 노드 포인터.
	//
	SetDropItem(dwGlobalID, pNewItem);                                                // - 전역
	AddDropItemCount(pItemCustom->GetNativeID());

	pNewItem->pQuadNode = pLandNode;												// - 트리 노드.
	pNewItem->pCellList = pLandNode->pData->m_ItemList.ADDHEAD ( pNewItem );		// - 트리 노드.

    m_pLandManager->freeItemID.pop_front();

	return pNewItem;
}

BOOL GLLandMan::DropMoney(D3DXVECTOR3 &vPos, LONGLONG lnAmount, bool bDropMonster /* = FALSE */, 
						  EMGROUP emGroup/* =EMGROUP_ONE */, DWORD dwHoldGID/* =GAEAID_NULL  */)
{
	// 떨어질 위치 노드를 찾는다.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pLandNode )
        return	FALSE;

	// Global ID를 할당한다
	if ( m_pLandManager->freeMoneyID.empty() == true )
        return FALSE;

    const DWORD dwGlobalID(m_pLandManager->freeMoneyID.front());

	// 메모리를 할당.
	//
	//CMoneyDrop *pNewMoney(m_pLandManager->moneyMemoryPool.getMem());
	CMoneyDrop *pNewMoney(m_pLandManager->moneyMemoryPool.construct());

	

	// Money 정보 ( 수량, 위치 ).
	//
	pNewMoney->fAge = 0.0f;
	pNewMoney->vPos = vPos;
	pNewMoney->lnAmount = lnAmount;
	pNewMoney->emGroup = emGroup;
	pNewMoney->dwHoldGID = dwHoldGID;

	// 관리 ID 부여.
	//
	pNewMoney->sMapID = GetBaseMapID();
	pNewMoney->dwGlobID = dwGlobalID;
	pNewMoney->dwCeID = pLandNode->dwCID;

	pNewMoney->bDropMonster = bDropMonster;
	AddDropMoneyCount(lnAmount);

	// 관리 노드 포인터.
	//
	SetDropMoney(dwGlobalID, pNewMoney);

	pNewMoney->pQuadNode = pLandNode;											// - 트리 노드.
	pNewMoney->pCellList = pLandNode->pData->m_MoneyList.ADDHEAD ( pNewMoney );	// - 트리 노드.

    m_pLandManager->freeMoneyID.pop_front();

	return TRUE;
}


DWORD GLLandMan::DropCrow( GLMobSchedule *pMobSch, bool bFirstCreateForInstMap )
{
	HRESULT hr;

	if ( !pMobSch->m_strGroupName.empty())
	{
		PGLCROW pGLLeader = GET_GROUPLEADER( pMobSch->m_strGroupName );

		// 연산자 shortcut으로, pGLLeader 가 NULL 이면, 뒤의 조건은 검사하지 않는다 //
		if (pGLLeader != NULL && !pGLLeader->GetMobSchedule()->m_bEachregen)
		{
			STARGETID sTarget = GET_GROUPTARGET(pMobSch->m_strGroupName);

			if (!pMobSch->m_bALive)
			{
				pMobSch->m_fTimer  = static_cast<float>(
					sc::Random::getInstance().RandomNumber(pMobSch->m_fReGenTimeMin, pMobSch->m_fReGenTimeMax));
			}

			if (!sTarget.ISNULL())
				return UINT_MAX;
		}
	}


	if( m_sLevelEtcFunc.m_bUseFunction[ EMETCFUNC_REGENONCE ] )
	{
		if ( pMobSch->m_dwVolatFlags & GLMobSchedule::EM_ONCE &&
			pMobSch->m_strGroupName != "" ) 
		{
			PGLCROW pGLLeader = GET_GROUPLEADER( pMobSch->m_strGroupName );

			if( pMobSch->m_bLeaderMonster )
				return UINT_MAX;

			if( !pGLLeader )
				return UINT_MAX;

			if( !pGLLeader->GetMobSchedule()->m_bGroupRegen )
				return UINT_MAX;

			if( !pGLLeader->GetMobSchedule()->m_bALive )
				return UINT_MAX;
		}
		else if( pMobSch->m_dwVolatFlags & GLMobSchedule::EM_ONCE &&
			pMobSch->m_strGroupName == "" )
		{
			return UINT_MAX;
		}
	}
	else
	{
		if ( pMobSch->m_dwVolatFlags & GLMobSchedule::EM_ONCE )
			return UINT_MAX;
	}

	D3DXVECTOR3 vPos = pMobSch->m_pAffineParts->vTrans;
	if ( pMobSch->m_bRendGenPos )
	{
		vPos = pMobSch->GetRendGenPos();
	}

	SNATIVEID sNativeID = pMobSch->m_CrowID;

	
	// 떨어질 위치 노드를 찾는다.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pLandNode )	return	UINT_MAX;

	// Global ID를 할당한다
    if ( m_pLandManager->freeCrowID.empty() == true )
        return UINT_MAX;
    
    const DWORD dwGlobalID(m_pLandManager->freeCrowID.front());    

	// 메모리를 할당
	GLCrow* pNewCrow = m_pGLGaeaServer->NEW_CROW();

    if ( pNewCrow ==NULL )
        return UINT_MAX;

	pNewCrow->SetGaeaID(dwGlobalID);

    DropCrowPreCreate(pNewCrow);

	hr = pNewCrow->CreateMob ( pMobSch, this, sNativeID, m_pd3dDevice, &vPos, bFirstCreateForInstMap );

	if ( FAILED(hr) )
	{
		m_pGLGaeaServer->RELEASE_CROW ( pNewCrow );

		return UINT_MAX;
	}


	// 관리 ID 부여.
	//
	pNewCrow->SetCurrentMap(m_mapID);
	pNewCrow->m_dwCeID = pLandNode->dwCID;

	// 관리 노드 포인터.
	//
	SetCrow(dwGlobalID, pNewCrow);											// - 전역.

	pNewCrow->m_pQuadNode = pLandNode;											// - 트리 노드.
	pNewCrow->m_pCellNode = pLandNode->pData->m_CROWList.ADDHEAD ( pNewCrow );	// - 트리 노드.

	// 알림 메시지 필요한지 검사후 메시지 발생.
	//
	if (pNewCrow->m_pCrowData->IsKnock())
	{
		const char* szMAP_NAME = m_pGLGaeaServer->GetMapName(GetBaseMapID());
        if (szMAP_NAME)
        {
            std::string strText(
                sc::string::format(
                    ID2SERVERTEXT("EMCROWACT_KNOCK"),
                    pNewCrow->m_pCrowData->GetName(),
                    szMAP_NAME));

		    GLMSG::SNET_SERVER_GENERALCHAT SendData;
		    if (SendData.SETTEXT(strText))
            {
                msgpack::sbuffer SendBuffer;
                msgpack::pack(SendBuffer, SendData);
		        m_pGLGaeaServer->SENDTOAGENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
            }
        }
	}

	// 그룹 몹이면 그룹 몹리스트에 넣는다.
	if ( pNewCrow->IsGroupMember() )
	{
		ADD_GROUPMEMBER( pNewCrow->GetGroupName(), dwGlobalID );
		if( pNewCrow->IsGroupLeader() )
		{
			ADD_GROUPLEADER( pNewCrow->GetGroupName(), dwGlobalID );
		}

		STARGETID targetID = GET_GROUPTARGET( pMobSch->m_strGroupName );

		PGLCROW pGLLeader = GET_GROUPLEADER( pMobSch->m_strGroupName );

		if (pGLLeader != NULL && !pGLLeader->GetMobSchedule()->m_bEachregen)
		{
			PGLCROW pGLLeader = GET_GROUPLEADER( pMobSch->m_strGroupName );

			if( pGLLeader )
				 targetID = pGLLeader->GetTargetID();
			else
				 targetID = GET_GROUPTARGET( pMobSch->m_strGroupName );
		}

		if( targetID.GaeaId != EMTARGET_NULL )
		{
			pNewCrow->NewTarget(targetID);
		}
	}    
	
    m_pLandManager->freeCrowID.pop_front();

	return dwGlobalID;
}

DWORD GLLandMan::DropCrow ( GLMobSchedule *pMobSch, SNATIVEID sNID, FLOAT fPosX, FLOAT fPosY, FLOAT fScale )
{
	HRESULT hr;

	D3DXVECTOR3 vPos;
	vPos.x = fPosX;
	vPos.z = fPosY;

	// 떨어질 위치 노드를 찾는다
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pLandNode )
		return	UINT_MAX;

    if ( m_pLandManager->freeCrowID.empty())
        return UINT_MAX;

    const DWORD dwGlobalID(m_pLandManager->freeCrowID.front());

	// 메모리를 할당
	PGLCROW pNewCrow = m_pGLGaeaServer->NEW_CROW();
	if ( NULL == pNewCrow )
	{
		return UINT_MAX;
	}

	if ( pMobSch != NULL )
		pMobSch->m_dwGlobID = dwGlobalID;

    DropCrowPreCreate(pNewCrow);

    /**
        서버에 Crow 를 생성 할때 원래 스케일을 기본 0 으로 줬는데
        MobNpcEdit 와 LevelEdit 에서 스케일 설정 기능이 추가됨으로 해서
        기본 스케일을 1.0f 기반으로 변경했으나 이 부분은
        SCROWACTION 구조체와 묶여있고 이 구조체로 저장된 데이터들이 이미 존재하고
        기본값을 0 으로 해놔서 이렇게 변환을 해 줌에 주의 한다.

     */
    fScale = (fScale == 0.f ? 1.f : fScale);

	hr = pNewCrow->CreateMob ( pMobSch, this, sNID, m_pd3dDevice, &vPos, FALSE, fScale );
	if ( FAILED(hr) )
	{
		m_pGLGaeaServer->RELEASE_CROW ( pNewCrow );

		return UINT_MAX;
	}

	// 관리 ID 부여
	pNewCrow->SetCurrentMap(m_mapID);	
	pNewCrow->m_dwCeID = pLandNode->dwCID;
	pNewCrow->SetGaeaID(dwGlobalID);

	// 관리 노드 포인터
	SetCrow(dwGlobalID, pNewCrow);											// - 전역

	pNewCrow->m_pQuadNode = pLandNode;											// - 트리 노드.
	pNewCrow->m_pCellNode = pLandNode->pData->m_CROWList.ADDHEAD ( pNewCrow );	// - 트리 노드.

	// 알림 메시지 필요한지 검사후 메시지 발생.
	//
	if (pNewCrow->m_pCrowData->IsKnock())
	{
		const char* szMAP_NAME = m_pGLGaeaServer->GetMapName(GetBaseMapID());
        if (szMAP_NAME)
        {
            std::string strText(
                sc::string::format(
                    ID2SERVERTEXT("EMCROWACT_KNOCK"),
                    pNewCrow->m_pCrowData->GetName(),
                    szMAP_NAME));

		    GLMSG::SNET_SERVER_GENERALCHAT NetMsg;
		    if (NetMsg.SETTEXT(strText))
            {
                msgpack::sbuffer SendBuffer;
                msgpack::pack(SendBuffer, NetMsg);
		        m_pGLGaeaServer->SENDTOAGENT(NET_MSG_SERVER_GENERALCHAT, SendBuffer);
            }
        }
	}

	m_listDelGlobID.push_back( dwGlobalID );

	// 그룹 몹이면 그룹 몹리스트에 넣는다.
	if ( pNewCrow->IsGroupMember() )
	{
		ADD_GROUPMEMBER( pNewCrow->GetGroupName(), dwGlobalID );		
		if( pNewCrow->IsGroupLeader() )
		{
			ADD_GROUPLEADER( pNewCrow->GetGroupName(), dwGlobalID );
		}
		STARGETID targetID = GET_GROUPTARGET( pMobSch->m_strGroupName );
		if( targetID.GaeaId != EMTARGET_NULL )
		{
			pNewCrow->NewTarget(targetID);
		}
	}

    m_pLandManager->freeCrowID.pop_front();

    return dwGlobalID;
}

DWORD GLLandMan::DropCrow ( SNATIVEID sNID, float fPosX, float fPosY, float fRotate, DWORD dwLayerIndex )
{
	if ( NULL == m_pGLGaeaServer )
		return UINT_MAX;

	/// 스케쥴러에 등록;
	GLMobSchedule* pMobSchedule = new GLMobSchedule();
	// m_pGLGaeaServer->NEW_MOBSCHEDULE(); 스케쥴러에서 관리하고 있어서 사용할 수 없다;
	if ( pMobSchedule )
	{
		// Regen 설정;
		pMobSchedule->SetRegenTimer( 0.f, 0.f );

		// Crow ID;
		pMobSchedule->SetCrowID( sNID );

		// Position;
		pMobSchedule->AddPosition( D3DXVECTOR3( fPosX, 0, fPosY ) );

		// Rotation;
		pMobSchedule->SetRotation( fRotate );

		// Ex Mode;
		//pMobSchedule->SetExMode();

		// Once Layer 에 스케쥴러 등록;
		if ( FALSE == AddMobSchManLayer( dwLayerIndex, true ) )
		{
			// MobSchMan 의 생성에 실패했다면 스케쥴러를 등록할 수 없다;
			// 스케쥴러를 제거하자;
			SAFE_DELETE( pMobSchedule );

			return UINT_MAX;
		}

		MOB_SCH_MAN_LIST_ITER it = m_mapMobSchManOnce.begin();
		for ( ; it != m_mapMobSchManOnce.end(); ++it )
		{
			SpMobScheduleManServer spMobSch = *it;
			if ( spMobSch->GetLayerIndex() == dwLayerIndex )
			{
				DWORD dwCrowGlobalID = spMobSch->DropCrow( pMobSchedule );

				GLCrow* pCrow = GetCrow( dwCrowGlobalID );
				if ( pCrow )
				{
					spMobSch->AddMobSch( pMobSchedule );
					m_listDelGlobID.push_back( dwCrowGlobalID );
					return dwCrowGlobalID;
				}
			}
		}

		// 등록에 실패할 경우;
		// 로직상 이곳에는 들어올 수 없다, 들어오면 안된다;
		//
		// [shhan][2016.08.02] 주석에 들어 올수 없다라고 되어 있는데 들어오는 상황이 생겨서, Server crash 발생함.
		//						spMobSch->AddMobSch( pMobSchedule ); 이전에는 실패시에도 셋팅이 되었지만, 성공할 경우 셋팅하도록 변경함.
		//						현재는 이쪽으로 들어올 수 있다. 하지만 crash 를 발생시키지 않도록 한다.
		sc::writeLogError( "Error!! Failed Drop Crow!!" );

		m_pGLGaeaServer->RELEASE_MOBSCHEDULE( pMobSchedule );
	}
	return UINT_MAX;
}

DWORD GLLandMan::DropCrow ( SNATIVEID sNID, WORD wPosX, WORD wPosY, float fRotate, DWORD dwLayerIndex )
{
	D3DXVECTOR3 vPos( 0.f, 0.f, 0.f );
	m_sLevelAxisInfo.MapPos2MiniPos( wPosX, wPosY, vPos.x, vPos.z );

	return DropCrow( sNID, vPos.x, vPos.z, fRotate, dwLayerIndex );
}

void GLLandMan::DropCrowPreCreate (PGLCROW pNewCrow )
{
    if (pNewCrow == 0)
    {
        return;
    }

    pNewCrow->SetMultipleHP(GetCrowMultipleHP());
    pNewCrow->SetMultipleAttack(GetCrowMultipleAttack());
    pNewCrow->SetMultipleDefense(GetCrowMultipleDefense());
    pNewCrow->SetMultipleExp(GetCrowMultipleExp());
	pNewCrow->SetMultipleDrop(GetCrowMultipleDrop());
}

DWORD GLLandMan::DropMaterial( GLMobSchedule *pMobSch, bool bFirstCreateForInstMap )
{
	HRESULT hr;

	D3DXVECTOR3 vPos = pMobSch->m_pAffineParts->vTrans;
	if ( pMobSch->m_bRendGenPos )
	{
		vPos = pMobSch->GetRendGenPos();
	}

	SNATIVEID sNativeID = pMobSch->m_CrowID;

	// 떨어질 위치 노드를 찾는다.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pLandNode )	return	UINT_MAX;

	// Global ID를 할당한다
    if ( m_pLandManager->freeMaterialID.empty() == true )
        return UINT_MAX;

    const DWORD dwGlobalID(m_pLandManager->freeMaterialID.front());

	// 메모리를 할당.
	//
	PGLMATERIAL pNewMaterial = m_pGLGaeaServer->NEW_MATERIAL();
	if ( NULL == pNewMaterial )
	{
		return UINT_MAX;
	}
	//PGLCROW pNewCrow = new GLCrow;

	hr = pNewMaterial->CreateMaterial ( pMobSch, this, sNativeID, m_pd3dDevice, &vPos );
	if ( FAILED(hr) )
	{
		m_pGLGaeaServer->RELEASE_MATERIAL ( pNewMaterial );

		return UINT_MAX;
	}

	// 관리 ID 부여
	pNewMaterial->SetCurrentMap(m_mapID);
	pNewMaterial->SetGaeaID(dwGlobalID);
	pNewMaterial->m_dwCeID = pLandNode->dwCID;	

	// 관리 노드 포인터
	SetMaterial(dwGlobalID, pNewMaterial);	// - 전역

	pNewMaterial->m_pQuadNode = pLandNode;											// - 트리 노드.
	pNewMaterial->m_pCellNode = pLandNode->pData->m_MaterialList.ADDHEAD ( pNewMaterial );	// - 트리 노드.

    m_pLandManager->freeMaterialID.pop_front();

    return dwGlobalID;
}

DWORD GLLandMan::DropMaterial ( SNATIVEID sNID, float fPosX, float fPosY )
{
	HRESULT hr;

	boost::shared_ptr<NavigationMesh> spNavi = GetNavi();
	if ( !spNavi )
		return UINT_MAX;

	D3DXVECTOR3 vPos(fPosX,0,fPosY);

	D3DXVECTOR3 vCollision;
	DWORD dwCoID;
	BOOL bCol;

	// 소수점 정밀도 관련 문제 확인 IsCollision
	spNavi->IsCollision ( vPos+D3DXVECTOR3(0.0f, 600.0f,0.0f), vPos+D3DXVECTOR3(0.0f, -500.0f,0.0f), vCollision, dwCoID, bCol );
	if ( !bCol )
		return UINT_MAX;

	// 떨어질 위치 노드를 찾는다.
	//
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)fPosX, (int)fPosY );
	if ( !pLandNode )	return	UINT_MAX;

	// Global ID를 할당한다
    if ( m_pLandManager->freeMaterialID.empty() == true )
        return UINT_MAX;

    const DWORD dwGlobalID(m_pLandManager->freeMaterialID.front());

	// 메모리를 할당.
	//
	PGLMATERIAL pNewMaterial = m_pGLGaeaServer->NEW_MATERIAL();
	if ( NULL == pNewMaterial )
	{
		return UINT_MAX;
	}

	hr = pNewMaterial->CreateMaterial ( NULL, this, sNID, m_pd3dDevice, &vPos );
	if ( FAILED(hr) )
	{
		m_pGLGaeaServer->RELEASE_MATERIAL ( pNewMaterial );

		return UINT_MAX;
	}

	// 관리 ID 부여
	pNewMaterial->SetCurrentMap(m_mapID);
	pNewMaterial->SetGaeaID(dwGlobalID);
	pNewMaterial->m_dwCeID = pLandNode->dwCID;

	// 관리 노드 포인터
	SetMaterial(dwGlobalID, pNewMaterial); // - 전역.

	pNewMaterial->m_pQuadNode = pLandNode;											        // - 트리 노드.
	pNewMaterial->m_pCellNode = pLandNode->pData->m_MaterialList.ADDHEAD ( pNewMaterial );	// - 트리 노드.

	m_listDelGlobIDMat.push_back( dwGlobalID );

    m_pLandManager->freeMaterialID.pop_front();

    return dwGlobalID;
}

DWORD GLLandMan::DropMaterial ( SNATIVEID sNID, WORD wPosX, WORD wPosY )
{
	D3DXVECTOR3 vPos(0,0,0);
	m_sLevelAxisInfo.MapPos2MiniPos ( wPosX, wPosY, vPos.x, vPos.z );

	return DropMaterial ( sNID, vPos.x, vPos.z );
}

DWORD GLLandMan::DropSummon ( PGLSUMMON pInSummonInfo, const D3DXVECTOR3 &vPos, const STARGETID &sTarget, DWORD dwMaxSummon, DWORD dwAIType, bool bEventSkill )
{
	if ( !pInSummonInfo )
		return UINT_MAX;

	// 요청 케릭터가 유효성 체크
	ServerActor* pOwner = m_pGLGaeaServer->GetActor( this, pInSummonInfo->m_sOwnerID );
	//GLChar* pOwner = m_pGLGaeaServer->GetChar( pInSummonInfo->m_dwOwnerGaeaID );
	if ( !pOwner ){
		return UINT_MAX;
	}

	// 소수점 정밀도 관련 문제 확인 IsCollision
	D3DXVECTOR3 vPosition(vPos.x, vPos.y, vPos.z);
	D3DXVECTOR3 vCollision;

	if ( !IsCollisionNavi ( vPosition+D3DXVECTOR3(0.0f,+10.0f,0.0f), vPosition+D3DXVECTOR3(0.0f,-10.0f,0.0f), vCollision ) )
	{
		boost::shared_ptr<NavigationMesh> spNavi = GetNavi();
		if ( spNavi )
		{
			DWORD CurrentCellID = spNavi->FindClosestCell(vPosition);
			vPosition = spNavi->SnapPointToCell(CurrentCellID, vPosition);
		}
	}
	else
	{
		vPosition = vCollision;
	}

	// 떨어질 위치 노드를 찾는다.
	LANDQUADNODE* pLandNode = m_LandTree.FindNode ( (int)vPosition.x, (int)vPosition.z );
	if ( !pLandNode )	return	UINT_MAX;

	// GUID 생성
    if ( m_pLandManager->freeSummonID.empty() == true )
        return UINT_MAX;

    const DWORD dwGlobalID(m_pLandManager->freeSummonID.front());

	// 소환수 생성
	PGLSUMMONFIELD pNewSummon(NULL);
	pNewSummon = m_pGLGaeaServer->NEW_SUMMON ();
	if ( NULL == pNewSummon )
	{
		return UINT_MAX;
	}
	HRESULT hr = pNewSummon->Create ( m_pd3dDevice, this, pInSummonInfo, pOwner, vCollision, sTarget );
	if ( FAILED ( hr ) )
	{
		// 생성실패 처리
		m_pGLGaeaServer->RELEASE_SUMMON ( pNewSummon );
		return UINT_MAX;
	}

	// 관리 ID 부여.
	//
	pNewSummon->SetCurrentMap(m_mapID);
	pNewSummon->SetGaeaID(dwGlobalID);	
	pNewSummon->m_dwCeID   = pLandNode->dwCID;	
	pNewSummon->SetAIType( dwAIType );
	pNewSummon->SetEnableEventSkill( bEventSkill );

	// 관리 노드 포인터.
	//
	SetSummon(dwGlobalID, pNewSummon); // - 전역.

	//pNewSummon->m_pGlobNode = m_GlobSummonList.ADDHEAD ( pNewSummon );				    // - 전역.
	pNewSummon->m_pQuadNode = pLandNode;											    // - 트리 노드.
	pNewSummon->m_pCellNode = pLandNode->pData->m_SummonList.ADDHEAD ( pNewSummon );	// - 트리 노드.

	// Note : 소환 갯수 검사
	//
	SUMMON_TYPE emType = pNewSummon->GETSUMMONTYPE();
	DWORD dwTypeMaxSummon = 0;
	if ( emType < SUMMON_TYPE_SIZE && emType != SUMMON_TYPE_NONE )
	{
		// 타입별 최대 소환 갯수 검사;
		dwTypeMaxSummon = GLCONST_SUMMON::dwMaxSummon[emType];
	}

	std::tr1::shared_ptr< AISystem::GLScriptedFSM > spScriptedFSM =
		AISystem::GLFSMContainer::Instance()->GetFSM( dwAIType );
	DWORD dwAITypeMaxSummon = MAXDWORD;
	if ( spScriptedFSM )
	{
		dwAITypeMaxSummon = spScriptedFSM->GetTypeMaxCount();
	}

	// 각 AI Type별 제한 + 10개 제한;
	if ( dwTypeMaxSummon <= pOwner->GetChildSummonTypeCount( emType ) ||
		dwAITypeMaxSummon <= pOwner->GetChildSummonAITypeCount( dwAIType ) ||
		10 <= pOwner->GetChildCrowCount() )
	{
		bool bFalling = false;

		// Script 관리 소환수가 아닌 경우;
		if ( SUMMON_TYPE_NORMAL != emType )
		{
			if ( dwTypeMaxSummon <= pOwner->GetChildSummonTypeCount( emType ) ||
				10 <= pOwner->GetChildCrowCount() )
			{
				bFalling = true;
			}
		}
		else
		{
			bFalling = true;
		}

		if ( bFalling )
		{
			Faction::GLActorID sRemoveSummonID = pOwner->RemoveFirstChildSummon( emType, dwAIType );
			if ( sRemoveSummonID.actorType == CROW_SUMMON )
			{
				PGLSUMMONFIELD pSummon = GetSummon( sRemoveSummonID.actorID_Num );
				if ( pSummon && pSummon->IsValidBody() )
					pSummon->TurnAction( GLAT_FALLING );
			}
		}
	}

	// 같은 CrowID의 최대 소환 개수 제한의 경우;
	if ( dwMaxSummon <= pOwner->GetChildSummonIDCount( pNewSummon->m_sSummonID ) )
	{
		Faction::GLActorID sRemoveSummonID = pOwner->RemoveFirstChildSummon( emType, dwAIType, pNewSummon->m_sSummonID );
		if ( sRemoveSummonID.actorType == CROW_SUMMON )
		{
			PGLSUMMONFIELD pSummon = GetSummon( sRemoveSummonID.actorID_Num );
			if ( pSummon && pSummon->IsValidBody() )
				pSummon->TurnAction( GLAT_FALLING );
		}
	}

	// 주인에게 GUID 등록
	//
	//pOwner->m_dwSummonGUIDList.insert(pOwner->m_dwSummonGUIDList.end(), dwGlobalID);
	pOwner->InsertChildCrow( Faction::GLActorID( CROW_SUMMON, dwGlobalID ) );

    m_pLandManager->freeSummonID.pop_front();

    return dwGlobalID;	
}

BOOL GLLandMan::DropOutSummon ( DWORD dwGlobID, const BOOL bErrlog )
{
	GASSERT ( dwGlobID<MAXSUMMON );

    GLSummonField* pSummon = GetSummon(dwGlobID);
	if (!pSummon)
	{  
		if (bErrlog)
			sc::writeLogError(sc::string::format("GLLandMan::DropOutSummon() - m_SummonArray[%d]==NULL", dwGlobID ));
		return FALSE;
	}

	// Note : 주인에게 해지
	ServerActor* pSummonOwner = m_pGLGaeaServer->GetActor( this, pSummon->m_sOwnerID );
	//GLChar* pSummonOwner = m_pGLGaeaServer->GetChar64(pSummon->m_OwnerDbNum, pSummon->m_OwnerSvrGroupNum);
	if ( !pSummonOwner )
	{
		if ( bErrlog )
			sc::writeLogError( "GLLandMan::DropOutSummon - pOwner is NULL." );
		return FALSE;
	}
	
	//pSummonOwner->m_dwSummonGUIDList.remove(dwGlobID);
	pSummonOwner->RemoveChildCrow( Faction::GLActorID( CROW_SUMMON, dwGlobID ) );

	// Global-ID 해지
    m_pLandManager->freeSummonID.push_back(dwGlobID);
	ResetSummon(dwGlobID);
	
	LANDQUADNODE* pLandNode = pSummon->m_pQuadNode;
	if ( pLandNode && pLandNode->pData )
	{
		if ( pLandNode->pData->m_SummonList.DELNODE ( pSummon->m_pCellNode ) == false )
		{
			const MapID& _charMapID(pSummon->GetCurrentMap());
			const MapID& _landMapID(GetMapID());
			sc::writeLogError(sc::string::format("DropOutSummon() : pSummon->m_pCellNode is invalid, SummonMapID(%1%/%2%, %3%/%4%), LandMapID(%5%/%6%, %7%/%8%)", 
				_charMapID.getBaseMapID().Mid(), _charMapID.getBaseMapID().Sid(), _charMapID.getGaeaMapID().Mid(), _charMapID.getGaeaMapID().Sid(),
				_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
		}
	}

	// 메모리 해지.
	//
	m_pGLGaeaServer->RELEASE_SUMMON ( pSummon );

	return TRUE;
}

BOOL GLLandMan::DropOutItem( DWORD dwGlobID, bool DbUpdate /*= false*/ )
{
	GASSERT(dwGlobID < MAXITEM);
	CItemDrop* pItemDrop = GetDropItem(dwGlobID);
    if (!pItemDrop)
        return FALSE;
	
	// Global-ID 해지
    m_pLandManager->freeItemID.push_back(dwGlobID);
	ResetDropItem(dwGlobID);

	// QuadNode-List 해지.
	//
	LANDQUADNODE* pLandNode = pItemDrop->pQuadNode;
	if ( pLandNode && pLandNode->pData )
	{
		if ( pLandNode->pData->m_ItemList.DELNODE ( pItemDrop->pCellList ) == false )
		{
			const SNATIVEID& _itemGaeaMapID(pItemDrop->sMapID);
			const MapID& _landMapID(GetMapID());
			sc::writeLogError(sc::string::format("DropOutItem() : pItemDrop->pCellList is invalid, ItemGaeaMapID(%1%/%2%), LandMapID(%3%/%4%, %5%/%6%)", 
				_itemGaeaMapID.Mid(), _itemGaeaMapID.Sid(),
				_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
		}
	}

	// 팻카드일 경우 아이템이 사라지면 팻DB 삭제
	const SITEM* pITEM = GLogicData::GetInstance().GetItem( pItemDrop->sItemCustom.GetNativeID() );
	if ( pITEM && pITEM->sBasicOp.emItemType == ITEM_PET_CARD && pItemDrop->sItemCustom.PetDbNum() != 0 )
	{
		// 펫카드 로직 변경으로 DB삭제 제거하고 로그 추가
		sc::writeLogError(
            sc::string::format(
                "GLLandMan Error PetCard Delet -> PetNum : %1%",
                pItemDrop->sItemCustom.PetDbNum()));
	}

	// 디비 처리
	if ( DbUpdate )
	{
		if ( db::DB_NONE != pItemDrop->sItemCustom.GetDbState() && db::DB_INSERT != pItemDrop->sItemCustom.GetDbState() )
		{
			m_pGLGaeaServer->AddGameAdoJob( 
				db::DbActionPtr( 
				new db::ItemDelete( pItemDrop->sItemCustom.GetGuid(), pItemDrop->sItemCustom.GetDbState(), 9085 ) ) );
		}
	}

	// 메모리 해지.
	//
	//m_pLandManager->itemMemoryPool.releaseMem(pItemDrop);
	m_pLandManager->itemMemoryPool.destroy(pItemDrop);

	return TRUE;
}

BOOL GLLandMan::DropOutMoney ( DWORD dwGlobID )
{
	GASSERT(dwGlobID < MAXMONEY);
	
    CMoneyDrop* pMoneyDrop = GetDropMoney(dwGlobID);
	if (!pMoneyDrop)
		return FALSE;
	
	// Global-ID 해지
    m_pLandManager->freeMoneyID.push_back(dwGlobID);
	ResetDropMoney(dwGlobID);

	// QuadNode-List 해지
	LANDQUADNODE* pLandNode = pMoneyDrop->pQuadNode;
	if ( pLandNode && pLandNode->pData )
	{
		if ( pLandNode->pData->m_MoneyList.DELNODE ( pMoneyDrop->pCellList ) == false )
		{
			const SNATIVEID& _moneyGaeaMapID(pMoneyDrop->sMapID);
			const MapID& _landMapID(GetMapID());
			sc::writeLogError(sc::string::format("DropOutMoney() : pMoneyDrop->pCellList is invalid, MoneyGaeaMapID(%1%/%2%), LandMapID(%3%/%4%, %5%/%6%)", 
				_moneyGaeaMapID.Mid(), _moneyGaeaMapID.Sid(),
				_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
		}
	}

	// 메모리 해지.
	//
	//m_pLandManager->moneyMemoryPool.releaseMem(pMoneyDrop);
	m_pLandManager->moneyMemoryPool.destroy(pMoneyDrop);

	return TRUE;
}

BOOL GLLandMan::DropOutCrow ( DWORD dwGlobID )
{

	if( dwGlobID >= MAXCROW ) 
	{
		if( m_listDelGlobID.empty() )
			return FALSE;

		dwGlobID = m_listDelGlobID.back();
		m_listDelGlobID.pop_back();
	}
	else
	{
		if( !m_listDelGlobID.empty() )
		{
			LISTDELCROWID_ITER iter = m_listDelGlobID.begin();
			LISTDELCROWID_ITER iter_end = m_listDelGlobID.end();

			for ( ;iter != iter_end; iter++) 
			{
				if ( *iter  == dwGlobID ) 
				{
					m_listDelGlobID.erase(iter);
					break;
				}
			}
		}
	}

	GLCrow* pCrow = GetCrow(dwGlobID);
    if ( !pCrow )
		return FALSE;

	// 그룹 몹이면 그룹 몹리스트에서 뺀다
	if ( pCrow->IsGroupMember() )
	{
		DEL_GROUPMEMBER( pCrow->GetGroupName(), dwGlobID );
		if( pCrow->IsGroupLeader() )
		{
			DEL_GROUPLEADER( pCrow->GetGroupName() );
			m_mapGroupInfo[pCrow->GetGroupName()].sTargetID.RESET();
		}
	}

	// Global-ID 해지
    m_pLandManager->freeCrowID.push_back(dwGlobID);
	ResetCrow(dwGlobID);

	// QuadNode-List 해지.
	//
	LANDQUADNODE* pLandNode = pCrow->m_pQuadNode;
	if ( pLandNode && pLandNode->pData )
	{
		if ( pLandNode->pData->m_CROWList.DELNODE ( pCrow->m_pCellNode ) == false )
		{
			const MapID& _crowMapID(pCrow->GetCurrentMap());
			const MapID& _landMapID(GetMapID());
			sc::writeLogError(sc::string::format("DropOutCrow() : pCrow->m_pCellNode is invalid, CrowMapID(%1%/%2%, %3%/%4%), LandMapID(%5%/%6%, %7%/%8%)", 
				_crowMapID.getBaseMapID().Mid(), _crowMapID.getBaseMapID().Sid(), _crowMapID.getGaeaMapID().Mid(), _crowMapID.getGaeaMapID().Sid(),
				_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
		}
	}

	// 만일 몹 스케쥴을 승계(링크)시켰다면 몹스케쥴의 플래그를 그대로 둔다.
	if( !pCrow->m_pCrowData->m_sAction.m_bMobLink )
	{
		// 몹스케쥴의 생존 플래그를 오프시킴.
		GLMobSchedule* pMobSch = pCrow->GetMobSchedule();
		if ( pMobSch )
		{
			pMobSch->m_bALive = FALSE;
			//pMobSch->m_dwGlobID = UINT_MAX;
		}
	}

	// 메모리 해지
	m_pGLGaeaServer->RELEASE_CROW ( pCrow );
	//SAFE_DELETE ( pCrow );

	return TRUE;
}

BOOL GLLandMan::DropOutMaterial ( DWORD dwGlobID )
{
	if( dwGlobID >= MAXCROW ) 
	{
		if( m_listDelGlobIDMat.empty() )
			return FALSE;

		dwGlobID = m_listDelGlobIDMat.back();
		m_listDelGlobIDMat.pop_back();

	}
	else
	{
		if( !m_listDelGlobIDMat.empty() )
		{
			LISTDELCROWID_ITER iter = m_listDelGlobIDMat.begin();
			LISTDELCROWID_ITER iter_end = m_listDelGlobIDMat.end();

			for ( ;iter != iter_end; iter++) 
			{
				if ( *iter  == dwGlobID ) 
				{
					m_listDelGlobIDMat.erase(iter);
					break;
				}
			}
		}
	}

    GLMaterial* pMaterial = GetMaterial(dwGlobID);
	if (!pMaterial)
		return FALSE;

	// Global-ID 해지
    m_pLandManager->freeMaterialID.push_back(dwGlobID);
	ResetMaterial(dwGlobID);
	
	// QuadNode-List 해지
	LANDQUADNODE* pLandNode = pMaterial->m_pQuadNode;	
	if ( pLandNode && pLandNode->pData )
	{
		if ( pLandNode->pData->m_MaterialList.DELNODE ( pMaterial->m_pCellNode ) == false )
		{
			const MapID& _materialMapID(pMaterial->GetCurrentMap());
			const MapID& _landMapID(GetMapID());
			sc::writeLogError(sc::string::format("DropOutMaterial() : pMaterial->m_pCellNode is invalid, MaterialMapID(%1%/%2%, %3%/%4%), LandMapID(%5%/%6%, %7%/%8%)", 
				_materialMapID.getBaseMapID().Mid(), _materialMapID.getBaseMapID().Sid(), _materialMapID.getGaeaMapID().Mid(), _materialMapID.getGaeaMapID().Sid(),
				_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
		}
	}

	// 몹스케쥴의 생존 플래그를 오프시킴.
	GLMobSchedule* pMobSch = pMaterial->GetMobSchedule();
	if ( pMobSch )
	{
		pMobSch->m_bALive = FALSE;
		//pMobSch->m_dwGlobID = UINT_MAX;
	}

	// 메모리 해지
	m_pGLGaeaServer->RELEASE_MATERIAL ( pMaterial );

	return TRUE;
}

BOOL GLLandMan::MoveCrow ( DWORD dwGlobID, const D3DXVECTOR3 &vPos )
{
	GASSERT ( dwGlobID<MAXCROW );

	GLCrow* pCrow = GetCrow(dwGlobID);
	if( !pCrow )
	{
		// Note : Level Edit 작업시 여기서 NULL 값이 나왔다. 
		sc::writeLogError(std::string("GLLandMan::MoveCrow() - pCrow==NULL"));
		return FALSE;
	}

	LANDQUADNODE* pPrevNode = pCrow->m_pQuadNode;
	if( !pPrevNode )
	{
		sc::writeLogError(
            std::string(
                "GLLandMan::MoveCrow() - pPrevNode==NULL"));
		return FALSE;
	}

	// 종전 셀에서 벗어나지 않았을 경우 그대로 둔다
	if ( pPrevNode->IsWithInThisNode ( (int)vPos.x, (int)vPos.z ) )
		return FALSE;

	// 새 지점의 위치 노드를 찾는다.
	LANDQUADNODE* pNextNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pNextNode )
		return FALSE;

	// 이전 리스트에서 제거.	
	if ( pPrevNode && pPrevNode->pData )
	{
		if ( pPrevNode->pData->m_CROWList.DELNODE ( pCrow->m_pCellNode ) == false )
		{
			const MapID& _crowMapID(pCrow->GetCurrentMap());
			const MapID& _landMapID(GetMapID());
			sc::writeLogError(sc::string::format("DropOutCrow() : pCrow->m_pCellNode is invalid, CrowMapID(%1%/%2%, %3%/%4%), LandMapID(%5%/%6%, %7%/%8%)", 
				_crowMapID.getBaseMapID().Mid(), _crowMapID.getBaseMapID().Sid(), _crowMapID.getGaeaMapID().Mid(), _crowMapID.getGaeaMapID().Sid(),
				_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
		}
	}


	//mjeon.ToDo
	//
	//Following structure and class must be re-named!! These kinds of name BUGS developers!!
	//First, remove the prefix, C and S.
	//This style of code is a lot in RAN source. Remove them all.
	//Better solution would be the linked-list that is implemented in only one class.
	//The class includes node information and all operations for itself.
	//
	//sc::SGLNODE	- Struct for GLNode of sc::CGLLIST.
	//sc::CGLLIST	- Class  for GLList implementation.
	//	


	// 찾은 노드에 삽입.
	pCrow->m_pQuadNode = pNextNode;											// - 트리 노드.
	pCrow->m_pCellNode = pNextNode->pData->m_CROWList.ADDHEAD ( pCrow );	// - 트리 노드.
		//mjeon
		//sc::CGLLIST.ADDHEAD() returns the pointer of newly added node(pointer of sc::SGLNODE).
		//IOW, pCrow->m_pCellNode points the node of itself in the linked-list.
		//
		//						points
		// pCrow->m_pCellnode ----------> sc::SGLNODE *pNode		  points
		//										and pNode->Data ---------> pCrow

	return TRUE;
}

BOOL GLLandMan::RegistPet ( PGLPETFIELD pPet )
{
	GASSERT(pPet);
	LANDQUADNODE* pNode = pPet->m_pLandMan->FindCellNode ( (int)pPet->m_vPos.x, (int)pPet->m_vPos.z );
	if ( !pNode )
		return FALSE;

	pPet->m_dwCellID = pNode->dwCID;

	pPet->m_pQuadNode = pNode;
	pPet->m_pCellNode = pNode->pData->m_PETList.ADDHEAD ( pPet );

	return TRUE;
}

BOOL GLLandMan::RemovePet ( PGLPETFIELD pPet )
{
	GASSERT(pPet);

	if ( m_GlobPETList.DELNODE ( pPet->m_pLandNode ) == false )
	{
		const MapID& _petMapID(pPet->m_sMapID);
		const MapID& _landMapID(GetMapID());
		sc::writeLogError(sc::string::format("RemovePet() : pPet->m_pLandNode is invalid, PetMapID(%1%/%2%, %3%/%4%), LandMapID(%5%/%6%, %7%/%8%)", 
			_petMapID.getBaseMapID().Mid(), _petMapID.getBaseMapID().Sid(), _petMapID.getGaeaMapID().Mid(), _petMapID.getGaeaMapID().Sid(),
			_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
	}

	LANDQUADNODE* pQuadNode(pPet->m_pQuadNode);
	if ( pQuadNode && pQuadNode->pData )
	{
		if ( pQuadNode->pData->m_PETList.DELNODE ( pPet->m_pCellNode ) == false )
		{
			const MapID& _petMapID(pPet->m_sMapID);
			const MapID& _landMapID(GetMapID());
			sc::writeLogError(sc::string::format("RemovePet() : pPet->m_pLandNode is invalid, PetMapID(%1%/%2%, %3%/%4%), LandMapID(%5%/%6%, %7%/%8%)", 
				_petMapID.getBaseMapID().Mid(), _petMapID.getBaseMapID().Sid(), _petMapID.getGaeaMapID().Mid(), _petMapID.getGaeaMapID().Sid(),
				_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
		}
	}

	pPet->m_dwCellID = 0;
	pPet->m_pQuadNode = NULL;
	pPet->m_pCellNode = NULL;
	pPet->m_pLandNode = NULL;

	return TRUE;
}

BOOL GLLandMan::DropChar ( GLChar* pChar )
{
	GASSERT(pChar);
	const D3DXVECTOR3& vPosition = pChar->GetPosition();
	LANDQUADNODE* pNode = m_LandTree.FindNode ( int(vPosition.x), int(vPosition.z) );	

	const MapID& _charMapID(pChar->GetCurrentMap());
	if ( m_mapID != _charMapID )
	{					
		sc::writeLogError(sc::string::format("RegistChar() : different LandMan, CharMapID(%1%/%2%, %3%/%4%), LandMapID(%5%/%6%, %7%/%8%)", 
			_charMapID.getBaseMapID().Mid(), _charMapID.getBaseMapID().Sid(), _charMapID.getGaeaMapID().Mid(), _charMapID.getGaeaMapID().Sid(),
			m_mapID.getBaseMapID().Mid(), m_mapID.getBaseMapID().Sid(), m_mapID.getGaeaMapID().Mid(), m_mapID.getGaeaMapID().Sid()));
	}
	
	if ( NULL == pNode )
	{
		sc::writeLogError( sc::string::format(
			"GLLandMan::DropChar, pNode is NULL!!, CharMapID(%1%/%2%, %3%/%4%), LandMapID(%5%/%6%, %7%/%8%), vPosition(%9%/%10%/%11%), ChaNum(%12%)",
			_charMapID.getBaseMapID().Mid(),
			_charMapID.getBaseMapID().Sid(),
			_charMapID.getGaeaMapID().Mid(),
			_charMapID.getGaeaMapID().Sid(),
			m_mapID.getBaseMapID().Mid(),
			m_mapID.getBaseMapID().Sid(),
			m_mapID.getGaeaMapID().Mid(),
			m_mapID.getGaeaMapID().Sid(),
			vPosition.x,
			vPosition.y,
			vPosition.z,
			pChar->CharDbNum() ) );

		return FALSE;
	}

	pChar->m_dwCeID = pNode->dwCID;
	pChar->m_pQuadNode = pNode;
	pChar->m_pCellNode = pNode->pData->m_PCList.ADDHEAD ( pChar );
	
	return TRUE;
}

BOOL GLLandMan::RemoveChar ( GLChar* pChar )
{
	if( !pChar )
		return FALSE;

	// Faction 이 있다면 제거한다;
	Faction::ManagerField* pFactionMan = GetFactionManager();
	if ( pFactionMan )
	{
		pFactionMan->removeActor( pChar->GetActorID() );
	}

	const MapID& _charMapID(pChar->GetCurrentMap());
	if ( AddGlobPcDelList(pChar->m_pCharNodeInLand) == false )
	{	
		sc::writeLogError(sc::string::format("RemoveChar() : pChar->m_pCharNodeInLand is NULL, CharMapID(%1%/%2%, %3%/%4%), LandMapID(%5%/%6%, %7%/%8%)", 
			_charMapID.getBaseMapID().Mid(), _charMapID.getBaseMapID().Sid(), _charMapID.getGaeaMapID().Mid(), _charMapID.getGaeaMapID().Sid(),
			m_mapID.getBaseMapID().Mid(), m_mapID.getBaseMapID().Sid(), m_mapID.getGaeaMapID().Mid(), m_mapID.getGaeaMapID().Sid()));
	}

	LANDQUADNODE* pQuadNode(pChar->m_pQuadNode);
	if ( pQuadNode && pQuadNode->pData )
	{
		if ( pQuadNode->pData->m_PCList.DELNODE ( pChar->m_pCellNode ) == false )
		{	
			sc::writeLogError(sc::string::format("RemoveChar() : pChar->m_pCellNode is invalid, CharMapID(%1%/%2%, %3%/%4%), LandMapID(%5%/%6%, %7%/%8%)", 
				_charMapID.getBaseMapID().Mid(), _charMapID.getBaseMapID().Sid(), _charMapID.getGaeaMapID().Mid(), _charMapID.getGaeaMapID().Sid(),
				m_mapID.getBaseMapID().Mid(), m_mapID.getBaseMapID().Sid(), m_mapID.getGaeaMapID().Mid(), m_mapID.getGaeaMapID().Sid()));
		}
	}
	else
	{
		sc::writeLogError(sc::string::format("RemoveChar() : pChar->pQuadNode is invalid, CharMapID(%1%/%2%, %3%/%4%), LandMapID(%5%/%6%, %7%/%8%) %9%, ChaNum(%10%)", 
			_charMapID.getBaseMapID().Mid(), _charMapID.getBaseMapID().Sid(), _charMapID.getGaeaMapID().Mid(), _charMapID.getGaeaMapID().Sid(),
			m_mapID.getBaseMapID().Mid(), m_mapID.getBaseMapID().Sid(), m_mapID.getGaeaMapID().Mid(), m_mapID.getGaeaMapID().Sid(),
			unsigned int(pChar->m_pQuadNode), pChar->CharDbNum() ));
	}

	
	pChar->m_dwCeID = 0;	
	pChar->m_pQuadNode = NULL;
	pChar->m_pCellNode = NULL;
	pChar->m_pCharNodeInLand = NULL;

	return TRUE;
}

BOOL GLLandMan::MovePet ( DWORD dwGUID, const D3DXVECTOR3 &vPos )
{
	PGLPETFIELD pPet = m_pGLGaeaServer->GetPET ( dwGUID );

	if ( !pPet )
		return FALSE;

	if ( !pPet->IsValid () )
		return FALSE;

	GLChar* pOwner = m_pGLGaeaServer->GetChar ( pPet->m_dwOwner );
	if ( !pOwner )
		return FALSE;

	if ( !pOwner->IsActState(EM_GETVA_AFTER) )
		return FALSE;

	// 종전 셀에서 벗어나지 않았을 경우 그대로 둔다
	LANDQUADNODE* pPrevNode = pPet->m_pQuadNode;
	if ( pPrevNode )
        if ( pPrevNode->IsWithInThisNode ( (int)vPos.x, (int)vPos.z ) )
			return FALSE;

	// 새 지점의 위치 노드를 찾는다.
	LANDQUADNODE* pNextNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pNextNode )
		return FALSE;

	// 이전 리스트에서 제거.
	if ( pPrevNode && pPrevNode->pData )
	{
		if ( pPrevNode->pData->m_PETList.DELNODE ( pPet->m_pCellNode ) == false )
		{
			const MapID& _petMapID(pPet->m_sMapID);
			const MapID& _landMapID(GetMapID());
			sc::writeLogError(sc::string::format("MovePet() : pPet->m_pCellNode is invalid, CharMapID(%1%/%2%, %3%/%4%), LandMapID(%5%/%6%, %7%/%8%)", 
				_petMapID.getBaseMapID().Mid(), _petMapID.getBaseMapID().Sid(), _petMapID.getGaeaMapID().Mid(), _petMapID.getGaeaMapID().Sid(),
				_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
		}
	}

	// 찾은 노드에 삽입.
	pPet->m_pQuadNode = pNextNode;										// - 트리 노드.
	pPet->m_pCellNode = pNextNode->pData->m_PETList.ADDHEAD ( pPet );	// - 트리 노드.

	return TRUE;
}

BOOL GLLandMan::MoveSummon ( DWORD dwGUID, const D3DXVECTOR3 &vPos )
{
	GASSERT ( dwGUID<MAXSUMMON );

	GLSummonField* pSummon = GetSummon(dwGUID);
	if (!pSummon)
	{
		sc::writeLogError(std::string("GLLandMan::MoveSummon() - pSummon==NULL"));
		return FALSE;
	}

	LANDQUADNODE* pPrevNode = pSummon->m_pQuadNode;
	if( !pPrevNode )
	{
		sc::writeLogError(
            std::string("GLLandMan::MoveSummon() - pPrevNode==NULL"));
		return FALSE;
	}

	// 종전 셀에서 벗어나지 않았을 경우 그대로 둔다
	if ( pPrevNode->IsWithInThisNode ( (int)vPos.x, (int)vPos.z ) )
		return FALSE;

	// 새 지점의 위치 노드를 찾는다.
	LANDQUADNODE* pNextNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pNextNode )
		return FALSE;

	// 이전 리스트에서 제거.
	if ( pPrevNode && pPrevNode->pData )
	{
		if ( pPrevNode->pData->m_SummonList.DELNODE ( pSummon->m_pCellNode ) == false )
		{
			const MapID& summonMapID(pSummon->GetCurrentMap());
			const MapID& _landMapID(GetMapID());
			sc::writeLogError(sc::string::format("MoveSummon() : pSummon->m_pCellNode is invalid, SummonMapID(%1%/%2%, %3%/%4%), LandMapID(%5%/%6%, %7%/%8%)", 
				summonMapID.getBaseMapID().Mid(), summonMapID.getBaseMapID().Sid(), summonMapID.getGaeaMapID().Mid(), summonMapID.getGaeaMapID().Sid(),
				_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
		}
	}

	// 찾은 노드에 삽입.
	pSummon->m_pQuadNode = pNextNode;											// - 트리 노드.
	pSummon->m_pCellNode = pNextNode->pData->m_SummonList.ADDHEAD ( pSummon );	// - 트리 노드.

	return TRUE;
}

BOOL GLLandMan::MoveChar ( DWORD dwGaeaID, const D3DXVECTOR3 &vPos )
{
	GLChar* pChar = m_pGLGaeaServer->GetChar(dwGaeaID);	
	GASSERT(pChar && "GLLandMan::MoveChar");
	if (!pChar)
	{
        sc::writeLogError(
            sc::string::format(
                "MoveChar ChaGaeaID %1%", dwGaeaID));
		return FALSE;
	}

	if (!pChar->IsActState(EM_GETVA_AFTER))
		return FALSE;

	LANDQUADNODE* pPrevNode = pChar->m_pQuadNode;

	// 종전 셀에서 벗어나지 않았을 경우 그대로 둔다.
	//
	if ( pPrevNode )
		if ( pPrevNode->IsWithInThisNode ( (int)vPos.x, (int)vPos.z ) )
			return FALSE;

	// 새 지점의 위치 노드를 찾는다.
	LANDQUADNODE* pNextNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pNextNode )
		return FALSE;

	// 이전 리스트에서 제거.
	if ( pPrevNode && pPrevNode->pData )
	{
		if ( pPrevNode->pData->m_PCList.DELNODE ( pChar->m_pCellNode ) == false )
		{
			const MapID& _charMapID(pChar->GetCurrentMap());
			const MapID& _landMapID(GetMapID());
			sc::writeLogError(sc::string::format("MoveChar() : pChar->m_pCellNode is invalid, CharMapID(%1%/%2%, %3%/%4%), LandMapID(%5%/%6%, %7%/%8%)", 
				_charMapID.getBaseMapID().Mid(), _charMapID.getBaseMapID().Sid(), _charMapID.getGaeaMapID().Mid(), _charMapID.getGaeaMapID().Sid(),
				_landMapID.getBaseMapID().Mid(), _landMapID.getBaseMapID().Sid(), _landMapID.getGaeaMapID().Mid(), _landMapID.getGaeaMapID().Sid()));
		}
	}

	// 찾은 노드에 삽입.
	pChar->m_pQuadNode = pNextNode;										// - 트리 노드.
	pChar->m_pCellNode = pNextNode->pData->m_PCList.ADDHEAD ( pChar );	// - 트리 노드.

	return TRUE;
}

HRESULT GLLandMan::ClearDropObj ()
{
	DWORD i=0;

	DropSaveCommission();

	for (i=0; i<MAXITEM; ++i)
        DropOutItem( i, true );
	for (i=0; i<MAXMONEY; ++i)
        DropOutMoney(i);
	for (i=0; i<MAXCROW; ++i)
        DropOutCrow(i);
	for (i=0; i<MAXCROW; ++i)
        DropOutMaterial(i);
	for (i=0; i<MAXSUMMON; ++i)
        DropOutSummon(i, FALSE);

	m_GlobPCList.DELALL ();
	if ( m_pCountRecorder )
		m_pCountRecorder->Clear_UserInfo();

	m_listDelGlobIDEx.clear();
	m_listDelGlobID.clear();
	m_listDelGlobIDExMat.clear();
	m_listDelGlobIDMat.clear();

	// PET
	m_GlobPETList.DELALL ();

	//m_MobSchMan->Reset ();
	//m_MobSchManEx.Reset ();
    DelAllMobSchManRegenLayer( true );
    DelAllMobSchManOnceLayer( true );

	// LANDEVENT
	m_pLandEvent->CleanUp();

	return S_OK;
}

HRESULT GLLandMan::ClearExptChaObj ()
{
	DWORD i=0;

	//m_MobSchMan->Reset ();
	//m_MobSchManEx.Reset ();
    DelAllMobSchManRegenLayer();
    DelAllMobSchManOnceLayer();

	return S_OK;
}

GLChar* GLLandMan::FindEnemyChar ( const PGLCROW pFinder )
{
	GASSERT(pFinder);

	EMCROWACT_UP emACT_UP = pFinder->GetActPattern();

	int nRange = (int) pFinder->m_pCrowData->m_wViewRange;
	EMATT_RGTYPE emAttRgType = pFinder->GetAttackRangeType();

	D3DXVECTOR3 vCenter = pFinder->GetPosition();
	int nX = int ( vCenter.x );
	int nZ = int ( vCenter.z );

	LANDQUADNODE* pQuadNode = NULL;
	BOUDRECT bRect(nX+nRange,nZ+nRange,nX-nRange,nZ-nRange);
	m_LandTree.FindNodes ( bRect, m_LandTree.GetRootNode(), &pQuadNode );

	if ( !m_vTempPC.empty() )	m_vTempPC.erase ( m_vTempPC.begin(), m_vTempPC.end() );

	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GASSERT(m_LandTree.GetCellNum()>pQuadNode->dwCID&&"인덱스가 셀사이즈를 초과하였습니다.");

		GLLandNode *pLandNode = pQuadNode->pData;
		
		GLCHARNODE *pCur = pLandNode->m_PCList.m_pHead;
		for ( ; pCur; pCur = pCur->pNext )
		{
			GLChar* pChar = pCur->Data;
			if( !pChar )
				continue;

			// GM 캐릭은 선공 하지 않게.
			//
			if ( !pChar->IsValidBody() )
				continue;

			if ( !pChar->IsVisibleDetect(pFinder) )
				continue;

			//	대련시에는 몹이 케릭터를 선공하지 않음.
			if ( pChar->GETCONFTING()->IsCONFRONTING() )
				continue;

			BOOL bTarget = FALSE;
			float fTarLeng = FLT_MAX;

			if ( emAttRgType==EMATT_SHORT )
			{
				// 근거리 공격일 경우.
				if ( pChar->IsFreeARSlot() )
				{
					D3DXVECTOR3 vLengA = pChar->GetPosition() - vCenter;
					fTarLeng = D3DXVec3Length(&vLengA);
					
					if ( fTarLeng <= nRange )
						bTarget = TRUE;
						
				}
			}
			else
			{
				D3DXVECTOR3 vLengA = pChar->GetPosition() - vCenter;
				fTarLeng = D3DXVec3Length(&vLengA);
				
				if ( fTarLeng <= nRange )
					bTarget = TRUE;
			}

			if ( bTarget )
			{
				if ( IsLayerSimpleCollision( pChar->GetPosition(), vCenter, 5.f ) )
				{
					continue;
				}
			}

			if ( !bTarget )
				continue;

			switch ( emACT_UP )
			{
			default:
				break;
			case EMCROWACT_UP_LOWLEVEL:
				if ( pChar->GETLEVEL() > pFinder->GETLEVEL() )
					bTarget = FALSE;
				break;
			case EMCROWACT_UP_LOWHP:
				if ( pChar->GETHP() > (int)pFinder->GETHP() )
					bTarget = FALSE;
				break;

			case EMCROWACT_UP_BRIGHT:
				if ( pChar->GETBRIGHT() != BRIGHT_LIGHT )
					bTarget = FALSE;
				break;

			case EMCROWACT_UP_DARK:
				if ( pChar->GETBRIGHT() != BRIGHT_DARK )
					bTarget = FALSE;
				break;

			case EMCROWACT_UP_BLOW:
				if ( !pChar->ISSTATEBLOW() )
					bTarget = FALSE;
				break;

			case EMCROWACT_UP_ARMER:
				{
					EMSLOT emRHand = pChar->GetCurRHand();
					if ( !pChar->VALID_SLOT_ITEM(emRHand) )
						bTarget = FALSE;
				}
				break;

			case EMCROWACT_UP_RUNNER:
				if ( !pChar->IsRunning() )
					bTarget = FALSE;
				break;
			};

			if ( !bTarget )
				continue;

			m_vTempPC.push_back ( std_afunc::CHARDIST(fTarLeng,pCur->Data) );
		}
	}

	if ( !m_vTempPC.empty() )
	{
		D3DXVECTOR3 vFinderPos = pFinder->GetPosition();
		vFinderPos.y += 15.0f;
		
		D3DXVECTOR3 vTargetPos;

		std_afunc::CCompareCharDist Comp;
		std::sort ( m_vTempPC.begin(), m_vTempPC.end(), Comp );

		ARRAY_CHAR_ITER iter = m_vTempPC.begin();
		ARRAY_CHAR_ITER iter_end = m_vTempPC.end();		
		for ( ; iter!=iter_end; ++iter )
		{
			const std_afunc::CHARDIST &Target = *iter;

			// 시야가 확보되는가?
			//
			vTargetPos = Target.pChar->GetPosition();
			vTargetPos.y += 15.0f;
			BOOL bCollision = m_CollisionMap.IsCollision_NoneConst( vFinderPos, vTargetPos, NULL, FALSE, NULL, NULL, EMCC_CULL_NONE );
			
			if ( bCollision )
				continue;

			return Target.pChar;
		}
	}

	return NULL;
}

//ARRAY_CHAR* GLLandMan::DetectPC ( const D3DXVECTOR3 &vDetectPosA, const D3DXVECTOR3 &vDetectPosB, const DWORD dwMaxFind )
//{
//	STARGETID TargetID(CROW_PC,EMTARGET_NULL);
//
//	if ( !m_vTempPC.empty() )
//		m_vTempPC.erase ( m_vTempPC.begin(), m_vTempPC.end() );
//
//	if ( !m_vDetectPC.empty() )
//		m_vDetectPC.erase ( m_vDetectPC.begin(), m_vDetectPC.end() );
//
//	float fTarLeng(0.0f);
//
//	// A ~ B 사이의 중점.
//	int nX = int ( vDetectPosA.x + vDetectPosB.x ) / 2;
//	int nZ = int ( vDetectPosA.z + vDetectPosB.z ) / 2;
//
//	// 중점에서 사이드 까지의 거리.
//	int nSX = abs ( int ( vDetectPosA.x - vDetectPosB.x ) / 2 );
//	int nSZ = abs ( int ( vDetectPosA.x - vDetectPosB.x ) / 2 );
//
//	LANDQUADNODE* pQuadNode = NULL;
//	BOUDRECT bRect(nX+nSX,nZ+nSZ,nX-nSX,nZ-nSZ);
//
//	m_LandTree.FindNodes ( bRect, m_LandTree.GetRootNode(), &pQuadNode );
//
//
//	D3DXVECTOR3 vMax, vMin;
//	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
//	{
//		GASSERT(m_LandTree.GetCellNum()>pQuadNode->dwCID&&"인덱스가 셀사이즈를 초과하였습니다.");
//
//		GLLandNode *pLandNode = pQuadNode->pData;
//		
//		GLCHARNODE *pCur = pLandNode->m_PCList.m_pHead;
//
//		for ( ; pCur; pCur = pCur->pNext )
//		{
//			GLChar* pChar = pCur->Data;
//			
//			if( !pChar )
//				continue;
//
//			float fBodyRadius = (float) pChar->GETBODYRADIUS();
//			D3DXVECTOR3 vPos = pChar->GetPosition();
//
//			vMax = vPos + D3DXVECTOR3(8,20,8);
//			vMin = vPos + D3DXVECTOR3(-8,0,-8);
//
//			BOOL bCol = COLLISION::IsCollisionLineToAABB ( vDetectPosA, vDetectPosB, vMax, vMin );
//			
//			if ( !bCol )
//				continue;
//
//			D3DXVECTOR3 vLengA = pChar->GetPosition() - vDetectPosA;
//			fTarLeng = D3DXVec3Length(&vLengA);
//
//			m_vTempPC.push_back ( std_afunc::CHARDIST(fTarLeng,pCur->Data) );
//		}
//	}
//
//	if ( !m_vTempPC.empty() )
//	{
//		D3DXVECTOR3 vTargetPos;
//		std_afunc::CCompareCharDist Comp;
//		std::sort ( m_vTempPC.begin(), m_vTempPC.end(), Comp );
//
//		ARRAY_CHAR_ITER iter = m_vTempPC.begin();
//		ARRAY_CHAR_ITER iter_end = m_vTempPC.end();		
//		
//		for ( ; iter!=iter_end; ++iter )
//		{
//			if ( m_vDetectPC.size() >= dwMaxFind )
//				break;
//
//			const std_afunc::CHARDIST &Target = *iter;
//
//			// 시야가 확보되는가?
//			//
//			//vTargetPos = Target.pChar->GetPosition();
//			//vTargetPos.y += 15.0f;
//
//			//BOOL bCollision = m_CollisionMap.IsCollision ( vFinderPos, vTargetPos, NULL, FALSE );
//			//if ( bCollision )	continue;
//
//			m_vDetectPC.push_back ( Target );
//		}
//	}
//
//	if ( m_vDetectPC.empty() )
//		return NULL;
//
//	return &m_vDetectPC;
//}
//
//ARRAY_CHAR* GLLandMan::DetectPC ( const D3DXVECTOR3 &vDetectPos, const int nRange, const DWORD dwMaxFind )
//{
//	STARGETID TargetID(CROW_PC,EMTARGET_NULL);
//
//	if ( !m_vTempPC.empty() )
//		m_vTempPC.erase ( m_vTempPC.begin(), m_vTempPC.end() );
//
//	if ( !m_vDetectPC.empty() )
//		m_vDetectPC.erase ( m_vDetectPC.begin(), m_vDetectPC.end() );
//
//
//	float fTarLeng(0.0f);
//
//	int nX = int ( vDetectPos.x );
//	int nZ = int ( vDetectPos.z );
//
//	LANDQUADNODE* pQuadNode = NULL;
//	BOUDRECT bRect(nX+nRange,nZ+nRange,nX-nRange,nZ-nRange);
//	m_LandTree.FindNodes ( bRect, m_LandTree.GetRootNode(), &pQuadNode );
//
//	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
//	{
//		GASSERT(m_LandTree.GetCellNum()>pQuadNode->dwCID&&"인덱스가 셀사이즈를 초과하였습니다.");
//
//		GLLandNode *pLandNode = pQuadNode->pData;
//		
//		GLCHARNODE *pCur = pLandNode->m_PCList.m_pHead;
//		for ( ; pCur; pCur = pCur->pNext )
//		{
//			GLChar* pChar = pCur->Data;
//			
//			if( !pChar )
//				continue;
//
//			D3DXVECTOR3 vLengA = pChar->GetPositionActor() - vDetectPos;
//			fTarLeng = D3DXVec3Length(&vLengA);
//			
//			if ( fTarLeng <= nRange )
//			{
//				m_vTempPC.push_back ( std_afunc::CHARDIST(fTarLeng,pCur->Data) );
//			}
//		}
//	}
//
//	if ( !m_vTempPC.empty() )
//	{
//		D3DXVECTOR3 vTargetPos;
//		std_afunc::CCompareCharDist Comp;
//		std::sort ( m_vTempPC.begin(), m_vTempPC.end(), Comp );
//
//		ARRAY_CHAR_ITER iter = m_vTempPC.begin();
//		ARRAY_CHAR_ITER iter_end = m_vTempPC.end();		
//		
//		for ( ; iter!=iter_end; ++iter )
//		{
//			if ( m_vDetectPC.size() >= dwMaxFind )
//				break;
//
//			const std_afunc::CHARDIST &Target = *iter;
//
//			// 시야가 확보되는가?
//			//
//			//vTargetPos = Target.pChar->GetPosition();
//			//vTargetPos.y += 15.0f;
//
//			//BOOL bCollision = m_CollisionMap.IsCollision ( vFinderPos, vTargetPos, NULL, FALSE );
//			//if ( bCollision )	continue;
//
//			m_vDetectPC.push_back ( Target );
//		}
//	}
//
//	if ( m_vDetectPC.empty() )
//		return NULL;
//
//	return &m_vDetectPC;
//}

ARRAY_CROW* GLLandMan::DetectCROW ( const D3DXVECTOR3 &vDetectPos, const int nRange, const DWORD dwMaxFind )
{
	STARGETID TargetID(CROW_PC,EMTARGET_NULL);

	if ( !m_vTempCROW.empty() )
		m_vTempCROW.erase ( m_vTempCROW.begin(), m_vTempCROW.end() );

	if ( !m_vDetectCROW.empty() )
		m_vDetectCROW.erase ( m_vDetectCROW.begin(), m_vDetectCROW.end() );

	
	float fTarLeng(0.0f);

	int nX = int ( vDetectPos.x );
	int nZ = int ( vDetectPos.z );

	LANDQUADNODE* pQuadNode = NULL;
	BOUDRECT bRect(nX+nRange,nZ+nRange,nX-nRange,nZ-nRange);
	m_LandTree.FindNodes ( bRect, m_LandTree.GetRootNode(), &pQuadNode );

	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GASSERT(m_LandTree.GetCellNum()>pQuadNode->dwCID&&"인덱스가 셀사이즈를 초과하였습니다.");

		GLLandNode* pLandNode = pQuadNode->pData;
        if (!pLandNode)
            continue;
		
		GLCROWNODE* pCur = pLandNode->m_CROWList.m_pHead;
		for ( ; pCur; pCur = pCur->pNext)
		{
			PGLCROW pCrow = pCur->Data;
            if (pCrow)
            {
			    D3DXVECTOR3 vLengA(pCrow->GetPosition() - vDetectPos);
			    fTarLeng = D3DXVec3Length(&vLengA);
			    if ( fTarLeng <= nRange &&
					!IsLayerSimpleCollision( pCrow->GetPosition(), vDetectPos, 5.f ) )
				{
				    m_vTempCROW.push_back(std_afunc::CROWDIST(fTarLeng, pCur->Data));
				}
            }
		}
	}

	if ( !m_vTempCROW.empty() )
	{
		//D3DXVECTOR3 vTargetPos;
		std_afunc::CCompareCrowDist Comp;
		std::sort(m_vTempCROW.begin(), m_vTempCROW.end(), Comp);

		ARRAY_CROW_ITER iter = m_vTempCROW.begin();
		ARRAY_CROW_ITER iter_end = m_vTempCROW.end();
		
		for ( ; iter!=iter_end; ++iter)
		{
			if (m_vDetectCROW.size() >= dwMaxFind)
				break;

			const std_afunc::CROWDIST &Target = *iter;

			// 시야가 확보되는가?
			//
			//vTargetPos = Target.pCrow->GetPosition();
			//vTargetPos.y += 15.0f;
			//BOOL bCollision = m_CollisionMap.IsCollision ( vFinderPos, vTargetPos, NULL, FALSE );
			//if ( bCollision )	continue;

			m_vDetectCROW.push_back(Target);
		}
	}

	if (m_vDetectCROW.empty())
		return NULL;
    else
	    return &m_vDetectCROW;
}

VEC_SK_TAR* GLLandMan::Detect_CROW_PC( const D3DXVECTOR3 &vDetectPosA, const D3DXVECTOR3 &vDetectPosB )
{
	STARGETID TargetID(CROW_PC,EMTARGET_NULL);

	float fTarLeng(0.0f);

	// A ~ B 사이의 중점.
	int nX = int ( vDetectPosA.x + vDetectPosB.x ) / 2;
	int nZ = int ( vDetectPosA.z + vDetectPosB.z ) / 2;

	// 중점에서 사이드 까지의 거리.
	int nSX = abs ( int ( vDetectPosA.x - vDetectPosB.x ) / 2 );
	int nSZ = abs ( int ( vDetectPosA.x - vDetectPosB.x ) / 2 );

	LANDQUADNODE* pQuadNode = NULL;
	BOUDRECT bRect(nX+nSX,nZ+nSZ,nX-nSX,nZ-nSZ);

	m_LandTree.FindNodes ( bRect, m_LandTree.GetRootNode(), &pQuadNode );


	STARGETID findTargetID;
	D3DXVECTOR3 vMax, vMin;
	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GASSERT(m_LandTree.GetCellNum()>pQuadNode->dwCID&&"인덱스가 셀사이즈를 초과하였습니다.");

		GLLandNode *pLandNode = pQuadNode->pData;

		GLCHARNODE *pCur = pLandNode->m_PCList.m_pHead;

		for ( ; pCur; pCur = pCur->pNext )
		{
			GLChar* pChar = pCur->Data;

			if( !pChar )
				continue;

			float fBodyRadius = (float) pChar->GETBODYRADIUS();
			D3DXVECTOR3 vPos = pChar->GetPosition();

			vMax = vPos + D3DXVECTOR3(8,20,8);
			vMin = vPos + D3DXVECTOR3(-8,0,-8);

			BOOL bCol = COLLISION::IsCollisionLineToAABB ( vDetectPosA, vDetectPosB, vMax, vMin );

			if ( !bCol )
				continue;
			
			findTargetID.emCrow = pChar->GetCrow();
			findTargetID.GaeaId = pChar->GetGaeaID();
			findTargetID.vPos = pChar->GetPosition();
			
			m_vecDetectID.push_back(findTargetID);
		}
	}

	if ( m_vecDetectID.empty() )
		return NULL;

	return &m_vecDetectID;
}

VEC_SK_TAR* GLLandMan::Detect_CROW_PC( const D3DXVECTOR3 &vDetectPos, const int nRange )
{
	STARGETID TargetID(CROW_PC,EMTARGET_NULL);

	int nX = int ( vDetectPos.x );
	int nZ = int ( vDetectPos.z );

	STARGETID findTargetID;
	LANDQUADNODE* pQuadNode = NULL;
	BOUDRECT bRect(nX+nRange,nZ+nRange,nX-nRange,nZ-nRange);
	m_LandTree.FindNodes ( bRect, m_LandTree.GetRootNode(), &pQuadNode );

	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GASSERT(m_LandTree.GetCellNum()>pQuadNode->dwCID&&"인덱스가 셀사이즈를 초과하였습니다.");

		GLLandNode *pLandNode = pQuadNode->pData;

		GLCHARNODE *pCur = pLandNode->m_PCList.m_pHead;
		for ( ; pCur; pCur = pCur->pNext )
		{
			GLChar* pChar = pCur->Data;

			if( !pChar )
				continue;

			if ( pChar->IsSafeTime() )
				continue;

			if ( pChar->IsSafeZone() )
				continue;

			// 대련일때는 타겟을 왜 찾지 않을까?
			//if ( pChar->GETCONFTING()->IsCONFRONTING() )
			//	continue;
			if( pChar->IsNonTargetable())
				continue;


			if ( IsAbleAttack_Range_LayerSimpleCollision( nRange, pChar->GetPositionActor(), vDetectPos, 5.f ) )
			{
				findTargetID.emCrow = pChar->GetCrow();
				findTargetID.GaeaId = pChar->GetGaeaID();
				findTargetID.vPos = pChar->GetPosition();

				m_vecDetectID.push_back(findTargetID);
			}


		}
	}

	if ( m_vecDetectID.empty() )
		return NULL;

	return &m_vecDetectID;
}

VEC_SK_TAR* GLLandMan::Detect_CROW_MOB( const D3DXVECTOR3 &vDetectPos, const int nRange )
{
	STARGETID TargetID(CROW_PC,EMTARGET_NULL);

	float fTarLeng(0.0f);

	int nX = int ( vDetectPos.x );
	int nZ = int ( vDetectPos.z );

	STARGETID findTargetID;
	LANDQUADNODE* pQuadNode = NULL;
	BOUDRECT bRect(nX+nRange,nZ+nRange,nX-nRange,nZ-nRange);
	m_LandTree.FindNodes ( bRect, m_LandTree.GetRootNode(), &pQuadNode );

	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GASSERT(m_LandTree.GetCellNum()>pQuadNode->dwCID&&"인덱스가 셀사이즈를 초과하였습니다.");

		GLLandNode* pLandNode = pQuadNode->pData;
		if (!pLandNode)
			continue;

		GLCROWNODE* pCur = pLandNode->m_CROWList.m_pHead;
		for ( ; pCur; pCur = pCur->pNext)
		{
			PGLCROW pCrow = pCur->Data;
			if (pCrow)
			{
				if ( IsAbleAttack_Range_LayerSimpleCollision( nRange, pCrow->GetPositionActor(), vDetectPos, 5.f ) )
				{
					findTargetID.emCrow = pCrow->GetCrow();
					findTargetID.GaeaId = pCrow->GetGaeaID();
					findTargetID.vPos = pCrow->GetPosition();

					m_vecDetectID.push_back(findTargetID);
				}
			}
		}
	}

	if (m_vecDetectID.empty())
		return NULL;
	else
		return &m_vecDetectID;
}

VEC_SK_TAR* GLLandMan::Detect_CROW_SUMMON( const D3DXVECTOR3 &vDetectPos, const int nRange )
{
	STARGETID TargetID(CROW_PC,EMTARGET_NULL);

	float fTarLeng(0.0f);

	int nX = int ( vDetectPos.x );
	int nZ = int ( vDetectPos.z );

	STARGETID findTargetID;
	LANDQUADNODE* pQuadNode = NULL;
	BOUDRECT bRect(nX+nRange,nZ+nRange,nX-nRange,nZ-nRange);
	m_LandTree.FindNodes ( bRect, m_LandTree.GetRootNode(), &pQuadNode );

	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext )
	{
		GASSERT(m_LandTree.GetCellNum()>pQuadNode->dwCID&&"인덱스가 셀사이즈를 초과하였습니다.");

		GLLandNode* pLandNode = pQuadNode->pData;
		if (!pLandNode)
			continue;

		GLSUMMONNODE* pCur = pLandNode->m_SummonList.m_pHead;
		for ( ; pCur; pCur = pCur->pNext)
		{
			PGLSUMMONFIELD pSummon = pCur->Data;

			if ( !pSummon )
				continue;

			ServerActor* pOwner = m_pGLGaeaServer->GetActor( this, pSummon->m_sOwnerID );
			if ( pOwner && pOwner->GetCrow() == CROW_PC )
			{
				GLChar* pSummonOwner = static_cast< GLChar* >( pOwner );
				if (!pSummonOwner)
					continue;
				if (pSummonOwner->IsSafeTime())
					continue;
				if (pSummonOwner->IsSafeZone())
					continue;
				if (pSummonOwner->GETCONFTING()->IsCONFRONTING())
					continue;
			}

			if ( IsAbleAttack_Range_LayerSimpleCollision( nRange, pSummon->GetPositionActor(), vDetectPos, 5.f ) )
			{
				findTargetID.emCrow = pSummon->GetCrow();
				findTargetID.GaeaId = pSummon->GetGaeaID();
				findTargetID.vPos = pSummon->GetPosition();

				m_vecDetectID.push_back(findTargetID);
			}
		}
	}

	if (m_vecDetectID.empty())
		return NULL;
    
    return &m_vecDetectID;
}

LANDQUADNODE* GLLandMan::FindCellNode ( int nx, int nz )
{
	return m_LandTree.FindNode ( nx, nz );
}

// [shhan][2015.09.01] 사용이 안되고 있음.
//
//HRESULT GLLandMan::SendMsgPCViewAround ( int nX, int nZ, GLChar* const pReqChar, const LPNET_MSG_GENERIC &nmg )
//{
//	const DWORD dwReqClientID(pReqChar->GETCLIENTID());
//	const int nMaxViewRange( MAX_VIEWRANGE );
//
//	DWORD dwClientID = 0;
//	LANDQUADNODE* pQuadNode = NULL;
//	BOUDRECT bRect(nX+nMaxViewRange,nZ+nMaxViewRange,nX-nMaxViewRange,nZ-nMaxViewRange);
//	m_LandTree.FindNodes ( bRect, m_LandTree.GetRootNode(), &pQuadNode );
//
//	GLLandNode* pLandNode = NULL;
//	GLCHARNODE* pCur = NULL;
//
//	for ( ; pQuadNode; pQuadNode = pQuadNode->pFindNext)
//	{
//		GASSERT(m_LandTree.GetCellNum()>pQuadNode->dwCID&&"Index over cell size");
//		pLandNode = pQuadNode->pData;		
//		if (pLandNode)
//        {		
//		    pCur = pLandNode->m_PCList.m_pHead;    		
//		    for ( ; pCur; pCur = pCur->pNext)
//		    {
//			    dwClientID = pCur->Data->ClientSlot();
//			    if (dwClientID != dwReqClientID)
//				{
//					// [shhan][2015.09.01] rm #1631 - [스킬]시야제한 효과 수정 요청
//					//
//					// 시야가 좁아진 유저에게는 msg 를 보낼 필요가 없다.
//					// 보내면 추후 해킹의 우려가 있다.
//					//
//					if ( pCur->GETVISIBLESIGHT() != MAX_VIEWRANGE )
//					{
//						const D3DXVECTOR3& vPosition = pCur->GetPosition();
//						const int nPosition[2] = 
//						{
//							int(vPosition.x), 
//							int(vPosition.z)
//						};
//
//						const int _maxViewRange2( pCur->GETVISIBLESIGHT() );
//						BOUDRECT sRectOnlySee(nPosition[0]+_maxViewRange2, nPosition[1]+_maxViewRange2, 
//												nPosition[0]-_maxViewRange2, nPosition[1]-_maxViewRange2);		
//
//						if ( sRectOnlySee.IsWithIn( nX, nZ ) )
//						{
//							m_pGLGaeaServer->SENDTOCLIENT(dwClientID, nmg);
//						}
//					}
//					else
//					{
//						m_pGLGaeaServer->SENDTOCLIENT(dwClientID, nmg);
//					}
//				}
//		    }
//        }
//	}
//
//	return S_OK;
//}

HRESULT GLLandMan::SendMsgPC ( const LPNET_MSG_GENERIC &nmg )
{
	GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
	for ( ; pCharNode; pCharNode = pCharNode->pNext )
	{
		GLChar* pChar = pCharNode->Data;		
		if (pChar)
            m_pGLGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), nmg);
	}

	return S_OK;
}

HRESULT GLLandMan::SendMsgPC ( DWORD dwCharDbNum, const LPNET_MSG_GENERIC &nmg )
{
    if (m_pGLGaeaServer)
    {
        GLChar* pChar = m_pGLGaeaServer->GetCharByDbNum(dwCharDbNum);

        if (pChar)
        {
            m_pGLGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), nmg);
            return S_OK;
        }
    }

    return S_FALSE;
}

HRESULT GLLandMan::SendMsgMySelf ( DWORD dwCharDbNum, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc )
{
    if (m_pGLGaeaServer)
    {
        GLChar* pChar = m_pGLGaeaServer->GetCharByDbNum(dwCharDbNum);
        DxMsgServer* pMsgSvr = m_pGLGaeaServer->GetMsgServer();

        if (pChar && pMsgSvr)
        {
            pMsgSvr->SendMySelf(pChar->ClientSlot(), MsgType, Buffer, UseCrc);
            return S_OK;
        }
    }

    return S_FALSE;
}

HRESULT GLLandMan::SendMsgAgent ( EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc )
{
    if (m_pGLGaeaServer)
    {
        DxMsgServer* pMsgSvr = m_pGLGaeaServer->GetMsgServer();

        if (pMsgSvr)
        {
            pMsgSvr->SendAgent(MsgType, Buffer, UseCrc);
            return S_OK;
        }
    }

    return S_FALSE;
}

HRESULT GLLandMan::SendMsgPC ( EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc )
{
    GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
    for ( ; pCharNode; pCharNode = pCharNode->pNext )
    {
        GLChar* pChar = pCharNode->Data;		
        if (pChar)
            m_pGLGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), MsgType, Buffer, UseCrc);
    }

    return S_OK;
}

HRESULT GLLandMan::SendMsgPC ( DWORD dwCharDbNum, EMNET_MSG MsgType, const msgpack::sbuffer& Buffer, bool UseCrc )
{
    if (m_pGLGaeaServer)
    {
        GLChar* pChar = m_pGLGaeaServer->GetCharByDbNum(dwCharDbNum);

        if (pChar)
        {
            m_pGLGaeaServer->SENDTOCLIENT(pChar->ClientSlot(), MsgType, Buffer, UseCrc);
            return S_OK;
        }
    }

    return S_FALSE;
}

HRESULT GLLandMan::UpdateSummon ( float fTime, float fElapsedTime )
{
	// Summmon 업데이트

    for (SUMMON_DROP_MAP_ITER iter=m_SummonDrop.begin(); iter!=m_SummonDrop.end(); )
    {        
        GLSummonField* pSummon = iter->second;
        if (!pSummon)
        {
            ++iter;
            continue;
        }

        pSummon->FrameMove(fTime, fElapsedTime);
        
        // Note : 죽으면 삭제
        if (pSummon->IsDie())
        {
            ++iter;

			// 만약 주인이 캐릭터이며 고용 소환수를 사용 가능한 상태이면;
			ServerActor* pSummonOwner = m_pGLGaeaServer->GetActor( this, pSummon->m_sOwnerID );
			/*GLChar* pSummonOwner = m_pGLGaeaServer->GetChar64(
				pSummon->m_OwnerDbNum, pSummon->m_OwnerSvrGroupNum );*/
			if ( pSummonOwner->GetCrow() == CROW_PC )
			{
				GLChar* pChar = static_cast< GLChar* >( pSummonOwner );
				if ( pChar /*&& pChar->m_sSummonable.bSummonable*/ )
				{
					// 현재 고용한 소환수와 비교하여 같으면 리셋한다;
					if ( pSummon->GetGaeaID() == pChar->m_sSummonable.hireInfo.dwGaeaID )
					{
						//sc::writeLogInfo( "Reset Hire Summon!!!" );
						pChar->m_sSummonable.hireInfo.RESET();
					}
				}
			}

			// 제거;
			DropOutSummon(pSummon->GetGaeaID());
        }
        else
        {
            ++iter;
        }
    }
	return S_OK;
}

HRESULT GLLandMan::UpdateItem ( float fTime, float fElapsedTime )
{
	// Drop Item 업데이트 ----------------------------------------------------

	DWORD dwBackupPartyID;

    for (ITEM_DROP_MAP_ITER iter=m_ItemDropData.begin(); iter!=m_ItemDropData.end(); )
    {
        CItemDrop* pItem = iter->second;

		if( pItem == NULL ) 
		{
			++iter;
			continue;
		}
		/*
        if (!pItem)
        {
            ++iter;
            continue;
        }

        pItem->Update(fElapsedTime);

        if (pItem->IsDropOut())
        {
            ++iter;
            DropOutItem(pItem->dwGlobID); 
        }
        else
        {
            ++iter;
        }*/

		// rm #826 1차 수정은 요기서 함..1차 수정후 문제가 또다시 발생하면 좀더 까다로운 로직 추가 예정( 루팅룰이 좀 바뀔수 있다. )
		
		dwBackupPartyID = PARTY_NULL;
		
		if( pItem->emGroup == EMGROUP_PARTY) dwBackupPartyID = pItem->dwHoldGID;

		pItem->Update(fElapsedTime);

		bool bResult = false;

		if( pItem->emGroup == EMGROUP_PARTY && dwBackupPartyID != pItem->dwHoldGID  )
		{
			bResult = true;
		}
		
		if( bResult && dwBackupPartyID != PARTY_NULL )	{

			GLPartyTenderManagerField* pTenderMgr = m_pGLGaeaServer->GetPartyTenderItemManager();

			const GLPartyID partyId( dwBackupPartyID );

			bool bLoot = pTenderMgr->isLootable(partyId, pItem->dwGlobID);

			if( bLoot ) pItem->bLock = false;
			
			if( !bLoot && !pItem->bLock ) 
			{
				pItem->bLock     = true;
				pItem->dwHoldGID = dwBackupPartyID;
			}			
		}
		else
		if( pItem->emGroup == EMGROUP_PARTY && pItem->bLock ) {
			// 락을 풀어야 한다.
			GLPartyTenderManagerField* pTenderMgr = m_pGLGaeaServer->GetPartyTenderItemManager();

			const GLPartyID partyId( dwBackupPartyID );

			bool bLoot = pTenderMgr->isLootable(partyId, pItem->dwGlobID);

			if( bLoot ) {
				pItem->bLock = false;
				pItem->dwHoldGID = PARTY_NULL;
			}
		}

		if( pItem->IsDropOut() )
		{
			++iter;
			DropOutItem( pItem->dwGlobID, true );
		}
		else
		{
			++iter;
		}
		
    }

    for (MONEY_DROP_MAP_ITER iter=m_MoneyDropData.begin(); iter!=m_MoneyDropData.end(); )
    {
        CMoneyDrop* pMoney = iter->second;
        if (!pMoney)
        {
            ++iter;
            continue;
        }

        pMoney->Update(fElapsedTime);
        if (pMoney->IsDropOut())
        {
            ++iter;
            // 주의 DropOutMoney 안에서 현재 iterator 가 삭제된다.
            DropOutMoney(pMoney->dwGlobID);
        }
        else
        {
            ++iter;
        }
    }

	return S_OK;
}

BOOL GLLandMan::DropSaveCommission()
{
    for (CROW_DROP_MAP_ITER iter=m_CrowDrop.begin(); iter!=m_CrowDrop.end(); ++iter)    
    {
        GLCrow* pCrow = iter->second;
        //	소환된 NPC 삭제
        if (pCrow && pCrow->m_bNpcRecall)
            m_pGLGaeaServer->SaveNpcCommissionDB(pCrow->m_dwCallCharID, pCrow->m_dwCallUserID, pCrow->m_lnCommission);
    }
	return TRUE;
}

HRESULT GLLandMan::UpdateCrow ( float fTime, float fElapsedTime )
{
	if (m_GlobPCList.m_dwAmount > 0 || m_bEmulator)
	{
        UpdateCrowNormal(fTime, fElapsedTime);
        UpdateMaterial(fTime, fElapsedTime);
	}
	return S_OK;
}

void GLLandMan::UpdateMaterial( float fTime, float fElapsedTime ) 
{
    PROFILE_BEGIN("m_GlobMATERIALList");

    for (MATERIAL_DROP_MAP_ITER iter=m_MaterialDrop.begin(); iter!=m_MaterialDrop.end(); )    
    {
        GLMaterial* pMaterial = iter->second;
        if (!pMaterial)
        {
            ++iter;
            continue;
        }

        pMaterial->FrameMove(fTime, fElapsedTime);
        bool bDropOut = false;

        DWORD GlobalId = pMaterial->GetGaeaID();
        BOOL bDie = pMaterial->IsDie();
        float Age = pMaterial->GetAge();

        // 사라져야할 Crow 처리
        if (pMaterial->m_pCrowData->m_sAction.m_dwActFlag & EMCROWACT_AUTODROP)
        {
            if (Age > pMaterial->m_pCrowData->m_sAction.m_fLiveTime)
            {
                if (!bDropOut)
                {
                    bDropOut = true;
                    ++iter;
                    DropOutMaterial(GlobalId);
                }
            }
        }
        
        if (bDie)
        {
            m_listDelGlobIDExMat.remove(GlobalId);
            {
                if (!bDropOut)
                {
                    bDropOut = true;
                    ++iter;
                    DropOutMaterial(GlobalId);
                }
            }
        }

        if (!bDropOut)
            ++iter;
    }
    PROFILE_END("m_GlobMATERIALList");
}

void GLLandMan::UpdateCrowNormal(float fTime, float fElapsedTime)
{
    PROFILE_BEGIN("m_GlobCROWList");

    for (CROW_DROP_MAP_ITER iter=m_CrowDrop.begin(); iter!=m_CrowDrop.end(); )
    {
        GLCrow* pCrow = iter->second;
        if ( !pCrow )
        {
            ++iter;
            continue;
        }

        bool bDropOut = false;
        pCrow->FrameMove(fTime, fElapsedTime);
        
        DWORD GlobalId = pCrow->GetGaeaID();
        BOOL bBoss = pCrow->IsCheckedActFlags( EMCROWACT_BOSS );
        bool bNpcRecall = pCrow->IsNpcRecall();
        __time64_t CrowGenTime = pCrow->GetGenTime();
        DWORD dwGenTime = pCrow->GetGenTimeFromData();
        DWORD CallCharID = pCrow->m_dwCallCharID;
        DWORD CallUserID = pCrow->m_dwCallUserID;
        LONGLONG Commission = pCrow->m_lnCommission;
        BOOL bDie = pCrow->IsDie();

        // 사라져야할 Crow 처리
        if (pCrow->m_pCrowData->m_sAction.m_dwActFlag & EMCROWACT_AUTODROP)
        {
            if (pCrow->GetAge() > pCrow->m_pCrowData->m_sAction.m_fLiveTime)
            {
                if (!bDropOut)
                {
                    bDropOut = true;
                    ++iter;
                    DropOutCrow(GlobalId);
                }
            }
        }

        // pCrow 가 삭제되었다. 사용하면 안된다.
        // 소환된 NPC 삭제
        if (!bBoss && bNpcRecall)
        {
            __time64_t tCurTime = CTime::GetCurrentTime().GetTime();
            __time64_t tGenTime = CrowGenTime +  dwGenTime;

            if (tCurTime > tGenTime) 
            {
                m_pGLGaeaServer->SaveNpcCommission(CallCharID, CallUserID, Commission);
                if (!bDropOut)
                {
                    bDropOut = true;
                    ++iter;
                    DropOutCrow(GlobalId);
                }
            }
        }

        if (bDie)
        {
            m_listDelGlobIDEx.remove(GlobalId);
            if (!bDropOut)
            {
                bDropOut = true;
                ++iter;
                DropOutCrow(GlobalId);
            }
        }

        if (!bDropOut)
            ++iter;
    }
    PROFILE_END("m_GlobCROWList");
}

HRESULT GLLandMan::UpdateReserveDropCrow(float fTime, float fElapsedTime)
{
	DROP_CROW_LIST_ITER iterCrow = this->m_DropCrowList.begin();
	while ( iterCrow != this->m_DropCrowList.end() )
	{
		iterCrow->m_fIdleTime += fElapsedTime;
		if ( iterCrow->m_fIdleTime > iterCrow->m_fMobLinkDelay )
		{
			GLLandMan::DropCrow(
				iterCrow->m_pMobSchedule, iterCrow->m_sMobLinkID, 
				iterCrow->m_fPositionXZ[0], iterCrow->m_fPositionXZ[1], 
				iterCrow->m_fMobLinkScale);
			iterCrow = this->m_DropCrowList.erase(iterCrow);			
		}
		else
			iterCrow++;
	}	

	return S_OK;
}
//
//mjeon
// InstantMap과 TutorialMap의 처리를 합치도록 한다. (불필요한 코드중복, 연산 등)
//
BOOL GLLandMan::Empty()
{
	if((m_GlobPCList.m_dwAmount == 0 && m_GlobPETList.m_dwAmount == 0))
		return TRUE;
	else
		return FALSE;
}

//
//mjeon.tbb
//
void GLLandMan::EnterLand(GLChar *pChar)
{	
	if( pChar )
	{
		// 이동 처리가 완료되기 직전에 해야할 일을 수행한다;
		pChar->ToDoRightBeforeRecallDone( this, GetMapID() );
	
		pChar->SetCurrentMap( m_mapID );
		
		pChar->m_pCharNodeInLand = m_GlobPCList.ADDHEAD( pChar );
		if ( m_pCountRecorder )
		{
			m_pCountRecorder->add_CurPlayerCount();
			m_pCountRecorder->add_CurPartyPlayerCount( pChar->GetPartyID().GetPartyID() );
			m_pCountRecorder->add_CurClubPlayerCount( pChar->GetClubDbNum() );
			m_pCountRecorder->add_CurCountryPlayerCount( pChar->GetCountryInfo().dwID );
			m_pCountRecorder->add_CurSchoolPlayerCount( pChar->GetSchool() );
		}
		
		const SMAPNODE* pMapNode = m_pGLGaeaServer->FindMapNode(GetMapID());

		if( PRISONLIMIT.IsLimitStatus(pMapNode, pChar->UserLevel(), PML_WAR ) ) {
			GLMSG::SNETPC_PRISON_AG msg;

			msg.dwChaNum = pChar->CharDbNum();

			m_pGLGaeaServer->SENDTOAGENT(&msg);
		}		
	}
	//
	//mjeon.CaptureTheField
	//
	//  여기에서 CTF에 관련된 각종 동기화 작업을 하자.
	//  캐릭터가 FieldServer로 신규접속할 경우(다른 FieldServer로부터의 이동 포함 - GLGaeaServer::DropPC()로부터)와
	//  기존 FieldServer에서 맵만 이동할 경우(GLGaeaServer::RequestInvenRecallThisSvr()로부터) 모두 이 루틴을 거친다.
	//
}

HRESULT GLLandMan::FrameMove(float fTime, float fElapsedTime, __time64_t &CurrentTime)//FRAMEMOVE_TIMEVARS &fmtv)
{
	HRESULT hr;
	
    // Land 에서 삭제할 캐릭터를 지운다
    FrameMoveClearDelList();

	//
	// 0.Trigger
	//

	PROFILE_BEGIN("m_spTriggerFinger::FrameMove");

	if (m_spTriggerFinger)
	{
		m_spTriggerFinger->FrameMove(fTime, fElapsedTime);
	}

	PROFILE_END("m_spTriggerFinger::FrameMove");

	//
	// 1.LandEvent
	//
	PROFILE_BEGIN("m_GlobLandEvent::FrameMove");
	if ( m_pLandEvent != NULL )
		m_pLandEvent->FrameMove(fElapsedTime);
	
	PROFILE_END("m_GlobLandEvent::FrameMove");


	//
	// 2.UpdateItem
	//
	PROFILE_BEGIN("UpdateItem()");
		hr = UpdateItem ( fTime, fElapsedTime );
		if ( FAILED(hr) )
		{
			PROFILE_END("UpdateItem()");
			return hr;
		}
	PROFILE_END("UpdateItem()");


	//
	// 3.UpdateCrow
	//
	PROFILE_BEGIN("UpdateCrow()");
		hr = UpdateCrow ( fTime, fElapsedTime );
		if ( FAILED(hr) )
		{
			PROFILE_END("UpdateCrow()");
			return hr;
		}
	PROFILE_END("UpdateCrow()");


	//
	// 4.UpdateSummon
	//
	PROFILE_BEGIN("UpdateSummon()");
	hr = UpdateSummon ( fTime, fElapsedTime );
	if ( FAILED(hr) )
	{
		PROFILE_END("UpdateSummon()");
		return hr;
	}
	PROFILE_END("UpdateSummon()");


	//
	// 5.MobSchMan.FrameMove
	//
    /**
        몬스터 리젠용 컨테이너.
        해당 컨테이너는 몹을 반복적으로 리젠하기 위해서 사용하는 컨테이너다.
        리젠만 반복으로 하게되며 몹 스케쥴러는 자동으로 관리되지 않는다.
    */
	PROFILE_BEGIN("m_mapMobSchManRegen->FrameMove()");
        //hr = m_MobSchMan->FrameMove ( fTime, fElapsedTime );
    BOOST_FOREACH( const MOB_SCH_MAN_MAP_VALUE& mapValue, m_mapMobSchManRegen )
    {
        SpMobScheduleManServer spMovSchMan( mapValue.second );

        spMovSchMan->FrameMove( fTime, fElapsedTime );
        if ( FAILED(hr) )
        {
            PROFILE_END("m_mapMobSchManRegen->FrameMove()");
            return hr;
        }
    }
	PROFILE_END("m_mapMobSchManRegen->FrameMove()");


	//
	// 6.MobSchManEx.FrameMove
	//
    /** 몬스터 자동관리 컨테이너.
        이 컨테이너는 몹을 한번만 리젠시키는 용도로 사용한다. 
        모든 몬스터가 소멸되면 스케쥴러가 자동으로 폐기되어 관리된다.
    */
    PROFILE_BEGIN("m_mapMobSchManOnce->FrameMove()");
    MOB_SCH_MAN_LIST_ITER it = m_mapMobSchManOnce.begin();
    while( it != m_mapMobSchManOnce.end() )
    {
        SpMobScheduleManServer spMobSch = *it;
        spMobSch->FrameMove( fTime, fElapsedTime );

        if( spMobSch->GetAliveCount() <= 0 )
        {
			// 남아있는게 없는 경우 바로 삭제한다 ( 예약하여 삭제할 필요가 없다 );
			CleanUpMobSchMan( spMobSch.get() );
			m_mapMobSchManOnce.erase( it++ );
		}
		else
		{
			it++;
		}
    }
    PROFILE_END("m_mapMobSchManOnce->FrameMove()");

	//
	// 7.DropCrow
	//
	PROFILE_BEGIN("UpdateReserveDropCrow()");
	// 예약되어 있는 몹 리스트를 관리;
	// 갱신 시간이 지나면 맵에 등장;
	//
	{
		UpdateReserveDropCrow(fTime, fElapsedTime);
	}
	PROFILE_END("UpdateReserveDropCrow()");


	//
	// 8.UpdateSkillAct
	//
	PROFILE_BEGIN("UpdateSkillAct()");
		// 지연된 스킬 영향 처리.
		//
		UpdateSkillAct(fElapsedTime);
	PROFILE_END("UpdateSkillAct()");


	//
	// 9.Characters FrameMove
	//
	PROFILE_BEGIN("pCharNode->Data->FrameMove()");
		GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
		if( pCharNode )
		{
			for ( ; pCharNode; pCharNode = pCharNode->pNext )
			{
				if( pCharNode )
				{
					GLChar* pChar = pCharNode->Data;			
					if (pChar)
						pChar->FrameMove(fTime, fElapsedTime, CurrentTime);
				}
			}
		}
	PROFILE_END("pCharNode->Data->FrameMove()");
	
	//
	// 10.Pets FrameMove
	//
	PROFILE_BEGIN("pPetNode->Data->FrameMove ()");
	GLPETNODE* pPetNode = m_GlobPETList.m_pHead;
	for ( ; pPetNode; pPetNode = pPetNode->pNext )
	{
		PGLPETFIELD pPetfield = pPetNode->Data;
		if (pPetfield && pPetfield->IsValid()) 
		{
			pPetfield->FrameMove(fTime, fElapsedTime);
			if (pPetfield->IsNotEnoughFull()) 
			{
				//GLLandMan::FrameMove가 TBB에 의해 쪼개져서 병렬처리될 경우, m_pGLGaeaServer의 member에 접근해야할 경우 해당 member는 동시성이 보장되어야 한다.
				//그러므로 TBB를 사용할 경우 vector -> concurrent_vector로 교체.
				m_pGLGaeaServer->ReserveDropOutPet ( SDROPOUTPETINFO(pPetfield->m_dwGUID,false,false) );
			}
		}
	}
	PROFILE_END("pPetNode->Data->FrameMove ()");


	//
	// 11.Move Characters
	//
	PROFILE_BEGIN("MoveChar()");
		// PC 'MOVE CELL'가 등록된 셀을 업데이트함.
		//
		pCharNode = m_GlobPCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			GLChar* pPC = pCharNode->Data;			
			if (pPC)
			    MoveChar(pPC->GetGaeaID(), pPC->GetViewPosition());
		}
	PROFILE_END("MoveChar()");

	//
	// 12.Move Pets
	//
	PROFILE_BEGIN("MovePET()");
		pPetNode = m_GlobPETList.m_pHead;
		for ( ; pPetNode; pPetNode = pPetNode->pNext )
		{
			PGLPETFIELD pPet = pPetNode->Data;
			if (pPet)
			    MovePet(pPet->m_dwGUID, pPet->m_vPos);
		}
	PROFILE_END("MovePET()");


	//
	// 13.UpdateViewAround
	//
	PROFILE_BEGIN("pCharNode->Data->UpdateViewAround()");
	// 케릭터가 보고 있는 영역을 검사하고 갱신한다. - 가장 마지막 부분에서 해야함.
	//		다른 (PC,MOB,NPC)등 이동개체가 모두 'MOVE CELL' 을 한후에 행해져야 한다.
	//
	{
		pCharNode = m_GlobPCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			GLChar* pChar = pCharNode->Data;
			if (pChar)
			    pChar->UpdateViewAround();
		}
	}
	PROFILE_END("pCharNode->Data->UpdateViewAround()");

	//
	// 14.Weather
	//
	PROFILE_BEGIN("Weather()");
	// 날씨 변화 클라이언트에 업데이트.
	//
	UpdateWeather();
	PROFILE_END("Weather()");


	//
	// 15.Remove MobSchMan
	//
	PROFILE_BEGIN( "UpdateRemoveMobSchManLayer()" );
	hr = UpdateRemoveMobSchManLayer();
	if ( FAILED( hr ) )
	{
		PROFILE_END( "UpdateRemoveMobSchManLayer()" );
	}
	PROFILE_END( "UpdateRemoveMobSchManLayer()" );


    // Land 에서 삭제할 캐릭터를 지운다
    // GLGaeaServer::FrameMove 상단에서 실제로 Char 를 삭제하기 때문에 여기서 지워주지 않으면
    // 지워진 캐릭터가 남아있어 문제가 발생할 가능성이 있다.
    FrameMoveClearDelList();

    ExtraFrameMoveInstantMap(fElapsedTime);

    
    // 수수료 관련 통계, 로그를 위한 FramMove;
    /*const SMAPNODE* pMapNode = m_pGLGaeaServer->FindMapNode( GetBaseMapID() );
    if ( (pMapNode) && (true == pMapNode->IsCommission()) )
    {
        m_fCommissionLogTimer += fElapsedTime;
        if ( m_fCommissionLogTimer > 1800.0f )
        {
            sc::writeLogInfo(
                sc::string::format(
                "CommissionLog: Land ( (%1%/%2%) / (%3%/%4%) ) Total BuyCommissionMoney %5%, SellCommissionMoney %6%",
                GetBaseMapID().Mid(),
                GetBaseMapID().Sid(),
                GetGaeaMapID().Mid(),
                GetGaeaMapID().Sid(),
                m_llBuyCommissionMoney,
                m_llSellCommissionMoney
                ));

            m_llBuyCommissionMoney = 0;
            m_llSellCommissionMoney = 0;
            m_fCommissionLogTimer = 0.0f;
        }        
    }*/

	if ( m_pCountRecorder )
		m_pCountRecorder->FrameMove(fElapsedTime);

	return S_OK;
}

void GLLandMan::UpdateWeather()
{
	DWORD dwNowWeather(0);
	DWORD i(0);

	std::vector<SONEMAPWEATHER> vecMapNowWeather;
	vecMapNowWeather.clear();

	GLPeriod & glPeriod = GLPeriod::GetInstance();

	if ( IsWeatherActive() )
	{

		//////////////////////////////////////////////////////////////////////////
		/// 전체 맵이 추가되는 날씨

		dwNowWeather = glPeriod.GetWeather ();

		if ( m_dwOldWeather != dwNowWeather )
		{
			GLMSG::SNETPC_WEATHER NetMsg;
			NetMsg.dwWeather = dwNowWeather;

			GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
			
			for ( ; pCharNode; pCharNode = pCharNode->pNext )
			{
				GLChar* pChar = pCharNode->Data;
				
				if( !pChar )
					continue;

				m_pGLGaeaServer->SENDTOCLIENT ( pChar->ClientSlot(), &NetMsg );
			}

			m_dwOldWeather = dwNowWeather;
		}

		// 일시적 날씨 효과. ( 번개 등등. )
		//
		DWORD dwWhimsical = glPeriod.GetWhimsicalWeather();
		
		if ( dwWhimsical )
		{
			GLMSG::SNETPC_WHIMSICALWEATHER NetMsg;
			NetMsg.dwWhimsical = dwWhimsical;

			GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
			
			for ( ; pCharNode; pCharNode = pCharNode->pNext )
			{
				GLChar* pChar = pCharNode->Data;
				
				if( !pChar )
					continue;

				m_pGLGaeaServer->SENDTOCLIENT ( pChar->ClientSlot(), &NetMsg );
			}
		}

		//////////////////////////////////////////////////////////////////////////
		/// 특정 맵에 추가되는 날씨
		// Add OneMap Weather
		vecMapNowWeather = glPeriod.GetOneMapWeather();
		bool bNewWeather = FALSE;
		if( vecMapNowWeather.size() != m_vecMapOldWeather.size() )
		{
			bNewWeather = TRUE;
		}
		else
		{
			for( i = 0; i < m_vecMapOldWeather.size(); i++ )
			{
				SONEMAPWEATHER mapNowWeather = vecMapNowWeather[i];
				SONEMAPWEATHER mapOldWeather = m_vecMapOldWeather[i];
				
				if( mapNowWeather != mapOldWeather )
				{
					bNewWeather = TRUE;
					break;
				}
			}
		}

		if( bNewWeather )
		{
			GLMSG::SNETPC_MAPWEATHER NetMsg;
			if( vecMapNowWeather.size() >= 32 )
			{
				NetMsg.dwMapWeatherSize = 31;
			}else{
				NetMsg.dwMapWeatherSize = (DWORD)vecMapNowWeather.size();
			}


			DWORD i;
			for( i = 0; i < vecMapNowWeather.size(); i++ )
			{
				if( i >= 32 )
					break;

				NetMsg.MapWeather[i] = vecMapNowWeather[i];
			}


			GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
			for ( ; pCharNode; pCharNode = pCharNode->pNext )
			{
				GLChar* pChar = pCharNode->Data;
				
				if( !pChar )
					continue;

				m_pGLGaeaServer->SENDTOCLIENT ( pChar->ClientSlot(), &NetMsg );
			}
			m_vecMapOldWeather = vecMapNowWeather; 
		}


		// 일시적 날씨 효과. ( 번개 등등. )
		//
		for( i = 0; i < m_vecMapOldWeather.size(); i++ )
		{
			DWORD dwWhimsical = m_vecMapOldWeather[i].dwWhimsicalWeather;
			
			if ( dwWhimsical )
			{
				GLMSG::SNETPC_MAPWHIMSICALWEATHER NetMsg;
				NetMsg.MapWeather = m_vecMapOldWeather[i];

				GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
				
				for ( ; pCharNode; pCharNode = pCharNode->pNext )
				{
					GLChar* pChar = pCharNode->Data;
					
					if( !pChar )
						continue;

					m_pGLGaeaServer->SENDTOCLIENT ( pChar->ClientSlot(), &NetMsg );
				}
			}
		}
	}
}

HRESULT	GLLandMan::RendQuad ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	// 트리가 생성되었을때만 그려준다.
	//
	if ( m_spNaviMesh )
	{
		D3DXVECTOR3 vMax, vMin;
		m_spNaviMesh->GetAABB ( vMax, vMin );
		EDITMESHS::RENDERAABB ( pd3dDevice, vMax, vMin );
	}

	if ( m_LandTree.GetRootNode() )
	{
		D3DXVECTOR3 vCenter ( (float)m_LandTree.GetAxisX(), 0.f, (float)m_LandTree.GetAxisZ() );
		D3DXVECTOR3 vSize ( (float)m_LandTree.GetSizeX()/2, 0.f, (float)m_LandTree.GetSizeZ()/2 );
		D3DXVECTOR3 vSnap ( (float)m_LandTree.GetDivSizeX(), 0, (float)m_LandTree.GetDivSizeZ() );

		DWORD dwZFunc;
		pd3dDevice->GetRenderState ( D3DRS_ZFUNC,		&dwZFunc );
		pd3dDevice->SetRenderState ( D3DRS_ZFUNC,		D3DCMP_ALWAYS );
		EDITMESHS::RENDERPLANE ( pd3dDevice, vCenter, vSize, vSnap );
		pd3dDevice->SetRenderState ( D3DRS_ZFUNC,		dwZFunc );
	}

	return S_OK;
}

HRESULT GLLandMan::Render(LPDIRECT3DDEVICEQ pd3dDevice, CLIPVOLUME& cv)
{
    for (CROW_DROP_MAP_ITER iter=m_CrowDrop.begin(); iter!=m_CrowDrop.end(); ++iter)
    {
        GLCrow* pCrow = iter->second;
        if (pCrow)
            pCrow->Render(pd3dDevice, cv, TRUE);
    }

	if (m_spNaviMesh)
        m_spNaviMesh->Render(pd3dDevice);

	GLLevelFileServer::Render ( pd3dDevice, cv );;

	return S_OK;
}


HRESULT GLLandMan::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GLLevelFileServer::InitDeviceObjects ( m_pd3dDevice );
	return S_OK;
}

HRESULT GLLandMan::RestoreDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	return S_OK;
}

HRESULT GLLandMan::InvalidateDeviceObjects()
{
	return S_OK;
}

HRESULT GLLandMan::DeleteDeviceObjects()
{
	GLLevelFileServer::DeleteDeviceObjects ();
	return S_OK;
}

BOOL GLLandMan::IsCollisionNavi ( D3DXVECTOR3 &vPoint1, D3DXVECTOR3 &vPoint2, D3DXVECTOR3 &vCollision )
{
	BOOL bCollision;
	DWORD dwCollisionID;
	if ( m_spNaviMesh )
	{
		m_spNaviMesh->IsCollision ( vPoint1, vPoint2, vCollision, dwCollisionID, bCollision );
	}

	return bCollision;
}

/**
 * 공격시 받는 HP값.
 *
 * @param sACT					[in] struct SSKILLACT.
 * @param nHP					[out] HP값 (변경되지 않으면 이전값 보존).
 */
void GetAttackHP(const SSKILLACT& sACT, int& nHP)
{
	if (sACT.nVAR_HP < 0)
	{
		switch (sACT.emAPPLY)
		{
		case SKILL::EMAPPLY_MELEE:
		case SKILL::EMAPPLY_RANGE:
			nHP = int(sACT.nVAR_HP * GLCONST_CHAR::fPK_POINT_DEC_PHY);
			break;

		case SKILL::EMAPPLY_MAGIC:
			nHP = int(sACT.nVAR_HP * GLCONST_CHAR::fPK_POINT_DEC_MAGIC);
			break;
		} //switch

        // 데미지 0 이 되는 문제로 임시로 넣은 코드이다.
        // 언젠가는 코드를 수정해야 한다. 2012-01-30 jgkim
        if (nHP == 0)
            nHP = -1;

	} //if
} //GetAttackHP

/**
 * DoSkillAct함수에서 처리한 결과 송신.
 *
 * @param sACT					[in] struct SSKILLACT.
 * @param pACTOR				[in] a actor.
 * @param pREACTOR				[in] a reactor.
 * @param nVAR_HP				[in] modified hp.
 */
void GLLandMan::SendSkillAct( const SSKILLACT& sACT, ServerActor* pACTOR, ServerActor* pREACTOR, int nVAR_HP, bool damageswap )
{
    if( !pACTOR )
        return;

	if ( sACT.VALID_VAR() )
	{
        int nActualVarHP(nVAR_HP);
        int nActualVarMP(sACT.nVAR_MP);
        int nActualVarSP(sACT.nVAR_SP);
		// 타겟의 기초 체력 변화.
		pREACTOR->VAR_BODY_POINT ( sACT.sID.emCrow, sACT.sID.GaeaId, sACT.dwDamageFlag, nActualVarHP, nActualVarMP, nActualVarSP );
		if( sACT.dwDamageFlag & DAMAGE_TYPE_SHOCK )
			pREACTOR->TurnAction(GLAT_SHOCK);

		// 발동된 스킬 값 MSG.
		GLMSG::SNETPC_SKILLFACT_BRD		NetMsgBRD;
		NetMsgBRD.emCrow 		= sACT.sID_TAR.emCrow;
		NetMsgBRD.dwID   		= sACT.sID_TAR.GaeaId;
		NetMsgBRD.sACTOR.emCrow 	= sACT.sID.emCrow;				//	스킬을 사용한 행위자 마킹.
		NetMsgBRD.sACTOR.dwID   	= sACT.sID.GaeaId;
		NetMsgBRD.sSkillID			= sACT.sSkillID;		
		NetMsgBRD.dwDamageFlag	= sACT.dwDamageFlag;
		NetMsgBRD.nVAR_HP 		= nActualVarHP;
		NetMsgBRD.nVAR_MP 		= nActualVarMP;
		NetMsgBRD.nVAR_SP 		= nActualVarSP;
		NetMsgBRD.wSkillLevel		= sACT.wSkillLevel;
        NetMsgBRD.wSchool 			= pACTOR->GetSchool();

		pREACTOR->SendMsgViewAround(&NetMsgBRD);								// '스킬대상'의 주변 클라이언트들에게 메세지 전송.

		if( sACT.sID_TAR.emCrow == CROW_PC )
		{
			GLChar* pChar = (GLChar*)pREACTOR;
			if ( GLHIDESET::CheckHideSet(EMHIDE_ON_HIT) )
			{
				pChar->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
				pChar->RestoreActStateByInvisible();
			}
			pChar->RELEASE_ENDURE_PROC();			
			m_pGLGaeaServer->SENDTOCLIENT( pREACTOR->GETCLIENTID (), &NetMsgBRD );		// 스킬 대상에게 메세지 전송.
		} //if

		pREACTOR->ReactDamage( pACTOR, sACT.dwDamageFlag );
	} //if	

	if( sACT.VALID_GATHER() && !damageswap )
	{
        int nActualGatherHP(sACT.nGATHER_HP);
        int nActualGatherMP(sACT.nGATHER_MP);
        int nActualGatherSP(sACT.nGATHER_SP);
		pACTOR->VAR_BODY_POINT ( sACT.sID.emCrow, sACT.sID.GaeaId, HEAL_TYPE_GATHER, nActualGatherHP, nActualGatherMP, nActualGatherSP );		// 자신에게 흡수값 반영.

		// 발동된 스킬 값 MSG.
		GLMSG::SNETPC_SKILLFACT_BRD			NetMsgBRD;
		NetMsgBRD.emCrow		= sACT.sID.emCrow;
		NetMsgBRD.dwID			= sACT.sID.GaeaId;
		NetMsgBRD.sACTOR.emCrow	= sACT.sID.emCrow;					// 스킬을 사용한 행위자 마킹.
		NetMsgBRD.sACTOR.dwID	= sACT.sID.GaeaId;
		NetMsgBRD.dwDamageFlag	= sACT.dwDamageFlag;
		NetMsgBRD.nVAR_HP 		= nActualGatherHP;
		NetMsgBRD.nVAR_MP 		= nActualGatherMP;
		NetMsgBRD.nVAR_SP 		= nActualGatherSP;
        
        if( pACTOR )
            NetMsgBRD.wSchool = pACTOR->GetSchool();

		pACTOR->SendMsgViewAround(&NetMsgBRD);									// '스킬대상'의 주변 클라이언트들에게 메세지 전송.

		if( sACT.sID_TAR.emCrow == CROW_PC )	
			m_pGLGaeaServer->SENDTOCLIENT( pACTOR->GETCLIENTID(), &NetMsgBRD );		// 스킬 대상에게 메세지 전송.
	} //if
} //GLLandMan::SendSkillAct

void GLLandMan::DoSkillAct( const SSKILLACT &sACT )
{
    ServerActor*	pACTOR   = m_pGLGaeaServer->GetTarget( this, sACT.sID );
	ServerActor*	pREACTOR = m_pGLGaeaServer->GetTarget( this, sACT.sID_TAR );

    if ( !pACTOR || !pREACTOR )
		return;

	if ( !pREACTOR->IsValidBody() )
		return;

	// 공격 속성이고 플레이어에서 플레이어를 공격시에, 대련자인지 검사.
	bool			bREACT		= true;
	int				nVAR_HP		= sACT.nVAR_HP;
	GLChar*			pChar		= NULL;
	GLSummonField*	pSummon		= NULL;

	if (sACT.sID.emCrow == CROW_PC)
	{
		pChar = m_pGLGaeaServer->GetChar(sACT.sID.GaeaId);		
	}
	else if (sACT.sID.emCrow == CROW_SUMMON)
	{
		pSummon = GetSummon(sACT.sID.GaeaId);
		if (pSummon)
        {
			if ( pSummon->m_sOwnerID.actorType == CROW_PC )
			{
				GLChar* pSummonOwner =
					static_cast< GLChar* >(
					m_pGLGaeaServer->GetActor( this, pSummon->m_sOwnerID ) );				
				if (pSummonOwner)
				{
					pChar = pSummonOwner;
				}
			}
        }
	} //if..else if

	//SSKILLACT sActTemp = sACT;

	bool bDamageswap = false;

	if( sACT.IsATTACK() )
	{
		if( sACT.sID.emCrow		== CROW_PC && 
			pREACTOR->GetCrow() == CROW_PC )
		{
			bREACT = false;
			if( pChar )
			{
                bREACT = ( pChar->IsReActionable(pREACTOR, EMIMPACT_SIDE_ENEMY) ? true : false );
				GetAttackHP( sACT, nVAR_HP );
			}
		}
		else if( sACT.sID.emCrow	 == CROW_SUMMON && 
				 pREACTOR->GetCrow() == CROW_SUMMON )
		{
			bREACT = false;			
			if( pSummon )
			{
				bREACT = ( pSummon->IsReActionable ( pREACTOR, EMIMPACT_SIDE_ENEMY ) ? true : false );
				GetAttackHP( sACT, nVAR_HP );
			}
		}
		else if( sACT.sID.emCrow	 == CROW_PC && 
				 pREACTOR->GetCrow() == CROW_SUMMON )
		{
			bREACT = false;			
			if( pChar )
			{
				bREACT = ( pChar->IsReActionable(pREACTOR, EMIMPACT_SIDE_ENEMY) ? true : false );
				GetAttackHP( sACT, nVAR_HP );
			}
		}
		else if( sACT.sID.emCrow	 == CROW_SUMMON && 
				 pREACTOR->GetCrow() == CROW_PC )
		{
			bREACT = false;			
			if( pSummon )
			{
                bREACT = ( pSummon->IsReActionable ( pREACTOR, EMIMPACT_SIDE_ENEMY ) ? true : false );
				GetAttackHP( sACT, nVAR_HP );
			}
		} //if..else if
	} //if
	
	if( bREACT )
	{
		SendSkillAct( sACT, pACTOR, pREACTOR, nVAR_HP );
		//SendSkillAct( sActTemp, pACTOR, pREACTOR, nVAR_HP, bDamageswap );
	}
} //GLLandMan::DoSkillAct

void GLLandMan::DoSkillActEx( const SSKILLACTEX &sACTEX )
{
    ServerActor* pACTOR   = m_pGLGaeaServer->GetTarget ( this, sACTEX.sID );
	ServerActor* pREACTOR = m_pGLGaeaServer->GetTarget ( this, sACTEX.sID_TAR );
	
	if ( !pREACTOR )
		return;

	if ( !pREACTOR->IsValidBody() )
		return;

	if ( pREACTOR->GetNowHP()==0 )
		return;
	
	PGLSKILL	pSkill	= GLSkillMan::GetInstance().GetData( sACTEX.idSKILL );	

	if( pSkill != NULL )		// 상대방에게 지속형 스킬 이팩트를 삽입 시도. ( 스킬 설정에 따라 지속성 여부 다름. )
	{
		DAMAGE_SPEC*	pDamageSpec = pREACTOR->GetDamageSpecPtr();
		if ( CHECK_FLAG_SKILL(pDamageSpec->m_emImmuneActionType, pSkill->m_sBASIC.emACTION_TYPE) ||
			CHECK_FLAG_SKILL(pDamageSpec->m_emImmuneApplyType, pSkill->m_sBASIC.emAPPLY))
			return;

		DWORD		dwSELSLOT(0);

		pREACTOR->RECEIVE_SKILLFACT( sACTEX.idSKILL, sACTEX.wSKILL_LVL, dwSELSLOT, sACTEX.sID );

		// 지속형 스킬일 경우.
		if( dwSELSLOT != UINT_MAX )
		{
			// 발동된 스킬 값 MSG.
			GLMSG::SNETPC_SKILLHOLD_BRD		NetMsgBRD;
			NetMsgBRD.emCrow    = sACTEX.sID_TAR.emCrow;
			NetMsgBRD.dwID		= sACTEX.sID_TAR.GaeaId;
			NetMsgBRD.skill_id	= sACTEX.idSKILL;
			NetMsgBRD.wLEVEL    = sACTEX.wSKILL_LVL;
			NetMsgBRD.wSELSLOT	= static_cast<WORD>(dwSELSLOT);
			NetMsgBRD.fPARAM    = pREACTOR->GetMaxHpRate();
            NetMsgBRD.sID       = sACTEX.sID;
						
			if( sACTEX.sID_TAR.emCrow == CROW_PC )
			{ // '스킬대상'의 주변 클라이언트들에게 메세지 전송.
				GLChar*		pChar	= (GLChar*) pREACTOR;
				if( pSkill && !( pChar->VehicleState() && pSkill->m_sBASIC.emIMPACT_SIDE != EMIMPACT_SIDE_ENEMY) )
				{																					
					m_pGLGaeaServer->SENDTOCLIENT ( pREACTOR->GETCLIENTID (), &NetMsgBRD );
					pREACTOR->SendMsgViewAround(&NetMsgBRD);
					if ( sACTEX.sID != sACTEX.sID_TAR ){
						if ( pSkill->IsBuff() )
						{
							if ( GLHIDESET::CheckHideSet(EMHIDE_TAKE_BUFF) )
							{
								pChar->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
								pChar->RestoreActStateByInvisible();
							}

						}
						else if ( pSkill->IsLimit() )
						{
							if ( GLHIDESET::CheckHideSet(EMHIDE_TAKE_LIMIT) )
							{
								pChar->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
								pChar->RestoreActStateByInvisible();
							}
						}
						else if ( pSkill->IsHarm() )
						{
							if ( GLHIDESET::CheckHideSet(EMHIDE_TAKE_DEBUFF) )
							{
								pChar->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
								pChar->RestoreActStateByInvisible();
							}
						}
					}
				}
			}
			else
			{ // 스킬 대상에게 메세지 전송.
				pREACTOR->SendMsgViewAround(&NetMsgBRD);
			} //if..else
		} //if
	} //if

	if( sACTEX.dwCUREFLAG!=NULL )
		pREACTOR->CURE_STATEBLOW ( sACTEX.dwCUREFLAG );

    // 상태이상 제거 플래그가 있다면
    for ( DWORD i = 0 ; i < SKILL::EMACTION_TYPE_NSIZE; ++i )
    {
        if ( sACTEX.dwRemoveFlag[i] == true )
            pREACTOR->SKILL_EFFECT_REMOVE ( i );
    }
	
	if( sACTEX.sSTATEBLOW.emBLOW!=EMBLOW_NONE && pREACTOR->STATEBLOW ( sACTEX.sSTATEBLOW ))
	{		
		// 상태이상 발생 Msg.
		GLMSG::SNETPC_STATEBLOW_BRD		NetMsgState;
		NetMsgState.emCrow		= sACTEX.sID_TAR.emCrow;
		NetMsgState.dwID		= sACTEX.sID_TAR.GaeaId;
		NetMsgState.emBLOW		= sACTEX.sSTATEBLOW.emBLOW;
		NetMsgState.fAGE		= sACTEX.sSTATEBLOW.fAGE;
		NetMsgState.fSTATE_VAR1 = sACTEX.sSTATEBLOW.fSTATE_VAR1;
		NetMsgState.fSTATE_VAR2 = sACTEX.sSTATEBLOW.fSTATE_VAR2;

        if( pACTOR )
            NetMsgState.wSchool = pACTOR->GetSchool();

		// '스킬대상'의 주변 클라이언트들에게 메세지 전송.
		//
		pREACTOR->SendMsgViewAround(&NetMsgState);

		// 스킬 대상에게 메세지 전송.
		if ( sACTEX.sID_TAR.emCrow == CROW_PC )
			m_pGLGaeaServer->SENDTOCLIENT ( pREACTOR->GETCLIENTID (), &NetMsgState );
	} //if

	// 강한타격, 끌어오기, 밀어내기
	if( sACTEX.fPUSH_PULL != 0.0f )
	{
		//	선도 인증중인지 체크해야 됨
		if( m_bGuidBattleMap && m_pGuidance )
		{
			if( pREACTOR->GetCrow() == CROW_PC && m_pGuidance->m_dwCERTIFY_CHARID == pREACTOR->GetCharID() )
				return;
		} ///if

		if ( sACTEX.bOnward )
			PushPullAct( sACTEX.sID_TAR, sACTEX.sID, sACTEX.fPUSH_PULL, sACTEX.fPUSH_SPEED, sACTEX.dwAniType );
		else
			PushPullAct( sACTEX.sID, sACTEX.sID_TAR, sACTEX.fPUSH_PULL, sACTEX.fPUSH_SPEED, sACTEX.dwAniType );
	} //if
	
}

void GLLandMan::DoSkillActExII( const SSKILLACTEX &sACTEX, const SSKILLACTEX &sCAACTEX )
{
	ServerActor* pACTOR   = m_pGLGaeaServer->GetTarget ( this, sACTEX.sID );
	ServerActor* pREACTOR = m_pGLGaeaServer->GetTarget ( this, sACTEX.sID_TAR );

	if ( !pREACTOR )
		return;

	if ( !pREACTOR->IsValidBody() )
		return;

	if ( pREACTOR->GetNowHP()==0 )
		return;

	PGLSKILL	pSkill	= GLSkillMan::GetInstance().GetData( sACTEX.idSKILL );	

	BOOL bSkillHitEnd = TRUE;

	switch( sACTEX.sID.emCrow )
	{
	case CROW_PC :
		{
			GLChar*	pChar = (GLChar*)pACTOR;
			bSkillHitEnd = pChar->IsSkillEnd(sACTEX.idSKILL);
		}
		break;
	case CROW_MOB :
		{
			GLCrow*	pChar = (GLCrow*)pACTOR;
			bSkillHitEnd = pChar->IsSkillEnd(sACTEX.idSKILL);
		}
		break;
	case CROW_SUMMON :
		{
			GLCrow*	pChar = (GLCrow*)pACTOR;
			bSkillHitEnd = pChar->IsSkillEnd(sACTEX.idSKILL);
		}
		break;
	}

	if( pSkill != NULL )		// 상대방에게 지속형 스킬 이팩트를 삽입 시도. ( 스킬 설정에 따라 지속성 여부 다름. )
	{
		DAMAGE_SPEC*	pDamageSpec = pREACTOR->GetDamageSpecPtr();
		if ( CHECK_FLAG_SKILL(pDamageSpec->m_emImmuneActionType, pSkill->m_sBASIC.emACTION_TYPE) ||
			CHECK_FLAG_SKILL(pDamageSpec->m_emImmuneApplyType, pSkill->m_sBASIC.emAPPLY))
			return;

		DWORD		dwSELSLOT(0);

		pACTOR->RECEIVE_SKILLCA( sCAACTEX );

		pREACTOR->RECEIVE_SKILLFACTII( sACTEX.idSKILL, sACTEX.wSKILL_LVL, dwSELSLOT, sACTEX.sID );

		// 지속형 스킬일 경우.
		if( dwSELSLOT != UINT_MAX )
		{
			// 발동된 스킬 값 MSG.
			GLMSG::SNETPC_SKILLHOLD_BRD		NetMsgBRD;
			NetMsgBRD.emCrow    = sACTEX.sID_TAR.emCrow;
			NetMsgBRD.dwID		= sACTEX.sID_TAR.GaeaId;
			NetMsgBRD.skill_id	= sACTEX.idSKILL;
			NetMsgBRD.wLEVEL    = sACTEX.wSKILL_LVL;
			NetMsgBRD.wSELSLOT	= static_cast<WORD>(dwSELSLOT);
			NetMsgBRD.fPARAM    = pREACTOR->GetMaxHpRate();
			NetMsgBRD.sID       = sACTEX.sID;

			switch( sACTEX.sID_TAR.emCrow )
			{
			case CROW_PC  :
				{
					NetMsgBRD.sEffInfo.eType = GLMSG::SNETPC_SKILLHOLD_BRD::E_REV_TARGET;

					GLChar*	pChar = (GLChar*) pREACTOR;

					pChar->GetEffList( dwSELSLOT, NetMsgBRD.sEffInfo.iRecvEAIdx, NetMsgBRD.sEffInfo.iRecvSAIdx );
				}
				break;
			case CROW_MOB :
				{
					NetMsgBRD.sEffInfo.eType = GLMSG::SNETPC_SKILLHOLD_BRD::E_REV_TARGET;

					GLCrow*	pChar = (GLCrow*) pREACTOR;

					pChar->GetEffList( dwSELSLOT, NetMsgBRD.sEffInfo.iRecvEAIdx, NetMsgBRD.sEffInfo.iRecvSAIdx );
				}
				break;
			case CROW_SUMMON :
				{
					NetMsgBRD.sEffInfo.eType = GLMSG::SNETPC_SKILLHOLD_BRD::E_REV_TARGET;

					GLSummonField*	pChar = (GLSummonField*) pREACTOR;

					pChar->GetEffList( dwSELSLOT, NetMsgBRD.sEffInfo.iRecvEAIdx, NetMsgBRD.sEffInfo.iRecvSAIdx );
				}
				break;
			}

			bool bCaApply = false;

			if( sACTEX.sID_TAR.emCrow == CROW_PC )
			{ // '스킬대상'의 주변 클라이언트들에게 메세지 전송.
				GLChar*		pChar	= (GLChar*) pREACTOR;
				if( pSkill && !( pChar->VehicleState() && pSkill->m_sBASIC.emIMPACT_SIDE != EMIMPACT_SIDE_ENEMY) )
				{																					
					m_pGLGaeaServer->SENDTOCLIENT ( pREACTOR->GETCLIENTID (), &NetMsgBRD );
					pREACTOR->SendMsgViewAround(&NetMsgBRD);
					if ( sACTEX.sID != sACTEX.sID_TAR ){
						if ( pSkill->IsBuff() )
						{
							if ( GLHIDESET::CheckHideSet(EMHIDE_TAKE_BUFF) )
							{
								pChar->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
								pChar->RestoreActStateByInvisible();
							}

						}
						else if ( pSkill->IsLimit() )
						{
							if ( GLHIDESET::CheckHideSet(EMHIDE_TAKE_LIMIT) )
							{
								pChar->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
								pChar->RestoreActStateByInvisible();
							}
						}
						else if ( pSkill->IsHarm() )
						{
							if ( GLHIDESET::CheckHideSet(EMHIDE_TAKE_DEBUFF) )
							{
								pChar->RemoveSkillFactBySpecial(EMSPECA_INVISIBLE);	
								pChar->RestoreActStateByInvisible();
							}
						}
					}
				}
			}
			else
			{ // 스킬 대상에게 메세지 전송.
				pREACTOR->SendMsgViewAround(&NetMsgBRD);
			} //if..else
		} //if
	} //if

	if( sACTEX.dwCUREFLAG!=NULL )
		pREACTOR->CURE_STATEBLOW ( sACTEX.dwCUREFLAG );

	// 상태이상 제거 플래그가 있다면
	for ( DWORD i = 0 ; i < SKILL::EMACTION_TYPE_NSIZE; ++i )
	{
		if ( sACTEX.dwRemoveFlag[i] == true && bSkillHitEnd )
			pREACTOR->SKILL_EFFECT_REMOVE ( i );
	}

	bool bStateApply = false;

	SSTATEBLOW sState = sACTEX.sSTATEBLOW;

	switch( sACTEX.sID_TAR.emCrow )
	{
	case CROW_PC :
		{
			GLChar* pTarChar = dynamic_cast<GLChar*>(pREACTOR);

			const SKILL::SSPEC_CA* pCA = pTarChar->GetSkillCA()->GetCaData();					

			if( pCA && pCA->bAllApply )
			{
				sState = SSTATEBLOW();
			}
		}
		break;
	case CROW_MOB :
		{
			GLCrow* pTarChar = dynamic_cast<GLCrow*>(pREACTOR);

			const SKILL::SSPEC_CA* pCA = pTarChar->GetSkillCA()->GetCaData();					

			if( pCA && pCA->bAllApply )
			{
				sState = SSTATEBLOW();
			}
		}
		break;
	}

	//if( sACTEX.sSTATEBLOW.emBLOW!=EMBLOW_NONE && pREACTOR->STATEBLOW ( sACTEX.sSTATEBLOW ))
	if( sState.emBLOW!=EMBLOW_NONE && pREACTOR->STATEBLOW ( sState ))
	{		
		// 상태이상 발생 Msg.
		GLMSG::SNETPC_STATEBLOW_BRD		NetMsgState;
		NetMsgState.emCrow		= sACTEX.sID_TAR.emCrow;
		NetMsgState.dwID		= sACTEX.sID_TAR.GaeaId;
		NetMsgState.emBLOW		= sACTEX.sSTATEBLOW.emBLOW;
		NetMsgState.fAGE		= sACTEX.sSTATEBLOW.fAGE;
		NetMsgState.fSTATE_VAR1 = sACTEX.sSTATEBLOW.fSTATE_VAR1;
		NetMsgState.fSTATE_VAR2 = sACTEX.sSTATEBLOW.fSTATE_VAR2;

		if( pACTOR )
			NetMsgState.wSchool = pACTOR->GetSchool();

		// '스킬대상'의 주변 클라이언트들에게 메세지 전송.
		//
		pREACTOR->SendMsgViewAround(&NetMsgState);

		// 스킬 대상에게 메세지 전송.
		if ( sACTEX.sID_TAR.emCrow == CROW_PC )
			m_pGLGaeaServer->SENDTOCLIENT ( pREACTOR->GETCLIENTID (), &NetMsgState );
	} //if

	// 강한타격, 끌어오기, 밀어내기
	if( sACTEX.fPUSH_PULL != 0.0f )
	{
		//	선도 인증중인지 체크해야 됨
		if( m_bGuidBattleMap && m_pGuidance )
		{
			if( pREACTOR->GetCrow() == CROW_PC && m_pGuidance->m_dwCERTIFY_CHARID == pREACTOR->GetCharID() )
				return;
		} ///if

		if ( sACTEX.bOnward )
			PushPullAct( sACTEX.sID_TAR, sACTEX.sID, sACTEX.fPUSH_PULL, sACTEX.fPUSH_SPEED, sACTEX.dwAniType );
		else
			PushPullAct( sACTEX.sID, sACTEX.sID_TAR, sACTEX.fPUSH_PULL, sACTEX.fPUSH_SPEED, sACTEX.dwAniType );
	} //if

}

/**
 * 끌러오기, 밀어내기 이동가능한지 체크하고 위치를 구한다.
 */
bool GLLandMan::GetPushPullPos( D3DXVECTOR3&		vDestPos,
								ServerActor*		pACTOR,
								ServerActor*		pREACTOR,
								const float			fPUSH_PULL )
{
	if ( fPUSH_PULL == 0.0f )
		return false;

	float			fPushPull	 = fPUSH_PULL;
	D3DXVECTOR3		vREACTOR_POS = pREACTOR->GetPosition();  		
	D3DXVECTOR3		vDir		 = ( vREACTOR_POS - pACTOR->GetPosition() );	//	REACTOR -> ACTOR 방향의 백터.

//	vDir.y = 0.0f;
	D3DXVec3Normalize( &vDir, &vDir );
	boost::shared_ptr<NavigationMesh> spNavi = GetNavi();
	if ( spNavi )
	{
		D3DXVECTOR3		vPosA = vREACTOR_POS;
		D3DXVECTOR3		vPosB;
		//D3DXVECTOR3		vPosB = vREACTOR_POS + vDir * fPushPull;

		BOOL			bCoA(FALSE);
		DWORD			dwCoA(0);
		D3DXVECTOR3		vCoA(0,0,0);

		spNavi->IsCollision ( D3DXVECTOR3(vPosA.x,vPosA.y+5.0f,vPosA.z), D3DXVECTOR3(vPosA.x,vPosA.y-5.0f,vPosA.z), vCoA, dwCoA, bCoA );
		if( bCoA )
		{
			BOOL			bCoB(FALSE);
			DWORD			dwCoB(0);

			//pNavi->IsCollision ( D3DXVECTOR3(vPosB.x,vPosB.y+5.0f,vPosB.z), D3DXVECTOR3(vPosB.x,vPosB.y-5.0f,vPosB.z), vDestPos, dwCoB, bCoB );
			//if( !bCoB )

			float		fNewPushPull = abs(fPushPull);

			float		fdxLength	= 1.0f;
			float		fAbs		= 1.0f;

			if ( fPushPull < 0.0f )
				fAbs = -1.0f;

			while ( 0.0f < fNewPushPull )
			{							
				vPosB = vREACTOR_POS + vDir * ( fNewPushPull * fAbs );

				// SimpleCollision 충돌이 안되면 바닥검사를 하게 한다.
				if ( !IsLayerSimpleCollision( vPosA, vPosB, 5.f ) )
				{
					spNavi->IsCollision ( D3DXVECTOR3(vPosB.x,vPosB.y+5.0f,vPosB.z), D3DXVECTOR3(vPosB.x,vPosB.y-5.0f,vPosB.z), vDestPos, dwCoB, bCoB );	
					if ( bCoB )
					{
						if ( spNavi->LineOfSightTest ( dwCoA, vCoA, dwCoB, vDestPos ) )
							return true;
					}
				}

				if( fNewPushPull > 40.0f )
					fdxLength = 10.0f;
				else if( fNewPushPull > 15.0f )
					fdxLength = 5.0f;
				else if( fNewPushPull > 5.0f )
					fdxLength = 2.0f;
				else
					fdxLength = 1.0f;

				fNewPushPull -= fdxLength;
			} //while
		} //if
	} //if

	return false;
} //GLLandMan::GetPushPullPos

void GLLandMan::PushPullAct( const STARGETID& 	dwActorID,
							 const STARGETID& 	dwReActorID,
							 const float 		fPUSH_PULL,
							 const float 		fPUSH_SPEED,
							 DWORD				dwSubAni )
{
	ServerActor*	pActor		= m_pGLGaeaServer->GetTarget( this, dwActorID );
	if( !pActor )
		return;

	ServerActor*	pReactor	= m_pGLGaeaServer->GetTarget( this, dwReActorID );
	if( !pReactor )
		return;

	D3DXVECTOR3		vPos;
	if( !GetPushPullPos( vPos, pActor, pReactor, fPUSH_PULL ) )
		return;

	pReactor->SendPushPull( vPos, fPUSH_SPEED, dwSubAni, dwActorID != dwReActorID);
	pReactor->SetPosition( vPos );
} //GLLandMan::PushPullAct

void GLLandMan::SwapPosAct( const STARGETID& 	dwActorID,
							const STARGETID& 	dwReActorID,
							const float 		fPUSH_SPEED,
							DWORD				dwSubAni,
							DWORD				dwSwapType )
{
	ServerActor*	pActor		= m_pGLGaeaServer->GetTarget( this, dwActorID );
	if( !pActor )
		return;

	ServerActor*	pReactor	= m_pGLGaeaServer->GetTarget( this, dwReActorID );
	if( !pReactor )
		return;

	D3DXVECTOR3		vActorPos	= pActor->GetPosition(); 
	D3DXVECTOR3		vReactorPos = pReactor->GetPosition();
	D3DXVECTOR3		vDist		= vReactorPos - vActorPos;
	float			fDist		= D3DXVec3Length( &vDist );
	D3DXVECTOR3		vPos;
	if( !GetPushPullPos( vPos, pActor, pReactor, fDist ) )
		return;

	if( dwSwapType == 0 || dwSwapType == 1 )			//actor
	{
		pActor->SendPushPull( vReactorPos, fPUSH_SPEED, dwSubAni, false );
		pActor->SetPosition( vReactorPos );
	} //IF

	if( dwSwapType == 0 || dwSwapType == 2 )			//reactor
	{
		pReactor->SendPushPull( vActorPos, fPUSH_SPEED, dwSubAni );
		pReactor->SetPosition( vActorPos );
	} //IF
} //GLLandMan::SwapPosAct

void GLLandMan::RegSkillAct ( const SSKILLACT &sACT )
{	
	if ( sACT.IsDELAY() )	// 지연처리.
		m_listSkillAct.push_back(sACT);	
	else					// 즉시처리.
		DoSkillAct(sACT);
}

void GLLandMan::RegSkillActEx ( const SSKILLACTEX &sACTEX )
{
	if ( sACTEX.IsDELAY() )
		m_listSkillActEx.push_back(sACTEX);	// 지연처리.	
	else
		DoSkillActEx(sACTEX);				// 즉시처리.
}

void GLLandMan::RegSkillActExII ( const SSKILLACTEX &sACTEX, const SSKILLACTEX &sCAACTEX )
{
	if ( sACTEX.IsDELAY() )
		m_listSkillActEx.push_back(sACTEX);	// 지연처리. 잠시보류
	else
		DoSkillActExII(sACTEX, sCAACTEX);				// 즉시처리.
}

void GLLandMan::UpdateSkillAct ( float fElapsedTime )
{
	LISTSACT_ITER iter		= m_listSkillAct.begin();
	while( iter != m_listSkillAct.end() )
	{
		LISTSACT_ITER iter_cur = iter++;

		SSKILLACT &sACT = (*iter_cur);
		sACT.fDELAY -= fElapsedTime;

		if( sACT.fDELAY <= 0.0f )
		{
			DoSkillAct(sACT);
			m_listSkillAct.erase(iter_cur);
		} //if
	} //while

	LISTSACTEX_ITER iterEx	 = m_listSkillActEx.begin();
	while( iterEx != m_listSkillActEx.end() )
	{
		LISTSACTEX_ITER iter_cur = iterEx++;

		SSKILLACTEX &sACT = (*iter_cur);
		sACT.fDELAY -= fElapsedTime;
		if( sACT.fDELAY <= 0.0f )
		{
			DoSkillActEx(sACT);
			m_listSkillActEx.erase(iter_cur);
		} //if
	} //while
}

BOOL GLLandMan::AddMobSchManLayer ( UINT nLayerIndex, BOOL bOnce )
{
    // Once Layer 를 사용할 것인가?;
    if ( bOnce )
    {
		for ( UINT i=0; i<m_vecRemovedMobSchManOnce.size(); ++i )
		{
			const UINT nTempLayerIndex = m_vecRemovedMobSchManOnce.at( i );
			if ( nTempLayerIndex == nLayerIndex )
			{
				// 삭제 예약 중인 경우에는 사용할 수 없다;
				sc::writeLogInfo( sc::string::format(
					"[ MobSchedule ] [ %1% Once Layer don't delete, because It is scheduling for to delete. ]",
					nLayerIndex ) );

				return TRUE;
			}
		}

		MOB_SCH_MAN_LIST_ITER it = m_mapMobSchManOnce.begin();
		for ( ; it != m_mapMobSchManOnce.end(); ++it )
		{
			SpMobScheduleManServer spMobSch = *it;
			if ( spMobSch->GetLayerIndex() == static_cast< DWORD >( nLayerIndex ) )
			{
				// 이미 Layer 가 존재할 경우 레이어를 생성하지 않는다;
				return TRUE;
			}
		}

		// Once Layer 를 새롭게 생성한다;
        SpMobScheduleManServer spMobSchMan( new GLMobScheduleManServer() );
		if ( NULL == spMobSchMan.get() )
			return FALSE;

		spMobSchMan->SetLandMan( this );
		spMobSchMan->SetLayerIndex( static_cast< DWORD >( nLayerIndex ) );
        
        m_mapMobSchManOnce.push_back( spMobSchMan );

		// Once Layer 에서는 Layer Effect 를 사용하지 않는다;
		// SNET_LEVEL_LAYER_FC
    }  
    else
    {
		for ( UINT i=0; i<m_vecRemovedMobSchManRegen.size(); ++i )
		{
			const UINT nTempLayerIndex = m_vecRemovedMobSchManRegen.at( i );
			if ( nTempLayerIndex == nLayerIndex )
			{
				// 삭제 예약 중인 경우에는 사용할 수 없다;
				sc::writeLogInfo( sc::string::format(
					"[ MobSchedule ] [ %1% Regen Layer don't delete, because It is scheduling for to delete. ]",
					nLayerIndex ) );

				return TRUE;
			}
		}

		MOB_SCH_MAN_MAP_ITER it = m_mapMobSchManRegen.find( nLayerIndex );
		if ( it != m_mapMobSchManRegen.end() )
		{
			SpMobScheduleManServer spMobSch( it->second );

			// Client 에 셋팅된 Layer Effect 를 렌더링 하라고 알려준다;
			GLMSG::SNET_LEVEL_LAYER_FC sNetMsg( GLMSG::SNET_LEVEL_LAYER_FC::LEVEL_LAYER_ACTIVE, nLayerIndex );
			SendMsgPC ( &sNetMsg );

			// 이미 Layer 가 존재할 경우 레이어를 생성하지 않는다;
			return TRUE;
		}

        // Base Layer 를 불러와서 그대로 사용한다;
        SpMobScheduleManServer spData = GetMobSchMan( nLayerIndex );
        if( spData == NULL )
            return FALSE;

        spData->SetLandMan( this );
		spData->SetLayerIndex( static_cast< DWORD >( nLayerIndex ) );
        m_mapMobSchManRegen.insert( std::make_pair( nLayerIndex, spData )  );

		// Client 에 셋팅된 Layer Effect 를 렌더링 하라고 알려준다;
		GLMSG::SNET_LEVEL_LAYER_FC sNetMsg( GLMSG::SNET_LEVEL_LAYER_FC::LEVEL_LAYER_ACTIVE, nLayerIndex );
		SendMsgPC ( &sNetMsg );
    }

    return TRUE;
}

/*void GLLandMan::CopyBaseLayer ( UINT nOnceLayerIndex, UINT nBaseLayerIndex )
{
	// 0 번 Base Layer 는 기본 레이어로 활용하기 때문에 복사를 허가하지 않는다;
	// 0 번을 굳이 사용하지 않고도 얼마든지 구현이 가능하기 때문에 웬만하면;
	// 이대로 두자;
	if ( 0 == nBaseLayerIndex )
		return;

	MOB_SCH_MAN_LIST_ITER it = m_mapMobSchManOnce.begin();
	for ( ; it != m_mapMobSchManOnce.end(); ++it )
	{
		SpMobScheduleManServer spOnceMobSchMan = *it;
		if ( spOnceMobSchMan->GetLayerIndex() == static_cast< DWORD >( nOnceLayerIndex ) )
		{
			SpMobScheduleManServer spBaseMobSchMan = GetMobSchMan( nBaseLayerIndex );
			if ( NULL == spBaseMobSchMan )
			{
				sc::writeLogError( sc::string::format( "Invalid Mob Layer ID : %1%", nBaseLayerIndex ) );
				return;
			}

			const MOBSCHEDULELIST* pGLMobSchList = spBaseMobSchMan->GetMobSchList();
			sc::SGLNODE<GLMobSchedule*>* pCurMobSchNode = pGLMobSchList->m_pHead;
			while ( pCurMobSchNode )
			{
				const GLMobSchedule* pCurBaseMobSch = pCurMobSchNode->Data;

				GLMobSchedule* pNewMobSch = m_pGLGaeaServer->NEW_MOBSCHEDULE();
				*pNewMobSch = *pCurBaseMobSch;

				// Mob Schduler 에서 관리하도록 한다;
				spOnceMobSchMan->AddMobSch( pNewMobSch );

				pCurMobSchNode = pCurMobSchNode->pNext;
			}
		}
	}
}*/

void GLLandMan::SetMobSchManRegenLayer ( UINT nLayerIndex )
{
    DelAllMobSchManRegenLayer( true );    
    BOOL bOk = AddMobSchManLayer( nLayerIndex, false );

    if( bOk == FALSE )
        sc::writeLogError( "GLLandMan::SetMobSchManRegenLayer() - Check LayerIndex!!" );
}

void GLLandMan::DelMobSchManRegenLayer ( UINT nLayerIndex, const bool bDelete )
{
    MOB_SCH_MAN_MAP_ITER it = m_mapMobSchManRegen.find( nLayerIndex );
    if ( it != m_mapMobSchManRegen.end() )
    {
		if ( bDelete )
		{
			SpMobScheduleManServer spMobSch( it->second );
			CleanUpMobSchMan( spMobSch.get() );

			m_mapMobSchManRegen.erase( it );

			// Client 에 Layer Effect 를 지우라고 알려준다;
			GLMSG::SNET_LEVEL_LAYER_FC sNetMsg(
				GLMSG::SNET_LEVEL_LAYER_FC::LEVEL_LAYER_ONE_DEACTIVE,
				nLayerIndex );
			SendMsgPC( &sNetMsg );
		}
		else
		{
			m_vecRemovedMobSchManRegen.push_back( nLayerIndex );
		}
    }
}

void GLLandMan::DelMobSchManOnceLayer ( UINT nLayerIndex, const bool bDelete )
{
	MOB_SCH_MAN_LIST_ITER it = m_mapMobSchManOnce.begin();
	for ( ; it != m_mapMobSchManOnce.end(); ++it )
	{
		SpMobScheduleManServer spMobSch = *it;
		if ( spMobSch->GetLayerIndex() == static_cast< DWORD >( nLayerIndex ) )
		{
			if ( bDelete )
			{
				CleanUpMobSchMan( spMobSch.get() );
				m_mapMobSchManOnce.erase( it );
			}
			else
			{
				m_vecRemovedMobSchManOnce.push_back( nLayerIndex );
			}

			break;
		}
	}
}

void GLLandMan::DelAllMobSchManRegenLayer ( const bool bDelete )
{
    MOB_SCH_MAN_MAP_ITER it = m_mapMobSchManRegen.begin();
    for ( ; it != m_mapMobSchManRegen.end(); ++it )
    {
		if ( bDelete )
		{
			SpMobScheduleManServer spMobSch( it->second );
			CleanUpMobSchMan( spMobSch.get() );
		}
		else
		{
			m_vecRemovedMobSchManRegen.push_back( it->first );
		}
    }

	if ( bDelete )
	{
		m_mapMobSchManRegen.clear();
	}

	// Client 에 Layer Effect 를 지우라고 알려준다;
	GLMSG::SNET_LEVEL_LAYER_FC sNetMsg(
		GLMSG::SNET_LEVEL_LAYER_FC::LEVEL_LAYER_ONE_DEACTIVE,
		0 );
	SendMsgPC( &sNetMsg );
}


void GLLandMan::DelAllMobSchManOnceLayer ( const bool bDelete )
{
	MOB_SCH_MAN_LIST_ITER it = m_mapMobSchManOnce.begin();
	for ( ; it != m_mapMobSchManOnce.end(); ++it )
    {
		SpMobScheduleManServer spMobSch = *it;
		if ( bDelete )
		{
			CleanUpMobSchMan( spMobSch.get() );
		}
		else
		{
			m_vecRemovedMobSchManOnce.push_back( spMobSch->GetLayerIndex() );
		}	
	}

	if ( bDelete )
	{
		m_vecRemovedMobSchManOnce.clear();
	}
}


void GLLandMan::DelAllMobSchLayer ( const bool bDelete )
{
    DelAllMobSchManRegenLayer( bDelete );
    DelAllMobSchManOnceLayer( bDelete );
}

void GLLandMan::CleanUpMobSchMan ( GLMobScheduleManServer* pMobSchMan )
{
	if (pMobSchMan == 0)
	{
		return;
	}

	const MOBSCHEDULELIST * GLMobSchList = pMobSchMan->GetMobSchList();
	sc::SGLNODE<GLMobSchedule*>* pCur = GLMobSchList->m_pHead;

	while ( pCur )
	{
		if ( pCur->Data->m_dwGlobID != UINT_MAX )
		{
			const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pCur->Data->m_CrowID );

			if ( !pCrowData )
				continue;

			if ( pCrowData->m_emCrow == CROW_MATERIAL ) 
			{
				m_listDelGlobIDExMat.remove(pCur->Data->m_dwGlobID);
				DropOutMaterial( pCur->Data->m_dwGlobID );
			}
			else
			{
				m_listDelGlobIDEx.remove(pCur->Data->m_dwGlobID);
				DropOutCrow( pCur->Data->m_dwGlobID );
			}
		}

		pCur->Data->Reset();
		pCur = pCur->pNext;
	}

	MAPMOBGROUB mapMobGroub = pMobSchMan->GetMobGroubMap ();
	MAPMOBGROUB_ITER pos = mapMobGroub.begin ();

	for ( ; pos != mapMobGroub.end(); ++pos )
	{
		GLMOBGROUB &sGROUB = (*pos).second;

		if ( sGROUB.m_vecSchedule.empty() )
			continue;

        if( sGROUB.m_dwRendGenIndex==UINT_MAX )
            continue;

		GLMobSchedule* pMobSch = sGROUB.m_vecSchedule[sGROUB.m_dwRendGenIndex];

		const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( pMobSch->m_CrowID );

		if ( !pCrowData )
			continue;

		if ( pCrowData->m_emCrow == CROW_MATERIAL ) 
		{
			DropOutMaterial ( pMobSch->m_dwGlobID );
		}
		else
		{
			DropOutCrow ( pMobSch->m_dwGlobID );
		}

		pMobSch->Reset();
	}
}

HRESULT GLLandMan::UpdateRemoveMobSchManLayer ( void )
{
	for ( UINT i=0; i<m_vecRemovedMobSchManRegen.size(); ++i )
	{
		UINT nLayerIndex = m_vecRemovedMobSchManRegen.at( i );

		MOB_SCH_MAN_MAP_ITER it = m_mapMobSchManRegen.find( nLayerIndex );
		if ( it != m_mapMobSchManRegen.end() )
		{
			SpMobScheduleManServer spMobSch( it->second );
			CleanUpMobSchMan( spMobSch.get() );
			m_mapMobSchManRegen.erase( it );
		}

		// Client 에 셋팅된 Layer Effect 를 지우라고 알려준다.
		GLMSG::SNET_LEVEL_LAYER_FC sNetMsg( GLMSG::SNET_LEVEL_LAYER_FC::LEVEL_LAYER_ONE_DEACTIVE, nLayerIndex );
		SendMsgPC ( &sNetMsg );
	}
	m_vecRemovedMobSchManRegen.clear();

	for ( UINT i=0; i<m_vecRemovedMobSchManOnce.size(); ++i )
	{
		UINT nLayerIndex = m_vecRemovedMobSchManOnce.at( i );

		MOB_SCH_MAN_LIST_ITER it = m_mapMobSchManOnce.begin();
		for ( ; it != m_mapMobSchManOnce.end(); )
		{
			SpMobScheduleManServer spMobSch = *it;
			if ( spMobSch->GetLayerIndex() == static_cast< DWORD >( nLayerIndex ) )
			{
				CleanUpMobSchMan( spMobSch.get() );
				it = m_mapMobSchManOnce.erase( it );
			}
			else
			{
				++it;
			}
		}
	}
	m_vecRemovedMobSchManOnce.clear();

	return S_OK;
}

// void GLLandMan::ClearCrowAndSchedule()
// {
//     CleanUpMobSchMan(m_MobSchMan.get());
//     m_MobSchMan->CleanUp();
//     m_MobSchManContainer.clear();
// 
// }

// void GLLandMan::AddMobSchMan( UINT index )
// {
//     if (GetSizeMobSchMan() > index)
//     {
//         GLLevelFileServer::AddMobSchMan(index);
//     }
// }

// *****************************************************
// Desc: 지정한 클럽원을 제외한 모든 케릭터 강제 이동
// *****************************************************
BOOL GLLandMan::DoGateOutPCAll ( DWORD dwExceptClubID, DWORD dwGateID )
{
	std::vector<DWORD> vecGATEOUTPC;
	vecGATEOUTPC.reserve ( m_GlobPCList.m_dwAmount );

	std::tr1::shared_ptr<GLClubField> pCLUB = m_pGLGaeaServer->GetClub(dwExceptClubID);
	
	if (!pCLUB)
        return FALSE;

	// 결정전 맵에 존재하는 동맹의 맴버를 제외한 모든 케릭터 강제 이동.
	// 동맹이 있을때
	if ( pCLUB->IsAlliance() )
	{
		std::tr1::shared_ptr<GLClubField> pCHIEFCLUB = m_pGLGaeaServer->GetClub(pCLUB->m_dwAlliance);
		if (!pCHIEFCLUB)
            return FALSE;

		GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			bool bIsExceptChar(false);

			CLUB_ALLIANCE_ITER pos = pCHIEFCLUB->m_setAlliance.begin();
			CLUB_ALLIANCE_ITER end = pCHIEFCLUB->m_setAlliance.end();
			
			for ( ; pos!=end; ++pos )
			{
				const GLCLUBALLIANCE &sExceptClub = pos->second;

				std::tr1::shared_ptr<GLClubField> pAlliClub = m_pGLGaeaServer->GetClub(sExceptClub.m_dwID);
				
				if ( pAlliClub )
				{
					if ( pCharNode->Data->m_ClubDbNum == sExceptClub.m_dwID )
					{
						bIsExceptChar = true;
					}
				}
			}

			if ( !bIsExceptChar )
			{
				vecGATEOUTPC.push_back ( pCharNode->Data->GetGaeaID() );
			}
		}
	}
	else
	{
		GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
		
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			if ( pCharNode->Data->m_ClubDbNum == dwExceptClubID )
				continue;

			vecGATEOUTPC.push_back ( pCharNode->Data->GetGaeaID() );
		}
	}

	GLMSG::SNETREQ_GATEOUT sNetMsg;	
	sNetMsg.dwGateID = dwGateID;

	std::vector<DWORD>::size_type nSIZE = vecGATEOUTPC.size();
	
	for ( std::vector<DWORD>::size_type i=0; i<nSIZE; ++i )
	{
		GLChar* pCHAR = m_pGLGaeaServer->GetChar ( vecGATEOUTPC[i] );
		
		if ( !pCHAR )
			continue;

		sNetMsg.dwChaNum = pCHAR->CharDbNum();
		sNetMsg.dwGaeaID = pCHAR->GetGaeaID();
		m_pGLGaeaServer->SENDTOAGENT(&sNetMsg);
	}

	return TRUE;
}

BOOL GLLandMan::DoGateOutPCAll ( DWORD dwGateID )
{
	GLMSG::SNETREQ_GATEOUT sNetMsg;
	sNetMsg.dwGateID = dwGateID;

	GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
	
	for ( ; pCharNode; pCharNode = pCharNode->pNext )
	{
		sNetMsg.dwChaNum = pCharNode->Data->CharDbNum();
		sNetMsg.dwGaeaID = pCharNode->Data->GetGaeaID();
		m_pGLGaeaServer->SENDTOAGENT(&sNetMsg);
	}

	return TRUE;
}

BOOL GLLandMan::DoMapEffectPC ( DWORD dwClubID, SMAP_EFFECT& sMapEffect )
{
	std::vector<DWORD> vecEffectPC;
	vecEffectPC.reserve ( m_GlobPCList.m_dwAmount );
	
	std::tr1::shared_ptr<GLClubField> pCLUB = m_pGLGaeaServer->GetClub(dwClubID);
	
	if ( !pCLUB )
		return FALSE;

	// 결정전 맵에 존재하는 동맹의 맴버를 제외한 모든 케릭터 강제 이동.
	// 동맹이 있을때
	if ( pCLUB->IsAlliance() )
	{
		std::tr1::shared_ptr<GLClubField> pCHIEFCLUB = m_pGLGaeaServer->GetClub(pCLUB->m_dwAlliance);
		if ( !pCHIEFCLUB )
			return FALSE;

		CLUB_ALLIANCE_ITER pos = pCHIEFCLUB->m_setAlliance.begin();
		CLUB_ALLIANCE_ITER end = pCHIEFCLUB->m_setAlliance.end();
		
		for ( ; pos!=end; ++pos )
		{
			const GLCLUBALLIANCE &sExceptClub = pos->second;
			std::tr1::shared_ptr<GLClubField> pAlliClub = m_pGLGaeaServer->GetClub(sExceptClub.m_dwID);
			
			if (!pAlliClub)
                continue;

			GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
			
			for ( ; pCharNode; pCharNode = pCharNode->pNext )
			{
				if ( pCharNode->Data->m_ClubDbNum == sExceptClub.m_dwID )
				{
					vecEffectPC.push_back ( pCharNode->Data->GetGaeaID() );
				}
			}			
		}
	}
	else
	{
		GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
		for ( ; pCharNode; pCharNode = pCharNode->pNext )
		{
			if ( pCharNode->Data->m_ClubDbNum == dwClubID )
				vecEffectPC.push_back ( pCharNode->Data->GetGaeaID() );
		}
	}

	GLMSG::SNETPC_MAPEFFECT_UPDATE_BRD sNetMsg;
	sNetMsg.bADD = true;
	sNetMsg.sMapEffect = sMapEffect;

	std::vector<DWORD>::size_type nSIZE = vecEffectPC.size();
	for ( std::vector<DWORD>::size_type i=0; i<nSIZE; ++i )
	{
		GLChar* pCHAR = m_pGLGaeaServer->GetChar ( vecEffectPC[i] );
		
		if ( !pCHAR )
			continue;

		pCHAR->ADDMAPEFF( sMapEffect );

		sNetMsg.dwGaeaID = pCHAR->GetGaeaID();
		m_pGLGaeaServer->SENDTOCLIENT ( pCHAR->ClientSlot(), &sNetMsg );
		pCHAR->SendMsgViewAround(&sNetMsg);
	}

	return TRUE;
}


void GLLandMan::ResetCrow4CTF()
{
    for (CROW_DROP_MAP_ITER iter=m_CrowDrop.begin(); iter!=m_CrowDrop.end(); ++iter)
    {
        GLCrow* pCrow = iter->second;
        if (pCrow)
            pCrow->Reset4CTF();
    }
}

const bool GLLandMan::AddGlobPcDelList(sc::SGLNODE<GLChar*>* pNode)
{    
    if (pNode)
    {
		GLChar* pChar = pNode->Data;
		if ( pChar )
		{
			if ( m_pCountRecorder )
			{
				m_pCountRecorder->del_CurPlayerCount();
				m_pCountRecorder->del_CurPartyPlayerCount( pChar->GetPartyID().GetPartyID() );
				m_pCountRecorder->del_CurClubPlayerCount( pChar->GetClubDbNum() );
				m_pCountRecorder->del_CurCountryPlayerCount( pChar->GetCountryInfo().dwID );
				m_pCountRecorder->del_CurSchoolPlayerCount( pChar->GetSchool() );

				Faction::ManagerField* const pFactionManager( m_pLandManager->GetFactionManager() );
				if ( pFactionManager )
				{
					const Faction::FactionID* const pFactionID =
						pFactionManager->getFactionID( pChar->GetActorID() );
					if ( pFactionID )
					{
						m_pCountRecorder->del_CurFactionPlayerCount( *pFactionID );
					}
				}
			}
		}
        
		// 기존코드는 DELNODE 를 호출해서 바로 지웠다.
		// 하지만 병렬처리 및 기타 상황에서 바로 지울때 문제가 발생하여 
		// 지울 항목들을 등록해 놓고 이후에 한꺼번에 지우도록 수정한다.
		return m_GlobPCList.DELNODE(pNode);

        //sc::CriticalSectionOwnerLock OwnerLock(m_GlobPCListLock);
        //m_GlobPCListDelList.insert(pNode);
    }

	return true;
}

void GLLandMan::FrameMoveClearDelList()
{
//     sc::CriticalSectionOwnerLock OwnerLock(m_GlobPCListLock);
//     for (std::set<sc::SGLNODE<GLChar*>* >::iterator iter=m_GlobPCListDelList.begin(); iter!=m_GlobPCListDelList.end(); ++iter)        
//         m_GlobPCList.DELNODE(*iter);
// 
//     m_GlobPCListDelList.clear();
}

void GLLandMan::StartTriggerFinger(int nStage, int nLevel, const std::string& strGuid)
{
    if (nLevel < 0)
    {
        return;
    }

    if (m_spTriggerFinger == 0)
    {
        m_spTriggerFinger = TriggerSystem::CreateServerFinger(this, m_pGLGaeaServer, nStage, nLevel, strGuid);
    }

    if (m_spTriggerFinger)
    {
        m_spTriggerFinger->Start();
    }
}

void GLLandMan::RemoveTriggerFinger()
{
	if (m_spTriggerFinger)
	{
		m_spTriggerFinger.reset();
	}
}

void GLLandMan::SendTriggerMessage(TriggerSystem::SMessage* msg)
{
	if (m_spTriggerFinger && msg)
	{
		m_spTriggerFinger->SendMessage(msg);
	}
}

void GLLandMan::PostTriggerMessage(TriggerSystem::SpMessage msg)
{
	if (m_spTriggerFinger && msg)
	{
		m_spTriggerFinger->PostMessage(msg);
	}	
}

void GLLandMan::SetCrowMultipleHP(float fMultiple)
{
    m_fCrowMultipleHP = fMultiple;
}

const float GLLandMan::GetCrowMultipleHP() const
{
    return m_fCrowMultipleHP;
}

void GLLandMan::SetCrowMultipleAttack(float fMultiple)
{
    m_fCrowMultipleAttack = fMultiple;
}

const float GLLandMan::GetCrowMultipleAttack() const
{
    return m_fCrowMultipleAttack;
}

void GLLandMan::SetCrowMultipleDefense(float fMultiple)
{
    m_fCrowMultipleDefense = fMultiple;
}

const float GLLandMan::GetCrowMultipleDefense() const
{
    return m_fCrowMultipleDefense;
}

void GLLandMan::SetCrowMultipleExp(float fMultiple)
{
    m_fCrowMultipleExp = fMultiple;
}

const float GLLandMan::GetCrowMultipleExp() const
{
    return m_fCrowMultipleExp;
}

void GLLandMan::SetCrowMultipleDrop(float fMultiple)
{
	m_fCrowMultipleDrop = fMultiple;
}

const float GLLandMan::GetCrowMultipleDrop() const
{
	return m_fCrowMultipleDrop;
}

GLChar* GLLandMan::GetChar( DWORD dwID )
{
	if (m_pGLGaeaServer)
	{
		return m_pGLGaeaServer->GetChar(dwID);
	}

	return 0;
}

GLChar* GLLandMan::GetCharByDbNum(DWORD dwCharDbNum)
{
    if (m_pGLGaeaServer)
    {
        return m_pGLGaeaServer->GetCharByDbNum(dwCharDbNum);
    }

    return 0;
}

void GLLandMan::CollectPC(CHARDBNUM_LIST& list)
{
    GLCHARNODE* pCharNode = m_GlobPCList.m_pHead;
    for ( ; pCharNode; pCharNode = pCharNode->pNext )
    {
        list.insert(pCharNode->Data->GetCharID());
    }  
}

/************************************************************************/
/* Trigger System 에서 맵을 나가게 하는 부분이다;                     */
/************************************************************************/
void GLLandMan::Teleport ( const DWORD dwCharDbNum )
{
    GLChar* pChar = GetCharByDbNum( dwCharDbNum );
    if ( pChar )
    {
		/// 이동정보를 세팅한다;
		MoveSystem::SMOVE_INFO sMoveInfo;
		sMoveInfo.dwChaDBNum = dwCharDbNum;
		sMoveInfo.sMapID = pChar->m_sSaveMapID;
		sMoveInfo.vPosition = pChar->m_vSavePos;

		/// 이동한다;
		MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo );

        /*// 서브 모델(소환수, 펫, 탈것)들 처리
        m_pGLGaeaServer->DropOutAllSummon(pChar);
        m_pGLGaeaServer->DropOutPET(pChar->m_dwPetGUID, true, true);
        m_pGLGaeaServer->SaveVehicle(pChar->ClientSlot(), pChar->GetGaeaID(), true);

        // 에이전트를 통해 이동 처리
        GLMSG::SNETPC_REQ_RECALL_AG NetMsgAg;
        NetMsgAg.dwChaNum = pChar->CharDbNum();
        NetMsgAg.mapIDTo = pChar->m_sSaveMapID;
        NetMsgAg.vPosition = pChar->m_vSavePos;
        m_pGLGaeaServer->SENDTOAGENT(&NetMsgAg);*/
    }
}

Faction::ManagerField* const GLLandMan::GetFactionManager(void) const
{
    return m_pLandManager->GetFactionManager();
}

InstanceSystem::InstanceField* const GLLandMan::GetScriptInstance(void) const
{
	return m_pLandManager->GetScriptInstance();
}

void GLLandMan::SetCommissionRate( float fCommissionRate )
{
    if ( GetGaeaMapID() == SNATIVEID(false) )
        return;

	const SMAPNODE* pMapNode = m_pGLGaeaServer->FindMapNode( GetBaseMapID() );
	if( NULL == pMapNode )
		return;

	if( true == pMapNode->IsCommission() )
    {
		m_fCommissionRate = fCommissionRate;

        /*sc::writeLogInfo(
            sc::string::format(
            "CommissionLog: Land ( (%1%/%2%) / (%3%/%4%) ) Commision Rate Changed!  Current Total BuyCommissionMoney %5%, SellCommissionMoney %6% ",
            GetBaseMapID().Mid(),
            GetBaseMapID().Sid(),
            GetGaeaMapID().Mid(),
            GetGaeaMapID().Sid(),
            m_llBuyCommissionMoney,
            m_llSellCommissionMoney
            ));*/

        m_llBuyCommissionMoney = 0;
        m_llSellCommissionMoney = 0;

        m_fCommissionLogTimer = 0.0f;
    }
	else
		m_fCommissionRate = 0.0f;
}

VEC_SK_TAR* GLLandMan::Detect_CROW( DWORD emCrow, const D3DXVECTOR3 &vDetectPos, const int nRange )
{
	if ( !m_vecDetectID.empty() )
		m_vecDetectID.erase ( m_vecDetectID.begin(), m_vecDetectID.end() );

	//	캐릭터.
	//if (emCrow & CROW_EX_PC)
		Detect_CROW_PC ( vDetectPos, nRange );

	// 몹
	//if (emCrow & CROW_EX_MOB)
		Detect_CROW_MOB ( vDetectPos, nRange );

	//// Material
	//if (emCrow & CROW_EX_MATERIAL)
	//	m_pLandMClient->GetCollisionLineToAabbDropMaterial(m_vecDetectID, vFromPt, vTargetPt, false, NULL, FALSE);

	////	소환수
	//if (emCrow & CROW_EX_SUMMON)
		Detect_CROW_SUMMON ( vDetectPos, nRange );

	////	Item.
	//if (emCrow & CROW_EX_ITEM)
	//	m_pLandMClient->GetCollisionDropItem(vFromPt, vTargetPt, m_vecDetectID);

	////	Money.
	//if (emCrow & CROW_EX_MONEY)
	//	m_pLandMClient->GetCollisionDropMoney(vFromPt, vTargetPt, m_vecDetectID);

	if (m_vecDetectID.empty())
		return NULL;

	std::sort(m_vecDetectID.begin(), m_vecDetectID.end(), STAR_ORDER());
	return &m_vecDetectID;
}

BOOL GLLandMan::IsObstacle( D3DXVECTOR3 vThisPos, D3DXVECTOR3 vPreviousPos  )
{
    // 장애물판정 로직은 부하가 크고 현 로직이 비효율적으로 되어 있어서 지금 현재 딱히 사용되는 곳이 없어서 사용하지 않도록 함;
    // 만약 사용하고 싶다면 새로 만드는것을 추천;
    // 장애물 액터를 따로 관리하는 자료구조를 도입하는것이 속도면에서 빠를것으로 보임;
	BOOL bMOVE = FALSE;
	D3DXVECTOR3 vDist;
	D3DXVECTOR3 vDistPrevious;
	BOOL bInObstaclePrevious = FALSE;
	BOOL bInObstacle = FALSE;

	BOOST_FOREACH(SUMMON_DROP_MAP_VALUE  &iter, m_SummonDrop)
	{
		GLSummonField* pSummonField = iter.second;

		if (!pSummonField)
			continue;

		if (!pSummonField->IsValidBody())
			continue;

		PCROWDATA pSummonCrowData = pSummonField->m_pSummonCrowData;
		
		if (pSummonCrowData && !pSummonCrowData->IsBarrier())
			continue;

		vDist = pSummonField->GetPositionActor() - vThisPos;
		vDistPrevious = pSummonField->GetPositionActor() - vPreviousPos;

		bInObstaclePrevious = IsInObstacle( pSummonCrowData->m_fBarrierRadius, 
											pSummonCrowData->m_fBarrierWidth, 
											D3DXVec3Length(&vDistPrevious) );

		bInObstacle = IsInObstacle( pSummonCrowData->m_fBarrierRadius, 
									pSummonCrowData->m_fBarrierWidth, 
									D3DXVec3Length(&vDist) );

		BOOL bIsInNew, bIsInOld = FALSE;
		bIsInOld = IsIn( pSummonCrowData->m_fBarrierRadius, D3DXVec3Length(&vDistPrevious) );
		bIsInNew = IsIn( pSummonCrowData->m_fBarrierRadius, D3DXVec3Length(&vDist) );

		if (  bIsInOld != bIsInNew )
			bMOVE= TRUE;
		//if (  bInObstacle )
		//	bMOVE= TRUE;
	}

	BOOST_FOREACH(CROW_DROP_MAP_VALUE  &iter, m_CrowDrop)
	{
		GLCrow* pCrow = iter.second;

		if ( !pCrow )
			continue;

		if (!pCrow->IsValidBody())
			continue;

		PCROWDATA pCrowData = pCrow->m_pCrowData;

		if (pCrowData && !pCrow->m_pCrowData->IsBarrier())
			continue;

		vDist = pCrow->GetPositionActor() - vThisPos;
		vDistPrevious = pCrow->GetPositionActor() - vPreviousPos;

		bInObstaclePrevious = IsInObstacle( pCrowData->m_fBarrierRadius, 
			pCrowData->m_fBarrierWidth, 
			D3DXVec3Length(&vDistPrevious) );

		bInObstacle = IsInObstacle( pCrowData->m_fBarrierRadius, 
			pCrowData->m_fBarrierWidth, 
			D3DXVec3Length(&vDist) );

		BOOL bIsInNew, bIsInOld = FALSE;
		bIsInOld = IsIn( pCrowData->m_fBarrierRadius, D3DXVec3Length(&vDistPrevious) );
		bIsInNew = IsIn( pCrowData->m_fBarrierRadius, D3DXVec3Length(&vDist) );

		if (  bIsInOld != bIsInNew  )
			bMOVE= TRUE;
		//if (  bInObstacle )
		//	bMOVE= TRUE;

	}
	return bMOVE;
}

BOOL GLLandMan::IsInObstacle( float fRadius, float fWidth, float fLength )
{
	if (fLength > fRadius - fWidth && fLength < fRadius)
	{
		return TRUE;
	}
	return FALSE;
}

//BOOL GLLandMan::ISCollisionSphereToLine( D3DXVECTOR3 vSphereCenter, float fRadius, D3DXVECTOR3 vLineBegin, D3DXVECTOR3 vLineDirecton )
//{
//	float fHsq;
//	D3DXVECTOR3 vcT = vSphereCenter - vLineBegin;
//	D3DXVECTOR3 vcL = vLineDirecton;
//
//	D3DXVECTOR3 vcH;
//	D3DXVec3Cross( &vcH, &vcT, &vcL);
//	fHsq = D3DXVec3Length(&vcH);
//
//	if ( fHsq < fRadius )
//		return FALSE;
//
//	return TRUE;
//}

BOOL GLLandMan::IsIn( float fRadius, float fLength )
{
	return fRadius > fLength;
}

GLLandMan* const GLLandManager::getLand(const DWORD _nSubMapID) const
{
	GASSERT(_nSubMapID < MAX_LAND_SID);

	if ( _nSubMapID >= MAX_LAND_SID )
		return 0;

	return this->pLand[_nSubMapID];
}

GLLandMan* const GLLandManager::getLand(const SNATIVEID& _mapID) const
{
    GASSERT(_mapID.wMainID < MAX_LAND_MID);
    GASSERT(_mapID.wSubID < MAX_LAND_SID);

    for ( unsigned int i = 0; i < this->nStage; i++ )
    {
        if ( pLand[i]->GetBaseMapID() == _mapID )
        {
            return pLand[i];
        }
    }

    return 0;
}

void GLLandMan::AddDropOutCrowCount(const SNATIVEID& sNID)
{
	if ( m_pCountRecorder )
		m_pCountRecorder->add_DropOutCrowCount(sNID.dwID);
}

void GLLandMan::AddDropItemCount(const SNATIVEID& sNID)
{
	if ( m_pCountRecorder )
		m_pCountRecorder->add_DropItemCount(sNID.dwID);
}

void GLLandMan::AddDropMoneyCount(const LONGLONG& lMoney)
{
	if ( m_pCountRecorder )
		m_pCountRecorder->add_DropMoneyCount(lMoney);
}

void GLLandMan::AddEnterPlayerCount()
{
	if ( m_pCountRecorder )
		m_pCountRecorder->add_EnterPlayerCount();
}


const bool GLLandMan::AddCurPlayerCount ( void )
{
	if ( NULL == m_pCountRecorder )
		return false;

	m_pCountRecorder->add_CurPlayerCount();

	return true;
}

const bool GLLandMan::AddCurPartyPlayerCount ( const DWORD dwID )
{
	if ( NULL == m_pCountRecorder )
		return false;

	m_pCountRecorder->add_CurPartyPlayerCount( dwID );

	return true;
}

const bool GLLandMan::AddCurClubPlayerCount ( const DWORD dwID )
{
	if ( NULL == m_pCountRecorder )
		return false;

	m_pCountRecorder->add_CurClubPlayerCount( dwID );

	return true;
}

const bool GLLandMan::AddCurCountryPlayerCount ( const DWORD dwID )
{
	if ( NULL == m_pCountRecorder )
		return false;

	m_pCountRecorder->add_CurCountryPlayerCount( dwID );

	return true;
}

const bool GLLandMan::AddCurSchoolPlayerCount ( const DWORD dwID )
{
	if ( NULL == m_pCountRecorder )
		return false;

	m_pCountRecorder->add_CurSchoolPlayerCount( dwID );

	return true;
}


const bool GLLandMan::DelCurPlayerCount ( void )
{
	if ( NULL == m_pCountRecorder )
		return false;

	m_pCountRecorder->del_CurPlayerCount();

	return true;
}

const bool GLLandMan::DelCurPartyPlayerCount ( const DWORD dwID )
{
	if ( NULL == m_pCountRecorder )
		return false;

	m_pCountRecorder->del_CurPartyPlayerCount( dwID );

	return true;
}

const bool GLLandMan::DelCurClubPlayerCount ( const DWORD dwID )
{
	if ( NULL == m_pCountRecorder )
		return false;

	m_pCountRecorder->del_CurClubPlayerCount( dwID );

	return true;
}

const bool GLLandMan::DelCurCountryPlayerCount ( const DWORD dwID )
{
	if ( NULL == m_pCountRecorder )
		return false;

	m_pCountRecorder->del_CurCountryPlayerCount( dwID );

	return true;
}

const bool GLLandMan::DelCurSchoolPlayerCount ( const DWORD dwID )
{
	if ( NULL == m_pCountRecorder )
		return false;

	m_pCountRecorder->del_CurSchoolPlayerCount( dwID );

	return true;
}


const bool GLLandMan::AddCurFactionPlayerCount ( const Faction::FactionID& sFactionID )
{
	if ( NULL == m_pCountRecorder )
		return false;

	m_pCountRecorder->add_CurFactionPlayerCount( sFactionID );

	return true;
}

const bool GLLandMan::DelCurFactionPlayerCount ( const Faction::FactionID& sFactionID )
{
	if ( NULL == m_pCountRecorder )
		return false;

	m_pCountRecorder->del_CurFactionPlayerCount( sFactionID );

	return true;
}


const DWORD GLLandMan::GetCurPlayerCount ( void ) const
{
	if ( NULL == m_pCountRecorder )
		return 0;

	return m_pCountRecorder->GetCurPlayerCount();
}

const bool GLLandMan::GetCurPartyPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const
{
	if ( NULL == m_pCountRecorder )
		return false;

	_refPlayerCounter = m_pCountRecorder->GetCurPartyPlayerCount();

	return true;
}

const bool GLLandMan::GetCurClubPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const
{
	if ( NULL == m_pCountRecorder )
		return false;

	_refPlayerCounter = m_pCountRecorder->GetCurClubPlayerCount();

	return true;
}

const bool GLLandMan::GetCurCountryPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const
{
	if ( NULL == m_pCountRecorder )
		return false;

	_refPlayerCounter = m_pCountRecorder->GetCurCountryPlayerCount();

	return true;
}

const bool GLLandMan::GetCurSchoolPlayerCount ( GLCountRecorder::MAP_COUNTER& _refPlayerCounter ) const
{
	if ( NULL == m_pCountRecorder )
		return false;

	_refPlayerCounter = m_pCountRecorder->GetCurSchoolPlayerCount();

	return true;
}

const bool GLLandMan::GetCurFactionPlayerCount ( GLCountRecorder::MAP_FACTION_COUNTER& _refPlayerCounter ) const
{
	if ( NULL == m_pCountRecorder )
		return false;

	_refPlayerCounter = m_pCountRecorder->GetCurFactionPlayerCount();

	return true;
}

void GLLandMan::TriggerStop()
{
	if( m_spTriggerFinger ) m_spTriggerFinger->Stop();
}

// Layer 가 활성화 되어 있다면 Layer Effect Rendering 을 하라고 Client 에게 보내준다.
void GLLandMan::SendLayerActiveInfo()
{
	MOB_SCH_MAN_MAP_CITER citer = m_mapMobSchManRegen.begin();
	for ( ; citer!=m_mapMobSchManRegen.end(); ++citer )
	{
		if ( IsExistLayerObjectForServer( (*citer).first ) )
		{
			// Client 에 셋팅된 Layer Effect 를 렌더링 하라고 알려준다.
			GLMSG::SNET_LEVEL_LAYER_FC sNetMsg( GLMSG::SNET_LEVEL_LAYER_FC::LEVEL_LAYER_ACTIVE, (*citer).first );
			SendMsgPC ( &sNetMsg );
		}
	}
}