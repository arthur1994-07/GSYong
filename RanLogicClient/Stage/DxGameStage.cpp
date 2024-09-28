#include "pch.h"

#include "../../SigmaCore/Loading/NSLoadingDirect.h"
#include "../../SigmaCore/MsgPack/MinMsgPack.h"

#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"
#include "../../EngineLib/GUInterface/UIMan.h"
#include "../../EngineLib/GUInterface/UIInfoCfg.h"
#include "../../EngineLib/Common/DXInputString.h"
#include "../../enginelib/Common/SUBPATH.h"
#include "../../EngineLib/DxEffect/DxPostEffect.h" // by luxes.
#include "../../EngineLib/DxTools/PostProcessing/NSBloomHDR.h"
#include "../../EngineLib/DxTools/PostProcessing/NSLightShafts.h"
#include "../../EngineLib/DxTools/PostProcessing/NSGlow.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/DxTools/DxWeatherMan.h"
#include "../../EngineLib/DxTools/DxEnvironment.h"
#include "../../EngineLib/DxTools/DxGlowMan.h"
#include "../../EngineLib/DxTools/DxPostProcess.h"
#include "../../EngineLib/DxTools/DxCubeMap.h"
#include "../../EngineLib/DxTools/DxSkyMan.h"
#include "../../EngineLib/DxTools/DxDynamicVB.h"	// 삭제 해야한다.
#include "../../EngineLib/DxTools/DxSurfaceTex.h"
#include "../../EngineLib/DxTools/DxBackUpRendTarget.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/DxTools/CubeMap/NSCubeMapCulling.h"
#include "../../EngineLib/DxTools/CubeMap/NSCubeMapUtil.h"
#include "../../enginelib/DxTools/DxClubMan.h"
#include "../../EngineLib/DxEffect/Frame/DxEffectMan.h"
#include "../../EngineLib/DxEffect/EffProj/DxEffProj.h"
#include "../../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../../EngineLib/DxEffect/Single/DxEffGameHelp.h"
#include "../../EngineLib/DxEffect/Single/DxEffBillboardChar.h"
#include "../../EngineLib/DxEffect/EffectLua/NSEffectLuaPlay.h"
#include "../../EngineLib/DxMeshs/SkinMesh/DxSkinAniControl.h"
#include "../../EngineLib/DxResponseMan.h"
#include "../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../EngineLib/Common/profile.h"

#include "../../RanLogic/Club/GLClubDefine.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Quest/GLQUEST.h"
#include "../../RanLogic/Quest/GLQuestMan.h"
#include "../../RanLogic/HideSet/GLHideSet.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../Tournament/GLTournamentClient.h"
#include "../Friend/GLFriendClient.h"

#include "../TriggerManager/GLTriggerManagerClient.h"
#include "../TriggerSystem/ClientTriggerSystem.h"

#include "../Club/GLClubClient.h"
#include "../Tool/DxParamSet.h"
#include "../GLGaeaClient.h"
#include "../Tutorial/GLBaseTutorial.h"
#include "../Land/GLLandManClient.h"
#include "../WishList/WishList.h"
#include "../LobyCharacter/GLLobyCharClient.h"
#include "../RanParamClient.h"
#include "./DxGameStage.h"
#include "./DxGlobalStage.h"

#include "../Widget/GLWidgetScript.h"
#include "../Widget/GLWidgetSaveProp.h"

#include "../Chat/GLChatMan.h"
#include "../GfxInterfaceBase.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL g_bWIREFRAME_VIEW;

DxCharJoinData::DxCharJoinData(GLClubClient* pMyClub)
    : m_bCharJoin(false)
    , m_dwCharID(0)
    , m_dwClientID(0)
    , m_dwGaeaID(0)
    , m_vPos(0,0,0)
    , m_dwPutOnItems(0)
    , m_dwNumInvenItem(0)
    , m_dwNumVNInvenItem(0)
    , m_dwNumSkill(0)
    , m_dwNumQuestProc(0)
    , m_dwNumQuestEnd(0)
    , m_dwChargeItem(0)
    , m_dwClubMember(0)
    , m_dwClubAlliance(0)
    , m_dwClubBattle(0)
    , m_dwItemCoolTime(0)
    , m_dwReceiveInvenItem(0)
    , m_dwReceiveVNInvenItem(0)
    , m_dwReceiveSkill(0)
    , m_dwReceiveQuestProc(0)
    , m_dwReceiveQuestEnd(0)
    , m_dwReceiveChargeItem(0)
    , m_dwReceiveClubMember(0)
    , m_dwReceiveClubAlliance(0)
    , m_dwReceiveClubBattle(0)
    , m_dwReceiveItemCoolTime(0)
    , m_bReceiveClubInfo(false)
	, m_bCullByObjectMap(true)
    , m_dwNumProductBook(0)
    , m_dwReceiveProductBook(0)
    , m_pMyClub(pMyClub)
{
    //m_pMyClub = new GLClubClient;
}

DxCharJoinData::~DxCharJoinData()
{
    //delete m_pMyClub;
    //m_pMyClub = NULL;
}

void DxCharJoinData::ResetData()
{
	m_bCharJoin = false;
	m_dwCharID = 0;

	m_dwClientID = 0;
	m_dwGaeaID = 0;
	m_sMapID.reset();
	m_vPos = D3DXVECTOR3(0,0,0);

	m_dwPutOnItems = 0;
	m_dwNumInvenItem = 0;
	m_dwNumVNInvenItem = 0;
	m_dwNumSkill = 0;
	m_dwNumProductBook = 0;
	m_dwNumQuestProc = 0;
	m_dwNumQuestEnd = 0;
	m_dwChargeItem = 0;
	m_dwClubMember = 0;
	m_dwClubAlliance = 0;
	m_dwClubBattle = 0;
	m_dwItemCoolTime = 0;

	m_dwReceiveInvenItem = 0;
	m_dwReceiveVNInvenItem = 0;
	m_dwReceiveSkill = 0;
	m_dwReceiveProductBook = 0;
	m_dwReceiveQuestProc = 0;
	m_dwReceiveQuestEnd = 0;
	m_dwReceiveChargeItem = 0;
	m_dwReceiveClubMember = 0;
	m_dwReceiveClubAlliance = 0;
	m_dwReceiveClubBattle = 0;
	m_bReceiveClubInfo = false;
	m_bCullByObjectMap = true;
	m_dwReceiveItemCoolTime = 0;

	m_pMyClub->RESET();
	m_CharData2.Assign(SCHARDATA2());
}

bool DxCharJoinData::CheckReceive(bool bLoginProcessLog)
{
    if (!m_bCharJoin)
	{
		if( bLoginProcessLog ) sc::writeLogError(std::string("LOGIN ERROR. Char Join Fail"));
        return false;
	}
    if (m_dwPutOnItems != SLOT_TSIZE)
	{
		if( bLoginProcessLog ) sc::writeLogError(std::string("LOGIN ERROR. Puton Item Fail"));
        return false;
	}
    if (m_dwNumInvenItem != m_dwReceiveInvenItem)
	{
		if( bLoginProcessLog ) sc::writeLogError(std::string("LOGIN ERROR. Inven Item Fail"));
        return false;
	}
    if (m_dwNumSkill != m_dwReceiveSkill)
	{
		if( bLoginProcessLog ) sc::writeLogError(std::string("LOGIN ERROR. Skill Fail"));
        return false;
	}
    if (m_dwNumQuestProc != m_dwReceiveQuestProc)
	{
		if( bLoginProcessLog ) sc::writeLogError(std::string("LOGIN ERROR. Process Quest Fail"));
        return false;
	}
    if (m_dwNumQuestEnd != m_dwReceiveQuestEnd)
	{
		if( bLoginProcessLog ) sc::writeLogError(std::string("LOGIN ERROR. End Quest Fail"));
        return false;
	}
    if (m_dwChargeItem != m_dwReceiveChargeItem)
	{
		if( bLoginProcessLog ) sc::writeLogError(std::string("LOGIN ERROR. Charge Item Fail"));
        return false;
	}
    if (m_dwItemCoolTime != m_dwReceiveItemCoolTime)
	{
		if( bLoginProcessLog ) sc::writeLogError(std::string("LOGIN ERROR. Item Cool Fail"));
        return false;
	}
    if (!m_bReceiveClubInfo)
	{
		if( bLoginProcessLog ) sc::writeLogError(std::string("LOGIN ERROR. Club Info Fail"));
        return false;
	}
    if (m_dwClubMember != m_dwReceiveClubMember)
	{
		if( bLoginProcessLog ) sc::writeLogError(std::string("LOGIN ERROR. Club Member Fail"));
        return false;
	}
    if (m_dwClubAlliance != m_dwReceiveClubAlliance)
	{
		if( bLoginProcessLog ) sc::writeLogError(std::string("LOGIN ERROR. Club Alliance Fail"));
        return false;
	}
    if (m_dwClubBattle != m_dwReceiveClubBattle)
	{
		if( bLoginProcessLog ) sc::writeLogError(std::string("LOGIN ERROR. Club Battle Fail"));
        return false;
	}
    if (m_dwNumProductBook != m_dwReceiveProductBook)
	{
		if( bLoginProcessLog ) sc::writeLogError(std::string("LOGIN ERROR. Product Book Fail"));
        return false;
	}
    if (GLogicData::GetInstance().GetServiceProvider() == SP_VIETNAM && m_dwNumVNInvenItem!=m_dwReceiveVNInvenItem)
        return false;
    
	if( bLoginProcessLog ) sc::writeLogError(std::string("LOGIN SUCCESS. Pass CheckReceive"));
    return true;
}

DxGameStage::DxGameStage(DxGlobalStage* pGlobalStage, GLGaeaClient* pGaeaClient, bool bLoginProcessLog)
    : m_pGaeaClient(pGaeaClient)
    , m_pGlobalStage(pGlobalStage)
    , m_bUseInterface(TRUE)
	, m_bLoginProcessLog(bLoginProcessLog)
{
    m_pCharJoin = new DxCharJoinData(pGaeaClient->GetMyClub());

	m_QuestByteStreamID = 0; // QUEST_BUFFER_OVER
}

DxGameStage::~DxGameStage(void)
{
    delete m_pCharJoin;
    m_pCharJoin = NULL;

	
}

HRESULT DxGameStage::ReSizeWindow ( WORD wWidth, WORD wHeight )
{
	// 인터페이스 초기화.    
    CUIMan::SetResolution(MAKELONG(wHeight,wWidth));

	//DxPostEffect::GetInstance().InvalidateDeviceObjects();
	//DxPostEffect::GetInstance().RestoreDeviceObjects( m_pd3dDevice );

	InvalidateDeviceObjects();
	RestoreDeviceObjects();

	return S_OK;
}

void DxGameStage::OnInitLogin()
{
	m_pCharJoin->ResetData();
}

void DxGameStage::SetSelectChar ( SCHARINFO_LOBBY *pCharInfo )
{
	m_pCharJoin->m_dwCharID = pCharInfo->m_dwCharID;
}

void DxGameStage::ResetCharJoinData ()
{
	// [shhan][2015.01.16] 현재는 GameToLobbyStage 에서 호출한다.
	//						m_pCharJoin 이 정보를 지우기 전에 WishList 를 저장한다.
	if( GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() && m_pGaeaClient )
	{
		WishList::CClient* pWishList = m_pGaeaClient->GetWishList();
		if( pWishList )
		{
			pWishList->SaveFile(
				m_pGaeaClient->GetConnServerGroup(),
				m_pCharJoin->m_CharData2.CharDbNum());
		}
	}

	m_pCharJoin->ResetData();
}

HRESULT DxGameStage::OneTimeSceneInit ( HWND hWnd, WORD wWidth, WORD wHeight, const char* szAppPath )
{
	GASSERT(hWnd);

	StringCchCopy ( m_szAppPath, MAX_PATH, szAppPath );
	m_hWnd = hWnd;

    TriggerSystem::InitClientTriggerSystem();

	m_pGaeaClient->OneTimeSceneInit ( m_szAppPath, hWnd );	

	return S_OK;
}

HRESULT DxGameStage::InitDeviceObjects ( LPDIRECT3DDEVICEQ pd3dDevice )
{
	GASSERT(pd3dDevice);

	HRESULT hr=S_OK;
	m_pd3dDevice = pd3dDevice;

	hr = m_pd3dDevice->GetDeviceCaps ( &m_d3dCaps );
	if ( FAILED(hr) )	return hr;

	WORD wWidth = 800;
	WORD wHeight = 600;

	//	Note : 현재해상도 받아오기.
	//
	LPDIRECT3DSURFACEQ pBackBuffer=NULL;
	m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

	if ( pBackBuffer )
	{
		pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
		pBackBuffer->Release();

		wWidth = static_cast<WORD> ( m_d3dsdBackBuffer.Width );
		wHeight = static_cast<WORD> ( m_d3dsdBackBuffer.Height );
	}

    {
// 	    RANPARAM::SETCHARID(m_CharJoinData.m_CharData2.m_szName);
		std::string name = m_pCharJoin->m_CharData2.m_szName;
		CUIInfoCfg::GetInstance().SetAddPathName(name);
        RANPARAM::LOAD_GAMEOPT(m_pGlobalStage->GetSubPath(),name);
        CUIInfoCfg::GetInstance().Load(m_pGlobalStage->GetSubPath());
    }

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	// 노말맵 사용 가능한지 여부 체크 // PS.2.0 하드웨어 가속이 가능 한지 체크
	if (!DxEffectMan::GetInstance().IsEnableCharPixelShader() || DxEffectMan::GetInstance().GetUseSwShader())
	{
		RANPARAM::bPossibleNormalMap = FALSE;
		RENDERPARAM::bUseNormalMap = FALSE;
        RENDERPARAM::bVolumeLight = FALSE;
	}
    else
    {
		RANPARAM::bPossibleNormalMap = TRUE;
	}

	DXPARAMSET::LOADDATA();	

	//  Note : GameHelp 이펙트 표시
	//bjju.temp 이름을 파일로 빼자

	DxEffGameHelp::EFFECTFILENAMECAPS  sEffectFilenameCaps;
	DxEffGameHelp::TEXTUREFILENAMECAPS sTextureFilenameCaps;

	sEffectFilenameCaps.strTargetEff_Default		 = GLCONST_CHAR::strTARGET_EFFECT;
	sEffectFilenameCaps.strTargetEff_Circle_Enemy	 = GLCONST_CHAR::strTARGET_EFFECT_ENEMY;
	sEffectFilenameCaps.strTargetEff_Circle_Npc		 = GLCONST_CHAR::strTARGET_EFFECT_NPC;
	sEffectFilenameCaps.strTargetEff_Circle_Alliance = GLCONST_CHAR::strTARGET_EFFECT_ALLIANCE;
	sEffectFilenameCaps.strTargetEff_Arrow_Enemy	 = GLCONST_CHAR::strARROW_EFFECT_ENEMY;
	sEffectFilenameCaps.strTargetEff_Arrow_Npc		 = GLCONST_CHAR::strARROW_EFFECT_NPC;
	sEffectFilenameCaps.strTargetEff_Arrow_Alliance  = GLCONST_CHAR::strARROW_EFFECT_ALLIANCE;
	sEffectFilenameCaps.strOverEff					 = GLCONST_CHAR::strCURSOR_TARGET_EFFECT;
	sEffectFilenameCaps.strClickEff					 = GLCONST_CHAR::strCLICK_EFFECT;
	sEffectFilenameCaps.strMineEff					 = GLCONST_CHAR::strMINE_EFFECT;

	sTextureFilenameCaps.strScopeCircle				 = GLCONST_CHAR::strSCOPE_TEXTURE_CIRCLE;
	sTextureFilenameCaps.strScopeFanwise			 = GLCONST_CHAR::strSCOPE_TEXTURE_FANWISE;

	DxEffGameHelp::GetInstance().SetEffectFileName( sEffectFilenameCaps, sTextureFilenameCaps );
	
	//  Note : 빌보드 텍스쳐를 로드한다.
	DxEffBillboardChar::GetInstance().LoadBillboardTex( GLCONST_CHAR::strCharBoard );

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	// 인터페이스 초기화.
    CUIMan::SetResolution(MAKELONG(wHeight,wWidth));	
    
    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//	Note : 뷰포트.
	//
	DxViewPort::GetInstance().SetGameCamera ();
	D3DXVECTOR3 vFromPt		= D3DXVECTOR3( 0.0f, 70.0f, -70.0f );
	D3DXVECTOR3 vLookatPt	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	DxViewPort::GetInstance().SetViewTrans(vFromPt, vLookatPt, vUpVec);
	DxViewPort::GetInstance().SetActiveMap(m_pGaeaClient);

	//!! SF_TO
    //hr = pInterface->UiInitDeviceObjects( m_pd3dDevice );


	// [shhan][2015.06.15] 물리적 메모리를 정리한다.
	//						DxGlobalStage::ChangeStage 에서 DxGlobalStage::DoInterimClean 후 DxGlobalStage::NSLoadingDirect::Reset(); 사이까지 메모리를 너무 많이 로드해서 한번 걸치게 한다.
	//						Lobby -> Game
	HANDLE hProcess	= GetCurrentProcess();
	if (hProcess)
	{
		EmptyWorkingSet( hProcess );
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//	Note : 가이아클라이언트 초기화.
	//
	hr = m_pGaeaClient->InitDeviceObjects ( m_pd3dDevice );
	GASSERT(!FAILED(hr)&&"m_pGaeaClient->InitDeviceObjects");
	if (FAILED(hr))
        return hr;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//	Note : 캐릭터가 생성될 맵 생성.
	//
	const MapID& sMapID(m_pCharJoin->m_sMapID);

	hr = m_pGaeaClient->SetActiveMap ( sMapID, m_pCharJoin->m_vPos );
	GASSERT(!FAILED(hr)&&"m_pGaeaClient->SetActiveMap");
	if ( FAILED(hr) )
        return hr;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	//	Note : 캐릭터를 맵상에 위치시킴.
	//	
	m_pGaeaClient->GetActiveMap()->SetCullByObjectMap(m_pCharJoin->m_bCullByObjectMap);
	boost::shared_ptr<NavigationMesh> spNavi = m_pGaeaClient->GetActiveMap()->GetNaviMesh();
	GLCharacter* pCharacter = m_pGaeaClient->GetCharacter();
	pCharacter->Create(spNavi, &m_pCharJoin->m_CharData2, &m_pCharJoin->m_vPos, m_pCharJoin->m_dwGaeaID, m_pd3dDevice, m_pGaeaClient->GetActiveMap()->GetLandMan() );

	// Country;
	Country::SCOUNTRY_INFO sCountryInfo;
	sCountryInfo.dwID = m_pCharJoin->m_dwCountryID;
	pCharacter->SetCountryInfo( sCountryInfo );

    pCharacter->PostEnterMap();
	//pCharacter->m_pMyClub = m_CharJoinData.m_pMyClub;

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	EMSERVICE_PROVIDER sp = m_pGlobalStage->GetServiceProvider();

	if (sp == SP_THAILAND || sp == SP_MALAYSIA || sp == SP_MALAYSIA_EN || sp == SP_PHILIPPINES)
	{
		//!! SF_TODO
		//m_pGaeaClient->GetInterface()->SetIPBonusClass( pCharacter->m_emIPBonus );
	}	

	//	Note : 캐릭터 위치로 카메라 보냄.
	D3DXVECTOR3 vCamera = m_pGaeaClient->GetCharacter()->GetPosition();
	vCamera.y += 10.0f;
	DxViewPort::GetInstance().CameraJump ( vCamera );

	CString strMiniMap;
	PLANDMANCLIENT pLandClient = m_pGaeaClient->GetActiveMap();
	if ( pLandClient )
	{
		CString strText = m_pGaeaClient->GetMapName(pLandClient->GetBaseMapID());

		const GLMapAxisInfo& sMapAxisInfo = pLandClient->GetMapAxisInfo();
		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Minimap_MoveActiveMap,
			"-s, -s, -b, -dw, -dw, -s",
			strText.GetBuffer(),
			sMapAxisInfo.GetMinMapTex(),
			pLandClient->IsUIMapSelect() == false,
			m_pGaeaClient->GetCharacter()->m_dwServerID,
			pLandClient->m_dwGuidClub,
			pLandClient->m_szGuidClubName );
	}

    //---------------------------------------------------//
    NSLoadingDirect::FrameMoveRender( FALSE );
    //---------------------------------------------------//

	DxEffSinglePropGMan::GetInstance().LoadEffectGProp ( "Q.egp", FALSE, TRUE );

	// InputString Off.
    EngineDeviceMan* pDevice = m_pGlobalStage->GetEngineDevice();
    DXInputString* pInputString = pDevice->GetInputString();
	pInputString->OffInput();

    //! 캐릭터 접속시 트리거
    triggermanager::GLTriggerManagerClient::GetInstance().Get_Trigger_Connect_To_Field().Do( pCharacter );

	if( GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() )
	{
		WishList::CClient* pWishList = m_pGaeaClient->GetWishList();
		if( pWishList )
		{
			pWishList->LoadFile(
				m_pGaeaClient->GetConnServerGroup(),                
				m_pCharJoin->m_CharData2.CharDbNum() );
		}
	}

	//---------------------------------------------------//
	NSLoadingDirect::FrameMoveRender( FALSE );
	//---------------------------------------------------//

	GLWidgetScript::GetInstance().ResetWidgetPosition();
	GLWidgetSaveProp::GetInstance().LoadXML(m_pCharJoin->m_CharData2.m_szName);
	// UI에 게임 화면 진입 알리기
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_EnterGameStage );

	// UI 프리로드
	GfxInterfaceBase::GetInstance()->InitGameStage();
	GfxInterfaceBase::GetInstance()->PreloadWidget();

	GfxInterfaceBase::GetInstance()->SetEdgeAAMode( RANPARAM::nUIAntiAliasing );

	// [shhan][2015.06.15] 물리적 메모리를 정리한다.
	//						DxGlobalStage::DoInterimClean 후 DxGlobalStage::NSLoadingDirect::Reset(); 사이까지 메모리를 너무 많이 로드해서 한번 걸치게 한다.
	//						Lobby -> Game
	//HANDLE hProcess	= GetCurrentProcess();
	if (hProcess)
	{
		EmptyWorkingSet( hProcess );
	}

	return S_OK;
}

HRESULT DxGameStage::RestoreDeviceObjects ()
{
	HRESULT hr;

	DxPostEffect::GetInstance().RestoreDeviceObjects( m_pd3dDevice ); // by luxes.

	LPDIRECT3DSURFACEQ pBackBuffer;
	m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

	pBackBuffer->GetDesc( &m_d3dsdBackBuffer );
	pBackBuffer->Release();

	//	Note : 뷰포트
	//
	DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)m_d3dsdBackBuffer.Width, 
		(float)m_d3dsdBackBuffer.Height, 5.0f, 1100.0f );

	hr = DxViewPort::GetInstance().SetViewPort ();
	if ( FAILED(hr) )
		return hr;

	hr = m_pGaeaClient->RestoreDeviceObjects ();
	if ( FAILED(hr) )
		return hr;

	//hr = m_pGaeaClient->RestoreDeviceObjects(m_pd3dDevice);
	if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT DxGameStage::InvalidateDeviceObjects ()
{
	DxPostEffect::GetInstance().InvalidateDeviceObjects(); // by luxes.

	HRESULT hr;
	hr = m_pGaeaClient->InvalidateDeviceObjects ();
	if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT DxGameStage::DeleteDeviceObjects ()
{
	if( GLUseFeatures::GetInstance().IsUsingRenewInvenWindow() )
	{
		WishList::CClient* pWishList = m_pGaeaClient->GetWishList();

		// [shhan][2015.01.16] GameToLobbyStage 시에는 m_pCharJoin->m_CharData2.CharDbNum() 값이 정상이 아니다.
		//						m_pCharJoin 의 유효성 검사가 필요하다.
		if( pWishList && m_pCharJoin->m_bCharJoin )
		{
			pWishList->SaveFile(
				m_pGaeaClient->GetConnServerGroup(),
				m_pCharJoin->m_CharData2.CharDbNum());
		}
	}

    RANPARAM::SAVE_GAMEOPT(m_pGlobalStage->GetSubPath());
    RANPARAM::SAVE(m_pGlobalStage->GetServiceProvider());

	HRESULT hr = m_pGaeaClient->DeleteDeviceObjects ();
	if (FAILED(hr))
        return hr;

    CUIInfoCfg::GetInstance().Save(m_pGlobalStage->GetSubPath());
    CUIInfoCfg::GetInstance().Clear();

	return S_OK;
}

HRESULT DxGameStage::FinalCleanup ()
{
	HRESULT hr = S_OK;
	//hr = m_pGaeaClient->FinalCleanup();
    m_pGaeaClient->NotifyClear();
    
	if (FAILED(hr))
        return hr;

	TriggerSystem::DestroyClientTriggerSystem();
	
	return S_OK;
}

HRESULT DxGameStage::Test ()
{
	HRESULT hr;

	hr = InvalidateDeviceObjects ();
	if (FAILED(hr) )	return hr;

	hr = DeleteDeviceObjects ();
	if (FAILED(hr) )	return hr;

	hr = FinalCleanup ();
	if (FAILED(hr) )	return hr;

	return S_OK;
}

HRESULT DxGameStage::Test2 ()
{
	HRESULT hr;

	hr = OneTimeSceneInit ( m_hWnd, 0, 0, m_szAppPath );
	if (FAILED(hr) )	return hr;

	hr = InitDeviceObjects ( m_pd3dDevice );
	if (FAILED(hr) )	return hr;

	hr = RestoreDeviceObjects ();
	if (FAILED(hr) )	return hr;

	return S_OK;
}


HRESULT DxGameStage::FrameMove ( float m_fTime, float m_fElapsedTime, BOOL bNotRendering )
{
	HRESULT hr;

	//if (m_pGlobalStage->GetBlockDetectState() )
	//{
	//	m_pInterface->SetBlockProgramFound(true);

	//	static bool bCONSOLE_TEXT(false);
	//	if ( !bCONSOLE_TEXT )
	//	{
	//		bCONSOLE_TEXT = true;
	//		PrintConsoleText ( ID2GAMEINTEXT("BLOCK_PROGRAM_FOUND_CONSOLE") );
	//	}
	//}

	////	Note : 날씨 변화 강제 테스트.
	////
	//if ( DxInputDevice::GetInstance().GetKeyState(DIK_F6)&DXKEY_UP )
	//	DxWeatherMan::GetInstance()->SetCheckAuto	( !DxWeatherMan::GetInstance()->GetCheckAuto() );

	////	Note : 그림자 변화 강제 테스트.
	////
	//if ( DxInputDevice::GetInstance().GetKeyState(DIK_F5)&DXKEY_UP )
	//	DxShadowMap::GetInstance().SetShadowDetail	( (EMSHADOWDETAIL) ((DxShadowMap::GetInstance().GetShadowDetail()+1)%5) );

	// float 정확도 체크
	CheckControlfp();

	if ( DxInputDevice::GetInstance().GetKeyState(DIK_F12)&DXKEY_DOWN )
	{
		m_bUseInterface = !m_bUseInterface;
	}

	PROFILE_BEGIN("CInnerInterface::FrameMove");
	GfxInterfaceBase::GetInstance()->UiFrameMove( m_pd3dDevice, m_fElapsedTime );
	PROFILE_END("CInnerInterface::FrameMove");

	DxGlobalStage* const pGlobalStage = m_pGaeaClient->GetGlobalStage();
	if ( !pGlobalStage->IsEmulator() && !pGlobalStage->IsSTATE(DxGlobalStage::EM_CHANGE) )
	{
		if (!pGlobalStage->GetNetClient()->IsOnline())
		{
			// 로그인 프로세스 확인을 위한 로그
			if( pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. Disconnected Drop Out "));

			//std::string strMsg( CGameTextMan::GetInstance().GetGameInText("PLAYINTERFACE_5") );
			GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CloseConnection, "-n", EMDROPOUT_DISCONNECTED);
		}
	}

	hr = m_pGaeaClient->FrameMove ( m_fTime, m_fElapsedTime, DxViewPort::GetInstance().GetClipVolume(), bNotRendering );
	if (FAILED(hr))
        return hr;

	// Note : post effect
	if ( DxPostEffect::GetInstance().IsRadialBlurON() )
		DxPostEffect::GetInstance().FrameMoveRadialBlur( m_fTime, m_fElapsedTime );

	// Note : 화면 효과
	DxEffProjMan::GetInstance().FrameMove( m_fElapsedTime );

    //DXInputString* pInputString = DxResponseMan::GetInstance().GetInputString();
	//CDebugSet::ToView(0, 11, "ime composition : %d", pInputString->CheckComposition());

	NSCubeMapUtil::NSCubeMapBlend::OnFrameMove( m_fElapsedTime );

	return S_OK;
}

HRESULT DxGameStage::Render ()
{
	HRESULT hr = S_OK;

	if ( DxFogMan::GetInstance().GetFogRange() != EMFR_OFF )
	{
		hr = RenderWorldCharEffect();	
		if ( FAILED(hr) )
			return hr;
	}
	
	if (m_bUseInterface == TRUE)
	{
		PROFILE_BEGIN("Oldinterface::Render");
		GfxInterfaceBase::GetInstance()->UiRender( m_pd3dDevice );
		PROFILE_END("Oldinterface::Render");


	}

	return S_OK;
}

HRESULT DxGameStage::RenderWorldCharEffect()
{

	//////////////////////////////////////////////////////////////////////////
	//						Surface 변화 확인 #1
	//////////////////////////////////////////////////////////////////////////
	// HDR ON	- m_pColorSuf16F_HDR
	// HDR OFF	- m_pColorBuffer
	//////////////////////////////////////////////////////////////////////////


	HRESULT hr = S_OK;

	CLIPVOLUME &CV = DxViewPort::GetInstance().GetClipVolume ();

	////	Note : 날씨 메니져
	////
	//PROFILE_BEGIN2("Weather Render_Prev");
	//DxWeatherMan::GetInstance()->Render_Prev ( m_pd3dDevice );
	//PROFILE_END2("Weather Render_Prev");

	DxEnvironment::GetInstance().ResetReflectionCV();
	if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && m_pGaeaClient->GetUsedMaterialSystem() )
	{
		DxEnvironment::GetInstance().RenderReflectionMaterial( m_pd3dDevice, m_pGaeaClient->GetActiveMap()->GetLandMan() );
	}
	else
	{
		PROFILE_BEGIN2("Environment Render");
		DxEnvironment::GetInstance().Render ( m_pd3dDevice, NULL, m_pGaeaClient->GetActiveMap()->GetLandMan() );
		PROFILE_END2("Environment Render");

		// Note : Post Efffect를 위해 렌더 타겟을 바꾼다.
		//DxPostEffect::GetInstance().ChangeFinalTarget( m_pd3dDevice );
	}




	// 셋팅되어져 있는 값은 DxSurfaceTex::GetInstance().m_pColorBuffer
	// 변경되면 DxPostEffect 쪽의 Buffer 이다.
	if ( !RENDERPARAM::g_bEnable_HDR )
	{
		// Note : Post Efffect를 위해 렌더 타겟을 바꾼다.
		DxPostEffect::GetInstance().ChangeFinalTarget( m_pd3dDevice, TRUE );
	}


	//////////////////////////////////////////////////////////////////////////
	//						Surface 변화 확인 #2
	//////////////////////////////////////////////////////////////////////////
	// HDR ON						- m_pColorSuf16F_HDR
	// HDR OFF	- DxPostEffect ON	- DxPostEffect::GetInstance().m_pTexFinal
	//			- DxPostEffect OFF	- m_pColorBuffer
	//////////////////////////////////////////////////////////////////////////


	LPDIRECT3DSURFACEQ pSurfaceTarget(NULL);
	hr = m_pd3dDevice->GetRenderTarget( 0, &pSurfaceTarget );
	{
		//	Note : 지형 랜더.
		//
		if ( m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX )
			m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, TRUE );

		if ( g_bWIREFRAME_VIEW )	m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		else						m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		//	Note : 지형, 캐릭터, 캐릭터 그림자, 캐릭터 반사.
		PROFILE_BEGIN2("GLGaeaClient::Render");
		hr = m_pGaeaClient->Render ( CV, pSurfaceTarget );
		PROFILE_END2("GLGaeaClient::Render");
		if ( FAILED(hr) )	return hr;

		m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		// Note : 
		OPTMManager::GetInstance().Render_HardAlpha();

		//	Note : 
		PROFILE_BEGIN2("GLGaeaClient::RenderSoftAlpha");
		m_pGaeaClient->RenderSoftAlpha( CV, pSurfaceTarget );
		PROFILE_END2("GLGaeaClient::RenderSoftAlpha");
	}
	SAFE_RELEASE ( pSurfaceTarget );





	// 셋팅되어져 있는 값은 DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR
	// 변경되면 DxPostEffect 쪽의 Buffer 이다.
	if ( RENDERPARAM::g_bEnable_HDR )
	{
		// 롤백 값을 DxSurfaceTex::GetInstance().m_pColorBuffer 로 변경하게 됨.
		m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorBuffer );

		// Note : Post Efffect를 위해 렌더 타겟을 바꾼다.
		DxPostEffect::GetInstance().ChangeFinalTarget( m_pd3dDevice, FALSE );
	}



	//////////////////////////////////////////////////////////////////////////
	//						Surface 변화 확인 #3
	//////////////////////////////////////////////////////////////////////////
	// HDR ON	- DxPostEffect ON	- DxPostEffect::GetInstance().m_pTexFinal
	//			- DxPostEffect OFF	- m_pColorBuffer
	// HDR OFF	- DxPostEffect ON	- DxPostEffect::GetInstance().m_pTexFinal
	//			- DxPostEffect OFF	- m_pColorBuffer
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// DxPostEffect ON	- DxPostEffect::GetInstance().m_pTexFinal
	// DxPostEffect OFF	- m_pColorBuffer
	//////////////////////////////////////////////////////////////////////////
	// 여기까지 왔으면 Buffer 는 DxSurfaceTex::GetInstance().m_pColorBuffer 혹은 DxPostEffect 쪽의 Buffer 이다.
	// DxSurfaceTex::GetInstance().m_pColorBuffer 상황은 DxPostEffect 가 지원 불가일 때임.
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	//			HDR - Bloom 적용.
	//m_pd3dDevice->StretchRect( DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR, NULL, DxSurfaceTex::GetInstance().m_pColorBuffer, NULL, D3DTEXF_POINT );
	NSBloomHDR::OnRender( m_pd3dDevice );
	//m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorBuffer );




	//////////////////////////////////////////////////////////////////////////
	//						Surface 변화 확인 #4
	//////////////////////////////////////////////////////////////////////////
	// HDR ON	- DxPostEffect ON	- DxPostEffect::GetInstance().m_pTexFinal 에 m_pColorTex16F_HDR 정보들이 들어갔다.
	//////////////////////////////////////////////////////////////////////////
	// DxPostEffect ON	- DxPostEffect::GetInstance().m_pTexFinal
	// DxPostEffect OFF	- m_pColorBuffer
	//////////////////////////////////////////////////////////////////////////



	//	Note : 
	PROFILE_BEGIN2("GLGaeaClient::RenderSingleEff");
	m_pGaeaClient->RenderSingleEff( CV );
	PROFILE_END2("GLGaeaClient::RenderSingleEff");

	//	Note : 효과
	PROFILE_BEGIN2("DxEffGroupPlayer::Render");
	DxEffGroupPlayer::GetInstance().Render ( m_pd3dDevice );
	PROFILE_END2("DxEffGroupPlayer::Render");

	PROFILE_BEGIN2("DxEffGameHelp::Render");
	DxEffGameHelp::GetInstance().Render ( m_pd3dDevice );
	PROFILE_END2("DxEffGameHelp::Render");

	PROFILE_BEGIN2("DxEffBillboardChar::Render");
	DxEffBillboardChar::GetInstance().Render ( );
	PROFILE_END2("DxEffBillboardChar::Render");

	// Note : 최적화한 Eff
	OPTMManager::GetInstance().Render();

	//PROFILE_BEGIN2("GLGaeaClient::RenderPickAlpha");
	//m_pGaeaClient->RenderPickAlpha();
	//PROFILE_END2("GLGaeaClient::RenderPickAlpha");



	// LightShafts 작업을 한다.
	{
		DxBackupTarget sBackupTarget;
		sBackupTarget.Backup( m_pd3dDevice );

		m_pGaeaClient->Render_LightShafts();
		NSLightShafts::OnRender( m_pd3dDevice );
	}

	if ( m_d3dCaps.RasterCaps & D3DPRASTERCAPS_FOGVERTEX )
		m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, FALSE );

	//	Note : 날씨 메니져
	//
	PROFILE_BEGIN2("Weather Render");
	DxWeatherMan::GetInstance()->Render ( m_pd3dDevice );
	PROFILE_END2("Weather Render");

	//	Note : 큐브 텍스쳐 렌더링..!!
	//
	//DxCubeMap::GetInstance().Render ( m_pd3dDevice );
	NSCubeMapUtil::NSCubeMapBlend::OnRender( m_pd3dDevice, NSCubeMapCulling::GetCubeMapTexture( DxViewPort::GetInstance().GetLookatPt() ) );

	//	Note : 글로우 처리를 하기위해 사용 한다.
	//

	//	Note : Glow 
	DxGlowMan::GetInstance().RenderTex ( m_pd3dDevice );
	DxGlowMan::GetInstance().Render ( m_pd3dDevice );
	DxGlowMan::GetInstance().RenderBURN( m_pd3dDevice );



	// StarGlow 렌더링
	if ( DxSurfaceTex::GetInstance().m_pGlowSuf_SRC )
	{
		DxBackupTarget sBackupTarget;
		sBackupTarget.Backup( m_pd3dDevice );

		m_pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pGlowSuf_SRC );
		m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);

		// StarGlow 렌더링
		m_pGaeaClient->RenderStarGlow();

		// Note : Glow 작업을 한다.
		NSGlow::OnRender( m_pd3dDevice, DxSurfaceTex::GetInstance().m_pColorBuffer );
	}



	// 지금까지 그린것을 굴절을 위해 복사해놓는다.
	PROFILE_BEGIN2("DxEnvironment::UserToRefraction()");
	DxEnvironment::GetInstance().UserToRefraction ( m_pd3dDevice );
	PROFILE_END2("DxEnvironment::UserToRefraction()");




	// Note : 포스트 이펙트.
	//			작업되면 DxSurfaceTex::GetInstance().m_pColorBuffer 값이 됨.
	DxPostEffect::GetInstance().RenderPostEffect( m_pd3dDevice ); // by luxes.



	//////////////////////////////////////////////////////////////////////////
	//						Surface 변화 확인 #5
	//////////////////////////////////////////////////////////////////////////
	// DxPostEffect ON	- m_pColorBuffer
	// DxPostEffect OFF	- m_pColorBuffer
	//////////////////////////////////////////////////////////////////////////







	//	Note : Wave 되야 하는것을 저장후 마지막으로 뿌린다.
	//
	PROFILE_BEGIN2("DxEnvironment::RenderWave()");
	DxEnvironment::GetInstance().RenderWave ( m_pd3dDevice, DxEffectMan::GetInstance().GetBumpTexture() );
	PROFILE_END2("DxEnvironment::RenderWave()");

	//if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && m_pGaeaClient->GetUsedMaterialSystem() )
	//{
	//	// Note : 화면 효과
	//	DxEffProjMan::GetInstance().Render( m_pd3dDevice );
	//}
	//else
	//{
	//	// Note : 후처리
	//	//DxPostProcess::GetInstance().Render( m_pd3dDevice );

	//	// Note : 화면 효과
	//	DxEffProjMan::GetInstance().Render( m_pd3dDevice );
	//}

	// Note : 화면 효과
	DxEffProjMan::GetInstance().Render( m_pd3dDevice );


	NSEffectPlay::RenderPostEffect( m_pd3dDevice );




	//m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorBuffer );

	return S_OK;
}

void DxGameStage::MsgProcess (NET_MSG_GENERIC* nmg)
{
	switch ( nmg->nType )
	{
	case NET_MSG_CHAT_LINK_FB :			// 링크용 채팅 메시지
	case NET_MSG_CHAT_CTRL_FB:			// 관리자용 채팅메시지
	case NET_MSG_CHAT_PRIVATE_FAIL :
	case NET_MSG_CHAT_BLOCK :
		{
			if( m_pGlobalStage->GetActiveStage() == m_pGlobalStage->GetGameStage() )
				GLChatMan::GetInstance().MsgProc( nmg );
		}
		break;

	case NET_MSG_LOBBY_CHAR_JOIN:
        MsgLobbyCharJoin(nmg);
		break;

// 	case NET_MSG_LOBBY_CHAR_PUTON:
// 		{
// 			GLMSG::SNETLOBBY_CHARPUTON *pCharJoin = (GLMSG::SNETLOBBY_CHARPUTON *) nmg;
// 
// 			m_CharJoinData.m_dwPutOnItems = SLOT_TSIZE;
// 			memcpy ( m_CharJoinData.m_CharData2.m_PutOnItems, pCharJoin->PutOnItems, sizeof(SITEMCUSTOM)*SLOT_TSIZE );
// 		}
// 		break;
	case NET_MSG_LOBBY_CHAR_PUTON_EX:
        MsgLobbyCharPutonEx(nmg);
		break;

	case NET_MSG_LOBBY_CHAR_ITEM:
        MsgLobbyCharItem(nmg);
		break;

//#if defined(VN_PARAM) //vietnamtest%%%
	case NET_MSG_LOBBY_CHAR_VIETNAM_ITEM:
        MsgLobbyCharVietnamItem(nmg);
		break;
//#endif

	case NET_MSG_LOBBY_CHAR_SKILL:
        MsgLobbyCharSkill(nmg);
		break;

	case NET_MSG_LOBBY_QUEST_END:
        MsgLobbyQuestEnd(nmg);
		break;

	case NET_MSG_LOBBY_QUEST_PROG:
        MsgLobbyQuestProg(nmg);
		break;

	case NET_MSG_LOBBY_CHARGE_ITEM:
        MsgLobbyChargeItem(nmg);
		break;

	case NET_MSG_LOBBY_ITEM_COOLTIME:
        MsgLobbyItemCoolTime(nmg);
		break;

	case NET_MSG_LOBBY_INSTANCE_LIST:
		MsgLobbyInstanceList(nmg);
		break;

	case NET_MSG_LOBBY_CHAR_DEFAULT:
		MsgLobbyCharDefault(nmg);
		break;

	case NET_MSG_LOBBY_CLUB_INFO:
        MsgLobbyClubInfo(nmg);
		break;

    case NET_MSG_LOBBY_CLUB_INFO_EX:
        MsgLobbyClubInfoEx(nmg);
        break;
    /*
	case NET_MSG_LOBBY_CLUB_MEMBER:
        MsgLobbyClubMember(nmg);
		break;
    */

    case NET_MSG_LOBBY_CLUB_MEMBER_MSG_PACK:
        MsgLobbyClubMemberMsgPack(nmg);
        break;

    case NET_MSG_LOBBY_CLUB_MEMBER_MSG_PACK_END:
        MsgLobbyClubMemberMsgPackEnd();
        break;

    case NET_MSG_LOBBY_CLUB_INFO_END:
        m_pCharJoin->m_bReceiveClubInfo = true;
        break;

	case NET_MSG_LOBBY_CLUB_ALLIANCE:
        MsgLobbyClubAlliance(nmg);
		break;

	case NET_MSG_LOBBY_CLUB_BATTLE:
        MsgLobbyClubBattle(nmg);
		break;

	case NET_MSG_GCTRL_CONFRONT_TAR:
		{
			MsgGCTRLConfront(nmg);
			break;
		}
			// 링크용 채팅 메시지
	case NET_MSG_GCTRL_TRADE_TAR:
		{
			
			MsgGCTRTradeTar(nmg);
			break;
		}

			// 강제 종료 메세지;
	case NET_MSG_GCTRL_DROP_OUT_FORCED:
		{
			MsgDropOutForced(nmg);
			break;
		}
	case NET_MSG_SERVER_GENERALCHAT:
	case NET_MSG_APEX_ANSWER:			// APEX 메세지
	case NET_MSG_GCTRL_REQ_TAKE_FB:
	case NET_MSG_GCTRL_PARTY_LURE_TAR:	//	파티 가입 요청 응답 절차 필요.
    case NET_MSG_GCTRL_PARTY_LURE_TAR_WITH_CHARNAME: //	파티 가입 요청 응답 절차 필요.	
	//case NET_MSG_EVENT_LOTTERY_FB:		//	복권
	case NET_MSG_GCTRL_WITH_VEHICLE_TAR:
		if (m_pGlobalStage->GetActiveStage() == m_pGlobalStage->GetGameStage())
        {
			m_pGaeaClient->MsgGame( nmg );
        }
		break;

    //case NET_MSG_ACTIVITY_SYNC:
    //case NET_MSG_ACTIVITY_REWARD_POINT_SYNC:
    //case NET_MSG_ACTIVITY_REWARD_BADGE_SYNC:
    //    if (m_pGlobalStage->GetActiveStage() == m_pGlobalStage->GetGameStage())
    //    {
    //        // 우회해서 메세지 처리
    //        m_pGaeaClient->MsgActivitySync( nmg );
    //    }
    //    break;

	case NET_MSG_GCTRL_QUEST_PROG_INVEN_INSERT2:
		MsgLobbyQuestProgItem( nmg );
		break;

	default:
		CDebugSet::ToListView ( "DxGameStage::MsgProcess unknown message type %d", nmg->nType );
		break;
	};
}

void DxGameStage::MsgLobbyClubMemberMsgPack(NET_MSG_GENERIC* nmg)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;

    SCLUBMEMBER Member;
    if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Member))
    {
        m_pCharJoin->m_dwReceiveClubMember = Member.vecClubMember.size();
        
        BOOST_FOREACH( const GLCLUBMEMBER& clubValue, Member.vecClubMember )
        {
            m_pCharJoin->m_pMyClub->AddMemberEx(clubValue.GetChaDbNum(), clubValue);
        }
        
    }
    else
    {
        sc::writeLogError("DxGameStage::MsgLobbyClubMemberMsgPack");
    }
}

void DxGameStage::MsgLobbyClubMemberMsgPackEnd()
{
    m_pCharJoin->m_dwReceiveClubMember = m_pCharJoin->m_dwClubMember;
    if (m_pCharJoin->CheckReceive(m_bLoginProcessLog))
    {
        LobbyGameComplete();
    }
}

void DxGameStage::MsgLobbyCharJoin(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETLOBBY_CHARJOIN *pCharJoin = (GLMSG::SNETLOBBY_CHARJOIN *) nmg;
    if ( m_pCharJoin->m_dwCharID != pCharJoin->Data.m_CharDbNum )	return;

    m_pCharJoin->m_bCharJoin = true;
    m_pCharJoin->m_dwClientID = pCharJoin->dwClientID;
    m_pCharJoin->m_dwGaeaID = pCharJoin->dwGaeaID;
    m_pCharJoin->m_sMapID = pCharJoin->sMapID;
    m_pCharJoin->m_vPos = pCharJoin->vPos;
	m_pCharJoin->m_bCullByObjectMap = pCharJoin->bCullByObjectMap;

    m_pCharJoin->m_CharData2.GETCHARDATA() = pCharJoin->Data;

    StringCchCopy ( m_pCharJoin->m_CharData2.m_szUID , USR_ID_LENGTH+1, pCharJoin->szUserID );

    m_pCharJoin->m_CharData2.m_wSKILLQUICK_ACT = pCharJoin->wSKILLQUICK_ACT;
    memcpy ( m_pCharJoin->m_CharData2.m_sSKILLQUICK, pCharJoin->sSKILLQUICK, sizeof(SNATIVEID)*EMSKILLQUICK_SIZE );
    memcpy ( m_pCharJoin->m_CharData2.m_sACTIONQUICK, pCharJoin->sACTIONQUICK, sizeof(SACTION_SLOT)*EMACTIONQUICK_SIZE );

    m_pCharJoin->m_dwNumInvenItem = pCharJoin->dwNumInvenItems;
    //#if defined(VN_PARAM) //vietnamtest%%%

    if (m_pGaeaClient->GetServiceProvider() == SP_VIETNAM)
    {
        m_pCharJoin->m_dwNumVNInvenItem = pCharJoin->dwNumVNInvenItems;
    }
    //#endif

    m_pCharJoin->m_dwNumSkill = pCharJoin->dwNumSkill;
    m_pCharJoin->m_dwNumQuestProc = pCharJoin->dwNumQuestProc;
    m_pCharJoin->m_dwNumQuestEnd = pCharJoin->dwNumQuestEnd;
    m_pCharJoin->m_dwChargeItem = pCharJoin->dwChargeItem;
    m_pCharJoin->m_dwItemCoolTime = pCharJoin->dwItemCoolTime;

    m_pCharJoin->m_CharData2.m_sStartMapID = pCharJoin->sStartMapID;
    m_pCharJoin->m_CharData2.m_dwStartGate = pCharJoin->dwStartGate;
    m_pCharJoin->m_CharData2.m_sLastCallMapID = pCharJoin->sLastCallMapID;
    m_pCharJoin->m_CharData2.m_vLastCallPos = pCharJoin->vLastCallPos;
    m_pCharJoin->m_CharData2.m_emIPBonus = pCharJoin->emIPBonus;

	m_pCharJoin->m_dwCountryID = pCharJoin->dwCountryID;
    //m_pCharJoin->m_bTutorial = pCharJoin->bTutorial; // 튜토리얼 진행여부. by luxes.

    //#if defined(_RELEASED) || defined(TW_PARAM) || defined(TH_PARAM) || defined(HK_PARAM) // ***Tracing Log print
    EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();

	// 모든 국가에서 Tracing 기능을 사용할 수 있도록 변경
    //if (sp == SP_OFFICE_TEST || sp == SP_TAIWAN || sp == SP_HONGKONG || sp == SP_THAILAND)
    //{
        //m_pCharJoin->m_CharData2.m_bTracingUser  = pCharJoin->bTracingChar;
    //}
    //#endif

    CDebugSet::ToListView ( "NET_MSG_LOBBY_CHAR_JOIN" );

    if ( m_pCharJoin->CheckReceive(m_bLoginProcessLog) )
    {
		LobbyGameComplete();
    }

    DxSkinAniControl::m_bBIGHEAD = pCharJoin->bBIGHEAD;
	DxSkinAniControl::m_bBIGHAND = pCharJoin->bBIGHAND;
	DxSkinAniControl::m_fBIGHEAD = pCharJoin->fBIGHEAD;
	DxSkinAniControl::m_fBIGHAND = pCharJoin->fBIGHAND;

	// UI에 게임 화면 진입 알리기
	//GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_EnterGameStage );
}

void DxGameStage::MsgLobbyCharPutonEx(NET_MSG_GENERIC* nmg)
{
    std::vector<GLMSG::CHARPUTON_EX_PACK_DATA> vecRecv;

    MSGPACK_AUTO_UNPACK<GLMSG::CHARPUTON_EX_PACK_DATA> autoUnpack;
    if( autoUnpack.GetVecData( nmg, vecRecv ) == false )
        return;
    
    BOOST_FOREACH( const GLMSG::CHARPUTON_EX_PACK_DATA& value, vecRecv )
    {
        m_pCharJoin->m_dwPutOnItems++;
        m_pCharJoin->m_CharData2.m_PutOnItems[value.nSlot] = value.PutOnItem;

		CDebugSet::ToListView( "NET_MSG_LOBBY_CHAR_PUTON_ITEM" );
		if ( m_pCharJoin->CheckReceive(m_bLoginProcessLog) )
		{
			NET_MSG_GENERIC nmg;
			nmg.SetSize( sizeof( NET_MSG_GENERIC ) );
			nmg.nType = NET_MSG_LOBBY_GAME_COMPLETE;

			GLLobyCharClient::GetInstance().MsgProcess( &nmg );
		}
    }

}

void DxGameStage::MsgLobbyCharItem(NET_MSG_GENERIC* nmg)
{
    std::vector<GLMSG::SNETLOBBY_INVENITEM_PACK> vecRecv;

    MSGPACK_AUTO_UNPACK<GLMSG::SNETLOBBY_INVENITEM_PACK> autoUnpack;
    if( autoUnpack.GetVecData( nmg, vecRecv ) == false )
        return;

    BOOST_FOREACH( GLMSG::SNETLOBBY_INVENITEM_PACK& value, vecRecv )
    {
        SINVENITEM &InvenItem = value.Data;


        ++m_pCharJoin->m_dwReceiveInvenItem;
        GLInventory &cInventory = m_pCharJoin->m_CharData2.m_cInventory;
        BOOL bOk = cInventory.InsertItem ( InvenItem.sItemCustom, InvenItem.wPosX, InvenItem.wPosY, true );

        CDebugSet::ToListView ( "NET_MSG_LOBBY_CHAR_ITEM" );
        if ( m_pCharJoin->CheckReceive(m_bLoginProcessLog) )
        {
            LobbyGameComplete();
        }
    }
}

void DxGameStage::MsgLobbyCharVietnamItem(NET_MSG_GENERIC* nmg)
{
    if (m_pGaeaClient->GetServiceProvider() != SP_VIETNAM)
        return;

    GLMSG::SNETLOBBY_VIETNAM_INVENITEM *pNetInvenItem = (GLMSG::SNETLOBBY_VIETNAM_INVENITEM *) nmg;
    SINVENITEM &InvenItem = pNetInvenItem->Data;

    ++m_pCharJoin->m_dwReceiveVNInvenItem;
    GLInventory &cInventory = m_pCharJoin->m_CharData2.m_cVietnamInventory;
    BOOL bOk = cInventory.InsertItem ( InvenItem.sItemCustom, InvenItem.wPosX, InvenItem.wPosY, true );

    CDebugSet::ToListView ( "NET_MSG_LOBBY_CHAR_VIETNAM_ITEM" );
    if ( m_pCharJoin->CheckReceive(m_bLoginProcessLog) )
    {
        LobbyGameComplete();
    }
}

void DxGameStage::MsgLobbyCharSkill(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETLOBBY_CHARSKILL *pNetMsg = (GLMSG::SNETLOBBY_CHARSKILL *) nmg;

    m_pCharJoin->m_dwReceiveSkill += pNetMsg->wNum;
    for ( WORD i=0; i<pNetMsg->wNum; ++i )
    {
        m_pCharJoin->m_CharData2.m_ExpSkills.insert ( std::make_pair ( pNetMsg->sSKILL[i].sNativeID.dwID, pNetMsg->sSKILL[i] ) );

		PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( pNetMsg->sSKILL[i].sNativeID.dwID );
		if ( pSkill && pSkill->m_sEXT_DATA.idDisguiseSkill != NATIVEID_NULL() )
		{
			PGLSKILL pDisguiseSkill = GLSkillMan::GetInstance().GetData ( pSkill->m_sEXT_DATA.idDisguiseSkill );
			if ( pDisguiseSkill )
			{
				std::pair<DWORD,SCHARSKILL> pairMap = 
					std::make_pair( pNetMsg->sSKILL[i].sNativeID.dwID, SCHARSKILL(pDisguiseSkill->m_sBASIC.sNATIVEID, pNetMsg->sSKILL[i].wLevel) );
				m_pCharJoin->m_CharData2.m_ExpChangeSkills.insert( pairMap );
			}
		}
    }

    CDebugSet::ToListView ( "NET_MSG_LOBBY_CHAR_SKILL" );
    if ( m_pCharJoin->CheckReceive(m_bLoginProcessLog) )
    {
        LobbyGameComplete();
    }
}

void DxGameStage::MsgLobbyQuestEnd(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETLOBBY_QUEST_END *pNetMsg = (GLMSG::SNETLOBBY_QUEST_END *) nmg;

#ifdef _TEMP_TRACING_LOG
	// QUEST_BUFFER_OVER
	sc::writeLogDebug(sc::string::format("MsgLobbyQuestEnd dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwChaNum %6%", pNetMsg->dwQID, pNetMsg->dwSeq, pNetMsg->bLastSeq, pNetMsg->dwLengthStream, pNetMsg->dwSize, pNetMsg->dwChaNum));
#endif

    GLQUESTPROG sQUEST_PROG_END;

	if( pNetMsg->dwSeq != 0 && pNetMsg->bLastSeq == FALSE )
	{
		// 분할 메세지중에서 처음을 받으면 기존 데이터 초기화
		// 서버의 워커 스레드가 하나이므로 상관없음. 만약 순서가 섞여서 들어온다면 아래에서 에러 로그 기록후 리턴
		if( pNetMsg->dwSeq == 1 )
		{
			m_QuestByteStreamID = 0;
			m_sQuestByteStream.ClearBuffer();
		}

		m_QuestByteStreamID = pNetMsg->dwQID;
		m_sQuestByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );
		
		return;
	}
	else if( pNetMsg->dwSeq != 0 && pNetMsg->bLastSeq == TRUE )
	{
		if( m_QuestByteStreamID != pNetMsg->dwQID )
		{
			sc::writeLogError(sc::string::format("MsgLobbyQuestEnd ID is not match (LAST) m_QuestByteStreamID : %1%, pNetMsg->dwQID : %2%", GLQuestMan::GetInstance().m_QuestByteStreamID, pNetMsg->dwQID));

			m_QuestByteStreamID = 0;
			m_sQuestByteStream.ClearBuffer();

			return;
		}

		m_sQuestByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );

		//	Note : 퀘스트 생성.
		//
		sQUEST_PROG_END.GET_BYBUFFER ( m_sQuestByteStream );
	}
	else
	{
		//	Note : 버퍼 생성.
		//
		se::ByteStream sByteStream;
		sByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );

		//	Note : 퀘스트 생성.
		//
		sQUEST_PROG_END.GET_BYBUFFER ( sByteStream );
	}

    //	Note : 리스트에 등록.
    //

    GLQuestPlay::MAPQUEST& mapQUEST = m_pCharJoin->m_CharData2.m_cQuestPlay.GetQuestEnd();
    if ( m_pCharJoin->m_CharData2.m_cQuestPlay.InsertEnd ( sQUEST_PROG_END, false ) )
    {
        // 성공 할때 증가
        ++m_pCharJoin->m_dwReceiveQuestEnd;
    }

    if ( m_pCharJoin->CheckReceive(m_bLoginProcessLog) )
    {
       LobbyGameComplete();
    }
}

void DxGameStage::MsgLobbyQuestProg(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETLOBBY_QUEST_PROG *pNetMsg = (GLMSG::SNETLOBBY_QUEST_PROG *) nmg;

#ifdef _TEMP_TRACING_LOG
	// QUEST_BUFFER_OVER
	sc::writeLogDebug(sc::string::format("MsgLobbyQuestProg dwQID %1%, dwSeq %2%, bLastSeq %3%, dwLengthStream %4%, dwSize %5%, dwChaNum %6%", pNetMsg->dwQID, pNetMsg->dwSeq, pNetMsg->bLastSeq, pNetMsg->dwLengthStream, pNetMsg->dwSize, pNetMsg->dwChaNum));
#endif

	GLQUESTPROG sQUEST_PROG_NEW;

	if( pNetMsg->dwSeq != 0 && pNetMsg->bLastSeq == FALSE )
	{
		// 분할 메세지중에서 처음을 받으면 기존 데이터 초기화
		// 서버의 워커 스레드가 하나이므로 상관없음. 만약 순서가 섞여서 들어온다면 아래에서 에러 로그 기록후 리턴
		if( pNetMsg->dwSeq == 1 )
		{
			m_QuestByteStreamID = 0;
			m_sQuestByteStream.ClearBuffer();
		}

		m_QuestByteStreamID = pNetMsg->dwQID;
		m_sQuestByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );
		
		return;
	}
	else if( pNetMsg->dwSeq != 0 && pNetMsg->bLastSeq == TRUE )
	{
		if( m_QuestByteStreamID != pNetMsg->dwQID )
		{
			sc::writeLogError(sc::string::format("MsgLobbyQuestProg ID is not match (LAST) m_QuestByteStreamID : %1%, pNetMsg->dwQID : %2%", m_QuestByteStreamID, pNetMsg->dwQID));

			m_QuestByteStreamID = 0;
			m_sQuestByteStream.ClearBuffer();

			return;
		}

		m_sQuestByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );

		//	Note : 퀘스트 생성.
		//
		sQUEST_PROG_NEW.GET_BYBUFFER ( m_sQuestByteStream );
	}
	else
	{
		se::ByteStream sByteStream;
		sByteStream.WriteBuffer ( pNetMsg->arraySTREAM, pNetMsg->dwLengthStream );

		//	Note : 퀘스트 생성.
		//
		sQUEST_PROG_NEW.GET_BYBUFFER ( sByteStream );
	}

    //	Note : 리스트에 등록.
    //

    GLQuestPlay::MAPQUEST& mapQUEST = m_pCharJoin->m_CharData2.m_cQuestPlay.GetQuestProc();
    if ( m_pCharJoin->m_CharData2.m_cQuestPlay.InsertProc ( sQUEST_PROG_NEW, false ) )
    {
        ++m_pCharJoin->m_dwReceiveQuestProc;
    }

    if ( m_pCharJoin->CheckReceive(m_bLoginProcessLog) )
    {
        LobbyGameComplete();
    }
}

void DxGameStage::MsgLobbyQuestProgItem( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_QUEST_PROG_INVEN_INSERT2* pNetMsg = ( GLMSG::SNETPC_QUEST_PROG_INVEN_INSERT2* ) nmg;

	GLQUESTPROG* pPROG = m_pCharJoin->m_CharData2.m_cQuestPlay.FindProc( pNetMsg->dwQID );
	if ( !pPROG )
		return;

	pPROG->m_sINVENTORY.InsertItem( pNetMsg->Data.sItemCustom, pNetMsg->Data.wPosX, pNetMsg->Data.wPosY );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_Quest_ProgItemInsert, "-dw", pNetMsg->dwQID );
	//	Note : 인터페이스에 갱신 요청.
	//!! SF_TODO
	//m_pInterface->REFRESH_QUEST_WINDOW ();
}

void DxGameStage::MsgLobbyChargeItem(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETLOBBY_CHARGE_ITEM *pNetMsg = (GLMSG::SNETLOBBY_CHARGE_ITEM *) nmg;

    ++m_pCharJoin->m_dwReceiveChargeItem;
    m_pCharJoin->m_CharData2.ADDSHOPPURCHASE ( pNetMsg->szPurKey, pNetMsg->nidITEM );

    if ( m_pCharJoin->CheckReceive(m_bLoginProcessLog) )
    {
        LobbyGameComplete();
    }
}

void DxGameStage::MsgLobbyItemCoolTime(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETLOBBY_ITEM_COOLTIME *pNetMsg = (GLMSG::SNETLOBBY_ITEM_COOLTIME *) nmg;

    m_pCharJoin->m_dwReceiveItemCoolTime += pNetMsg->dwNum;
    for ( WORD i=0; i<pNetMsg->dwNum; ++i )
    {
        ITEM_COOLTIME& sCoolTime = pNetMsg->sItemCoolTime[i];
        if ( pNetMsg->emCoolType == EMCOOL_ITEMID )
        {
            m_pCharJoin->m_CharData2.m_mapCoolTimeID.insert(
                SCHARDATA2::COOLTIME_MAP_VALUE(sCoolTime.dwCoolID, sCoolTime));
        }
        else if ( pNetMsg->emCoolType == EMCOOL_ITEMTYPE )
        {
            m_pCharJoin->m_CharData2.m_mapCoolTimeType.insert(
                SCHARDATA2::COOLTIME_MAP_VALUE(sCoolTime.dwCoolID, sCoolTime));
        }
    }

    CDebugSet::ToListView ( "NET_MSG_LOBBY_ITEM_COOLTIME" );
    if ( m_pCharJoin->CheckReceive(m_bLoginProcessLog) )
    {
        LobbyGameComplete();
    }
}

void DxGameStage::MsgLobbyInstanceList(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETLOBBY_INSTANCE_LIST* pNetMsg((GLMSG::SNETLOBBY_INSTANCE_LIST*)nmg);	
	if ( NULL == pNetMsg )
		return;

	const DWORD nInstance(pNetMsg->nInstanceInformation);
	for ( DWORD _i(0); _i < nInstance; ++_i )
	{
		const InstanceSystem::InstanceInformationClient& _instanceInformation = pNetMsg->instanceInformation[_i];
		m_pGaeaClient->addInstanceInformation(_instanceInformation);
	}	
}

void DxGameStage::MsgLobbyCharDefault(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETLOBBY_CHAR_DEFAULT *pNetMsg = (GLMSG::SNETLOBBY_CHAR_DEFAULT *)nmg;

	GLHIDESET::fInvisiblePerceiveAngle = pNetMsg->fInvisiblePerceiveAngle;
	GLHIDESET::fInvisiblePerceiveRadius = pNetMsg->fInvisiblePerceiveRadius;	
	GLHIDESET::fInvisiblePerceiveURadius = 1.0f / pNetMsg->fInvisiblePerceiveRadius;
	GLHIDESET::fInvisiblePerceiveLevel = pNetMsg->fInvisiblePerceiveLevel;	
	GLHIDESET::fInvisibleUpdateTime = pNetMsg->fInvisibleUpdateTime;
}

void DxGameStage::MsgLobbyClubInfo(NET_MSG_GENERIC* nmg)
{
    // NET_MSG_LOBBY_CLUB_INFO
    // GLMSG::SNETLOBBY_CLUB_INFO* pPacket = (GLMSG::SNETLOBBY_CLUB_INFO *) nmg;
    MIN_STATIC_ASSERT(GLMSG::SNETLOBBY_CLUB_INFO::VERSION <= 2);

    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    
    GLMSG::SNETLOBBY_CLUB_INFO RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;    
    
    GLClubClient* pClub = m_pCharJoin->GetClub();
    pClub->SetDbNum(RecvData.dwClubID);
    pClub->SetName(RecvData.m_ClubName.c_str());

    pClub->SetMasterCharDbNum(RecvData.dwMasterID);
    pClub->SetMasterCharLevel(static_cast<WORD>(RecvData.dwMasterLevel));
    pClub->SetMasterName(RecvData.m_MasterName.c_str());
    pClub->m_dwCDCertifior = RecvData.dwCDCertifior;

    pClub->m_dwRank = RecvData.dwRank;
    pClub->m_dwMarkVER = RecvData.dwMarkVER;

    pClub->m_tOrganize = RecvData.tOrganize;
    pClub->m_tDissolution = RecvData.tDissolution;

    pClub->m_dwBattleWin  = RecvData.dwBattleWin;
    pClub->m_dwBattleLose = RecvData.dwBattleLose;
    pClub->m_dwBattleDraw = RecvData.dwBattleDraw;

    pClub->m_dwAllianceBattleWin  = RecvData.dwAllianceBattleWin;
    pClub->m_dwAllianceBattleLose = RecvData.dwAllianceBattleLose;
    pClub->m_dwAllianceBattleDraw = RecvData.dwAllianceBattleDraw;

    // 클럽 멤버원 지정    
    m_pCharJoin->m_dwClubMember   = RecvData.dwMEMBER_NUM;
    m_pCharJoin->m_dwClubAlliance = RecvData.dwALLIANCE_NUM;
    m_pCharJoin->m_dwClubBattle   = RecvData.dwBATTLE_NUM;

    pClub->m_dwAlliance = RecvData.dwAlliance;
    pClub->PublicSet(RecvData.m_PublicType);
    pClub->AutoKickOutSet(RecvData.m_AutoKickOut);
}

void DxGameStage::MsgLobbyClubInfoEx(NET_MSG_GENERIC* nmg)
{
    // NET_MSG_LOBBY_CLUB_INFO_EX
    // GLMSG::SNETLOBBY_CLUB_INFO_EX* pPacket = (GLMSG::SNETLOBBY_CLUB_INFO_EX*) nmg;
    MIN_STATIC_ASSERT(GLMSG::SNETLOBBY_CLUB_INFO_EX::VERSION <= 2);

    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;

    GLMSG::SNETLOBBY_CLUB_INFO_EX RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    GLClubClient* pClub = m_pCharJoin->GetClub();
    pClub->SetNotice(RecvData.m_Notice.c_str());
    pClub->SetNoticeDate(RecvData.m_tNoticeDate);
    pClub->SetNoticeChaName(RecvData.m_NoticeChaName.c_str());
    pClub->SetNoticeNewbie(RecvData.m_NoticeNewbie.c_str());

    m_pCharJoin->m_bReceiveClubInfo = true;

    if (m_pCharJoin->CheckReceive(m_bLoginProcessLog))
    {
        LobbyGameComplete();
    }
}
/*
void DxGameStage::MsgLobbyClubMember(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETLOBBY_CLUB_MEMBER* pPacket = (GLMSG::SNETLOBBY_CLUB_MEMBER*) nmg;

    m_pCharJoin->m_dwReceiveClubMember += pPacket->m_MemberNum;
    for (DWORD i=0; i<pPacket->m_MemberNum; ++i)
    {
        m_pCharJoin->m_pMyClub->AddMember(
            pPacket->m_Members[i].m_ChaDbNum,
            pPacket->m_Members[i].m_Name,
            pPacket->m_Members[i].m_Flag,
            pPacket->m_Members[i].m_ChaLevel,
            pPacket->m_Members[i].m_LastPlayTime,
            pPacket->m_Members[i].m_Online);
    }

    if (m_pCharJoin->CheckReceive())
    {
        LobbyGameComplete();
    }
}
*/
void DxGameStage::MsgLobbyClubAlliance(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETLOBY_CLUB_ANCE_2CLT *pNetMsg = (GLMSG::SNETLOBY_CLUB_ANCE_2CLT *) nmg;

    m_pCharJoin->m_dwReceiveClubAlliance += pNetMsg->dwAllianceNum;
    for ( DWORD i=0; i<pNetMsg->dwAllianceNum; ++i )
    {
        m_pCharJoin->m_pMyClub->AddAlliance(
            pNetMsg->sALLIANCE[i].m_dwID,
            pNetMsg->sALLIANCE[i].m_szName,
            pNetMsg->sALLIANCE[i].m_szMasterName,
            pNetMsg->sALLIANCE[i].m_MarkVersion);
    }

    if ( m_pCharJoin->CheckReceive(m_bLoginProcessLog) )
    {
       LobbyGameComplete();
    }
}

void DxGameStage::MsgLobbyClubBattle(NET_MSG_GENERIC* nmg)
{
    GLMSG::SNETLOBBY_CLUB_BATTLE *pNetMsg = (GLMSG::SNETLOBBY_CLUB_BATTLE *) nmg;

    m_pCharJoin->m_dwReceiveClubBattle += pNetMsg->dwBattleNum;
    GLCLUBBATTLE sBattle;

    for ( DWORD i=0; i<pNetMsg->dwBattleNum; ++i )
    {
        sBattle = pNetMsg->sBATTLE[i];
        m_pCharJoin->m_pMyClub->ADDBATTLECLUB ( sBattle ); 
    }

    if ( m_pCharJoin->CheckReceive(m_bLoginProcessLog) )
    {
        LobbyGameComplete();
    }
}

BOOL DxGameStage::GetUsedMaterialSystem()
{
	if ( !m_pGaeaClient )
		return FALSE;

	return m_pGaeaClient->GetUsedMaterialSystem();
}

void DxGameStage::MsgGCTRLConfront(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNETPC_REQ_CONFRONT_TAR *pNetMsg = (GLMSG::SNETPC_REQ_CONFRONT_TAR *)nmg;

	std::tr1::shared_ptr<GLCharClient> pChar = m_pGaeaClient->GetChar ( pNetMsg->dwID );
	if( NULL == pChar )
		return;		

	if( TRUE == RANPARAM::bDIS_CONFT || 
		true == m_pGaeaClient->GetTournamentClient()->GetIsTournamentView() )
	{
		//	Note : 대련 거절시에 메시지 전송.
		GLMSG::SNETPC_REQ_CONFRONT_ANS NetMsgAns( pNetMsg->emTYPE, pNetMsg->dwID, EMCONFRONT_REFUSE, pNetMsg->sOption );
		m_pGaeaClient->NETSENDTOAGENT( &NetMsgAns );

		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, 
			sc::string::format( ID2GAMEINTEXT( "EMCONFRONT_REFUSE_AUTO" ), pChar->GetName() ).c_str() );

		return;
	}

	if( m_pGaeaClient->GetCharacter()->VehicleState() || pChar->VehicleState() )
	{
		GLMSG::SNETPC_REQ_CONFRONT_ANS NetMsgAns( pNetMsg->emTYPE, pNetMsg->dwID, EMCONFRONT_VEHICLE, pNetMsg->sOption );
		m_pGaeaClient->NETSENDTOAGENT( &NetMsgAns );

		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "EMCONFRONT_BIKE_FAIL" ) );

		return;
	}

	m_pGaeaClient->GetCharacter()->SetAnsData( pNetMsg );

	lua_tinker::table tbData( GLWidgetScript::GetInstance().GetLuaState() );
	tbData.set( 1, pNetMsg->sOption.bBOUND );
	tbData.set( 2, pNetMsg->sOption.wRECOVER );
	tbData.set( 3, pNetMsg->sOption.fHP_RATE );
	tbData.set( 4, pNetMsg->sOption.fTAR_HP_RATE );

	GLWidgetCallFuncValue args[ 2 ];
	args[ 0 ].SetString( pChar->GetName() );
	args[ 1 ].SetTable( &tbData );

	GLWidgetScript::GetInstance().LuaCallFunc< void >( 
		NSWIDGET_SCRIPT::g_strFunc_Conft, args, 2 );
}

void DxGameStage::MsgGCTRTradeTar(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_TRADE_TAR* pPacket = (GLMSG::SNET_TRADE_TAR*) nmg;
	if (pPacket->m_Crc32 != sc::string::getCrc32(pPacket, pPacket->Size()-sizeof(unsigned int)))
		return;

	GLLandManClient* pLand = m_pGaeaClient->GetActiveMap();
	if (!pLand)
		return;

	std::tr1::shared_ptr<GLCharClient> pChar = pLand->GetChar( pPacket->dwMasterID );
	if (!pChar)
		return;

	std::string strName(pChar->GetName());

	//	Note : 거래 자동 거부.
	//
	bool bPMARKETOPEN = m_pGaeaClient->GetCharacter()->m_sPMarket.IsOpen();			

	// 토너먼트중 거래를 할수가 없다.
	// TOURNAMENT_UNIQ_DISABLE
	bool bTOURNAMENT = m_pGaeaClient->GetTournamentClient()->GetIsTournamentView();

	bool bIsBlock = false;
	std::tr1::shared_ptr<SFRIEND> spFriend = m_pGaeaClient->GetFriendClient()->GetData( strName.c_str() );
	if( spFriend != NULL )
		bIsBlock = spFriend->IsBLOCK();

	GLTradeClient* pTradeClient = m_pGaeaClient->GetTradeClient();
	pTradeClient->GetTarTrade().SetTarget( pPacket->dwMasterID );

	if ( RANPARAM::bDIS_TRADE || bPMARKETOPEN || bIsBlock || bTOURNAMENT )
	{
		//	P2P 거래 거절 처리
		/*GLMSG::SNET_TRADE_TAR_ANS NetMsg;
		NetMsg.dwChaNum = pChar->CharDbNum();
		NetMsg.dwMasterID = pPacket->dwMasterID;
		NetMsg.emAns = EMTRADE_CANCEL;
		NetMsg.m_Crc32 = sc::string::getCrc32(&NetMsg, NetMsg.Size()-sizeof(unsigned int));
		m_pGaeaClient->NETSENDTOAGENT(&NetMsg);*/

		pTradeClient->ReqTradeAns( false );

		std::string strText(sc::string::format(ID2GAMEINTEXT("TRADE_FB_CANCEL_AUTO"), strName.c_str()));
		m_pGaeaClient->PrintMsgText(NS_UITEXTCOLOR::DISABLE, strText.c_str());
		return;
	}

	GLWidgetScript::GetInstance().LuaCallFunc<void>(NSWIDGET_SCRIPT::g_strFunc_TradeTarMessageBox, "-s", strName.c_str());	
}

void DxGameStage::MsgDropOutForced(NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_DROP_OUT_FORCED *pNetMsg = (GLMSG::SNET_DROP_OUT_FORCED *) nmg;
	DxGlobalStage* pGlobalStage = m_pGaeaClient->GetGlobalStage();
	// 로그인 프로세스 확인을 위한 로그
	if( m_pGlobalStage->m_bLoginProcessLog) sc::writeLogError(std::string("LOGIN ERROR. Forced Drop Out "));

	//sc::writeLogError(std::string("overlapping login"));

	//const std::string strMsg( (pNetMsg->emForced==EMDROPOUT_REQLOGIN)? CGameTextMan::GetInstance().GetGameInText("PLAYINTERFACE_8"):CGameTextMan::GetInstance().GetGameInText("PLAYINTERFACE_9") );
	const int nType((pNetMsg->emForced==EMDROPOUT_REQLOGIN)? EMDROPOUT_REQLOGIN : EMDROPOUT_ERROR);
	GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_CloseConnection, "-n", nType);

	m_pGaeaClient->CloseConnect();
}

void DxGameStage::LobbyGameComplete()
{
	CDebugSet::ToListView ( "NET_MSG_LOBBY_GAME_COMPLETE" );
	m_pGlobalStage->ToChangeStage( DxGlobalStage::EM_STAGE_GAME );
}
