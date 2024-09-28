#include "../pch.h"

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Loading/NSLoadingDirect.h"

#include "../../EngineLib/DxTools/Deffered/NSDeffered.h"
#include "../../EngineLib/DxTools/DxSurfaceTex.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../EngineLib/DxTools/DxShadowMap.h"
#include "../../EngineLib/DxTools/DxViewPort.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Single/DxEffSingleMan.h"
#include "../../EngineLib/DxEffect/Single/DxEffGameHelp.h"
#include "../../EngineLib/DxSound/DxSoundLib.h"
#include "../../EngineLib/DxMeshs/DxSimpleMeshMan.h"
#include "../../EngineLib/G-Logic/NSGameLogicInfo.h"
#include "../../EngineLib/Common/profile.h"
#include "../../EngineLib/Water/NSWaterMain.h"
#include "../../EngineLib/GlobalParam.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/Land/GLLandMark.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/TriggerSystem/TriggerFinger.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../Country/GLVictoriousCountryManClient.h"
#include "../Crow/GLCrowRenList.h"
#include "../LandEvent/GLLandEventNodeClient.h"
#include "../GLGaeaClient.h"
#include "./GLLandManClient.h"
#include "../TriggerSystem/ClientTriggerSystem.h"

#include "../GfxInterfaceBase.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

GLLandManClient::GLLandManClient(GLGaeaClient* pGaeaClient) 
	: GLLevelFileClient(pGaeaClient)
    , m_fAge(0.0f)
	, m_pd3dDevice(NULL)
	
	//, m_bPeaceZone(FALSE) // GLLevelFile 에 이미 정의되어 있음.
	
	, m_bClubBattle(false)
	, m_bClubBattleHall(false)
	, m_fCommission(0.0f)
	, m_bClubDeathMatch(false)
	, m_bClubDeathMatchHall(false)
	, m_bClubBattleInProgress( false )
	
	, m_dwClubMapID( 0 )
	, m_dwGuidClub(0)
	, m_dwGuidClubMarkVer(0)
{	
	memset( m_szGuidClubName, 0, sizeof(char) * (CHAR_SZNAME) );
	m_dwCOUNT = 0;
    m_pLandEvent = new GLLandEventClient(m_pGaeaClient);

    if ( GLUseFeatures::GetInstance().IsUsingNewInstance() )
        m_FactionMananger = new Faction::ManagerClient(pGaeaClient);
    else
        m_FactionMananger = NULL;

	m_spLandMan = boost::shared_ptr<DxLandMan>(new DxLandMan);
}

GLLandManClient::~GLLandManClient(void)
{
	CleanUp ();
    SAFE_DELETE(m_pLandEvent);
	SAFE_DELETE(m_FactionMananger);
}

std::tr1::shared_ptr<GLCharClient> GLLandManClient::GetChar(DWORD dwID)
{
    CHARMAPID_ITER iter = m_PCArray.find(dwID);
    if (iter != m_PCArray.end())
        return iter->second;
    else
        return std::tr1::shared_ptr<GLCharClient> ((GLCharClient*) 0);
}

std::tr1::shared_ptr<GLCrowClient> GLLandManClient::GetCrow(DWORD dwID)
{
    DROP_CROW_MAP_ITER iter = m_DropCrow.find(dwID);
    if (iter != m_DropCrow.end())
        return iter->second;
    else
        return std::tr1::shared_ptr<GLCrowClient> ((GLCrowClient*) 0);
}

void GLLandManClient::SetDropCrow(DWORD GlobId, std::tr1::shared_ptr<GLCrowClient> spCrow)
{
    DROP_CROW_MAP_ITER iter = m_DropCrow.find(GlobId);
    if (iter != m_DropCrow.end())
        m_DropCrow.erase(iter);
    if (spCrow)
        m_DropCrow.insert(DROP_CROW_MAP_VALUE(GlobId, spCrow));
}

void GLLandManClient::DeleteDropCrow(DWORD GlobId)
{
    DROP_CROW_MAP_ITER iter = m_DropCrow.find(GlobId);
    if (iter != m_DropCrow.end())
        m_DropCrow.erase(iter);
}

std::tr1::shared_ptr<GLMaterialClient> GLLandManClient::GetMaterial(DWORD dwID)
{
    DROP_MATERIAL_MAP_ITER iter = m_DropMaterial.find(dwID);
    if (iter != m_DropMaterial.end())
        return iter->second;
    else
        return std::tr1::shared_ptr<GLMaterialClient> ((GLMaterialClient*) 0);
}

void GLLandManClient::SetDropMaterial(DWORD GlobId, std::tr1::shared_ptr<GLMaterialClient> spDropMaterial)
{
    DROP_MATERIAL_MAP_ITER iter = m_DropMaterial.find(GlobId);
    if (iter != m_DropMaterial.end())
        m_DropMaterial.erase(iter);
    if (spDropMaterial)
        m_DropMaterial.insert(DROP_MATERIAL_MAP_VALUE(GlobId, spDropMaterial));
}

void GLLandManClient::DeleteDropMaterial(DWORD GlobId)
{
    DROP_MATERIAL_MAP_ITER iter = m_DropMaterial.find(GlobId);
    if (iter != m_DropMaterial.end())
        m_DropMaterial.erase(iter);
}

std::tr1::shared_ptr<CItemClientDrop> GLLandManClient::GetItem(DWORD dwGlobID)
{
    DROP_ITEM_ITER iter = m_DropItem.find(dwGlobID);
    if (iter != m_DropItem.end())
        return iter->second;
    else
        return std::tr1::shared_ptr<CItemClientDrop> ((CItemClientDrop*) 0);
}

void GLLandManClient::SetDropItem(DWORD GlobId, std::tr1::shared_ptr<CItemClientDrop> spDropItem)
{    
    DROP_ITEM_ITER iter = m_DropItem.find(GlobId);
    if (iter != m_DropItem.end())
        m_DropItem.erase(iter);

    if (spDropItem)
        m_DropItem.insert(DROP_ITEM_VALUE(GlobId, spDropItem));
}

void GLLandManClient::DeleteDropItem(DWORD GlobId)
{
    DROP_ITEM_ITER iter = m_DropItem.find(GlobId);
    if (iter != m_DropItem.end())
        m_DropItem.erase(iter);
}

std::tr1::shared_ptr<CMoneyClientDrop> GLLandManClient::GetMoney(DWORD dwGlobID)
{
    DROP_MONEY_MAP_ITER iter = m_DropMoney.find(dwGlobID);
    if (iter != m_DropMoney.end())
        return iter->second;
    else
        return std::tr1::shared_ptr<CMoneyClientDrop> ((CMoneyClientDrop*) 0);
}

void GLLandManClient::SetDropMoney(DWORD GlobId, std::tr1::shared_ptr<CMoneyClientDrop> spDropMoney)
{
    DROP_MONEY_MAP_ITER iter = m_DropMoney.find(GlobId);
    if (iter != m_DropMoney.end())
        m_DropMoney.erase(iter);

    if (spDropMoney)
        m_DropMoney.insert(DROP_MONEY_MAP_VALUE(GlobId, spDropMoney));
}

void GLLandManClient::DeleteDropMoney(DWORD GlobId)
{
    DROP_MONEY_MAP_ITER iter = m_DropMoney.find(GlobId);
    if (iter != m_DropMoney.end())
        m_DropMoney.erase(iter);
}

std::tr1::shared_ptr<GLAnyPet> GLLandManClient::GetPet(DWORD dwGUID)
{
    PETMAPID_ITER iter = m_PETArray.find(dwGUID);
    if (iter != m_PETArray.end())
        return iter->second;
    else
        return std::tr1::shared_ptr<GLAnyPet> ((GLAnyPet*) 0);
}

std::tr1::shared_ptr<GLSummonClient> GLLandManClient::GetSummon(DWORD dwGUID)
{
    DROP_SUMMON_MAP_ITER iter = m_DropSummon.find(dwGUID);
    if (iter != m_DropSummon.end())
        return iter->second;
    else
        return std::tr1::shared_ptr<GLSummonClient> ((GLSummonClient*) 0);
}

void GLLandManClient::SetDropSummon(DWORD GlobId, std::tr1::shared_ptr<GLSummonClient> spSummon)
{
    DROP_SUMMON_MAP_ITER iter = m_DropSummon.find(GlobId);
    if (iter != m_DropSummon.end())
        m_DropSummon.erase(iter);
    if (spSummon)
        m_DropSummon.insert(DROP_SUMMON_MAP_VALUE(GlobId, spSummon));
}

void GLLandManClient::DeleteDropSummon(DWORD GlobId)
{
    DROP_SUMMON_MAP_ITER iter = m_DropSummon.find(GlobId);
    if (iter != m_DropSummon.end())
        m_DropSummon.erase(iter);
}

BOOL GLLandManClient::LoadFile ( const char *szFile )
{
	GASSERT(szFile);
	GASSERT(m_pd3dDevice);
	HRESULT	hr = S_OK;

	//	Note : Map 데이터 리셋.
	CleanUp ();

	BOOL bOk(FALSE);

	bOk = GLLevelFileClient::LoadFile ( szFile, m_pd3dDevice );
	if (!bOk)
        return FALSE;

	bOk = m_spLandMan->LoadFile ( m_sLevelHead.m_strWldFile.c_str(), m_pd3dDevice, TRUE );
	if (!bOk)
        return FALSE;

	if ( !m_sLevelHead.m_strNaviBypassFile.empty() )
	{
		m_spLandMan->LoadNaviBypassFile( m_sLevelHead.m_strNaviBypassFile.c_str() );
	}

	// GameCamera 를 셋팅한다.
	// 혹시 xxx.set 파일에서 ZoomOut 을 변경 했을 수 있으므로 작업이 필요함.
	DxViewPort::GetInstance().SetGameCamera();

	if( m_spLandMan->GetMapID() == SNATIVEID(false) )
	{
		m_spLandMan->SetMapID( GetBaseMapID() );
	}

	//	Note : Map 대이터 초기화.
	m_vMax = D3DXVECTOR3(+10000,0,+10000);
	m_vMin = D3DXVECTOR3(-10000,0,-10000);

	boost::shared_ptr<NavigationMesh> spNaviMesh = m_spLandMan->GetNaviMesh ();
	if ( spNaviMesh )
		spNaviMesh->GetAABB ( m_vMax, m_vMin );

	hr = m_LandTree.SetState ( m_vMax.x, m_vMax.z, m_vMin.x, m_vMin.z, m_sLevelHead.m_eDivision );
	if (FAILED(hr))
        return hr;

	hr = m_LandTree.MakeTree ();
	if (FAILED(hr))
        return hr;

	return TRUE;
}

HRESULT GLLandManClient::Create( const char* szSetFileName, 
                                LPDIRECT3DDEVICEQ pd3dDevice, 
                                D3DXVECTOR3 &vBasicPos, 
                                const MapID& mapID, 
                                BOOL bPeaceZone )
{
	GASSERT(szSetFileName);
	GASSERT(pd3dDevice);
	HRESULT	hr = S_OK;

	m_pd3dDevice = pd3dDevice;

	//	이전 대이터 삭제.
	CleanUp ();

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	m_strFileName = szSetFileName;

	//	Note : 파일에서 읽은 MAP ID 가 아니라 인수로 넘어온 ID를 사용.
	//
    if ( mapID.isValid() == true )
	{
		SetMapID(mapID);
		SetPeaceZone(bPeaceZone);
		SetPKZone(true);
	}

	//	Note : 멥을 읽어옴.
	//
	hr = LoadFile(szSetFileName);
	if (FAILED(hr))
        return hr;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	SMAPNODE* pMapNode = m_pGaeaClient->FindMapNode(GetBaseMapID());
	if (pMapNode)
	{
		m_bUIMapInfo = pMapNode->IsUIMapInfo();
		m_bUIMapSelect = pMapNode->IsUIMapSelect();
		m_bPKMap = pMapNode->IsPkMap();
		m_bPartyMap = pMapNode->IsPartyMap();		
		m_bExpeditionMap = pMapNode->IsExpeditionMap();
		m_emInstanceType = GLLevelFile::EMINSTANCE_TYPE( pMapNode->GetInstanceType() );
		m_spLandMan->SetBgmFile(pMapNode->m_BgmName.c_str());
		SetClubBattleZone(pMapNode->IsClubBattleZone());
		SetLunchBoxForbid(pMapNode->IsLunchBoxForbid());
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//	Note : 멥의 초기 위치 주변을 미리 읽어둔다.
	////
	//D3DXVECTOR3 vMax = vBasicPos + D3DXVECTOR3(1000,60,1000);
	//D3DXVECTOR3 vMin = vBasicPos + D3DXVECTOR3(-1000,-60,-1000);
	m_spLandMan->LoadBasicPos ( m_pd3dDevice, vBasicPos );

	m_bPeaceZone = bPeaceZone;

	if(m_MobSchMan.get() != NULL)
	{
		//	Note : Mob/Npc 의 형상 파일들을 미리 읽어 둡니다.
		//
		GLMobScheduleMan::MOBDESC& setMobDesc = m_MobSchMan->GetMobDesc();
		GLMobScheduleMan::MOBDESC_ITER iter = setMobDesc.begin();
		GLMobScheduleMan::MOBDESC_ITER iter_end = setMobDesc.end();
		SNATIVEID sMobID;
		for ( ; iter!=iter_end; ++iter )
		{
			sMobID.dwID = (*iter);

			//	Note : 몹의 설정 정보를 가져옴.
			const PCROWDATA pCrowData = GLCrowDataMan::GetInstance().GetCrowData ( sMobID );
			if( !pCrowData )
			{
				sc::writeLogError(
					sc::string::format(
					"GLLandManClient::Create()-GLCrowDataMan::GetCrowData() [%1%/%2%] 가져오기 실패.",
					sMobID.wMainID, sMobID.wSubID));
			}

			if ( pCrowData )
			{
				//---------------------------------------------------//
				NSLoadingDirect::FrameMoveRender( FALSE );
				//---------------------------------------------------//

				DxSkinCharData* pCharData = DxSkinCharDataContainer::GetInstance().LoadData ( pCrowData->GetSkinObjFile(), m_pd3dDevice );
				if( !pCharData )
				{
					if( strlen( pCrowData->GetSkinObjFile() ) )
					{
						sc::writeLogError(
							sc::string::format(
							"GLLandManClient::Create()-DxSkinCharDataContainer::LoadData() [%1%] 가져오기 실패.", pCrowData->GetSkinObjFile()));
					}
				}
			}
		}
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	hr = InitDeviceObjects ( m_pd3dDevice );
	if (FAILED(hr))
        return hr;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	hr = RestoreDeviceObjects();
	if (FAILED(hr))
        return hr;

	if ( m_FactionMananger )
	{
		m_FactionMananger->SetLandName(
			m_pGaeaClient->GetMapNameStr( GetBaseMapID() ) );
	}

	return S_OK;
}

CEVENT_NODE_CLIENT* GLLandManClient::GetLandEventNode ( DWORD dwID )
{
    if (dwID>=MAXLANDEVENT)
        return NULL;
    else
        return m_pLandEvent->GetEvent(dwID);
}

void GLLandManClient::CleanUp ()
{
/*
	DWORD i=0;

	for (i=0; i<MAXITEM; ++i)
        DropOutItem(i);
	for (i=0; i<MAXMONEY; ++i)
        DropOutMoney(i);
	for (i=0; i<MAXCROW; ++i)
        DropOutCrow(i);
	for (i=0; i<MAXCROW; ++i)
        DropOutMaterial(i);
	for (i=0; i<MAXSUMMON; ++i)
        DropOutSummon(i);
	
	if (!m_PCArray.empty())
	{
		CHARMAPID_ITER iter_end = (--m_PCArray.end());
		DWORD dwEndID = (*iter_end).first;
		for (i=0; i<dwEndID+1; ++i)
            DropOutChar(i);
	}

	if (!m_PETArray.empty())
	{
		PETMAPID_ITER iter_end = (--m_PETArray.end());
		DWORD dwEndID = (*iter_end).first;
		for (i=0; i<dwEndID+1; ++i)
            DropOutPet(i);
	}

	m_pLandEvent->CleanUp();
*/
	InvalidateDeviceObjects();
	DeleteDeviceObjects();

	m_spLandMan->CleanUp();
}

HRESULT GLLandManClient::ActiveMap ()
{
	return m_spLandMan->ActiveMap( m_spLandMan );
}

void GLLandManClient::SETITEM_PICKDELAY ( DWORD dwGlobID )
{
    std::tr1::shared_ptr<CItemClientDrop> spItem = GetItem(dwGlobID);
	if (spItem)
        spItem->fNextMsgDelay = GLCONST_CHAR::fFIELD_NEXTPICK;
}

void GLLandManClient::SETMONEY_PICKDELAY ( DWORD dwGlobID )
{
    std::tr1::shared_ptr<CMoneyClientDrop> spMoney = GetMoney(dwGlobID);
	if (spMoney)
        spMoney->fNextMsgDelay = GLCONST_CHAR::fFIELD_NEXTPICK;
}

BOOL GLLandManClient::ISITEM_PICKDELAY ( DWORD dwGlobID )
{
	//PITEMCLIENTDROP pItem = GetItem ( dwGlobID );
	//if ( !pItem || pItem->fNextMsgDelay > 0.0f )	return TRUE;
	return FALSE;
}

BOOL GLLandManClient::ISMONEY_PICKDELAY ( DWORD dwGlobID )
{
	//PMONEYCLIENTDROP pMoney = GetMoney ( dwGlobID );
	//if ( !pMoney || pMoney->fNextMsgDelay > 0.0f )	return TRUE;
	return FALSE;
}

HRESULT GLLandManClient::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_pd3dDevice = pd3dDevice;
	m_spLandMan->InitDeviceObjects ( m_pd3dDevice );

	GLLevelFile::InitDeviceObjects ( m_pd3dDevice );

	return S_OK;
}

HRESULT GLLandManClient::RestoreDeviceObjects()
{
	m_spLandMan->RestoreDeviceObjects ( m_pd3dDevice );
	GLLevelFile::RestoreDeviceObjects ( m_pd3dDevice );
	
	for (CHARMAPID_ITER iter=m_PCArray.begin(); iter!=m_PCArray.end(); ++iter)
	{
        std::tr1::shared_ptr<GLCharClient> pChar = iter->second;
		if (pChar)
		    pChar->RestoreDeviceObjects(m_pd3dDevice);
	}

    for (DROP_CROW_MAP_ITER iter=m_DropCrow.begin(); iter!=m_DropCrow.end(); ++iter)
	{
        std::tr1::shared_ptr<GLCrowClient> spCrow = iter->second;
		if (spCrow)
		    spCrow->RestoreDeviceObjects(m_pd3dDevice);
	}
	
    for (DROP_MATERIAL_MAP_ITER iter=m_DropMaterial.begin(); iter!=m_DropMaterial.end(); ++iter)
    {    
        std::tr1::shared_ptr<GLMaterialClient> spMaterial = iter->second;
        if (spMaterial)
            spMaterial->RestoreDeviceObjects(m_pd3dDevice);
    }
	
	for (PETMAPID_ITER iter=m_PETArray.begin(); iter!=m_PETArray.end(); ++iter)
	{
		std::tr1::shared_ptr<GLAnyPet> pAnypet = iter->second;
		if (pAnypet)
		    pAnypet->RestoreDeviceObjects(m_pd3dDevice);
	}
	
	for (DROP_SUMMON_MAP_ITER iter=m_DropSummon.begin(); iter!=m_DropSummon.end(); ++iter)
	{
        std::tr1::shared_ptr<GLSummonClient> spSummon = iter->second;
		if (spSummon)
		    spSummon->RestoreDeviceObjects(m_pd3dDevice);
	}

	// 승리국가 세팅;
	Country::GLVictoriousCountryManClient* pVCountryMan =
		Country::GLVictoriousCountryManClient::Instance();
	if ( pVCountryMan )
	{
		const TSTRING strFlagName =
			pVCountryMan->GetVCountryFileNameFirst(
			Country::CAPTURE_THE_ISLANDS );

		if ( false == strFlagName.empty() )
			SetWorldBattleFlag( strFlagName.c_str() );
	}

	return S_OK;
}

HRESULT GLLandManClient::InvalidateDeviceObjects()
{
	m_spLandMan->InvalidateDeviceObjects ();
	GLLevelFile::InvalidateDeviceObjects ();

    for (CHARMAPID_ITER iter=m_PCArray.begin(); iter!=m_PCArray.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCharClient> pChar = iter->second;
		if (pChar)
		    pChar->InvalidateDeviceObjects();
	}

    for (DROP_CROW_MAP_ITER iter=m_DropCrow.begin(); iter!=m_DropCrow.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCrowClient> spCrow = iter->second;
		if (spCrow)
		    spCrow->InvalidateDeviceObjects();
	}
    
    for (DROP_MATERIAL_MAP_ITER iter=m_DropMaterial.begin(); iter!=m_DropMaterial.end(); ++iter)
    {    
        std::tr1::shared_ptr<GLMaterialClient> spMaterial = iter->second;
        if (spMaterial)
            spMaterial->InvalidateDeviceObjects();
    }

	for (PETMAPID_ITER iter=m_PETArray.begin(); iter!=m_PETArray.end(); ++iter)
	{
		std::tr1::shared_ptr<GLAnyPet> pAnypet = iter->second;
		if (pAnypet)
		    pAnypet->InvalidateDeviceObjects();
	}

    for (DROP_SUMMON_MAP_ITER iter=m_DropSummon.begin(); iter!=m_DropSummon.end(); ++iter)
    {
        std::tr1::shared_ptr<GLSummonClient> spSummon = iter->second;
		if (spSummon)
		    spSummon->InvalidateDeviceObjects();
	}

	return S_OK;
}

HRESULT GLLandManClient::DeleteDeviceObjects()
{
	ClearDropObj ();
	GLLevelFile::DeleteDeviceObjects ();

	m_spLandMan->DeleteDeviceObjects ();

	return S_OK;
}


HRESULT GLLandManClient::FrameMoveItem( float fElapsedTime )
{	
    for (DROP_ITEM_ITER iter=m_DropItem.begin(); iter!=m_DropItem.end(); ++iter)
    {
        std::tr1::shared_ptr<CItemClientDrop> spItemDrop = iter->second;
        
        // 임시 소유자 유효성 검사
        if (spItemDrop->bCanGet)
            continue;

        spItemDrop->fAge += fElapsedTime;

        if (GLCONST_CHAR::wMAXITEM_HOLD < spItemDrop->fAge)
        {
            spItemDrop->bCanGet = TRUE;
            STARGETID sTARID(CROW_ITEM, spItemDrop->dwGlobID, spItemDrop->vPos);
            DxEffGameHelp::GetInstance().UpdateGameHelpEffect(EMMINE_EFFECT, spItemDrop->matWld, sTARID);
        }
    }
 
    for (DROP_MONEY_MAP_ITER iter=m_DropMoney.begin(); iter!=m_DropMoney.end(); ++iter)
    {
        std::tr1::shared_ptr<CMoneyClientDrop> spMoneyDrop = iter->second;
        if (!spMoneyDrop)
            continue;
        //	Note : 임시 소유자 유효성 검사.
        //
        if (spMoneyDrop->bCanGet)
            continue;

        spMoneyDrop->fAge += fElapsedTime;

        if (GLCONST_CHAR::wMAXITEM_HOLD < spMoneyDrop->fAge)
        {
            spMoneyDrop->bCanGet = TRUE;
            STARGETID sTARID(CROW_MONEY, spMoneyDrop->dwGlobID, spMoneyDrop->vPos);
            DxEffGameHelp::GetInstance().UpdateGameHelpEffect(EMMINE_EFFECT, spMoneyDrop->matWld, sTARID);
        }
    }

	return S_OK;
}

void GLLandManClient::AllCharClientUpdateSuit()
{
    for (CHARMAPID_ITER iter=m_PCArray.begin(); iter!=m_PCArray.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCharClient> pChar = iter->second;
		if (pChar)
		    pChar->UpdateSuit();
	}
}

HRESULT GLLandManClient::FrameMove(float fTime, float fElapsedTime, const CLIPVOLUME &cv, BOOL bNotRendering)
{
	m_fAge += fElapsedTime;

	PROFILE_BEGIN("m_spTriggerFinger::FrameMove");
	if (m_spTriggerFinger)
	{
		m_spTriggerFinger->FrameMove(fTime, fElapsedTime);
	}
	PROFILE_END("m_spTriggerFinger::FrameMove");

	FrameMoveItem( fElapsedTime );

	PROFILE_BEGIN("DxLandMan::FrameMove");
	m_spLandMan->FrameMove ( fTime, fElapsedTime, cv, bNotRendering );
	PROFILE_END("DxLandMan::FrameMove");

	GLLevelFile::FrameMove ( fTime, fElapsedTime );

	PROFILE_BEGIN("m_GlobLandEvent::FrameMove");
	// Note : 랜드 이벤트
	//
	m_pLandEvent->FrameMove(fElapsedTime);
	PROFILE_END("m_GlobLandEvent::FrameMove");

	PROFILE_BEGIN("m_GlobPCList::FrameMove");
    for (CHARMAPID_ITER iter=m_PCArray.begin(); iter!=m_PCArray.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCharClient> pChar = iter->second;
		if (pChar)
		    pChar->FrameMove( GetLandManSp(), fTime, fElapsedTime );
	}
	PROFILE_END("m_GlobPCList::FrameMove");

	// PET
	// 여기서 다른 팻들의 FrameMove 처리
	PROFILE_BEGIN("m_GlobAnyPetList::FrameMove");
	for (PETMAPID_ITER iter=m_PETArray.begin(); iter!=m_PETArray.end(); ++iter)
	{
		std::tr1::shared_ptr<GLAnyPet> pAnypet = iter->second;
		if (pAnypet)
		    pAnypet->FrameMove( GetLandManSp(), fTime, fElapsedTime );
	}
	PROFILE_END("m_GlobAnyPetList::FrameMove");

	// SUMMON
	// 여기서 다른 팻들의 FrameMove 처리
	PROFILE_BEGIN("m_GlobSummonList::FrameMove");
    for (DROP_SUMMON_MAP_ITER iter=m_DropSummon.begin(); iter!=m_DropSummon.end(); ++iter)
    {
        std::tr1::shared_ptr<GLSummonClient> spSummon = iter->second;		
		if (spSummon)
		    spSummon->FrameMove( GetLandManSp(), fTime, fElapsedTime );
	}
	PROFILE_END("m_GlobSummonList::FrameMove");

	PROFILE_BEGIN("m_GlobCROWList::FrameMove");
    for (DROP_CROW_MAP_ITER iter=m_DropCrow.begin(); iter!=m_DropCrow.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCrowClient> spCrow = iter->second;
		if (spCrow)
		    spCrow->FrameMove( GetLandManSp(), fTime, fElapsedTime );
	}
	PROFILE_END("m_GlobCROWList::FrameMove");

	PROFILE_BEGIN("m_GlobMaterialList::FrameMove");

    for (DROP_MATERIAL_MAP_ITER iter=m_DropMaterial.begin(); iter!=m_DropMaterial.end(); ++iter)
    {    
        std::tr1::shared_ptr<GLMaterialClient> spMaterial = iter->second;
        if (spMaterial)
            spMaterial->FrameMove( GetLandManSp(), fTime, fElapsedTime );
    }
	PROFILE_END("m_GlobMaterialList::FrameMove");

	for (DROP_ITEM_ITER iter=m_DropItem.begin(); iter!=m_DropItem.end(); ++iter)
	{
        std::tr1::shared_ptr<CItemClientDrop> spItemDrop = iter->second;
		spItemDrop->FrameMove( GetLandManSp(), fElapsedTime );
		//if (spItemDrop->fNextMsgDelay > 0.0f)
  //          spItemDrop->fNextMsgDelay -= fElapsedTime;
	}

	for (DROP_MONEY_MAP_ITER iter=m_DropMoney.begin(); iter!=m_DropMoney.end(); ++iter)
	{
        std::tr1::shared_ptr<CMoneyClientDrop> spMoneyDrop = iter->second;
		if ( spMoneyDrop )
		{
			spMoneyDrop->FrameMove( GetLandManSp(), fElapsedTime );
		}
		//if (spMoneyDrop && spMoneyDrop->fNextMsgDelay > 0.0f)
  //          spMoneyDrop->fNextMsgDelay -= fElapsedTime;
	}
	return S_OK;
}

HRESULT GLLandManClient::Render ( CLIPVOLUME &CV, LPDIRECT3DSURFACEQ pColorBuffer )
{
	if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && GetUsedMaterialSystem() )
	{
		//////////////////////////////////////////////////////////////////////////
		//					RenderDefferedOpaqueForPSF
		D3DXMATRIX matWorld;
		D3DXMatrixIdentity( &matWorld );

		//LPDIRECT3DSURFACEQ	pColorBuffer(NULL);
		//if ( RENDERPARAM::g_bEnable_HDR )
		//{
		//	pColorBuffer = DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR;
		//}
		//else
		//{
		//	pColorBuffer = DxSurfaceTex::GetInstance().m_pColorBuffer;
		//}

		if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_DF )
		{
			NSDeffered::RenderDefferedForSM30DF
			(
				m_pd3dDevice,
				DxFogMan::GetInstance().GetFogColor(),
				m_spLandMan.get(),
				DxSurfaceTex::GetInstance().GetWidth(),
				DxSurfaceTex::GetInstance().GetHeight(),
				pColorBuffer,
				DxSurfaceTex::GetInstance().m_pColorSuf_1st,
				DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
				DxSurfaceTex::GetInstance().m_pColorSuf_4th,
				DxSurfaceTex::GetInstance().m_pColorTex_1st,
				DxSurfaceTex::GetInstance().m_pColorTex_2nd, 
				DxSurfaceTex::GetInstance().m_pColorTex_4th, 
				FALSE,
				0.f,
				matWorld,
				DxViewPort::GetInstance().GetMatView(),
				DxViewPort::GetInstance().GetMatProj(),
				DxViewPort::GetInstance().GetClipVolume(),
				FALSE,
				TRUE,
				TRUE,
				TRUE
			);
		}
		else
		{
			NSDeffered::RenderDefferedForSM30NM
			(
				m_pd3dDevice,
				DxFogMan::GetInstance().GetFogColor(),
				m_spLandMan.get(),
				DxSurfaceTex::GetInstance().GetWidth(),
				DxSurfaceTex::GetInstance().GetHeight(),
				pColorBuffer,
				DxSurfaceTex::GetInstance().m_pColorSuf_1st,
				DxSurfaceTex::GetInstance().m_pColorSuf_4th,
				DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
				DxSurfaceTex::GetInstance().m_pColorSuf_3rd,	// Color는 unsign64 를 쓰겠다.
				DxSurfaceTex::GetInstance().m_pColorSuf_LBuffer,
				DxSurfaceTex::GetInstance().m_pSufFourCC_INTZ,
				DxSurfaceTex::GetInstance().m_pColorTex_1st,
				DxSurfaceTex::GetInstance().m_pColorTex_4th,
				DxSurfaceTex::GetInstance().m_pColorTex_2nd,
				DxSurfaceTex::GetInstance().m_pColorTex_3rd, 	// Color는 unsign64 를 쓰겠다.
				DxSurfaceTex::GetInstance().m_pColorTex_LBuffer,
				DxSurfaceTex::GetInstance().m_pTexFourCC_INTZ, 
				matWorld,
				DxViewPort::GetInstance().GetMatView(),
				DxViewPort::GetInstance().GetMatProj(),
				DxViewPort::GetInstance().GetClipVolume(),
				FALSE
			);
		}

		// 지형 물 렌더링
		NSWaterMain::Render( m_pd3dDevice );
	}
	else
	{
		m_spLandMan->Render ( m_pd3dDevice, CV );
	}

	GLLevelFile::Render ( NULL, CV );

	return S_OK;
}

HRESULT GLLandManClient::Render_2Pass ( CLIPVOLUME &CV )
{
	//m_spLandMan->Render_2Pass ( m_pd3dDevice, CV, TRUE );

	GLLevelFile::Render ( NULL, CV );

	return S_OK;
}

void GLLandManClient::RegistName( const CLIPVOLUME &CV )
{
	++m_dwCOUNT;

	D3DXVECTOR3 vCOLL(0.f,0.f,0.f);

	GLCharacter* const pMyCharacter = m_pGaeaClient->GetCharacter();
	const DWORD dwMyGaeaID = pMyCharacter->GetGaeaID();
	D3DXVECTOR3 vMyPosition(pMyCharacter->GetPosition());
	D3DXVECTOR3 vViewPosition(DxViewPort::GetInstance().GetFromPt());
	
	GfxInterfaceBase::GetInstance()->NameDisplaySetCount(m_dwCOUNT);

	//	Note : 이름 목록에 자기 자신 추가.
	{
		CROWREN sDISP_NAME;
		sDISP_NAME.SetData(m_pGaeaClient, pMyCharacter);

		GfxInterfaceBase::GetInstance()->AddNameDisplay( sDISP_NAME );

		//!! SF_TODO
		//if (pMyCharacter->m_bItemShopOpen)
		//	m_pInterface->ItemShopSetRender(dwMyGaeaID);
	}

	// 이름목록에 자신의 팻이름 추가
	GLPetClient* pMYPET = m_pGaeaClient->GetPetClient ();
	if (pMYPET->IsVALID())
	{
		if ( pMYPET->IsVisibleDisplayName() )
		{
			CROWREN sDISP_NAME;
			sDISP_NAME.SetData(m_pGaeaClient, pMYPET);

			GfxInterfaceBase::GetInstance()->AddNameDisplay( sDISP_NAME, pMyCharacter->GetName() );
		}

	}

	for (CHARMAPID_ITER iter=m_PCArray.begin(); iter!=m_PCArray.end(); ++iter)
	{
		std::tr1::shared_ptr<GLCharClient> pChar = iter->second;
		if ( pChar == NULL )
			continue; // 유효하지 않은 케릭터면 실패;

		if ( pChar->IsVisibleDetect() == FALSE )
			continue; // 은신 중이고 감지하지 못하면 실패;
		// 오브젝트에 의한 가려짐도 IsVisibleDetect()함수에서 처리 되고 있음;

		if ( COLLISION::IsCollisionVolume(CV,pChar->m_vMax,pChar->m_vMin) == FALSE )
			continue; // 컬링에 의해 보이지 않는 상태이면 패스;		

		if ( (GLCONST_CHAR::bBATTLEROYAL == FALSE) || (pChar->IsDie() == false) )	// 배틀로얄 중이 아니거나, 케릭터가 살아있다면 출력;	
		{
			if ( pChar->IsVisibleDisplayName() )
			{
				CROWREN sDISP_NAME;
				sDISP_NAME.SetData(m_pGaeaClient, pChar.get());

				GfxInterfaceBase::GetInstance()->AddNameDisplay( sDISP_NAME );

				//!! SF_TODO
				//if ( pChar->m_bItemShopOpen )
				//	m_pInterface->ItemShopSetRender(pChar->GetGaeaID());
			}
		}
	}

	// SUMMON
	// 소환수들의 Render 처리
	//!! SF_TODO
	//m_pInterface->SummonSetCount(m_dwCOUNT);

	for (DROP_SUMMON_MAP_ITER iter=m_DropSummon.begin(); iter!=m_DropSummon.end(); ++iter)
	{
		std::tr1::shared_ptr<GLSummonClient> spSummon = iter->second;

		if ( spSummon->IsVisibleDetect() == FALSE )
			continue;

		if (spSummon && COLLISION::IsCollisionVolume(CV, spSummon->m_vMax, spSummon->m_vMin))
		{
			if ( spSummon->IsVisibleDisplayName() )
			{
				// Note : 내 소환수만 처리한다.
				if (m_pGaeaClient->GETMYGAEAID() == spSummon->m_dwOwnerGaeaID)
				{
					CROWREN sDISP_NAME;
					sDISP_NAME.SetData( m_pGaeaClient, spSummon.get() );
					GfxInterfaceBase::GetInstance()->AddNameDisplay( sDISP_NAME, pMyCharacter->GetName() );
				}
				else
				{
					CROWREN sDISP_NAME;
					sDISP_NAME.SetData(m_pGaeaClient, spSummon.get());
					const std::tr1::shared_ptr<GLCharClient>& pCHAR = m_pGaeaClient->GetChar(spSummon->m_dwOwnerGaeaID);
					if (pCHAR)
					{
						GfxInterfaceBase::GetInstance()->AddNameDisplay( sDISP_NAME, pCHAR->GetName() );
					}
				}
			}
		}
	}

	// PET
	// 여기서 다른 팻들의 Render 처리;
	for (PETMAPID_ITER iter=m_PETArray.begin(); iter!=m_PETArray.end(); ++iter)
	{
		std::tr1::shared_ptr<GLAnyPet> pPet = iter->second;
		// 팻이름 표시;
		if ( pPet->IsVisibleDetect() == FALSE )
			continue;

		if ( COLLISION::IsCollisionVolume( CV,pPet->m_vMax,pPet->m_vMin ) )
		{
			if ( pPet->IsVisibleDisplayName() )
			{
				CROWREN sDISP_NAME;
				sDISP_NAME.SetData(m_pGaeaClient, pPet);
				const std::tr1::shared_ptr<GLCharClient>& pCHAR = m_pGaeaClient->GetChar( pPet->m_dwOwner );
				if (pCHAR)
				{
					GfxInterfaceBase::GetInstance()->AddNameDisplay( sDISP_NAME, pCHAR->GetName() );
				}
			}
		}
	}

	//CROW
	for (DROP_CROW_MAP_ITER iter=m_DropCrow.begin(); iter!=m_DropCrow.end(); ++iter)
	{
		std::tr1::shared_ptr<GLCrowClient> spCrow = iter->second;
		if ( spCrow == NULL )
			continue; // 유효하지 않은 액터라면 실패;

		if ( spCrow->IsVisibleDetect() == FALSE )
			continue; // 은신 중이고 감지하지 못하면 실패;

		if ( COLLISION::IsCollisionVolume(CV, spCrow->m_vMax, spCrow->m_vMin) == FALSE )
			continue; // 카메라 절두체 밖에 있다면 실패;

		if ( spCrow->IsDie() == FALSE )
		{ // 죽었다면 이름은 렌더하지 않음;	

			if ( spCrow->IsVisibleDisplayName() )
			{
				CROWREN sDISP_NAME;
				sDISP_NAME.SetData(m_pGaeaClient, spCrow.get());

				GfxInterfaceBase::GetInstance()->AddNameDisplay( sDISP_NAME );
			}
		}
	}    

	for (DROP_MATERIAL_MAP_ITER iter=m_DropMaterial.begin(); iter!=m_DropMaterial.end(); ++iter)
	{    
		std::tr1::shared_ptr<GLMaterialClient> spMaterial = iter->second;
		if (!spMaterial)
			continue;

		if (!spMaterial->IsDie() && COLLISION::IsCollisionVolume(CV, spMaterial->m_vMax, spMaterial->m_vMin))
		{
			if ( spMaterial->IsVisibleDisplayName() )
			{
				CROWREN sDISP_NAME;
				sDISP_NAME.SetData(m_pGaeaClient, spMaterial.get());

				GfxInterfaceBase::GetInstance()->AddNameDisplay( sDISP_NAME );
			}
		}
	}

	for (DROP_ITEM_ITER iter=m_DropItem.begin(); iter!=m_DropItem.end(); ++iter)
	{
		std::tr1::shared_ptr<CItemClientDrop> spItemDrop = iter->second;		
		//CItemClientDrop &sItemDrop = *pCurItem->Data;

		bool bCV = spItemDrop->IsCollision(CV);
		if (bCV)
		{
			if ( spItemDrop->IsVisibleDisplayName() )
			{
				CROWREN sDISP_NAME;
				sDISP_NAME.SetData(m_pGaeaClient, spItemDrop);

				GfxInterfaceBase::GetInstance()->AddNameDisplay( sDISP_NAME );
			}
		}
	}

	for (DROP_MONEY_MAP_ITER iter=m_DropMoney.begin(); iter!=m_DropMoney.end(); ++iter)
	{
		std::tr1::shared_ptr<CMoneyClientDrop> spMoneyDrop = iter->second;
		if (!spMoneyDrop)
			continue;
		bool bCV = spMoneyDrop->IsCollision(CV);
		if (bCV)
		{
			if ( spMoneyDrop->IsVisibleDisplayName() )
			{
				CROWREN sDISP_NAME;
				sDISP_NAME.SetData(m_pGaeaClient, spMoneyDrop);

				GfxInterfaceBase::GetInstance()->AddNameDisplay( sDISP_NAME );
			}
		}
	}
}

HRESULT GLLandManClient::Render_MobItem( CLIPVOLUME &CV )
{
    D3DXVECTOR3 vCOLL(0.f,0.f,0.f);

    GLCharacter* const pMyCharacter = m_pGaeaClient->GetCharacter();
    const DWORD dwMyGaeaID = pMyCharacter->GetGaeaID();
    D3DXVECTOR3 vMyPosition(pMyCharacter->GetPosition());
    D3DXVECTOR3 vViewPosition(DxViewPort::GetInstance().GetFromPt());

    for (CHARMAPID_ITER iter=m_PCArray.begin(); iter!=m_PCArray.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCharClient> pChar = iter->second;
        if ( pChar == NULL )
            continue; // 유효하지 않은 케릭터면 실패;

        if ( pChar->IsVisibleDetect() == FALSE )
            continue; // 은신 중이고 감지하지 못하면 실패;
        // 오브젝트에 의한 가려짐도 IsVisibleDetect()함수에서 처리 되고 있음;

        if ( COLLISION::IsCollisionVolume(CV,pChar->m_vMax,pChar->m_vMin) == FALSE )
            continue; // 컬링에 의해 보이지 않는 상태이면 패스;		

        pChar->Render(m_pd3dDevice, CV, FALSE);
    }

    //// SUMMON
    //// 소환수들의 Render 처리
    //m_pInterface->SummonSetCount(m_dwCOUNT);

    for (DROP_SUMMON_MAP_ITER iter=m_DropSummon.begin(); iter!=m_DropSummon.end(); ++iter)
    {
        std::tr1::shared_ptr<GLSummonClient> spSummon = iter->second;
        // Note : DIE 일때는 이펙트만 그린다.
        if ( spSummon->isAction(GLAT_DIE) )
        {
            spSummon->RenderAniEff(m_pd3dDevice);
            continue;
        }

        if ( spSummon->IsVisibleDetect() == FALSE )
            continue;

        spSummon->Render(m_pd3dDevice, CV, FALSE);
    }

    // PET
    // 여기서 다른 팻들의 Render 처리;
    for (PETMAPID_ITER iter=m_PETArray.begin(); iter!=m_PETArray.end(); ++iter)
    {
        std::tr1::shared_ptr<GLAnyPet> pPet = iter->second;
        // 팻이름 표시;
        if ( pPet->IsVisibleDetect() == FALSE )
            continue;

        pPet->Render ( m_pd3dDevice );
    }

    //CROW
    for (DROP_CROW_MAP_ITER iter=m_DropCrow.begin(); iter!=m_DropCrow.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCrowClient> spCrow = iter->second;
        if ( spCrow == NULL )
            continue; // 유효하지 않은 액터라면 실패;

        if ( spCrow->IsVisibleDetect() == FALSE )
            continue; // 은신 중이고 감지하지 못하면 실패;

        if ( COLLISION::IsCollisionVolume(CV, spCrow->m_vMax, spCrow->m_vMin) == FALSE )
            continue; // 카메라 절두체 밖에 있다면 실패;

        spCrow->Render(m_pd3dDevice, CV, FALSE);
    }    

    for (DROP_MATERIAL_MAP_ITER iter=m_DropMaterial.begin(); iter!=m_DropMaterial.end(); ++iter)
    {    
        std::tr1::shared_ptr<GLMaterialClient> spMaterial = iter->second;
        if (!spMaterial)
            continue;

        spMaterial->Render(m_pd3dDevice, CV, FALSE);
    }

    DWORD dwOldLighting;
    m_pd3dDevice->GetRenderState ( D3DRS_LIGHTING, &dwOldLighting );
    m_pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );

    DWORD dwOldColorOp;
    m_pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP,   &dwOldColorOp );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );

    for (DROP_ITEM_ITER iter=m_DropItem.begin(); iter!=m_DropItem.end(); ++iter)
    {
        std::tr1::shared_ptr<CItemClientDrop> spItemDrop = iter->second;		
        //CItemClientDrop &sItemDrop = *pCurItem->Data;

        bool bCV = spItemDrop->IsCollision(CV);
        if (bCV)
        {
            spItemDrop->RenderItem(m_pd3dDevice, NULL);
        }
    }

    for (DROP_MONEY_MAP_ITER iter=m_DropMoney.begin(); iter!=m_DropMoney.end(); ++iter)
    {
        std::tr1::shared_ptr<CMoneyClientDrop> spMoneyDrop = iter->second;
        if (!spMoneyDrop)
            continue;

        bool bCV = spMoneyDrop->IsCollision(CV);
        if (bCV)
        {
            spMoneyDrop->RenderItem(m_pd3dDevice, NULL);
        }
    }

    m_pd3dDevice->SetRenderState ( D3DRS_LIGHTING, dwOldLighting );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, dwOldColorOp );

    return S_OK;
}

HRESULT GLLandManClient::Render_MobItem_SoftAlpha( CLIPVOLUME &CV )
{
	D3DXVECTOR3 vCOLL(0.f,0.f,0.f);

	GLCharacter* const pMyCharacter = m_pGaeaClient->GetCharacter();
	const DWORD dwMyGaeaID = pMyCharacter->GetGaeaID();
	D3DXVECTOR3 vMyPosition(pMyCharacter->GetPosition());
	D3DXVECTOR3 vViewPosition(DxViewPort::GetInstance().GetFromPt());

	for (CHARMAPID_ITER iter=m_PCArray.begin(); iter!=m_PCArray.end(); ++iter)
	{
		std::tr1::shared_ptr<GLCharClient> pChar = iter->second;
		if ( pChar == NULL )
			continue; // 유효하지 않은 케릭터면 실패;

		if ( pChar->IsVisibleDetect() == FALSE )
			continue; // 은신 중이고 감지하지 못하면 실패;
		// 오브젝트에 의한 가려짐도 IsVisibleDetect()함수에서 처리 되고 있음;

		if ( COLLISION::IsCollisionVolume(CV,pChar->m_vMax,pChar->m_vMin) == FALSE )
			continue; // 컬링에 의해 보이지 않는 상태이면 패스;

		m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		pChar->RenderSoftAlpha( m_pd3dDevice );
	}

	for (DROP_SUMMON_MAP_ITER iter=m_DropSummon.begin(); iter!=m_DropSummon.end(); ++iter)
	{
		std::tr1::shared_ptr<GLSummonClient> spSummon = iter->second;
		// Note : DIE 일때는 이펙트만 그린다.
		if ( spSummon->isAction(GLAT_DIE) )
		{
			continue;
		}

		if ( spSummon->IsVisibleDetect() == FALSE )
			continue;

		m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		spSummon->RenderSoftAlpha( m_pd3dDevice, CV );
	}

	// PET
	// 여기서 다른 팻들의 Render 처리;
	for (PETMAPID_ITER iter=m_PETArray.begin(); iter!=m_PETArray.end(); ++iter)
	{
		std::tr1::shared_ptr<GLAnyPet> pPet = iter->second;

		if ( pPet->IsVisibleDetect() == FALSE )
			continue;

		m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		pPet->RenderSoftAlpha( m_pd3dDevice );
	}

	//CROW
	for (DROP_CROW_MAP_ITER iter=m_DropCrow.begin(); iter!=m_DropCrow.end(); ++iter)
	{
		std::tr1::shared_ptr<GLCrowClient> spCrow = iter->second;
		if ( spCrow == NULL )
			continue; // 유효하지 않은 액터라면 실패;

		if ( spCrow->IsVisibleDetect() == FALSE )
			continue; // 은신 중이고 감지하지 못하면 실패;

		if ( COLLISION::IsCollisionVolume(CV, spCrow->m_vMax, spCrow->m_vMin) == FALSE )
			continue; // 카메라 절두체 밖에 있다면 실패;

		m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		spCrow->RenderSoftAlpha( m_pd3dDevice, CV );
	}    

	for (DROP_MATERIAL_MAP_ITER iter=m_DropMaterial.begin(); iter!=m_DropMaterial.end(); ++iter)
	{    
		std::tr1::shared_ptr<GLMaterialClient> spMaterial = iter->second;
		if (!spMaterial)
			continue;

		m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		spMaterial->RenderSoftAlpha( m_pd3dDevice, CV );
	}

	return S_OK;
}

HRESULT GLLandManClient::Render_MobItem_MaterialEffect( CLIPVOLUME &CV )
{
	D3DXVECTOR3 vCOLL(0.f,0.f,0.f);

	GLCharacter* const pMyCharacter = m_pGaeaClient->GetCharacter();
	const DWORD dwMyGaeaID = pMyCharacter->GetGaeaID();
	D3DXVECTOR3 vMyPosition(pMyCharacter->GetPosition());
	D3DXVECTOR3 vViewPosition(DxViewPort::GetInstance().GetFromPt());

	for (CHARMAPID_ITER iter=m_PCArray.begin(); iter!=m_PCArray.end(); ++iter)
	{
		std::tr1::shared_ptr<GLCharClient> pChar = iter->second;
		if ( pChar == NULL )
			continue; // 유효하지 않은 케릭터면 실패;

		if ( pChar->IsVisibleDetect() == FALSE )
			continue; // 은신 중이고 감지하지 못하면 실패;
		// 오브젝트에 의한 가려짐도 IsVisibleDetect()함수에서 처리 되고 있음;

		if ( COLLISION::IsCollisionVolume(CV,pChar->m_vMax,pChar->m_vMin) == FALSE )
			continue; // 컬링에 의해 보이지 않는 상태이면 패스;

		m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		pChar->RenderMaterialSkinEffect( m_pd3dDevice );
	}

	for (DROP_SUMMON_MAP_ITER iter=m_DropSummon.begin(); iter!=m_DropSummon.end(); ++iter)
	{
		std::tr1::shared_ptr<GLSummonClient> spSummon = iter->second;
		// Note : DIE 일때는 이펙트만 그린다.
		if ( spSummon->isAction(GLAT_DIE) )
		{
			continue;
		}

		if ( spSummon->IsVisibleDetect() == FALSE )
			continue;

		m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		spSummon->RenderMaterialSkinEffect( m_pd3dDevice, CV );
	}

	// PET
	// 여기서 다른 팻들의 Render 처리;
	for (PETMAPID_ITER iter=m_PETArray.begin(); iter!=m_PETArray.end(); ++iter)
	{
		std::tr1::shared_ptr<GLAnyPet> pPet = iter->second;

		if ( pPet->IsVisibleDetect() == FALSE )
			continue;

		m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		pPet->RenderMaterialSkinEffect( m_pd3dDevice );
	}

	//CROW
	for (DROP_CROW_MAP_ITER iter=m_DropCrow.begin(); iter!=m_DropCrow.end(); ++iter)
	{
		std::tr1::shared_ptr<GLCrowClient> spCrow = iter->second;
		if ( spCrow == NULL )
			continue; // 유효하지 않은 액터라면 실패;

		if ( spCrow->IsVisibleDetect() == FALSE )
			continue; // 은신 중이고 감지하지 못하면 실패;

		if ( COLLISION::IsCollisionVolume(CV, spCrow->m_vMax, spCrow->m_vMin) == FALSE )
			continue; // 카메라 절두체 밖에 있다면 실패;

		m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		spCrow->RenderMaterialSkinEffect( m_pd3dDevice, CV );
	}    

	for (DROP_MATERIAL_MAP_ITER iter=m_DropMaterial.begin(); iter!=m_DropMaterial.end(); ++iter)
	{    
		std::tr1::shared_ptr<GLMaterialClient> spMaterial = iter->second;
		if (!spMaterial)
			continue;

		m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		spMaterial->RenderMaterialSkinEffect( m_pd3dDevice, CV );
	}

	return S_OK;
}

HRESULT GLLandManClient::Render_MobItem_Shadow( CLIPVOLUME &CV, BOOL bWorldSystemOLD )
{
	D3DXVECTOR3 vCOLL(0.f,0.f,0.f);

	GLCharacter* const pMyCharacter = m_pGaeaClient->GetCharacter();
	const DWORD dwMyGaeaID = pMyCharacter->GetGaeaID();
	D3DXVECTOR3 vMyPosition(pMyCharacter->GetPosition());
	D3DXVECTOR3 vViewPosition(DxViewPort::GetInstance().GetFromPt());

	for (CHARMAPID_ITER iter=m_PCArray.begin(); iter!=m_PCArray.end(); ++iter)
	{
		std::tr1::shared_ptr<GLCharClient> pChar = iter->second;
		if ( pChar == NULL )
			continue; // 유효하지 않은 케릭터면 실패;

		if ( pChar->IsVisibleDetect() == FALSE )
			continue; // 은신 중이고 감지하지 못하면 실패;
		// 오브젝트에 의한 가려짐도 IsVisibleDetect()함수에서 처리 되고 있음;

		if ( COLLISION::IsCollisionVolume(CV,pChar->m_vMax,pChar->m_vMin) == FALSE )
			continue; // 컬링에 의해 보이지 않는 상태이면 패스;

		pChar->RenderShadow( m_pd3dDevice, CV, bWorldSystemOLD );

		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
		{
			pChar->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL1(), GetUsedMaterialSystem(), 0 );
			pChar->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL2(), GetUsedMaterialSystem(), 1 );
		}
	}

	for (DROP_SUMMON_MAP_ITER iter=m_DropSummon.begin(); iter!=m_DropSummon.end(); ++iter)
	{
		std::tr1::shared_ptr<GLSummonClient> spSummon = iter->second;
		// Note : DIE 일때는 이펙트만 그린다.
		if ( spSummon->isAction(GLAT_DIE) )
		{
			continue;
		}

		if ( spSummon->IsVisibleDetect() == FALSE )
			continue;

		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
		{
			spSummon->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL1(), GetUsedMaterialSystem(), 0 );
			spSummon->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL2(), GetUsedMaterialSystem(), 1 );
		}

		//m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		//m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		spSummon->RenderShadow( m_pd3dDevice, CV, bWorldSystemOLD );
	}

	// PET
	// 여기서 다른 팻들의 Render 처리;
	for (PETMAPID_ITER iter=m_PETArray.begin(); iter!=m_PETArray.end(); ++iter)
	{
		std::tr1::shared_ptr<GLAnyPet> pPet = iter->second;

		if ( pPet->IsVisibleDetect() == FALSE )
			continue;

		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
		{
			pPet->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL1(), GetUsedMaterialSystem(), 0 );
			pPet->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL2(), GetUsedMaterialSystem(), 1 );
		}

		//m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		//m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		pPet->RenderShadow( m_pd3dDevice, CV, bWorldSystemOLD );
	}

	//CROW
	for (DROP_CROW_MAP_ITER iter=m_DropCrow.begin(); iter!=m_DropCrow.end(); ++iter)
	{
		std::tr1::shared_ptr<GLCrowClient> spCrow = iter->second;
		if ( spCrow == NULL )
			continue; // 유효하지 않은 액터라면 실패;

		if ( spCrow->IsVisibleDetect() == FALSE )
			continue; // 은신 중이고 감지하지 못하면 실패;

		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
		{
			spCrow->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL1(), GetUsedMaterialSystem(), 0 );
			spCrow->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL2(), GetUsedMaterialSystem(), 1 );
		}

		if ( COLLISION::IsCollisionVolume(CV, spCrow->m_vMax, spCrow->m_vMin) == FALSE )
			continue; // 카메라 절두체 밖에 있다면 실패;

		//m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		//m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		spCrow->RenderShadow( m_pd3dDevice, CV, bWorldSystemOLD );
	}    

	for (DROP_MATERIAL_MAP_ITER iter=m_DropMaterial.begin(); iter!=m_DropMaterial.end(); ++iter)
	{    
		std::tr1::shared_ptr<GLMaterialClient> spMaterial = iter->second;
		if (!spMaterial)
			continue;

		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
		{
			spMaterial->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL1(), GetUsedMaterialSystem(), 0 );
			spMaterial->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL2(), GetUsedMaterialSystem(), 1 );
		}

		//m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
		//m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

		spMaterial->RenderShadow( m_pd3dDevice, CV, bWorldSystemOLD );
	}

	return S_OK;
}

HRESULT GLLandManClient::Render_MobItem_Reflect( CLIPVOLUME &CV )
{
	// 타 캐릭터와 몹들은 반사를 하지 않는다.

	return S_OK;
}

HRESULT GLLandManClient::Render_AlphaObejct ( CLIPVOLUME &CV, LPDIRECT3DSURFACEQ pColorBuffer )
{
	if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && GetUsedMaterialSystem() )
	{
		//LPDIRECT3DSURFACEQ	pColorBuffer(NULL);
		//if ( RENDERPARAM::g_bEnable_HDR )
		//{
		//	pColorBuffer = DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR;
		//}
		//else
		//{
		//	pColorBuffer = DxSurfaceTex::GetInstance().m_pColorBuffer;
		//}

		D3DXMATRIX matWorld;
		D3DXMatrixIdentity( &matWorld );
		D3DCOLOR colorClear = D3DCOLOR_XRGB(0,0,0);
		if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_DF )
		{
			NSDeffered::RenderDefferedForSM30DF
			(
				m_pd3dDevice,
				colorClear,
				m_spLandMan.get(),
				DxSurfaceTex::GetInstance().GetWidth(),
				DxSurfaceTex::GetInstance().GetHeight(),
				pColorBuffer,
				DxSurfaceTex::GetInstance().m_pColorSuf_1st,
				DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
				DxSurfaceTex::GetInstance().m_pColorSuf_4th,
				DxSurfaceTex::GetInstance().m_pColorTex_1st,
				DxSurfaceTex::GetInstance().m_pColorTex_2nd, 
				DxSurfaceTex::GetInstance().m_pColorTex_4th, 
				FALSE,
				0.f,
				matWorld,
				DxViewPort::GetInstance().GetMatView(),
				DxViewPort::GetInstance().GetMatProj(),
				DxViewPort::GetInstance().GetClipVolume(),
				TRUE,
				TRUE,
				TRUE,
				FALSE
			);
		}
		else
		{
			NSDeffered::RenderDefferedForSM30NM
			(
				m_pd3dDevice,
				colorClear,
				m_spLandMan.get(),
				DxSurfaceTex::GetInstance().GetWidth(),
				DxSurfaceTex::GetInstance().GetHeight(),
				pColorBuffer,
				DxSurfaceTex::GetInstance().m_pColorSuf_1st,
				DxSurfaceTex::GetInstance().m_pColorSuf_4th,
				DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
				DxSurfaceTex::GetInstance().m_pColorSuf_3rd,	// Color는 unsign64 를 쓰겠다.
				DxSurfaceTex::GetInstance().m_pColorSuf_LBuffer,
				DxSurfaceTex::GetInstance().m_pSufFourCC_INTZ,
				DxSurfaceTex::GetInstance().m_pColorTex_1st,
				DxSurfaceTex::GetInstance().m_pColorTex_4th,
				DxSurfaceTex::GetInstance().m_pColorTex_2nd,
				DxSurfaceTex::GetInstance().m_pColorTex_3rd, 	// Color는 unsign64 를 쓰겠다.
				DxSurfaceTex::GetInstance().m_pColorTex_LBuffer,
				DxSurfaceTex::GetInstance().m_pTexFourCC_INTZ, 
				matWorld,
				DxViewPort::GetInstance().GetMatView(),
				DxViewPort::GetInstance().GetMatProj(),
				DxViewPort::GetInstance().GetClipVolume(),
				TRUE
			);
		}
	}
	else
	{
		m_spLandMan->Render_LOW_SoftAlpha ( m_pd3dDevice, CV );
	}

	return S_OK;
}

void GLLandManClient::Render_SingleEffect ( CLIPVOLUME &CV )
{
	if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && GetUsedMaterialSystem() )
	{
		m_spLandMan->RenderEff_Deffered( m_pd3dDevice, CV );
	}
	else
	{
		m_spLandMan->Render_LOW_SingleEff ( m_pd3dDevice, CV );
	}

	GLLevelFile::Render_EFF ( m_pd3dDevice, CV );
}

void GLLandManClient::RenderShadow()
{
	m_spLandMan->RenderShadow( m_pd3dDevice );
}

void GLLandManClient::RenderShadow_SpotLight1( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_spLandMan->RenderShadow_SpotLight1( pd3dDevice );
}

void GLLandManClient::RenderShadow_SpotLight2( LPDIRECT3DDEVICEQ pd3dDevice )
{
	m_spLandMan->RenderShadow_SpotLight2( pd3dDevice );
}

void GLLandManClient::RenderStarGlow()
{
	m_spLandMan->RenderStarGlow( m_pd3dDevice );
}

void GLLandManClient::Render_LightShafts()
{
	m_spLandMan->Render_LightShafts( m_pd3dDevice );
}

BOOL GLLandManClient::DropItem ( SDROP_CLIENT_ITEM *pItemDrop )
{
	//	Note : 떨어질 위치 노드를 찾는다.
	//
	CLIENTQUADNODE* pLandNode = m_LandTree.FindNode ( (int)pItemDrop->vPos.x, (int)pItemDrop->vPos.z );
	if ( !pLandNode )
        return	FALSE;

	SITEM* pITEM = GLogicData::GetInstance().GetItem( pItemDrop->sItemClient.sNativeID );
	if ( !pITEM )
        return FALSE;

	//	Note : 메모리를 할당.
	//
    std::tr1::shared_ptr<CItemClientDrop> pNewItem(new CItemClientDrop);

	//	Note : 지형의 높이랑 맞추어서 떨어지는 지점을 선택한다.
	//		( 내비게이션이랑 오차가 약간 있는걸 감안. )
	D3DXVECTOR3 vPoint1 = pItemDrop->vPos + D3DXVECTOR3(0,8,0);
	D3DXVECTOR3 vPoint2 = pItemDrop->vPos + D3DXVECTOR3(0,-5,0);
	
	D3DXVECTOR3 vCollision(FLT_MAX,FLT_MAX,FLT_MAX);
	BOOL bCollision = FALSE;
	LPDXFRAME pDxFrame = NULL;
	m_spLandMan->IsCollision ( vPoint1, vPoint2, vCollision, bCollision, pDxFrame, EMCC_CULL_NONE_ONLY_WALL );

	D3DXVECTOR3 vDropPos = pItemDrop->vPos;
	if (bCollision)
        vDropPos = vCollision;
	vDropPos.y += 1.0f;	

	//	Note : 아이템 정보 ( 종류, 위치 ).
	//
	pNewItem->sItemClient = pItemDrop->sItemClient;
	pNewItem->vPos = vDropPos;
	D3DXMatrixTranslation ( &pNewItem->matWld, pNewItem->vPos.x, pNewItem->vPos.y, pNewItem->vPos.z );	

	//	Note : 관리 ID 부여.
	//
	pNewItem->sMapID = GetBaseMapID();
	pNewItem->dwGlobID = pItemDrop->dwGlobID;
	pNewItem->dwCeID = pLandNode->dwCID;

	pNewItem->bCanGet = pItemDrop->bCanGet;

	SetDropItem(pNewItem->dwGlobID, pNewItem);
	
	std::string strFieldFile = pITEM->sBasicOp.strFieldFile;
	std::transform ( strFieldFile.begin(), strFieldFile.end(), strFieldFile.begin(), tolower );

	bool bSIMMESH = false;
	bool bEFFECT = false;
	std::string::size_type idx;
	idx = strFieldFile.find ( ".x" );
	if ( idx!=std::string::npos )
        bSIMMESH = true;

	idx = strFieldFile.find ( ".egp" );
	if ( idx!=std::string::npos )
        bEFFECT = true;

	pNewItem->vMax = D3DXVECTOR3(10,10,10);
	pNewItem->vMin = D3DXVECTOR3(-10,-10,-10);

	if ( bSIMMESH )
	{
		DxSimMesh* pSimMesh = GLItemMan::GetInstance().GetFieldMesh ( pNewItem->sItemClient.sNativeID );
		if ( !pSimMesh )
		{
			sc::writeLogError(
                sc::string::format(
                    "item field mesh null %1%", pITEM->GetName()));
		}
		else
		{
			pSimMesh->GetBoundBox ( pNewItem->vMax, pNewItem->vMin );
		}
		
		pNewItem->pSimMesh = pSimMesh;
	}
	else if ( bEFFECT )
	{
		DxEffSinglePropGroupResource spPropGroup = DxEffSinglePropGMan::GetInstance().LoadEffectGProp( strFieldFile.c_str(), FALSE, TRUE );
		if ( spPropGroup.IsValid() )
		{
			pNewItem->vMax = spPropGroup.get()->m_vMax;
			pNewItem->vMin = spPropGroup.get()->m_vMin;
		}

		STARGETID sTARID ( CROW_ITEM, pNewItem->dwGlobID, pNewItem->vPos );
		DxEffGroupPlayer::GetInstance().PassiveEffect ( strFieldFile.c_str(), pNewItem->matWld, sTARID );
	}

	if( pNewItem->bCanGet )
	{
		STARGETID sTARID ( CROW_ITEM, pNewItem->dwGlobID, pNewItem->vPos );
		DxEffGameHelp::GetInstance().UpdateGameHelpEffect( EMMINE_EFFECT, pNewItem->matWld, sTARID );
	}

	COLLISION::MINDETECTAABB ( pNewItem->vMax, pNewItem->vMin, 1.1f, 4.0f );
	COLLISION::TransformAABB ( pNewItem->vMax, pNewItem->vMin, pNewItem->matWld );


	if ( pItemDrop->fAge < 0.01f )
	{
		std::string strSOUND = "ITEMDROP_COIN";

		switch ( pITEM->sBasicOp.emItemType )
		{
		case ITEM_INC_GRINDINGRATE:
		case ITEM_ANTI_RESET:
		case ITEM_ANTI_DISAPPEAR:
		case ITEM_REVIVE:
		case ITEM_SUIT:
		case ITEM_VEHICLE:
			{
				switch ( pITEM->sSuitOp.emSuit )
				{
				case SUIT_HEADGEAR:
				case SUIT_UPPER:
				case SUIT_LOWER:
				case SUIT_HAND:
				case SUIT_VEHICLE:
                case SUIT_WAISTBAND:
					strSOUND = "ITEMDROP_SUIT";
					break;

				case SUIT_FOOT:
					strSOUND = "ITEMDROP_SHOES";
					break;

				case SUIT_HANDHELD:
					strSOUND = "ITEMDROP_WAPON";
					break;

				case SUIT_NECK:
				case SUIT_WRIST:
				case SUIT_FINGER:
                case SUIT_EARRING:
                case SUIT_ACCESSORIES:
					strSOUND = "ITEMDROP_RING";
					break;
				};
			}
			break;

		case ITEM_SKILL:
		case ITEM_POSTBOX_CARD:
			strSOUND = "ITEMDROP_SCROLL";
			break;

		case ITEM_EFFECT_RELEASE_CURE :
		case ITEM_CURE:
		case ITEM_LUNCHBOX:
			strSOUND = "ITEMDROP_DRUGS";
			break;

		case ITEM_ARROW:
		case ITEM_RECALL:
		case ITEM_KEY:
		case ITEM_GRINDING:
		case ITEM_CHARM:
		case ITEM_TICKET:
		case ITEM_SKP_RESET:
		case ITEM_STAT_RESET:
		case ITEM_SKP_STAT_RESET_B:
		case ITEM_SKP_STAT_RESET_A:
		case ITEM_BOX:
		case ITEM_CLEANSER:
		case ITEM_LOUDSPEAKER:
		case ITEM_FIRECRACKER:
		case ITEM_CHARACTER_CARD:
		case ITEM_INVEN_CARD:
		case ITEM_STORAGE_CARD:
		case ITEM_STORAGE_CONNECT:
		case ITEM_PREMIUMSET:
		case ITEM_PRIVATEMARKET:
		case ITEM_RANDOMITEM:
		case ITEM_DISJUNCTION:
		case ITEM_REMODEL:
		case ITEM_GARBAGE_CARD:
		case ITEM_TELEPORT_CARD:
		case ITEM_BULLET:
		case ITEM_CHANGE_ACTORS_DISGUISE_INFO:

			strSOUND = "ITEMDROP_SUIT";
			break;

		case ITEM_QITEM:
			strSOUND = "ITEMDROP_?BOX";
			break;
		};

		std::string strItemRank = sc::string::format("ITEMDROP_RANK%d", (int)pITEM->sBasicOp.emLevel);
		DxSoundLib::GetInstance()->PlaySound ( strSOUND );
		DxSoundLib::GetInstance()->PlaySound ( strItemRank );
	}

	return TRUE;
}

BOOL GLLandManClient::DropMoney ( LONGLONG lnAmount, D3DXVECTOR3 vPos, DWORD dwGlobID, float fAge, BOOL bCanGet )
{
	//	Note : 떨어질 위치 노드를 찾는다.
	//
	CLIENTQUADNODE* pLandNode = m_LandTree.FindNode ( (int)vPos.x, (int)vPos.z );
	if ( !pLandNode )
        return	FALSE;

	//	Note : 메모리를 할당.
	//
    std::tr1::shared_ptr<CMoneyClientDrop> pNewMoney(new CMoneyClientDrop);

	//	Note : 지형의 높이랑 맞추어서 떨어지는 지점을 선택한다.
	//		( 내비게이션이랑 오차가 약간 있는걸 감안. )
	D3DXVECTOR3 vPoint1 = vPos + D3DXVECTOR3(0,8,0);
	D3DXVECTOR3 vPoint2 = vPos + D3DXVECTOR3(0,-5,0);
	
	D3DXVECTOR3 vCollision(FLT_MAX,FLT_MAX,FLT_MAX);
	BOOL bCollision = FALSE;
	LPDXFRAME pDxFrame = NULL;
	m_spLandMan->IsCollision ( vPoint1, vPoint2, vCollision, bCollision, pDxFrame, EMCC_CULL_NONE_ONLY_WALL );

	D3DXVECTOR3 vDropPos = vPos;
	if ( bCollision )	
		vDropPos = vCollision;
	vDropPos.y += 1.0f;

	//	Note : 아이템 정보 ( 종류, 위치 ).
	//
	pNewMoney->fAge = fAge;
	pNewMoney->lnAmount = lnAmount;
	pNewMoney->vPos = vDropPos;
	D3DXMatrixTranslation ( &pNewMoney->matWld, pNewMoney->vPos.x, pNewMoney->vPos.y, pNewMoney->vPos.z );


	//	Note : 관리 ID 부여.
	//
	pNewMoney->sMapID = GetBaseMapID();
	pNewMoney->dwGlobID = dwGlobID;
	pNewMoney->dwCeID = pLandNode->dwCID;

	pNewMoney->bCanGet = bCanGet;

	// 관리 노드 포인터
    SetDropMoney(dwGlobID, pNewMoney);												// - 전역.
	
	DxSimMesh* pMoneyMesh = NULL;
	if ( lnAmount>200 )			pMoneyMesh = GLItemMan::GetInstance().GetMoneyMesh ( 2 );
	else if ( lnAmount>60 )		pMoneyMesh = GLItemMan::GetInstance().GetMoneyMesh ( 1 );
	else						pMoneyMesh = GLItemMan::GetInstance().GetMoneyMesh ( 0 );
	
	if ( !pMoneyMesh )
	{
		sc::writeLogError(
            sc::string::format(
                "money field mesh null ( amount %1%)", lnAmount));
	}

	pNewMoney->pMoneyMesh = pMoneyMesh;

	if ( fAge < 0.01f )
	{
		DxSoundLib::GetInstance()->PlaySound ( "ITEMDROP_COIN" );
	}

	if( pNewMoney->bCanGet )
	{
		STARGETID sTARID ( CROW_MONEY, pNewMoney->dwGlobID, pNewMoney->vPos );
		DxEffGameHelp::GetInstance().UpdateGameHelpEffect( EMMINE_EFFECT, pNewMoney->matWld, sTARID );
	}

	return TRUE;
}

BOOL GLLandManClient::CreateAnyPet ( PSDROPPET pDropPet )
{
	const SMAPNODE* const pMapNode = m_pGaeaClient->FindMapNode(this->GetBaseMapID());
	const BOOL bInstanceCheck( pMapNode && (pMapNode->IsInstantMap() || pMapNode->IsNewInstanceMap()
		&& (GetGaeaMapID() == pDropPet->m_sMapID.getGaeaMapID())) );

	if ( (pDropPet->m_sMapID != GetBaseMapID()) && bInstanceCheck == FALSE )
		return FALSE;

	// 중복되면 이전것을 지우다.
	if  ( GetPet ( pDropPet->m_dwGUID ) )
		DropOutPet ( pDropPet->m_dwGUID );
	
	CLIENTQUADNODE* pLandNode = m_LandTree.GetNode ( pDropPet->m_dwCellID );
	if ( !pLandNode )					
        return FALSE;

	std::tr1::shared_ptr<GLAnyPet> pNewPet(new GLAnyPet(m_pGaeaClient));
	HRESULT hr = pNewPet->Create ( pDropPet, m_spLandMan->GetNaviMesh(), m_pd3dDevice );
	if (FAILED(hr))
        return FALSE;
	
	m_PETArray.insert ( std::make_pair(pDropPet->m_dwGUID,pNewPet) );
		
	std::tr1::shared_ptr<GLCharClient> const pOwner = GetChar(pDropPet->m_dwOwner);
	if ( pOwner && pOwner->IsVisibleDetect() == TRUE )
	{
		// 생성 효과
		D3DXMATRIX matEffect;
		D3DXMatrixTranslation ( &matEffect, pDropPet->m_vPos.x, pDropPet->m_vPos.y, pDropPet->m_vPos.z );

		std::string strGEN_EFFECT = GLCONST_CHAR::strPET_GEN_EFFECT.c_str();
		STARGETID sTargetID(CROW_PET,pNewPet->m_dwGUID,pNewPet->GetPosition());
		DxEffGroupPlayer::GetInstance().NewEffGroup
			(
			strGEN_EFFECT.c_str(),
			matEffect,
			&sTargetID,
			TRUE,
			FALSE
			);
	}

	return TRUE;
}

BOOL GLLandManClient::DropPet ( PSDROPPET pDropPet )
{
	const SMAPNODE* const pMapNode = m_pGaeaClient->FindMapNode(this->GetBaseMapID());
	const BOOL bInstanceCheck( pMapNode && (pMapNode->IsInstantMap() || pMapNode->IsNewInstanceMap()
		&& (GetGaeaMapID() == pDropPet->m_sMapID.getGaeaMapID())) );

	if ( (pDropPet->m_sMapID != GetBaseMapID()) && bInstanceCheck == FALSE )
		return FALSE;

	// 중복되면 이전것을 지우다.
	if  ( GetPet ( pDropPet->m_dwGUID ) ) DropOutPet ( pDropPet->m_dwGUID );
	
	CLIENTQUADNODE* pLandNode = m_LandTree.GetNode ( pDropPet->m_dwCellID );
	if ( !pLandNode )					
        return FALSE;

	std::tr1::shared_ptr<GLAnyPet> pNewPet(new GLAnyPet(m_pGaeaClient));
	HRESULT hr = pNewPet->Create ( pDropPet, m_spLandMan->GetNaviMesh(), m_pd3dDevice );
	if (FAILED(hr))
        return FALSE;
	
	m_PETArray.insert ( std::make_pair(pDropPet->m_dwGUID,pNewPet) );

	return TRUE;
}

BOOL GLLandManClient::DropOutPet ( DWORD dwGUID )
{	
	PETMAPID_ITER iter = m_PETArray.find(dwGUID);
	if (m_PETArray.end() != iter)
    {
        m_PETArray.erase(iter);
        return TRUE;
    }
    else
    {	
	    return FALSE;
    }
}

BOOL GLLandManClient::DropSummon ( PSDROPSUMMON pDropSummon )
{
	HRESULT hr=S_OK;

	if( !pDropSummon ) 
        return FALSE;

	if ( pDropSummon->m_sMapID != GetBaseMapID() )	
        return E_FAIL;

	//	Note : 떨어질 위치 노드를 찾는다.
	//
	CLIENTQUADNODE* pLandNode = m_LandTree.GetNode ( pDropSummon->m_dwCellID );
	if ( !pLandNode )	
        return	FALSE;

	//	Note : 메모리를 할당.
	//
    std::tr1::shared_ptr<GLSummonClient> pNewSummon(new GLSummonClient(m_pGaeaClient));
	hr = pNewSummon->Create(this, pDropSummon, m_pd3dDevice);
	if (FAILED(hr))
        return FALSE;

	//	Note : 관리 ID 부여.
	//
	pNewSummon->SetGaeaID(pDropSummon->m_dwGUID);
	
    SetDropSummon(pNewSummon->GetGaeaID(), pNewSummon);

	//pNewSummon->m_pQuadNode = pLandNode;											// - 트리 노드.
	
	// Note : 주인에게 등록
	std::tr1::shared_ptr<GLCharClient> pChar = GetChar( pDropSummon->m_dwOwner );

	if ( pChar )
	{
		pChar->m_dwSummonGUIDList.insert( pChar->m_dwSummonGUIDList.end(), pDropSummon->m_dwGUID );
	}
	else
	{
		GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		if (pCharacter)
		{
			pCharacter->m_dwSummonGUIDList.insert( pCharacter->m_dwSummonGUIDList.end(), pDropSummon->m_dwGUID );
		}
	}

	return TRUE;
}

BOOL GLLandManClient::DropOutSummon ( DWORD dwGUID )
{
    std::tr1::shared_ptr<GLSummonClient> spSummon = GetSummon(dwGUID);
    if (!spSummon)
        return FALSE;

	DeleteDropSummon(dwGUID);

	// Note : 주인에게 해지
	std::tr1::shared_ptr<GLCharClient> pChar = GetChar(spSummon->m_dwOwnerGaeaID);
	if (pChar)
	{
		pChar->m_dwSummonGUIDList.remove(spSummon->GetGaeaID());
	}
	else
	{
		GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
		pCharacter->m_dwSummonGUIDList.remove(spSummon->GetGaeaID());
	}	
	return TRUE;
}

BOOL GLLandManClient::DropCrow ( SDROP_CROW *pCrowDrop )
{
	HRESULT hr=S_OK;

	if( !pCrowDrop ) 
        return FALSE;

	if ( pCrowDrop->sMapID != GetBaseMapID() )	
        return E_FAIL;

	//	Note : 떨어질 위치 노드를 찾는다.
	//
	CLIENTQUADNODE* pLandNode = m_LandTree.GetNode(pCrowDrop->dwCeID);
	if (!pLandNode)
        return FALSE;

	// 메모리를 할당.
    std::tr1::shared_ptr<GLCrowClient> pNewCrow(new GLCrowClient(m_pGaeaClient));
	hr = pNewCrow->CreateCrow(this, pCrowDrop, m_pd3dDevice);
	if (FAILED(hr))
        return FALSE;

	// 관리 ID 부여
	pNewCrow->SetGaeaID(pCrowDrop->dwGlobalID);
	
	// 관리 노드 포인터
    SetDropCrow(pCrowDrop->dwGlobalID, pNewCrow);
		
	pNewCrow->SetQuestCompleteNPC( m_pGaeaClient->CHECKQUESTNPCSTATE( pNewCrow ) );

	return TRUE;
}

BOOL GLLandManClient::DropMaterial ( SDROP_MATERIAL *pCrowDrop )
{
	HRESULT hr=S_OK;

	if( !pCrowDrop ) 
        return FALSE;

	if ( pCrowDrop->sMapID != GetBaseMapID() )	
        return E_FAIL;

	//	Note : 떨어질 위치 노드를 찾는다.
	//
	CLIENTQUADNODE* pLandNode = m_LandTree.GetNode ( pCrowDrop->dwCeID );
	if ( !pLandNode )	
        return	FALSE;

	//	Note : 메모리를 할당.
	//
    std::tr1::shared_ptr<GLMaterialClient> pNewMaterial(new GLMaterialClient(m_pGaeaClient));
	hr = pNewMaterial->CreateMaterial ( this, pCrowDrop, m_pd3dDevice );
	if (FAILED(hr))	
    {
        //SAFE_DELETE( pNewMaterial );
        return FALSE;
    }

	//	Note : 관리 ID 부여.
	//
	pNewMaterial->SetGaeaID(pCrowDrop->dwGlobID);
	pNewMaterial->m_dwCeID = pLandNode->dwCID;

	SetDropMaterial(pCrowDrop->dwGlobID, pNewMaterial);
	
	return TRUE;
}

BOOL GLLandManClient::DropChar(std::tr1::shared_ptr<SDROP_CHAR> spCharDrop)
{
	HRESULT hr=S_OK;

	if (!spCharDrop)
        return FALSE;

	if (spCharDrop->m_Base.sMapID != GetBaseMapID())	
        return E_FAIL;

	//	Note : 같은 ID가 이미 존재하는지 검사.
	//
	if ( FindChar(spCharDrop->m_Base.dwGaeaID) )
	{
		//	Note : 이전에 존재하는 것을 제거함.
		//
		DropOutChar ( spCharDrop->m_Base.dwGaeaID );
	}

	//	Note : 떨어질 위치 노드를 찾는다.
	//
	CLIENTQUADNODE* pLandNode = m_LandTree.GetNode ( spCharDrop->m_Base.dwCeID );
	if (!pLandNode)
        return FALSE;

	// 메모리를 할당
    std::tr1::shared_ptr<GLCharClient> pNewChar(new GLCharClient(m_pGaeaClient));
	hr = pNewChar->Create(this, spCharDrop, m_pd3dDevice);
	if (FAILED(hr))
        return FALSE;

	// 관리 ID 부여.
	pNewChar->SetGaeaID(spCharDrop->m_Base.dwGaeaID);
	pNewChar->m_dwCeID = pLandNode->dwCID;
    pNewChar->m_CharDbNum = spCharDrop->m_Base.dwCharID;  // DBNum 부여;

	//	Note : 관리 노드 포인터.
	//
	m_PCArray.insert(CHARMAPID_VALUE(pNewChar->GetGaeaID(), pNewChar)); // - 전역.

	//	Note : 케릭터 이름 맵 등록하기.
	//
	m_mapPC.insert(std::make_pair(pNewChar->GetCharData().m_Base.szName, pNewChar));
	m_DBArry.insert(std::make_pair(pNewChar->GetCharID(),pNewChar));
	pNewChar->NewBlowEffect(spCharDrop->sSTATEBLOWS);

	return TRUE;
}

BOOL GLLandManClient::DropOutItem ( DWORD dwGlobID )
{	
	std::tr1::shared_ptr<CItemClientDrop> spItemDrop = GetItem(dwGlobID);
    if (!spItemDrop)
        return FALSE;

    DeleteDropItem(dwGlobID);

	// 자신의 소유 아이템 표시 삭제
	if (spItemDrop->bCanGet)
	{
		STARGETID sTARID(CROW_ITEM, spItemDrop->dwGlobID, spItemDrop->vPos);
		DxEffGameHelp::GetInstance().BlindEff(EMMINE_EFFECT, sTARID);
	}

	STARGETID sTARID ( CROW_ITEM, spItemDrop->dwGlobID, spItemDrop->vPos );
	DxEffGroupPlayer::GetInstance().DeletePassiveEffect ( sTARID );

	return TRUE;
}

BOOL GLLandManClient::DropOutMoney ( DWORD dwGlobID )
{
    std::tr1::shared_ptr<CMoneyClientDrop> pMoneyDrop = GetMoney(dwGlobID);
    if (!pMoneyDrop)
        return FALSE;

	DeleteDropMoney(dwGlobID);

	// 자신의 소유 아이템 표시 삭제
	if( pMoneyDrop->bCanGet )
	{
		STARGETID sTARID ( CROW_MONEY, pMoneyDrop->dwGlobID, pMoneyDrop->vPos );
		DxEffGameHelp::GetInstance().BlindEff( EMMINE_EFFECT, sTARID );
	}
	return TRUE;
}

void GLLandManClient::DropOutCrow(DWORD dwGlobID)
{	
    DeleteDropCrow(dwGlobID);
}

BOOL GLLandManClient::DropOutMaterial ( DWORD dwGlobID )
{
	DeleteDropMaterial(dwGlobID);    
	return TRUE;
}

BOOL GLLandManClient::DropOutChar ( DWORD dwGlobID )
{
	std::tr1::shared_ptr<GLCharClient> pChar = FindChar(dwGlobID);
    if (!pChar)
        return FALSE;
	
	//	Note : 케릭터 ID 맵 지워주기.
	//
	{
		CHARMAPID_ITER iter = m_PCArray.find(dwGlobID);
		if (iter != m_PCArray.end())
		{
			m_PCArray.erase(iter);
		}
		else
		{
	        sc::writeLogError(std::string("GLLandManClient::DropOutChar m_PCArray.end()!=iter"));
		}
	}

	//	Note : 케릭터 이름 맵 지워주기.
	//
	{
		CHARMAP_ITER iter = m_mapPC.find ( pChar->GetCharData().m_Base.szName );
		if ( m_mapPC.end()!=iter )
		{
			m_mapPC.erase( iter );
		}
		else
		{
			sc::writeLogError(std::string("GLLandManClient::DropOutChar m_mapPC.end() != iter"));
		}		
	}
	// Note : 케릭터 이름 맵 지워주기
	{
		CHARMAPID_ITER iter = m_DBArry.find(pChar->GetCharID());
		if (iter != m_DBArry.end())
		{
			m_DBArry.erase(iter);
		}
		else
		{
			sc::writeLogError(std::string("GLLandManClient::DropOutChar m_DBArry.end()!=iter"));
		}
	}
	
	return TRUE;
}

std::tr1::shared_ptr<GLCharClient> GLLandManClient::FindChar(const std::string& strName)
{
	CHARMAP_ITER iter = m_mapPC.find(strName);
	if (m_mapPC.end() != iter)
        return iter->second;
    else
        return std::tr1::shared_ptr<GLCharClient> ((GLCharClient*) 0);
}

std::tr1::shared_ptr<GLCharClient> GLLandManClient::FindCharByDBnum(DWORD dwDBnum)
{
	CHARMAPID_ITER iter = m_DBArry.find(dwDBnum);
	if (m_DBArry.end() != iter)
        return iter->second;
    else
        return std::tr1::shared_ptr<GLCharClient> ((GLCharClient*) 0);
}

std::tr1::shared_ptr<GLCharClient> GLLandManClient::FindChar(DWORD dwID)
{
	CHARMAPID_ITER iter = m_PCArray.find ( dwID );
	if (m_PCArray.end() != iter)
        return iter->second;
    else
        return std::tr1::shared_ptr<GLCharClient> ((GLCharClient*) 0);
}

WORD GLLandManClient::GetTargetBodyRadius ( const STARGETID &sTargetID )
{
	//	Note : 타겟의 위치 정보를 가져옴.
	if ( sTargetID.emCrow == CROW_PC )
	{
		std::tr1::shared_ptr<GLCharClient> pChar = GetChar ( sTargetID.GaeaId );
		if ( pChar )
            return pChar->GETBODYRADIUS();
	}
	else if ( sTargetID.emCrow == CROW_NPC || sTargetID.emCrow == CROW_MOB )
	{
		std::tr1::shared_ptr<GLCrowClient> spCrow = GetCrow(sTargetID.GaeaId);
		if (spCrow)
            return spCrow->GETBODYRADIUS();
	}
	else if ( sTargetID.emCrow == CROW_MATERIAL )
	{
		std::tr1::shared_ptr<GLMaterialClient> spMaterial = GetMaterial ( sTargetID.GaeaId );
		if (spMaterial)
            return spMaterial->GetBodyRadius();
	}
	else if( sTargetID.emCrow == CROW_PET )	// PetData
	{
	}
	else if( sTargetID.emCrow == CROW_SUMMON )	// SummonData
	{
		std::tr1::shared_ptr<GLSummonClient> pSummon = GetSummon ( sTargetID.GaeaId );
		if ( pSummon )
            return pSummon->GETBODYRADIUS();
	}
	else
	{
		GASSERT(0&&"emCrow가 잘못된 지정자 입니다." );
	}

	return WORD(0xFFFF);
}

HRESULT GLLandManClient::ClearDropObj ()
{
	DWORD i=0;
	for (i=0; i<MAXITEM; ++i)
        DropOutItem(i);
	for (i=0; i<MAXMONEY; ++i)
        DropOutMoney(i);
	for (i=0; i<MAXCROW; ++i)
        DropOutCrow(i);
	for (i=0; i<MAXCROW; ++i)
        DropOutMaterial(i);
	for (i=0; i<MAXSUMMON; ++i)
        DropOutSummon(i);
	
	CHARMAPID_ITER iter_del;
	CHARMAPID_ITER iter = m_PCArray.begin();
	CHARMAPID_ITER iter_end = m_PCArray.end();
	for ( ; iter!=iter_end; )
	{
		iter_del = iter++;
		DropOutChar ( (*iter_del).first );
	}

	PETMAPID_ITER iterPet_del;
	PETMAPID_ITER iterPet = m_PETArray.begin();
	PETMAPID_ITER iterPet_end = m_PETArray.end();
	for ( ; iterPet!=iterPet_end; )
	{
		iterPet_del = iterPet++;
		DropOutPet ( (*iterPet_del).first );
	}

	//LandEvent
    m_pLandEvent->CleanUp();

	return S_OK;
}

void GLLandManClient::DisplayAllUser()
{
	CHARMAPID_ITER iter = m_PCArray.begin();
	CHARMAPID_ITER iter_end = m_PCArray.end();

	CString strText;

	for ( ; iter!=iter_end; ++iter)
	{
		std::tr1::shared_ptr<GLCharClient> spChar = iter->second;
		m_pGaeaClient->PrintConsoleText(
            sc::string::format(
                "%s : charid = %d",
                spChar->m_strName.c_str(),
                spChar->GetCharData().m_Base.dwCharID));
	}
}

BOOL GLLandManClient::ChangeCharMap(char* szOldName, char* szNewName)
{
	// 이름이 동일하다면 밑에 작업을 할 필요가 없다.
	if ( strcmp(szOldName, szNewName) == 0 )
		return TRUE;

	CHARMAP_ITER iter = m_mapPC.find(szOldName);
	if (iter != m_mapPC.end())
	{
		std::tr1::shared_ptr<GLCharClient> pCHAR = iter->second;
		m_mapPC.insert(std::make_pair(szNewName, pCHAR));
		m_mapPC.erase(iter);
		pCHAR->m_strName = szNewName;
	}
	return TRUE;
}

BOOL GLLandManClient::ResetClientObjects ()
{
	DWORD i=0;
	for ( i=0; i<MAXITEM; ++i )		DropOutItem(i);
	for ( i=0; i<MAXMONEY; ++i )	DropOutMoney(i);
	for ( i=0; i<MAXCROW; ++i )		DropOutCrow(i);
	for ( i=0; i<MAXCROW; ++i )		DropOutMaterial(i);
	for ( i=0; i<MAXSUMMON; ++i )	DropOutSummon(i);

	m_DropItem.clear();
    m_DropMoney.clear();
    m_DropCrow.clear();
    m_DropMaterial.clear();
    m_PETArray.clear();
    m_DropSummon.clear();
	return TRUE;
}

const bool GLLandManClient::IsCollision(const D3DXVECTOR3& vPoint0, const D3DXVECTOR3& vPoint1, EMCOLLMODE emCullMode ) const
{
	LPDXFRAME pDxFrame(NULL);
	BOOL bCollision(FALSE);
	D3DXVECTOR3 vCollision(0.0f, 0.0f, 0.0f);
	m_spLandMan->IsCollision(vPoint0, vPoint1, vCollision, bCollision, pDxFrame, emCullMode );

	return bCollision ? true : false;
}

void GLLandManClient::GetCollisionDropItem(
    const D3DXVECTOR3& vFromPt,
    const D3DXVECTOR3& vTargetPt,
    std::vector<STARGETID>& vDetect) const
{
    for (DROP_ITEM_CITER iter=m_DropItem.begin(); iter!=m_DropItem.end(); ++iter)
    {        
        const std::tr1::shared_ptr<CItemClientDrop>& spItemDrop = iter->second;
        bool bCOLL = spItemDrop->IsCollision(vFromPt, vTargetPt);
        if (bCOLL)
        {
            // 검출된 타깃 등록.
            STARGETID findTargetID;
            findTargetID.emCrow = CROW_ITEM;
            findTargetID.GaeaId = spItemDrop->dwGlobID;
            findTargetID.vPos   = spItemDrop->vPos;
            vDetect.push_back(findTargetID);
        }
    }
}

void GLLandManClient::GetSpherePointCollisionDropItem(
    const D3DXVECTOR3& vTarPos,
    float fLength,
    std::vector<STARGETID>& vDetect) const
{
    for (DROP_ITEM_CITER iter=m_DropItem.begin(); iter!=m_DropItem.end(); ++iter)
    {        
        const std::tr1::shared_ptr<CItemClientDrop>& spItemDrop = iter->second;        
        BOOL bCol = COLLISION::IsSpherePointCollision(spItemDrop->vPos, vTarPos, fLength);
        if (bCol)
        {
            // 검출된 타깃 등록.
            STARGETID findTargetID;
            findTargetID.emCrow = CROW_ITEM;
            findTargetID.GaeaId = spItemDrop->dwGlobID;
            findTargetID.vPos = spItemDrop->vPos;
            vDetect.push_back(findTargetID);
        }
    }
}

void GLLandManClient::GetNearDropItem(
    float& fMinDist,
    D3DXVECTOR3& vMinPos,
    float& fMinDistNotCan,
    D3DXVECTOR3& vMinPosNotCan,
    bool& bFind,
    const D3DXVECTOR3& vCharPos) const
{
    for (DROP_ITEM_CITER iter=m_DropItem.begin(); iter!=m_DropItem.end(); ++iter)
    {        
        std::tr1::shared_ptr<CItemClientDrop> spItemDrop = iter->second;
        if (spItemDrop->bCanGet)
        {	
            // 주울 수 있는 아이템에 대해서만 하자.
            float fCurDist = D3DXVec3Length(&(spItemDrop->vPos - vCharPos));
            if (fMinDist > fCurDist)
            {
                fMinDist = fCurDist;
                vMinPos = spItemDrop->vPos;
                bFind = true;
            }
        }

    	// 주울 수 없더라도 제일 가까운거 체크 해 놓는다.
        float fCurDist = D3DXVec3Length(&(spItemDrop->vPos - vCharPos));
        if (fMinDistNotCan > fCurDist)
        {
            fMinDistNotCan = fCurDist;
            vMinPosNotCan = spItemDrop->vPos;
        }        
    }
}

void GLLandManClient::GetCollisionDropMoney(
    const D3DXVECTOR3& vFromPt,
    const D3DXVECTOR3& vTargetPt,
    std::vector<STARGETID>& vDetect) const
{    
    for (DROP_MONEY_MAP_CITER iter=m_DropMoney.begin(); iter!=m_DropMoney.end(); ++iter)
    {
        std::tr1::shared_ptr<CMoneyClientDrop> spMoneyDrop = iter->second;        
        bool bCOLL = spMoneyDrop->IsCollision(vFromPt, vTargetPt);
        if (bCOLL)
        {
            //	Note : 검출된 타깃 등록.
            STARGETID findTargetID;
            findTargetID.emCrow = CROW_MONEY;
            findTargetID.GaeaId = spMoneyDrop->dwGlobID;
            findTargetID.vPos   = spMoneyDrop->vPos;
            vDetect.push_back(findTargetID);
        }
    }
}

void GLLandManClient::GetSpherePointCollisionDropMoney(
    const D3DXVECTOR3& vTarPos,
    float fLength,
    std::vector<STARGETID>& vDetect) const
{
    for (DROP_MONEY_MAP_CITER iter=m_DropMoney.begin(); iter!=m_DropMoney.end(); ++iter)
    {
        std::tr1::shared_ptr<CMoneyClientDrop> spMoneyDrop = iter->second;
        BOOL bCol = COLLISION::IsSpherePointCollision(spMoneyDrop->vPos, vTarPos, fLength);
        if (bCol)
        {
            //	Note : 검출된 타깃 등록.
            STARGETID findTargetID;
            findTargetID.emCrow = CROW_MONEY;
            findTargetID.GaeaId = spMoneyDrop->dwGlobID;
            findTargetID.vPos   = spMoneyDrop->vPos;
            vDetect.push_back(findTargetID);
        }
    }
}

void GLLandManClient::GetNearDropMoney(
    float& fMinDist,
    D3DXVECTOR3& vMinPos,
    float& fMinDistNotCan,
    D3DXVECTOR3& vMinPosNotCan,
    bool& bFind,
    const D3DXVECTOR3& vCharPos) const
{
    for (DROP_MONEY_MAP_CITER iter=m_DropMoney.begin(); iter!=m_DropMoney.end(); ++iter)
    {
        std::tr1::shared_ptr<CMoneyClientDrop> spMoneyDrop = iter->second;
        if (spMoneyDrop->bCanGet)
        {	
            // 주울 수 있는 아이템에 대해서만 하자.
            float fCurDist = D3DXVec3Length(&(spMoneyDrop->vPos - vCharPos));
            if (fMinDist > fCurDist)
            {
                fMinDist = fCurDist;
                vMinPos = spMoneyDrop->vPos;
                bFind = true;
            }
        }

        // 주울 수 없더라도 제일 가까운거 체크 해 놓는다.
        float fCurDist = D3DXVec3Length(&(spMoneyDrop->vPos - vCharPos));
        if (fMinDistNotCan > fCurDist)
        {
            fMinDistNotCan = fCurDist;
            vMinPosNotCan = spMoneyDrop->vPos;
        }        
    }
}

void GLLandManClient::GetCollisionLineToSphereDropMaterial(
    VEC_SK_TAR& vecDetectID,
    const D3DXVECTOR3& vLineDir,
    const D3DXVECTOR3& vP0,
    const D3DXVECTOR3& vP1,
    const D3DXVECTOR3& vFromPt,
    const D3DXVECTOR3& vTargetPt,
    float fRadius) const
{
    for (DROP_MATERIAL_MAP_CITER iter=m_DropMaterial.begin(); iter!=m_DropMaterial.end(); ++iter)
    {
        std::tr1::shared_ptr<GLMaterialClient> spMaterial= iter->second;
        if (!spMaterial->IsValidBody())
            continue;

        if (D3DXVec3Dot(&vLineDir, &(spMaterial->GetPosition() - vP0)) > 0 &&
            D3DXVec3Dot(&vLineDir, &(spMaterial->GetPosition() - vP1)) < 0)
        {
            if (COLLISION::IsCollisionLineToSphere(vFromPt, vTargetPt, spMaterial->GetPosition(), fRadius))
            {
                STARGETID findTargetID;
                findTargetID.emCrow	= spMaterial->m_pCrowData->m_emCrow;
                findTargetID.GaeaId	= spMaterial->GetGaeaID();
                findTargetID.vPos	= spMaterial->GetPosition();
                vecDetectID.push_back(findTargetID);
            }
        }
    }
}

void GLLandManClient::GetCollisionLineToAabbDropMaterial(
    VEC_SK_TAR& vecDetectID,
    const D3DXVECTOR3& vFromPt,
    const D3DXVECTOR3& vTargetPt,
    bool CheckReActionAble,
    GLCharacter* pChar,
    BOOL bEnemy) const
{
    for (DROP_MATERIAL_MAP_CITER iter=m_DropMaterial.begin(); iter!=m_DropMaterial.end(); ++iter)
    {
        std::tr1::shared_ptr<GLMaterialClient> spMaterial= iter->second;
        if (!spMaterial->IsValidBody())
            continue;

        BOOL bCol = COLLISION::IsCollisionLineToAABB(vFromPt, vTargetPt, spMaterial->m_vMax, spMaterial->m_vMin);
        if (bCol)
        {
            //	Note : 검출된 타깃 등록.
            STARGETID findTargetID;
            findTargetID.emCrow = spMaterial->m_pCrowData->m_emCrow;
            findTargetID.GaeaId = spMaterial->GetGaeaID();
            findTargetID.vPos = spMaterial->GetPosition();

            if (CheckReActionAble)
            {
                if (pChar && pChar->IsReActionable(findTargetID, bEnemy ? EMIMPACT_SIDE_ENEMY : EMIMPACT_SIDE_OUR))
                    vecDetectID.push_back(findTargetID);
            }
            else
            {
                vecDetectID.push_back(findTargetID);
            }
        }
    }
}

void GLLandManClient::GetSpherePointCollisionDropMaterial(
    VEC_SK_TAR& vecDetectID,
    const D3DXVECTOR3& vTarPos,
    float fLength,
    bool CheckValidBody) const
{
    for (DROP_MATERIAL_MAP_CITER iter=m_DropMaterial.begin(); iter!=m_DropMaterial.end(); ++iter)
    {
        std::tr1::shared_ptr<GLMaterialClient> spMaterial= iter->second;
        if (!spMaterial)
            continue;

        if (CheckValidBody && !spMaterial->IsValidBody())
            continue;

        float fTarRange = spMaterial->GetBodyRadius() + fLength;
        BOOL bCol = COLLISION::IsSpherePointCollision(spMaterial->GetPosition(), vTarPos, fTarRange);
        if (bCol)
        {
            //	Note : 검출된 타깃 등록.
            STARGETID findTargetID;
            findTargetID.emCrow = spMaterial->m_pCrowData->m_emCrow;
            findTargetID.GaeaId = spMaterial->GetGaeaID();
            findTargetID.vPos = spMaterial->GetPosition();
            vecDetectID.push_back(findTargetID);
        }
    }
}

void GLLandManClient::GetCollisionLineToSphereDropSummon(
    VEC_SK_TAR& vecDetectID,
    const D3DXVECTOR3& vLineDir,
    const D3DXVECTOR3& vP0,
    const D3DXVECTOR3& vP1,
    const D3DXVECTOR3& vFromPt,
    const D3DXVECTOR3& vTargetPt,
    float fRadius) const
{
    for (DROP_SUMMON_MAP_CITER iter=m_DropSummon.begin(); iter!=m_DropSummon.end(); ++iter)
    {
        std::tr1::shared_ptr<GLSummonClient> spSummon = iter->second;
        if (!spSummon->IsValidBody())
            continue;

		if (spSummon->IsNonTargetable())
			continue;

        if (D3DXVec3Dot(&vLineDir, &(spSummon->GetPosition() - vP0)) > 0 &&
            D3DXVec3Dot(&vLineDir, &(spSummon->GetPosition() - vP1)) < 0)
        {
            if (COLLISION::IsCollisionLineToSphere(vFromPt, vTargetPt, spSummon->GetPosition(), fRadius))
            {
                STARGETID findTargetID;
                findTargetID.emCrow	= CROW_SUMMON;
                findTargetID.GaeaId	= spSummon->GetGaeaID();
                findTargetID.vPos	= spSummon->GetPosition();
                vecDetectID.push_back(findTargetID);
            }
        }
    }
}

void GLLandManClient::GetCollisionLineToAabbDropSummon(
    VEC_SK_TAR& vecDetectID,
    const D3DXVECTOR3& vFromPt,
    const D3DXVECTOR3& vTargetPt,
    bool CheckReActionAble,
    GLCharacter* pChar,
    BOOL bEnemy) const
{
    for (DROP_SUMMON_MAP_CITER iter=m_DropSummon.begin(); iter!=m_DropSummon.end(); ++iter)
    {
        std::tr1::shared_ptr<GLSummonClient> spSummon = iter->second;        
        if (!spSummon->IsValidBody())
            continue;

		if (spSummon->IsNonTargetable())
			continue;

        BOOL bCol = COLLISION::IsCollisionLineToAABB(vFromPt, vTargetPt, spSummon->m_vMax, spSummon->m_vMin);
        if (bCol)
        {
            //	Note : 검출된 타깃 등록
            STARGETID findTargetID;
            findTargetID.emCrow = CROW_SUMMON;
            findTargetID.GaeaId = spSummon->GetGaeaID();
            findTargetID.vPos   = spSummon->GetPosition();

            if (CheckReActionAble)
            {
                if (!pChar)
                    continue;
                if (pChar->IsReActionable(findTargetID, bEnemy ? EMIMPACT_SIDE_ENEMY : EMIMPACT_SIDE_OUR))
                    vecDetectID.push_back(findTargetID);
            }
            else
            {
                vecDetectID.push_back(findTargetID);
            }
        }
    }
}

void GLLandManClient::GetSpherePointCollisionDropSummon(
    const D3DXVECTOR3& vTarPos,
    float fLength,
    std::vector<STARGETID>& vDetect,
    bool CheckValidBody) const
{
    for (DROP_SUMMON_MAP_CITER iter=m_DropSummon.begin(); iter!=m_DropSummon.end(); ++iter)
    {
        std::tr1::shared_ptr<GLSummonClient> spSummon = iter->second;

        if (CheckValidBody && !spSummon->IsValidBody())
            continue;

		if (spSummon->IsNonTargetable())
			continue;

        float fTarRange = spSummon->GetBodyRadius() + fLength;

        BOOL bCol = COLLISION::IsSpherePointCollision(spSummon->GetPosition(), vTarPos, fTarRange);
        if (bCol)
        {
            //	Note : 검출된 타깃 등록
            STARGETID findTargetID;
            findTargetID.emCrow = CROW_SUMMON;
            findTargetID.GaeaId = spSummon->GetGaeaID();
            findTargetID.vPos   = spSummon->GetPosition();

            vDetect.push_back(findTargetID);
        }
    }
}

bool GLLandManClient::FindStorageOpenNpc(
    const D3DXVECTOR3& vPos,
    GLCharacter* pChar,
    DWORD nPartyMember,
    DWORD& nNpcID,
    DWORD& nTalkID)
{
	if ( !m_pGaeaClient )
		return false;

	CTime cCurSvrTime = m_pGaeaClient->GetCurrentTime();
    for (DROP_CROW_MAP_ITER iter=m_DropCrow.begin(); iter!=m_DropCrow.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCrowClient> spCrow= iter->second;
        if (spCrow->GetCrow() != CROW_NPC)
            continue;
        if (!spCrow->IsValidBody())
            continue;
        if (!spCrow->IsVisibleDetect())
            continue;

        const SCROWDATA* pData = spCrow->m_pCrowData;
        if (!pData)
            continue;

        float fDistTalkable = spCrow->GETBODYRADIUS() + pChar->GETBODYRADIUS() + spCrow->GET_INTERACTION_RANGE() + 2.0f;

        BOOL bCol = COLLISION::IsSpherePointCollision(spCrow->GetPosition(), vPos, fDistTalkable);
        if (!bCol)
            continue;

        CNpcTalkControl::NPCMAP* pTalkMap = pChar->FindDefaultTalk(pData->GetTalkFile());
        if (!pTalkMap)
            continue;

        BOOST_FOREACH(CNpcTalkControl::NPCMAP_VALUE& it, *pTalkMap)
        {
            std::tr1::shared_ptr< SNpcTalk > spTalk = it.second;
            if (!spTalk)
                continue;

            if (spTalk->m_nACTION != SNpcTalk::EM_BASIC)
                continue;

            if (spTalk->m_dwACTION_NO != SNpcTalk::EM_STORAGE)
                continue;

            if (!spTalk->DoTEST(pChar, nPartyMember, cCurSvrTime.GetHour()))
                continue;

            nNpcID = spCrow->GetGaeaID();
            nTalkID = spTalk->m_dwGLOB_ID;

            return true;
        }
    }

    return false;
}

void GLLandManClient::DetectCrowDie(
    VEC_SK_TAR& vecDetectID,
    const D3DXVECTOR3& vFromPt,
    const D3DXVECTOR3& vTargetPt)
{    
    for (DROP_CROW_MAP_ITER iter=m_DropCrow.begin(); iter!=m_DropCrow.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCrowClient> spCrow= iter->second;
        if ( !spCrow->IsDie() )
            continue;
        if (!spCrow->IsVisibleDetect())
            continue;

        BOOL bCol = COLLISION::IsCollisionLineToAABB(vFromPt, vTargetPt, spCrow->m_vMax, spCrow->m_vMin);
        if (bCol)
        {
            // 검출된 타깃 등록
            STARGETID findTargetID;
            findTargetID.emCrow = spCrow->m_pCrowData->m_emCrow;
            findTargetID.GaeaId = spCrow->GetGaeaID();
            findTargetID.vPos   = spCrow->GetPosition ();

            vecDetectID.push_back(findTargetID);
        }
    }
}

void GLLandManClient::DetectCrowWithCylinder(
    VEC_SK_TAR& vecDetectID,
    const D3DXVECTOR3& vLineDir,
    const D3DXVECTOR3& vP0,
    const D3DXVECTOR3& vP1,
    float fRadius)
{
    for (DROP_CROW_MAP_ITER iter=m_DropCrow.begin(); iter!=m_DropCrow.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCrowClient> spCrow= iter->second;
        if (!spCrow->IsVisibleDetect() || !spCrow->IsValidBody())
            continue;

        float fDot0 = D3DXVec3Dot(&vLineDir, &(spCrow->GetPosition() - vP0));
        float fDot1 = D3DXVec3Dot(&vLineDir, &(spCrow->GetPosition() - vP1));
        if (fDot0 >= 0.0f && fDot1 <= 0.0f)
        {
            if (COLLISION::IsCollisionLineToSphere(vP0, vP1, spCrow->GetPosition(), fRadius))
            { 
                STARGETID findTargetID;
                findTargetID.emCrow	= spCrow->m_pCrowData->m_emCrow;
                findTargetID.GaeaId	= spCrow->GetGaeaID();
                findTargetID.vPos	= spCrow->GetPosition();

                vecDetectID.push_back(findTargetID);
            }
        }
    }
}

void GLLandManClient::GetCollisionLineToAabbDropCrow(
    VEC_SK_TAR& vecDetectID,
    const D3DXVECTOR3& vFromPt,
    const D3DXVECTOR3& vTargetPt,
    bool CheckValidBody,
    bool CheckVisibleDetect,
    bool CheckDie,
    bool CheckReactionable,
    GLCharacter* pChar,
    BOOL bEnemy)
{
    for (DROP_CROW_MAP_ITER iter=m_DropCrow.begin(); iter!=m_DropCrow.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCrowClient> spCrow= iter->second;
        if (CheckDie && !spCrow->IsDie() )
            continue;

        if (CheckVisibleDetect && !spCrow->IsVisibleDetect())
            continue;

        if (CheckValidBody && !spCrow->IsValidBody())
            continue;

        BOOL bCol = COLLISION::IsCollisionLineToAABB(vFromPt, vTargetPt, spCrow->m_vMax, spCrow->m_vMin);
        if (bCol)
        {
            STARGETID findTargetID;
            findTargetID.emCrow = spCrow->m_pCrowData->m_emCrow;
            findTargetID.GaeaId = spCrow->GetGaeaID();
            findTargetID.vPos = spCrow->GetPosition();

            if (CheckReactionable)
            {
                if (pChar && pChar->IsReActionable(findTargetID, bEnemy ? EMIMPACT_SIDE_ENEMY : EMIMPACT_SIDE_OUR))
                    vecDetectID.push_back(findTargetID);
            }
            else
            {
                vecDetectID.push_back(findTargetID);
            }
        }
    }
}

void GLLandManClient::GetSpherePointCollisionDropCrow(
    VEC_SK_TAR& vecDetectID,
    const D3DXVECTOR3& vTarPos,
    float fLength,
    bool CheckValidBody)
{
    for (DROP_CROW_MAP_ITER iter=m_DropCrow.begin(); iter!=m_DropCrow.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCrowClient> spCrow= iter->second;
        if (CheckValidBody)
        {
            if (!spCrow->IsValidBody() || !spCrow->IsVisibleDetect())
                continue;
        }

        float fTarRange = spCrow->GETBODYRADIUS() + fLength;

        BOOL bCol = COLLISION::IsSpherePointCollision(spCrow->GetPosition(), vTarPos, fTarRange);
        if (bCol)
        {
            STARGETID findTargetID;
            findTargetID.emCrow = spCrow->m_pCrowData->m_emCrow;
            findTargetID.GaeaId = spCrow->GetGaeaID();
            findTargetID.vPos   = spCrow->GetPosition();

            vecDetectID.push_back(findTargetID);
        }
    }
}

BOOL GLLandManClient::GetCollisionMobToPoint(const D3DXVECTOR3& vPoint, WORD wBodyRadius)
{
    for (DROP_CROW_MAP_ITER iter=m_DropCrow.begin(); iter!=m_DropCrow.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCrowClient> spCrow= iter->second;
        if (!spCrow->IsValidBody())
            continue;
        if (!spCrow->IsHaveVisibleBody())
            continue;

        D3DXVECTOR3 vDist(spCrow->GetPosition() - vPoint);
        if (DxIsZeroVector(vDist))
            return TRUE;

        // vDist.y = 0.0f;
        float fDist = D3DXVec3Length(&vDist);
        if (fDist < (wBodyRadius + spCrow->GETBODYRADIUS()))
            return TRUE;
    }
    return FALSE;
}

std::tr1::shared_ptr<GLCrowClient> GLLandManClient::GetQuestNpc(const GLQUEST_STEP& step)
{    
    for (DROP_CROW_MAP_ITER iter=m_DropCrow.begin(); iter!=m_DropCrow.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCrowClient> spCrow= iter->second;        
        if (spCrow->m_sNativeID == step.m_dwNID_NPCTALK)
            return spCrow;
    }
    return std::tr1::shared_ptr<GLCrowClient> ((GLCrowClient*) 0);
}

void GLLandManClient::GetCollisionLineToSphereDropSummon(
    VEC_SK_TAR& vecDetectID,
    const D3DXVECTOR3& vLineDir,
    const D3DXVECTOR3& vP0,
    const D3DXVECTOR3& vP1,
    const D3DXVECTOR3& vFromPt,
    const D3DXVECTOR3& vTargetPt,
    float fRadius,
    bool CheckValidBody)
{
    for (DROP_SUMMON_MAP_ITER iter=m_DropSummon.begin(); iter!=m_DropSummon.end(); ++iter)
    {
        std::tr1::shared_ptr<GLSummonClient> spSummon = iter->second;
        if (CheckValidBody && !spSummon->IsValidBody())
            continue;

		if (spSummon->IsNonTargetable())
			continue;

        if (D3DXVec3Dot(&vLineDir, &(spSummon->GetPosition() - vP0)) > 0 &&
            D3DXVec3Dot(&vLineDir, &(spSummon->GetPosition() - vP1)) < 0)
        {
            if (COLLISION::IsCollisionLineToSphere(vFromPt, vTargetPt, spSummon->GetPosition(), fRadius))
            {
                STARGETID findTargetID;
                findTargetID.emCrow	= CROW_SUMMON;
                findTargetID.GaeaId	= spSummon->GetGaeaID();
                findTargetID.vPos	= spSummon->GetPosition();
                vecDetectID.push_back(findTargetID);
            }
        }
    }
}

void GLLandManClient::GetCollisionLineToAabbDropSummon(
    VEC_SK_TAR& vecDetectID,
    const D3DXVECTOR3& vFromPt,
    const D3DXVECTOR3& vTargetPt,
    bool CheckValidBody,
    bool CheckReactionable,
    GLCharacter* pChar,
    BOOL bEnemy)
{
    for (DROP_SUMMON_MAP_ITER iter=m_DropSummon.begin(); iter!=m_DropSummon.end(); ++iter)
    {
        std::tr1::shared_ptr<GLSummonClient> spSummon = iter->second;
        if (CheckValidBody && !spSummon->IsValidBody())
            continue;

		if (spSummon->IsNonTargetable())
			continue;

        BOOL bCol = COLLISION::IsCollisionLineToAABB(vFromPt, vTargetPt, spSummon->m_vMax, spSummon->m_vMin);
        if (bCol)
        {
            STARGETID findTargetID;
            findTargetID.emCrow = CROW_SUMMON;
            findTargetID.GaeaId = spSummon->GetGaeaID();
            findTargetID.vPos   = spSummon->GetPosition();

            if (CheckReactionable)
            {
                if (pChar && pChar->IsReActionable(findTargetID, bEnemy ? EMIMPACT_SIDE_ENEMY : EMIMPACT_SIDE_OUR))
                    vecDetectID.push_back(findTargetID);
            }
            else
            {
                vecDetectID.push_back(findTargetID);
            }
        }
    }
}

void GLLandManClient::GetSpherePointCollisionDropSummon(
    VEC_SK_TAR& vecDetectID,
    const D3DXVECTOR3& vTarPos,
    float fLength,
    bool CheckValidBody)
{
    for (DROP_SUMMON_MAP_ITER iter=m_DropSummon.begin(); iter!=m_DropSummon.end(); ++iter)
    {
        std::tr1::shared_ptr<GLSummonClient> spSummon = iter->second;
        if (CheckValidBody && !spSummon->IsValidBody())
            continue;

		if (spSummon->IsNonTargetable())
			continue;

        float fTarRange = spSummon->GetBodyRadius() + fLength;

        BOOL bCol = COLLISION::IsSpherePointCollision(spSummon->GetPosition(), vTarPos, fTarRange );
        if (bCol)
        {
            STARGETID findTargetID;
            findTargetID.emCrow = CROW_SUMMON;
            findTargetID.GaeaId = spSummon->GetGaeaID();
            findTargetID.vPos   = spSummon->GetPosition();
            vecDetectID.push_back(findTargetID);
        }
    }
}

void GLLandManClient::PrintChar()
{
    for (CHARMAPID_ITER iter=m_PCArray.begin(); iter!=m_PCArray.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCharClient> spChar = iter->second;
        m_pGaeaClient->PrintConsoleText(
            sc::string::format(
                "%1% : charid = %2%",
                spChar->m_strName,
                spChar->GetCharData().m_Base.dwCharID));
    }
}

void GLLandManClient::GetCollisionLineToAabbChar(
    VEC_SK_TAR& vecDetectID,
    const D3DXVECTOR3& vFromPt,
    const D3DXVECTOR3& vTargetPt,
    bool CheckDie,
    bool CheckVisible,
    bool CheckValidBody,
    bool CheckReActionAble,
    GLCharacter* pChar,
    BOOL bEnemy)
{
    for (CHARMAPID_ITER iter=m_PCArray.begin(); iter!=m_PCArray.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCharClient> spChar = iter->second;
        //  [1/7/2013 gbgim]
		// - 아래로 인해 죽었을 때 클릭해도 타게팅 지정이 안되던 문제가 있어서 주석처리함
		//if (CheckDie && !spChar->IsDie() )
        //    continue;
        if (CheckVisible && !spChar->IsVisibleDetect())
            continue;
        //if (CheckValidBody && !spChar->IsValidBody())
        //    continue;

        BOOL bCol = COLLISION::IsCollisionLineToAABB(vFromPt, vTargetPt, spChar->m_vMax, spChar->m_vMin);
        if (bCol)
        {
            STARGETID findTargetID;
            findTargetID.emCrow = CROW_PC;
            findTargetID.GaeaId = spChar->GetGaeaID();
            findTargetID.vPos   = spChar->GetPosition();
            if (CheckReActionAble)
            {
                if (pChar && pChar->IsReActionable(findTargetID, bEnemy ? EMIMPACT_SIDE_ENEMY : EMIMPACT_SIDE_OUR))
                    vecDetectID.push_back(findTargetID);
            }
            else
            {
                vecDetectID.push_back(findTargetID);
            }
        }
    }
}

void GLLandManClient::GetCollisionLineToSphereChar(
    VEC_SK_TAR& vecDetectID,
    const D3DXVECTOR3& vLineDir,
    const D3DXVECTOR3& vP0,
    const D3DXVECTOR3& vP1,
    const D3DXVECTOR3& vFromPt,
    const D3DXVECTOR3& vTargetPt,
    float fRadius,
    bool CheckVisible,
    bool CheckValidBody)
{
    for (CHARMAPID_ITER iter=m_PCArray.begin(); iter!=m_PCArray.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCharClient> spChar = iter->second;
        if (CheckVisible && !spChar->IsVisibleDetect())
            continue;
        if (CheckValidBody && !spChar->IsValidBody())
            continue;

        if (D3DXVec3Dot(&vLineDir, &(spChar->GetPosition() - vP0)) > 0 &&
            D3DXVec3Dot(&vLineDir, &(spChar->GetPosition() - vP1)) < 0)
        {
            if (COLLISION::IsCollisionLineToSphere(vFromPt, vTargetPt, spChar->GetPosition(), fRadius))
            {
                STARGETID findTargetID;
                findTargetID.emCrow	= CROW_PC;
                findTargetID.GaeaId	= spChar->GetGaeaID();
                findTargetID.vPos	= spChar->GetPosition();
                vecDetectID.push_back(findTargetID);
            }
        }
    }
}

void GLLandManClient::GetSpherePointCollisionChar(
    VEC_SK_TAR& vecDetectID,
    const D3DXVECTOR3& vTarPos,
    float fLength,
    bool CheckVisible,
    bool CheckValidBody)
{
    for (CHARMAPID_ITER iter=m_PCArray.begin(); iter!=m_PCArray.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCharClient> spChar = iter->second;
        if (CheckVisible && !spChar->IsVisibleDetect())
            continue;

        if (CheckValidBody && !spChar->IsValidBody())
            continue;

        BOOL bCol = COLLISION::IsSpherePointCollision(spChar->GetPosition(), vTarPos, fLength);
        if (bCol)
        {
            STARGETID findTargetID;
            findTargetID.emCrow = CROW_PC;
            findTargetID.GaeaId = spChar->GetGaeaID();
            findTargetID.vPos   = spChar->GetPosition();
            vecDetectID.push_back(findTargetID);
        }
    }
}

BOOL GLLandManClient::GetUsedMaterialSystem() const
{
	return m_spLandMan->GetUsedMaterialSystem();
}

// TAG_PVEClientStartStage_2
void GLLandManClient::StartTriggerFinger(int nStage, int nLevel)
{
    if (nLevel < 0)
    {
        return;
    }

    if (m_spTriggerFinger == 0)
    {
        m_spTriggerFinger = TriggerSystem::CreateClientFinger(m_pGaeaClient, nStage, nLevel);
    }

    if (m_spTriggerFinger)
    {
        m_spTriggerFinger->Start();
    }
}

void GLLandManClient::SendTriggerMessage(TriggerSystem::SMessage* msg)
{
    if (m_spTriggerFinger && msg)
    {
        m_spTriggerFinger->SendMessage(msg);
    }
}

void GLLandManClient::PostTriggerMessage(TriggerSystem::SpMessage msg)
{
    if (m_spTriggerFinger && msg)
    {
        m_spTriggerFinger->PostMessage(msg);
    }	
}

DxLandMan* GLLandManClient::GetLandMan()
{ 
	return m_spLandMan.get(); 
}

boost::shared_ptr<NavigationMesh> GLLandManClient::GetNaviMesh()
{ 
	return m_spLandMan->GetNaviMesh(); 
}

BOOL GLLandManClient::IsObstacle(  D3DXVECTOR3 vThisPos, D3DXVECTOR3 vPreviousPos  )
{
	BOOL bMOVE = FALSE;
	D3DXVECTOR3 vDist;
	D3DXVECTOR3 vDistPrevious;
	BOOL bInObstaclePrevious = FALSE;
	BOOL bInObstacle = FALSE;

	BOOST_FOREACH(DROP_SUMMON_MAP_VALUE  &iter, m_DropSummon)
	{
		std::tr1::shared_ptr<GLSummonClient>  pSummonField = iter.second;
		if (!pSummonField->IsValidBody())
			continue;

		if (!pSummonField)
			continue;

		if (!pSummonField->IsValidBody())
			continue;

		PCROWDATA pSummonCrowData = pSummonField->m_pSummonCrowData;

		if (pSummonCrowData && !pSummonCrowData->IsBarrier())
			continue;

		vDist = pSummonField->GetPosition() - vThisPos;
		vDistPrevious = pSummonField->GetPosition() - vPreviousPos;

		bInObstaclePrevious = IsInObstacle( pSummonCrowData->m_fBarrierRadius, 
			pSummonCrowData->m_fBarrierWidth, 
			D3DXVec3Length(&vDistPrevious) );

		bInObstacle = IsInObstacle( pSummonCrowData->m_fBarrierRadius, 
			pSummonCrowData->m_fBarrierWidth, 
			D3DXVec3Length(&vDist) );

		//BOOL bIsInNew, bIsInOld = FALSE;
		//bIsInOld = IsIn( pSummonCrowData->m_fBarrierRadius, D3DXVec3Length(&vDistPrevious) );
		//bIsInNew = IsIn( pSummonCrowData->m_fBarrierRadius, D3DXVec3Length(&vDist) );

		if ( bInObstacle )
			bMOVE= TRUE;
	}

	BOOST_FOREACH(DROP_CROW_MAP_VALUE  &iter, m_DropCrow)
	{
		std::tr1::shared_ptr<GLCrowClient>  pCrow = iter.second;

		if (!pCrow)
			continue;

		if (!pCrow->IsValidBody())
			continue;

		PCROWDATA pCrowData = pCrow->m_pCrowData;

		if (pCrowData && !pCrow->m_pCrowData->IsBarrier())
			continue;

		vDist = pCrow->GetPosition() - vThisPos;
		vDistPrevious = pCrow->GetPosition() - vPreviousPos;

		bInObstaclePrevious = IsInObstacle( pCrowData->m_fBarrierRadius, 
			pCrowData->m_fBarrierWidth, 
			D3DXVec3Length(&vDistPrevious) );

		bInObstacle = IsInObstacle( pCrowData->m_fBarrierRadius, 
			pCrowData->m_fBarrierWidth, 
			D3DXVec3Length(&vDist) );

		//BOOL bIsInNew, bIsInOld = FALSE;
		//bIsInOld = IsIn( pCrowData->m_fBarrierRadius, D3DXVec3Length(&vDistPrevious) );
		//bIsInNew = IsIn( pCrowData->m_fBarrierRadius, D3DXVec3Length(&vDist) );

		if ( bInObstacle )
			bMOVE= TRUE;

	}
	return bMOVE;
}

BOOL GLLandManClient::IsInObstacle( float fRadius, float fWidth, float fLength )
{
	if (fLength > fRadius - fWidth && fLength < fRadius)
	{
		return TRUE;
	}
	return FALSE;
}

void GLLandManClient::SetObstacleMat( D3DMATRIX mat )
{
	m_mat = mat;
}

D3DMATRIX GLLandManClient::GetObstacleMat()
{
	return m_mat;
}

void GLLandManClient::SetObstacleVec( D3DXVECTOR3 vec )
{
	m_vVector = vec;
}

D3DXVECTOR3 GLLandManClient::GetObstacleVec()
{
	return m_vVector;
}

BOOL GLLandManClient::IsIn( float fRadius, float fLength )
{
	return fRadius > fLength;
}

// 난사군도 관련 국기 변경 함수
void GLLandManClient::SetWorldBattleFlag( const TCHAR* pWorldBattleFlag )
{
	if ( pWorldBattleFlag )
		NSGameLogicInfo::g_strWinnerCountryWB = pWorldBattleFlag;
	else
		NSGameLogicInfo::g_strWinnerCountryWB = _T("");

	GLLevelFile::SetWorldBattleFlag_LandEff( pWorldBattleFlag );
	m_spLandMan->SetWorldBattleFlag( pWorldBattleFlag );

	// 만약 승리 국가 국기를 NPC 에 셋팅하거나. Land(Effect(Skin)) 에도 작업되어야 하면 변경이 필요하다.
	// 처음에 렌더링 될 때는 NSGameLogicInfo::g_strWinnerCountryWB 를 사용해서 잘 나올 수 있지만,
	// 국기가 변경되는 GLGaeaClient::MsgVCountryAddFAC 에서 호출이 된다면. 위에 말한 것들이 변경이 안된다.
	// 그래서 추가가 필요해진다.
}


/*HRESULT GLLandManClient::Render_MobItem ( CLIPVOLUME &CV )
{
	++m_dwCOUNT;
	
	LPDXFRAME pDxFrame;
	D3DXVECTOR3 vCOLL(0.f,0.f,0.f);

	GLCharacter* const pMyCharacter = m_pGaeaClient->GetCharacter();
	const DWORD dwMyGaeaID = pMyCharacter->GetGaeaID();
	D3DXVECTOR3 vMyPosition(pMyCharacter->GetPosition());
	D3DXVECTOR3 vViewPosition(DxViewPort::GetInstance().GetFromPt());
	m_pInterface->NameDisplaySetCount(m_dwCOUNT);

	//	Note : 이름 목록에 자기 자신 추가.
	{
		CROWREN sDISP_NAME;
		sDISP_NAME.INIT(pMyCharacter);
		sDISP_NAME.SETTYPEFLAG(m_pGaeaClient, pMyCharacter);
		m_pInterface->NameDisplayAddName(sDISP_NAME);

		if (pMyCharacter->m_bItemShopOpen)
			m_pInterface->ItemShopSetRender(dwMyGaeaID);
	}

	// 이름목록에 자신의 팻이름 추가
	GLPetClient* pMYPET = m_pGaeaClient->GetPetClient ();
	if (pMYPET->IsVALID())
	{
		if (COLLISION::IsCollisionVolume(CV,pMYPET->m_vMax,pMYPET->m_vMin))
		{
			D3DXVECTOR3 vPOS(pMYPET->GetPosition() + D3DXVECTOR3(0.0f, 8.0f, 0.0f));
			BOOL bCOLL(FALSE);

			if ( GLOBAL_PARAM::IsValue( GP_LINE_CULLING ) )
			{
				m_spLandMan->IsCollision(vViewPosition, vPOS, vCOLL, bCOLL, pDxFrame, EMCC_CULL_NONE );
			}

			if (!bCOLL && GLOBAL_PARAM::IsValue( GP_DISPLAY_NAME ))
			{
				CROWREN sDISP_MYPETNAME;
				sDISP_MYPETNAME.INIT ( pMYPET );
				sDISP_MYPETNAME.SETTYPEFLAG ( pMyCharacter, pMYPET );
				m_pInterface->NameDisplayAddName(sDISP_MYPETNAME, pMyCharacter->GetName());
			}
		}
	}

    for (CHARMAPID_ITER iter=m_PCArray.begin(); iter!=m_PCArray.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCharClient> pChar = iter->second;
		if ( pChar == NULL )
            continue; // 유효하지 않은 케릭터면 실패;

		if ( pChar->IsVisibleDetect() == FALSE )
			continue; // 은신 중이고 감지하지 못하면 실패;
		// 오브젝트에 의한 가려짐도 IsVisibleDetect()함수에서 처리 되고 있음;

		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
		{
			pChar->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL1(), GetUsedMaterialSystem(), 0 );
			pChar->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL2(), GetUsedMaterialSystem(), 1 );
		}

		if ( COLLISION::IsCollisionVolume(CV,pChar->m_vMax,pChar->m_vMin) == FALSE )
			continue; // 컬링에 의해 보이지 않는 상태이면 패스;		

		if ( (GLCONST_CHAR::bBATTLEROYAL == FALSE) || (pChar->IsDie() == false) )
		{ // 배틀로얄 중이 아니거나, 케릭터가 살아있다면 출력;	
			BOOL bCOLL(FALSE);

			if ( GLOBAL_PARAM::IsValue( GP_LINE_CULLING ) )
			{
				m_spLandMan->IsCollision(vViewPosition, pChar->GetPosition(), vCOLL, bCOLL, pDxFrame, EMCC_CULL_NONE);
			}

			if ( bCOLL == FALSE && GLOBAL_PARAM::IsValue( GP_DISPLAY_NAME ) && pChar->IsHideNaming() == false)
			{
				CROWREN sDISP_NAME;
				sDISP_NAME.INIT(pChar);
				sDISP_NAME.SETTYPEFLAG(m_pGaeaClient, pMyCharacter, pChar);
				m_pInterface->NameDisplayAddName(sDISP_NAME);
				if ( pChar->m_bItemShopOpen )
					m_pInterface->ItemShopSetRender(pChar->GetGaeaID());
			}			
		}		

		pChar->Render(m_pd3dDevice, CV, FALSE);
		pChar->RenderShadow(m_pd3dDevice, CV, GetUsedMaterialSystem() );
	}

	// SUMMON
	// 소환수들의 Render 처리
	m_pInterface->SummonSetCount(m_dwCOUNT);

    for (DROP_SUMMON_MAP_ITER iter=m_DropSummon.begin(); iter!=m_DropSummon.end(); ++iter)
    {
        std::tr1::shared_ptr<GLSummonClient> spSummon = iter->second;
		// Note : DIE 일때는 이펙트만 그린다.
		if ( spSummon->isAction(GLAT_DIE) )
		{
			spSummon->RenderAniEff(m_pd3dDevice);
			continue;
		}

		if ( spSummon->IsVisibleDetect() == FALSE )
			continue;

		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
		{
			spSummon->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL1(), GetUsedMaterialSystem(), 0 );
			spSummon->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL2(), GetUsedMaterialSystem(), 1 );
		}

		if (spSummon && COLLISION::IsCollisionVolume(CV, spSummon->m_vMax, spSummon->m_vMin))
		{
			D3DXVECTOR3 vPOS(spSummon->GetPosition() + D3DXVECTOR3(0.0f, 8.0f, 0.0f));
			BOOL bCOLL(FALSE);

			if ( GLOBAL_PARAM::IsValue( GP_LINE_CULLING ) )
			{
				m_spLandMan->IsCollision(vViewPosition, vPOS, vCOLL, bCOLL, pDxFrame, EMCC_CULL_NONE );
			}

			if (!bCOLL && GLOBAL_PARAM::IsValue( GP_DISPLAY_NAME ) && spSummon->IsHideNaming() == false)
			{		
				// Note : 내 소환수만 처리한다.
				if (m_pGaeaClient->GETMYGAEAID() == spSummon->m_dwOwnerGaeaID)
				{
					m_pInterface->SummonAddDisp(spSummon->GetGaeaID());
				}
				else
				{
					CROWREN sDISP_NAME;
					sDISP_NAME.INIT(spSummon);
					sDISP_NAME.SETTYPEFLAG(m_pGaeaClient, pMyCharacter, spSummon);
					std::tr1::shared_ptr<GLCharClient> pCHAR = m_pGaeaClient->GetChar(spSummon->m_dwOwnerGaeaID);
					if (pCHAR)
						m_pInterface->NameDisplayAddName(sDISP_NAME, pCHAR->GetName());
				}
			}
		}

		spSummon->Render(m_pd3dDevice);
		spSummon->RenderShadow(m_pd3dDevice, CV, GetUsedMaterialSystem());
	}

	// PET
	// 여기서 다른 팻들의 Render 처리;
	for (PETMAPID_ITER iter=m_PETArray.begin(); iter!=m_PETArray.end(); ++iter)
	{
		std::tr1::shared_ptr<GLAnyPet> pPet = iter->second;
		// 팻이름 표시;
		if ( pPet->IsVisibleDetect() == FALSE )
			continue;

		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
		{
			pPet->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL1(), GetUsedMaterialSystem(), 0 );
			pPet->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL2(), GetUsedMaterialSystem(), 1 );
		}

		if ( COLLISION::IsCollisionVolume( CV,pPet->m_vMax,pPet->m_vMin ) )
		{
			D3DXVECTOR3 vPOS(pPet->GetPosition() + D3DXVECTOR3(0.0f, 8.0f, 0.0f));
			BOOL bCOLL(FALSE);

			if ( GLOBAL_PARAM::IsValue( GP_LINE_CULLING ) )
			{
				m_spLandMan->IsCollision ( vViewPosition, vPOS, vCOLL, bCOLL, pDxFrame, EMCC_CULL_NONE );
			}

			if ( !bCOLL && GLOBAL_PARAM::IsValue( GP_DISPLAY_NAME ) )
			{
				CROWREN sDISP_NAME;
				sDISP_NAME.INIT ( pPet );
				sDISP_NAME.SETTYPEFLAG ( pMyCharacter, pPet );
				std::tr1::shared_ptr<GLCharClient> pCHAR = m_pGaeaClient->GetChar( pPet->m_dwOwner );
				if (pCHAR)
                    m_pInterface->NameDisplayAddName(sDISP_NAME, pCHAR->GetName());
			}
		}

		pPet->Render ( m_pd3dDevice );
		pPet->RenderShadow ( m_pd3dDevice, CV, GetUsedMaterialSystem() );
	}

	//CROW
    for (DROP_CROW_MAP_ITER iter=m_DropCrow.begin(); iter!=m_DropCrow.end(); ++iter)
    {
        std::tr1::shared_ptr<GLCrowClient> spCrow = iter->second;
		if ( spCrow == NULL )
            continue; // 유효하지 않은 액터라면 실패;

		if ( spCrow->IsVisibleDetect() == FALSE )
			continue; // 은신 중이고 감지하지 못하면 실패;

		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
		{
			spCrow->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL1(), GetUsedMaterialSystem(), 0 );
			spCrow->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL2(), GetUsedMaterialSystem(), 1 );
		}

		if ( COLLISION::IsCollisionVolume(CV, spCrow->m_vMax, spCrow->m_vMin) == FALSE )
			continue; // 카메라 절두체 밖에 있다면 실패;

		if ( spCrow->IsDie() == FALSE )
		{ // 죽었다면 이름은 렌더하지 않음;			
			BOOL bCOLL(FALSE);

			if ( GLOBAL_PARAM::IsValue( GP_LINE_CULLING ) )
			{
				m_spLandMan->IsCollision(vViewPosition, spCrow->GetPosition() + D3DXVECTOR3(0.0f, 8.0f, 0.0f), vCOLL, bCOLL, pDxFrame, EMCC_CULL_NONE);
			}

			if ( bCOLL == FALSE && GLOBAL_PARAM::IsValue( GP_DISPLAY_NAME ) && spCrow->IsHideNaming() == false )
			{
				CROWREN sDISP_NAME;
				sDISP_NAME.INIT(spCrow);
				sDISP_NAME.SETTYPEFLAG(pMyCharacter, spCrow);
				m_pInterface->NameDisplayAddName(sDISP_NAME);
			}
		}

		spCrow->Render(m_pd3dDevice, CV, FALSE);
		spCrow->RenderShadow(m_pd3dDevice, CV, GetUsedMaterialSystem());
	}    
    
    for (DROP_MATERIAL_MAP_ITER iter=m_DropMaterial.begin(); iter!=m_DropMaterial.end(); ++iter)
    {    
        std::tr1::shared_ptr<GLMaterialClient> spMaterial = iter->second;
        if (!spMaterial)
            continue;

		if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
		{
			spMaterial->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL1(), GetUsedMaterialSystem(), 0 );
			spMaterial->RenderShadow_SpotLight( m_pd3dDevice, DxShadowMap::GetInstance().GetClipVolume_PL2(), GetUsedMaterialSystem(), 1 );
		}

        if (!spMaterial->IsDie() && COLLISION::IsCollisionVolume(CV, spMaterial->m_vMax, spMaterial->m_vMin))
        {
			D3DXVECTOR3 vPOS(spMaterial->GetPosition() + D3DXVECTOR3(0.0f, 8.0f, 0.0f));
			BOOL bCOLL(FALSE);

			if ( GLOBAL_PARAM::IsValue( GP_LINE_CULLING ) )
			{
				m_spLandMan->IsCollision(vViewPosition, vPOS, vCOLL, bCOLL, pDxFrame, EMCC_CULL_NONE);
			}

            if (!bCOLL && GLOBAL_PARAM::IsValue( GP_DISPLAY_NAME ) && spMaterial->IsHideNaming() == false)
            {
                CROWREN sDISP_NAME;
                sDISP_NAME.INIT(spMaterial);
                sDISP_NAME.SETTYPEFLAG(pMyCharacter, spMaterial);
                m_pInterface->NameDisplayAddName(sDISP_NAME);
            }
        }

        spMaterial->Render(m_pd3dDevice, CV, FALSE);
        spMaterial->RenderShadow(m_pd3dDevice, CV, GetUsedMaterialSystem());
    }

	DWORD dwOldLighting;
	m_pd3dDevice->GetRenderState ( D3DRS_LIGHTING, &dwOldLighting );
	m_pd3dDevice->SetRenderState ( D3DRS_LIGHTING, FALSE );

	DWORD dwOldColorOp;
	m_pd3dDevice->GetTextureStageState( 0, D3DTSS_COLOROP,   &dwOldColorOp );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	
	for (DROP_ITEM_ITER iter=m_DropItem.begin(); iter!=m_DropItem.end(); ++iter)
	{
        std::tr1::shared_ptr<CItemClientDrop> spItemDrop = iter->second;		
		//CItemClientDrop &sItemDrop = *pCurItem->Data;

		bool bCV = spItemDrop->IsCollision(CV);
		if (bCV)
		{
			D3DXVECTOR3& vPOS = spItemDrop->vPos;
			BOOL bCOLL(FALSE);

			// 중급 이상에서만 충돌검사를 하도록 수정함.
			if ( RENDERPARAM::g_emTnLMode >= TnL_PS_3_0_DF && GLOBAL_PARAM::IsValue( GP_LINE_CULLING ) )
			{
				m_spLandMan->IsCollision(vViewPosition, vPOS, vCOLL, bCOLL, pDxFrame, EMCC_CULL_NONE );
			}

			if ( !bCOLL && GLOBAL_PARAM::IsValue( GP_DISPLAY_NAME ) )
			{
				CROWREN sDISP_NAME;
				sDISP_NAME.INIT(spItemDrop);
				sDISP_NAME.SETTYPEFLAG(pMyCharacter, spItemDrop);
				m_pInterface->NameDisplayAddName(sDISP_NAME);
			}
			spItemDrop->RenderItem(m_pd3dDevice, spItemDrop->matWld, NULL);
		}
	}

	for (DROP_MONEY_MAP_ITER iter=m_DropMoney.begin(); iter!=m_DropMoney.end(); ++iter)
	{
        std::tr1::shared_ptr<CMoneyClientDrop> spMoneyDrop = iter->second;
		if (!spMoneyDrop)
            continue;
		bool bCV = spMoneyDrop->IsCollision(CV);
		if (bCV)
		{
			D3DXVECTOR3& vPOS = spMoneyDrop->vPos;
			BOOL bCOLL(FALSE);

			// 중급 이상에서만 충돌검사를 하도록 수정함.
			if ( RENDERPARAM::g_emTnLMode >= TnL_PS_3_0_DF && GLOBAL_PARAM::IsValue( GP_LINE_CULLING ) )
			{
				m_spLandMan->IsCollision(vViewPosition, vPOS, vCOLL, bCOLL, pDxFrame, EMCC_CULL_NONE );
			}

			if ( !bCOLL && GLOBAL_PARAM::IsValue( GP_DISPLAY_NAME ) )
			{
				CROWREN sDISP_NAME;
				sDISP_NAME.INIT(spMoneyDrop);
				sDISP_NAME.SETTYPEFLAG(pMyCharacter, spMoneyDrop);
				m_pInterface->NameDisplayAddName(sDISP_NAME);
			}
			spMoneyDrop->RenderItem(m_pd3dDevice, spMoneyDrop->matWld, NULL);
		}
	}

	m_pd3dDevice->SetRenderState ( D3DRS_LIGHTING, dwOldLighting );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, dwOldColorOp );

	return S_OK;
}*/
