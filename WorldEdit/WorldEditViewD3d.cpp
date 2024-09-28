#include "stdafx.h"

#include "../VisualMaterialLib/Manager/DxVMManager.h"
#include "../VisualMaterialLib/Manager/NSVMRender.h"
#include "../VisualMaterialLib/Util/NSNearPointLight.h"

#include "../MfcExLib/ExLibs/ToolBarEx.h"
#include "../RanLogic/RANPARAM.h"

#include "WorldEdit.h"

#include "../EngineLib/Common/profile.h"
#include "../EngineLib/GUInterface/Cursor.h"
#include "../EngineLib/DxEffect/NSEffectThread.h"
#include "../EngineLib/DxEffect/TexEff/NSTexEffVisualMaterial.h"
#include "../EngineLib/DxEffect/Frame/DxEffectTiling.h"	
#include "../EngineLib/DxEffect/Frame/DxEffectMan.h"
#include "../EngineLib/DxSound/StaticSoundMan.h"
#include "../EngineLib/DxTools/DebugSet.h"
#include "../EngineLib/DxTools/EditMeshs.h"
#include "../EngineLib/DxTools/PostProcessing/NSLightShafts.h"
#include "../EngineLib/DxTools/PostProcessing/NSBloomHDR.h"
#include "../EngineLib/DxTools/PostProcessing/NSGlow.h"
#include "../EngineLib/DxTools/Occlusion/NSOcclusionCulling.h"
#include "../EngineLib/DxTools/Occlusion/DxOcclusionQuery.h"
#include "../EngineLib/DxTools/MaterialSkin/NSMaterialSkinManager.h"
#include "../EngineLib/DxTools/Material/NSMaterialManager.h"
#include "../EngineLib/DxTools/Material/MaterialMesh.h"
#include "../EngineLib/DxTools/Deffered/NSDeffered.h"
#include "../EngineLib/DxTools/CubeMap/NSCubeMapCulling.h"
#include "../EngineLib/DxTools/SkyBox/DxSkyBoxMan.h"
#include "../EngineLib/DxTools/Light/NSShadowLight.h"
#include "../EngineLib/DxTools/Light/DxShadowLight.h"
#include "../EngineLib/DxTools/DxBackUpRendTarget.h"
#include "../EngineLib/DxTools/dxshadowmap.h"
#include "../EngineLib/DxTools/DxSurfaceTex.h"
#include "../EngineLib/DxTools/DxEnvironment.h"
#include "../EngineLib/DxTools/DxGlowMan.h"
#include "../EngineLib/DxTools/DxCubeMap.h"
#include "../EngineLib/DxTools/DxSkyMan.h"
#include "../EngineLib/DxTools/DxEditMat.h"
#include "../EngineLib/DxTools/DxPostProcess.h"
#include "../EngineLib/DxTools/DxDynamicVB.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxTools/DxInputDevice.h"
#include "../EngineLib/DxTools/DxfontMan.h"
#include "../EngineLib/DxTools/dxWeatherMan.h"
#include "../EngineLib/DxTools/DxFogMan.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../EngineLib/DxTools/NSBakeCommon.h"
#include "../EngineLib/DxTools/Collision.h"
#include "../EngineLib/DxTools/CubeMap/NSCubeMapUtil.h"
#include "../EngineLib/G-Logic/GLPeriod.h"
#include "../EngineLib/Common/SubPath.h"
#include "../EngineLib/DxLand/DxPiece/NsOCTree.h"
#include "../EngineLib/DxLand/LightMap/NSLightMapFX.h"
#include "../EngineLib/DxLand/DxPrefabMan.h"
#include "../EngineLib/Water/NSWaterMain.h"
#include "../EngineLib/DxLand/DxPiece/NSPieceQuickSort.h"
#include "../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"

#include "../EngineLib/DxResponseMan.h"

#include "../RanLogicClient/Stage/DxGlobalStage.h"
#include "../RanLogicClient/Char/GLCharacter.h"
#include "../RanLogicClient/GLGaeaClient.h"

#include "./DirectionEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "State/WorldStateMachine.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL					g_bFPS_VISIBLE;
extern BOOL					g_bCONSOLE_VISIBLE;
extern BOOL					g_bFORCE_FOG_OFF;
extern DWORD				g_dwCLOSED_POINT_INDEX;
extern D3DXVECTOR3			g_vCLOSED_POINT;
extern D3DXVECTOR3			g_vPICK_RAY_DIR;
extern D3DXVECTOR3			g_vPICK_RAY_ORIG;
extern std::string			g_strPICK_FRAME;
extern std::string			g_strPICK_TEXTURE;
extern BOOL					g_bPLAY_ANIMATION;
extern BOOL					g_bWORLD_TOOL;
extern BOOL					g_bSKINOBJECT_RENDER_NAME;
extern BOOL					g_bWIREFRAME_VIEW;
BOOL				        g_bSET_PIECE(FALSE);			// 조각 파일 심는거 가능한가 체크.
D3DXVECTOR3			        g_vDYNAMIC_POINT(0.f,0.f,0.f);	// 계속 움직이는 마우스 포인터
D3DXVECTOR3	                g_vSNAP_POINT(0.f,0.f,0.f);	// 스냅을 위한 위치값
DxPiece*			        g_pPIECE_SNAP(NULL);			// 스냅용 Piece?

DxSetLandMan*				CWorldEditView::g_rSetLandMan(NULL);
DxPrefab*					CWorldEditView::g_pPrefab(NULL);					// 선택된 Prefab
RENDER_TYPE					CWorldEditView::m_RenderType(RENDER_TEXTURE_LIGHTMAP);
WATER_TYPE					CWorldEditView::m_WaterType(RENDER_WATER_RT_REFLECTION);
DWORD						CWorldEditView::m_dwRenderPSType(ID_RENDER_PS_3_0_HIGH);
DWORD						CWorldEditView::m_dwRenderCharType(ID_RENDER_CHAR_SS);
DxPiece*					CWorldEditView::g_pPIECE(NULL);						// 선택된 Piece?
DxPiece*					CWorldEditView::g_pDYNAMIC_PIECE(NULL);				// 현 마우스가 가르키는 Piece
DxPieceEdit*				CWorldEditView::g_pPIECEEDIT(NULL);					// 땅에 심을 Piece의 종류
TSTRING						CWorldEditView::g_strPIECENAME(_T(""));				// 땅에 심을 Piece의 이름

void CWorldEditView::TempCreateChar ()
{
	m_pCharacter->Create ( m_spDxLandMan->GetNaviMesh(), NULL, NULL, 0, m_pd3dDevice, m_spDxLandMan.get() );
}

void CWorldEditView::SkinLoadForTool( const TCHAR* szName )
{
	if ( !m_pCharacter )
		return;

	m_pCharacter->SkinLoadForTool( GetD3DDevice(), szName );
}

_VIEWTYPE CWorldEditView::m_ViewType = FRAME_VIEW;
_SET_TYPE CWorldEditView::m_SetType = BASE_VIEW;
_CUR_TYPE CWorldEditView::m_CurType = _CUR_SELECT;
BOOL	  CWorldEditView::g_bMiniMap = FALSE;
BOOL	  CWorldEditView::g_bOrthoView = FALSE;
BOOL	  CWorldEditView::g_bOcclsionPointRender = FALSE;
BOOL	  CWorldEditView::g_bOcclsionOctreeRender = FALSE;
BOOL	  CWorldEditView::g_bCubeMapPointRender = FALSE;
BOOL	  CWorldEditView::g_bCubeMapOctreeRender = FALSE;
BOOL	  CWorldEditView::g_bSpecCamaraMode = FALSE;


HRESULT CWorldEditView::ConfirmDevice ( D3DCAPSQ* pCaps, DWORD dwBehavior, D3DFORMAT Format )
{
	
	if ( dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING )
		return E_FAIL;

    return S_OK;
}

HRESULT CWorldEditView::OneTimeSceneInit()
{
	// 초기값을 등록한다.
	g_rSetLandMan = &m_SetLandMan;

	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;	

	// 각종 장치 관리.
	m_pEngineDevice->OneTimeSceneInit(
        pFrame->m_szAppPath,
        this,
        RANPARAM::strFontType,
        false,
        RENDERPARAM::emLangSet,
        RANPARAM::strGDIFont);

	// 에디트 메트릭스 생성
	m_pDxEditmat = new DxEditMat;

    CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

    std::string FullPath(pFrame->m_szAppPath);
    FullPath.append(pPath->ObjFileEdit()); //SUBPATH::OBJ_FILE_EDIT);
	m_pDxEditmat->SetParam(FullPath, m_hWnd);
	m_EditBox.SetPath(FullPath.c_str());

	// 위치 지정 구 초기화.
	m_p3dPosMesh = new DxFrameMesh(TRUE);
    FullPath = pFrame->m_szAppPath;
	FullPath.append(pPath->ObjFileEdit()); //SUBPATH::OBJ_FILE_EDIT);
	m_p3dPosMesh->SetFile( FullPath.c_str(), "3dPos.x" );

	if ( m_pGlobalStage->GetGaeaClient() )
	{
		m_pGlobalStage->GetGaeaClient()->KeySettingLoad();
	}

	g_bWORLD_TOOL = TRUE;

	WorldStateMachine::InitWorldStateMachine();

	return S_OK;
}

HRESULT CWorldEditView::InitDeviceObjects()
{
	HRESULT hr = S_OK;

	// 각종 장치 초기화
	//
	m_pEngineDevice->InitDeviceObjects ( m_pd3dDevice );

	DxFontMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	CD3DFontPar* pD3dFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	//	Note	:	디버그셋 경로 설정 및 초기화
	CDebugSet::InitDeviceObjects( pD3dFont9 );

	// 게임 커서.
	//
	CCursor::GetInstance().InitDeviceObjects ();

	// 뷰포트.
	//
    DxViewPort::GetInstance().SetToolCamera();

	D3DXVECTOR3 vFromPt		= D3DXVECTOR3( 0.0f, 60.0f, -60.0f );
	D3DXVECTOR3 vLookatPt	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );

	m_SetLandMan.InitDeviceObjects ( m_pd3dDevice );
	m_SetLandManPrefab.InitDeviceObjects ( m_pd3dDevice );

	// 옥트리 메니져 초기화.
	//
	m_spDxLandMan->InitDeviceObjects( m_pd3dDevice );

	m_ReplacePiece.InitDeviceObjects ( m_pd3dDevice );

	// 메트릭스 에디트의 초기화.
	//
	m_pDxEditmat->InitDeviceObjects ( m_pd3dDevice );
	m_p3dPosMesh->InitDeviceObjects ( m_pd3dDevice );
	m_EditBox.InitDeviceObjects ( m_pd3dDevice );

	m_pCharacter->Create ( m_spDxLandMan->GetNaviMesh(), NULL, NULL, 0, m_pd3dDevice, m_spDxLandMan.get() );

	m_CurType = _CUR_NONE;

	m_DxMapEditMan.InitDeviceObjects( m_pd3dDevice );
	m_DxPieceEdit.OnCreateDevice( m_pd3dDevice );

	TextureManager::GetInstance().EnableTextureAlphaTypeDebug();

	// [shhan][2015.01.12] Texture Thread Loading 을 하기 때문에, 굳이 VB, IB 까지 할 필요는 없다.
	//						또한 Thread 상에서 VB, IB, 들을 Lock 하는 것도 위험성이 있다.
	// Note : DynamicLoad를 사퓖E?것을 뫄寔다.
	//NSOCTREE::EnableDynamicLoad();

	return S_OK;
}

HRESULT CWorldEditView::RestoreDeviceObjects()
{
	HRESULT hr;

	// Baking 중인데 여기로 들어온다면...
	while ( NSOcclusionCulling::g_bBaking )
	{
		Sleep(1);
	}

	// Baking 중인데 여기로 들어온다면...
	while ( NSBakeCommon::g_bBaking )
	{
		Sleep(1);
	}

	// 각종 장치 설정
	//
	m_pEngineDevice->RestoreDeviceObjects ();

	// 뷰포트
	//
	DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)m_d3dsdBackBuffer.Width, 
		(float)m_d3dsdBackBuffer.Height, 5.f, 3000.0f );
	
	hr = DxViewPort::GetInstance().SetViewPort ();
	if ( FAILED(hr) )	return hr;

	m_SetLandMan.RestoreDeviceObjects ( m_pd3dDevice );
	m_SetLandManPrefab.RestoreDeviceObjects ( m_pd3dDevice );
	m_spDxLandMan->RestoreDeviceObjects ( m_pd3dDevice );
	m_ReplacePiece.RestoreDeviceObjects ( m_pd3dDevice );

	m_pCharacter->RestoreDeviceObjects ( m_pd3dDevice );

	m_p3dPosMesh->RestoreDeviceObjects ( m_pd3dDevice );
	m_sObjectMRS.OnResetDevice( m_pd3dDevice );
	
	m_DxMapEditMan.RestoreDeviceObjects ( m_pd3dDevice );
	m_DxPieceEdit.OnResetDevice( m_pd3dDevice );

	// 글로퓖E처리를 하기위해 사퓖E한다.
	DxGlowMan::GetInstance().SetProjectActiveON();
	DxPostProcess::GetInstance().SetProjectActiveON();

    SAFE_RELEASE( m_pSwapChainMain );
	GetD3DDevice()->GetSwapChain( 0, &m_pSwapChainMain );

	NSCubeMapUtil::NSCubeMapForPSF::OnResetDevice( m_pd3dDevice );

	//// [CascadeShadow] 작업보류중
	//CWorldEditView::m_dwRenderPSType(ID_RENDER_PS_2_0);

	//PROFILE_INIT();
	return S_OK;
}

void CWorldEditView::SetNSLightMapFX_SetRenderMode()
{
	// LightMap Rendering 모드 셋팅.
	switch ( m_RenderType )
	{
	case RENDER_LIGHTMAP:
		NSLightMapFX::SetRenderMode( NSLIGHTMAPDEF::LMRT_LIGHTMAP );
		break;
	case RENDER_LIGHTMAP_POINT:
		NSLightMapFX::SetRenderMode( NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT );
		break;
	case RENDER_LIGHTMAP_CHECKER:
		NSLightMapFX::SetRenderMode( NSLIGHTMAPDEF::LMRT_CHECKER );
		break;
	default:
		NSLightMapFX::SetRenderMode( NSLIGHTMAPDEF::LMRT_TEXTURE_LIGHTMAP );
		break;
	};
}

HRESULT CWorldEditView::FrameMove( BOOL bNotRendering )
{
	// GPU 딜레이
	m_dwDelayGPU = timeGetTime() - m_dwTimeGetTimePrev;

	// Baking 중인데 여기로 들어온다면...
	if ( NSOcclusionCulling::g_bBaking )
	{
		return S_OK;
	}

	// Baking 중인데 여기로 들어온다면...
	if ( NSBakeCommon::g_bBaking )
	{
		return S_OK;
	}


	CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;

	PROFILE_BLOCKSTART();
	PROFILE_BEGIN("FrameMove");

	//	float 정확도 향상
	CheckControlfp();

	// Note : 캐릭터의 이동속도 조절
	if( DxInputDevice::GetInstance().GetKeyState(DIK_MINUS)&DXKEY_UP )
	{
		m_pCharacter->OptionMoveDown();
	}

	if( DxInputDevice::GetInstance().GetKeyState(DIK_EQUALS)&DXKEY_UP )
	{
		m_pCharacter->OptionMoveUp();
	}

	// Note : 
	if (pMainFrame->m_wndTrueToolBar.GetSafeHwnd() != 0)
	{
		m_sObjectMRS.SetWVL( pMainFrame->m_wndTrueToolBar.m_comboMatrix.GetCurSel() );
	}

	if( (!g_bFORCE_FOG_OFF) && (m_CurType!=_CUR_GAME_TEST) && (m_CurType!=_CUR_CHAR_MOVE) && (m_CurType!=_CUR_SELECT_POINT) )
	{
		if( m_sObjectMRS.IsCollMode() )	
            m_CurType = _CUR_SELECT;
	}

	// 각종 장치 갱신
	m_pEngineDevice->FrameMove( m_fTime, m_fElapsedTime, m_bDefWin );

	SetNSLightMapFX_SetRenderMode();

	// Water Rendering 모드 셋팅.
	switch ( m_WaterType )
	{
	case RENDER_WATER_CUBEMAP:
		DxEnvironment::GetInstance().SetRenderEditMode( DxEnvironment::RENDER_WATER_CUBEMAP );
		break;
	case RENDER_WATER_DIFFUSE_TEXTURE:
		DxEnvironment::GetInstance().SetRenderEditMode( DxEnvironment::RENDER_WATER_DIFFUSE_TEXTURE );
		break;
	case RENDER_WATER_RT_REFLECTION:
	default:
		DxEnvironment::GetInstance().SetRenderEditMode( DxEnvironment::RENDER_WATER_RT_REFLECTION );
		break;
	};

	// 뷰포트
	D3DXVECTOR3 vPos(0.f,0.f,0.f);
	if( m_sObjectMRS.IsCollMode() ) vPos = g_vCLOSED_POINT;
	else							vPos = m_sObjectMRS.GetPosition();

	// 캐릭터 이동 때는 카메라 모드를 기본으로 셋팅함.
	if ( (m_CurType==_CUR_CHAR_MOVE) || (m_CurType==_CUR_GAME_TEST) )
	{
		g_bSpecCamaraMode = FALSE;
	}

	if ( g_bSpecCamaraMode )
	{
		DxViewPort::GetInstance().FrameMoveSpecCam( m_fElapsedTime );
	}
	else
	{
		if (WorldStateMachine::IsInMouse(this))
		{
			DxViewPort::GetInstance().FrameMoveMAX( m_fElapsedTime, vPos );
		}
		else
		{
			DxViewPort::GetInstance().FrameMove( m_fTime, m_fElapsedTime );
		}
	}	

	D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt();
	D3DXVECTOR3 &vLookatPt = DxViewPort::GetInstance().GetLookatPt();

	switch ( m_ViewType )
	{
	case FRAME_VIEW:    
    case PIECE_REPLACE_VIEW:
    case PIECE_SETPOS_VIEW:
    case EFFECT_SET_VIEW:
    case EFFECT_TILE_VIEW:
    case EFFECT_GRASS_VIEW:
	case WATER_SURF_VIEW:
	case PREFAB_VIEW:
		PROFILE_BEGIN("LandManSet");
		g_rSetLandMan->FrameMove ( m_fTime, m_fElapsedTime );
		PROFILE_END("LandManSet");
		break;
	
	case OCTREE_VIEW:
		m_spDxLandMan->FrameMove ( m_fTime, m_fElapsedTime, DxViewPort::GetInstance().GetClipVolume(), bNotRendering );
		break;

	case PIECE_VIEW:
		m_ReplacePiece.UpdateEffect ( m_fTime, m_fElapsedTime );
		m_ReplacePiece.FrameMove ( m_fTime, m_fElapsedTime );
		break;

	case TILE_EDIT_VIEW:
		switch ( m_SetType )
		{
		case BASE_VIEW:
			if ( Pick() )
			{
				m_DxMapEditMan.FindTiles ( g_vPICK_RAY_ORIG, g_vPICK_RAY_DIR );
			}
			break;

		case HEIGHT_VIEW:
			if ( Pick() )
			{
				m_DxMapEditMan.FindTiles ( g_vPICK_RAY_ORIG, g_vPICK_RAY_DIR );

				DWORD mKeyState = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );
				if ( mKeyState&(DXKEY_DOWNED) )
				{
					m_DxMapEditMan.ModifyHeight_MAIN ( m_pd3dDevice );			// 높이 수정
				}
			}
			break;

		case TILE_COLOR_VIEW:
			if ( Pick() )
			{
				m_DxMapEditMan.FindTiles ( g_vPICK_RAY_ORIG, g_vPICK_RAY_DIR );

				DWORD mKeyState = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );
				if ( mKeyState&(DXKEY_DOWNED) )
				{
					m_DxMapEditMan.ModifyTexUV_MAIN ( m_pd3dDevice );			// 텍스쳐 UV 수정
				}
			}
			break;

		case MAP_PIECE_VIEW:
			if ( Pick() )
			{
				m_DxMapEditMan.FindTiles ( g_vPICK_RAY_ORIG, g_vPICK_RAY_DIR );
				m_DxMapEditMan.SetPiece ( g_vPICK_RAY_ORIG, g_vPICK_RAY_DIR );	

				DWORD mKeyState = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );
				if ( mKeyState&(DXKEY_UP) )
				{
					m_DxMapEditMan.SetPiece ( m_pd3dDevice );						// 오틒E㎷?삽입
				}
				
			}
			break;
		};
		break;
	case FRAMEMESH_VIEW:
	//case STATICMESH_VIEW:
		m_DxPieceEdit.FrameMoveFrame( m_fTime, m_fElapsedTime );
		break;
	case STATICMESH_VIEW:
		m_DxPieceEdit.FrameMove( m_fTime, m_fElapsedTime );
		break;

	case CAMCOLL_VIEW:
		PROFILE_BEGIN("LandManSet");
		g_rSetLandMan->FrameMove ( m_fTime, m_fElapsedTime );
		PROFILE_END("LandManSet");
		break;
	};

	switch ( m_CurType )
	{
	case _CUR_NONE:
		break;

	//case _CUR_SELECT:
	//	CurSelect ();

	//	pMainFrame->m_cDlgEdit.UpdatePage_Always();

	//	break;

	case _CUR_SELECT_POINT:
		CurSelectPoint ();
		break;

	case _CUR_ROTATE:
	case _CUR_MOVE:
	case _CUR_SCALE:
//		CurMove ();
		break;

	case _CUR_CHAR_MOVE:		
	case _CUR_GAME_TEST:
		CurCharMove ();
		break;

	case _CUR_EDIT_BOX:
		m_EditBox.FrameMove ( m_fTime, m_fElapsedTime );
		break;

	case _CUR_TILE_DRAW:
	case _CUR_TILE_GET:
	case _CUR_TILE_DEL:		
		if ( m_pActiveEffect && Pick() )
		{
			switch(m_pActiveEffect->GetTypeID())
			{
			case DEF_EFFECT_TILING:
				{
					DxEffectTiling *pTiling = (DxEffectTiling *) m_pActiveEffect;
					pTiling->TileEdit ( g_vPICK_RAY_ORIG, g_vPICK_RAY_DIR );
				}
				break;
			};
		}
		break;
	};

	DxMeshes::FrameMove( m_fElapsedTime );

    // VisualMaterialFX 파일이 변경되었다면 다시 재 로딩함.
    if ( NS_VM_MANAGER::NeedReload( m_fElapsedTime ) )
    {
        // VisualMaterial FX 정리함.
        NS_VM_MANAGER::ClearUpVMFX( FALSE );

        // TextureEff Reload
        {
            DxFrameMesh* pFrameMesh = GetFrameMesh();
	        if ( pFrameMesh )
	        {
                pFrameMesh->ReloadTexEffVMFX( GetD3DDevice() );
	        }

            DxPieceEdit* pPieceEdit = GetPieceEdit();
            if ( pPieceEdit )
            {
                pPieceEdit->ReloadTexEffVMFX( GetD3DDevice() );
            }
            
            DxPieceManagerPSF* pPieceManager = GetPieceManager();
            pPieceManager->ReloadTexEffVMFX( GetD3DDevice() );
        }
    }
    
    // 커서 Process
    if (WorldStateMachine::IsInMouse(this))
    {
	    FrameMoveCurNext( m_fElapsedTime );
    }

	//////////////////////////////////////////////////////////////////////////
	// NSSkinAniThread
	// TAG_ProcessAnimationThread_3
	// 모든 정보가 COMPLETE_DATA 될 때 까지 기다리도록 한다.
	NSSkinAniThread::Wait_CompleteSkinAniControl( m_fElapsedTime );

	// NSEffectThread 대기
	NSEffectThread::Wait_FrameMove( m_pd3dDevice, m_fElapsedTime );

	PROFILE_END("FrameMove");

	return S_OK;
}

HRESULT CWorldEditView::Render()
{
	PROFILE_BEGIN("Render");

	// Baking 중인데 여기로 들어온다면...
	if ( NSOcclusionCulling::g_bBaking )
	{
		return S_OK;
	}

	// Baking 중인데 여기로 들어온다면...
	if ( NSBakeCommon::g_bBaking )
	{
		return S_OK;
	}


	// PS Mode 를 셋팅한다.
	if ( m_dwRenderPSType==ID_RENDER_PS_3_0_HIGH ) 
	{
		RENDERPARAM::g_emTnLMode = TnL_PS_3_0_CSM;
	}
	else if ( m_dwRenderPSType==ID_RENDER_PS_3_0 ) 
	{
		RENDERPARAM::g_emTnLMode = TnL_PS_3_0_NM;
	}
	else if ( m_dwRenderPSType==ID_RENDER_PS_2_0 ) 
	{
		RENDERPARAM::g_emTnLMode = TnL_PS_3_0_DF;
	}
	else if ( m_dwRenderPSType==ID_RENDER_PS_1_1 ) 
	{
		RENDERPARAM::g_emTnLMode = TnL_PS_1_1;
	}
	else if ( m_dwRenderPSType==ID_RENDER_FIXED ) 
	{
		RENDERPARAM::g_emTnLMode = TnL_FIXED;
	}

	if ( g_bWIREFRAME_VIEW )
	{
		RENDERPARAM::g_emTnLMode = TnL_FIXED;
	}


	// PS Mode 를 셋팅한다.
	if ( m_dwRenderCharType==ID_RENDER_CHAR_SS ) 
	{
		RENDERPARAM::g_emCharacterQulity = TnL_CHAR_PS_3_0;
	}
	else if ( m_dwRenderCharType==ID_RENDER_CHAR_DEFAULT ) 
	{
		RENDERPARAM::g_emCharacterQulity = TnL_CHAR_PS_2_0;
	}

	


	NSMATERIALMESH::g_nCreateCount = 0;

    DxBackupTarget sBackupTarget;
    sBackupTarget.Backup( GetD3DDevice(), DxBackupTarget::ALL );

    LPDIRECT3DSURFACE9 pSwapChainBuffer = NULL;
	m_pSwapChainMain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pSwapChainBuffer );
	GetD3DDevice()->SetRenderTarget( 0, pSwapChainBuffer );

	// Prefab일경우 Fog를 끈다.
	BOOL bFogOff = g_bFORCE_FOG_OFF;
//	if ( GetSetLandMan()->IsPrefab() )
//		bFogOff = TRUE;

	//	Fog 셋팅
	D3DCOLOR colorClear = D3DCOLOR_XRGB(89,135,179);
    DxFogMan::GetInstance().RenderFogSB ( m_pd3dDevice, bFogOff );

	if ( (m_ViewType==FRAME_VIEW || 
        m_ViewType==PIECE_REPLACE_VIEW ||
        m_ViewType==PIECE_SETPOS_VIEW || 
        m_ViewType==EFFECT_SET_VIEW ||
        m_ViewType==EFFECT_TILE_VIEW ||
        m_ViewType==EFFECT_GRASS_VIEW ||
        m_ViewType==CAMCOLL_VIEW ||
		m_ViewType==WATER_SURF_VIEW ) && 
        !DxFogMan::GetInstance().GetFrameMode_Fog() )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, FALSE );
		colorClear = DxFogMan::GetInstance().GetDayColor ();
	}
	else if( m_ViewType==NAVI_VIEW )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, FALSE );
		colorClear = DxFogMan::GetInstance().GetDayColor ();
	}
	else if( m_ViewType==FRAMEMESH_VIEW )
	{	
		// PEACE_EDIT 모드일때 포그 컬러가 아닌 컬러로 셋팅
		colorClear = D3DCOLOR_XRGB(128,128,128);
	}
	else
	{
		colorClear = DxFogMan::GetInstance().GetFogColor();
	}


	if ( g_bOrthoView )
	{
		D3DXVECTOR3 vLookatPt = DxViewPort::GetInstance().GetLookatPt();
		D3DXVECTOR3 vFromPtOrig = DxViewPort::GetInstance().GetFromPt();

		D3DXVECTOR3 vDir = vFromPtOrig - vLookatPt;
		float fLength = D3DXVec3Length( &vDir );

		// Note : View Port를 설정한다.
		D3DXMATRIX matView;
		D3DXVECTOR3 vFromPt = vLookatPt + D3DXVECTOR3( 0.f, fLength, 0.f );
		D3DXVECTOR3 vUpVec( 0.f, 0.f, 1.f );
		DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );
		D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );

		// Note : Projection를 설정한다.
		D3DXMATRIX matProj;
		float fNearPlane = 10.f;
		float fFarPlane = 100000.f;
		D3DXMatrixOrthoLH( &matProj, fLength, fLength, fNearPlane, fFarPlane );	// D3DXMatrixPerspectiveFovLH
		DxViewPort::GetInstance().SetProjectionOrthoLH ( fLength, fLength, fNearPlane, fFarPlane );

		m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
		m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

		DxViewPort::GetInstance().m_CV = DxViewPort::GetInstance().ComputeClipVolume ( vLookatPt, vFromPt, vUpVec, fNearPlane, fFarPlane, 1.f, 1.f, D3DX_PI/2 );

		NSMaterialManager::SetViewProjection( m_pd3dDevice );

		m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	}

	HRESULT hr(S_OK);
	hr = m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR );

	if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM )
	{
		hr = m_pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pSufFourCC_INTZ );
	}
	else
	{
		hr = m_pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pZBuffer );
	}

	// Clear the viewport
	hr = m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, colorClear, 1.0f, 0L );

	//if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && m_spDxLandMan->GetUsedMaterialSystem() && m_ViewType==OCTREE_VIEW )
	//{
	//	hr = m_pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pZBuffer );

	//	hr = m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorSuf_1st );

	//	// Clear the viewport
	//	hr = m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );
	//}


	// Begin the scene
	hr = m_pd3dDevice->BeginScene();
	if( SUCCEEDED(hr) )
	{
		// Note : 월드에딧에서는 정보를 받아와야 하기 때문에 이 Code 가 추가됨.
		NSMaterialManager::SetFogParam( DxFogMan::GetInstance().GetFogStart_RealTime(), DxFogMan::GetInstance().GetFogEnd_RealTime() );

		// Glow 
		DxGlowMan::GetInstance().RenderTex ( m_pd3dDevice );

		// Light 설정.
		//
		DxLightMan::GetInstance()->Render ( m_pd3dDevice, ((m_ViewType==OCTREE_VIEW) ? FALSE : TRUE), m_spDxLandMan->GetUsedMaterialSystem() );

		// 버텍스 쉐이큱E콘스탄트 설정.
		//
		D3DXVECTOR3 vFog;
		m_pd3dDevice->GetRenderState ( D3DRS_FOGSTART,	(DWORD*)&vFog.x );
		m_pd3dDevice->GetRenderState ( D3DRS_FOGEND,	(DWORD*)&vFog.y );
		vFog.z = vFog.y - vFog.x;
		vFog.z = 1.f/vFog.z;
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_FOG, (float*)&vFog, 1 );

		DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIRECT, (float*)&Light.m_Light.Direction, 1 );
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIFFUSE, (float*)&Light.m_Light.Diffuse, 1 );
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTAMBIENT, (float*)&Light.m_Light.Ambient, 1 );

		D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_CAMERAPOSITION, (float*)&vFromPt, 1 );

		D3DXMATRIX matView = DxViewPort::GetInstance().GetMatView();
		D3DXMATRIX matProj = DxViewPort::GetInstance().GetMatProj();

		m_pd3dDevice->SetVertexShaderConstantF ( VSC_SKIN_DEFAULT, (float*)&D3DXVECTOR4 (1.f, 0.5f, 0.f, 765.01f), 1 );

		D3DXVECTOR3	vLightVector = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
		D3DXVec3TransformNormal ( &vLightVector, &vLightVector, &matView );
		D3DXVec3Normalize ( &vLightVector, &vLightVector);
		vLightVector = -vLightVector;
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_LIGHTDIRECT_VIEW, (float*)&vLightVector, 1 );

		D3DXMatrixTranspose( &matView, &matView );
		D3DXMatrixTranspose( &matProj, &matProj );

		m_pd3dDevice->SetVertexShaderConstantF ( VSC_MATWVP_01, (float*)&matView, 4 );
		m_pd3dDevice->SetVertexShaderConstantF ( VSC_MATWVP_02, (float*)&matProj, 4 );
		
		// 포인트 라이트 VertexShaderConstant
		//
		D3DLIGHTQ	pLight;
		D3DXVECTOR4	vPointPos;
		D3DXVECTOR3	vPointDiff;
		for ( int i=0; i<7; ++i )
		{
			if ( DxLightMan::GetInstance()->GetClosedLight(i+1) )
			{
				pLight = DxLightMan::GetInstance()->GetClosedLight(i+1)->m_Light;
				vPointDiff = D3DXVECTOR3 ( pLight.Diffuse.r, pLight.Diffuse.g, pLight.Diffuse.b );
				vPointPos.x = pLight.Position.x;
				vPointPos.y = pLight.Position.y;
				vPointPos.z = pLight.Position.z;
				vPointPos.w = pLight.Range;
			}
			else
			{
				vPointPos = D3DXVECTOR4 ( 0.f, 0.f, 0.f, 0.1f );
				vPointDiff = D3DXVECTOR3 ( 0.f, 0.f, 0.f );
			}
			m_pd3dDevice->SetVertexShaderConstantF (i*2+VSC_PLIGHTPOS01, (float*)&vPointPos, 1);
			m_pd3dDevice->SetVertexShaderConstantF (i*2+VSC_PLIGHTDIFF01, (float*)&vPointDiff, 1);
		}

		D3DXMATRIX matIdentity;
		D3DXMatrixIdentity( &matIdentity );
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

		if ( g_bWIREFRAME_VIEW )	m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		else						m_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		
		CLIPVOLUME cv = DxViewPort::GetInstance().GetClipVolume ();

		//// 구름 그림자
		////
		//PROFILE_BEGIN("Weather_Prev");
		//DxWeatherMan::GetInstance()->Render_Prev ( m_pd3dDevice );
		//PROFILE_END("Weather_Prev");





		// 지형 그림자를 생성하도록 한다. 
		switch ( m_ViewType )
		{
		case OCTREE_VIEW:
			DxShadowMap::GetInstance().ClearShadow ( m_pd3dDevice, m_spDxLandMan->GetUsedMaterialSystem() );

			m_pCharacter->RenderShadow( m_pd3dDevice, FALSE );

			m_spDxLandMan->RenderShadow( m_pd3dDevice );

			// PS_3_0_HIGH 이상일 경우만 동작한다.
			if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_CSM )
			{
				m_spDxLandMan->RenderShadow_SpotLight1( m_pd3dDevice );
				if ( m_CurType==_CUR_CHAR_MOVE || m_CurType == _CUR_GAME_TEST )
				{
					m_pCharacter->RenderShadow_SpotLight( m_pd3dDevice, m_spDxLandMan->GetUsedMaterialSystem(), 0 );
				}

				m_spDxLandMan->RenderShadow_SpotLight2( m_pd3dDevice );
				if ( m_CurType==_CUR_CHAR_MOVE || m_CurType == _CUR_GAME_TEST )
				{
					m_pCharacter->RenderShadow_SpotLight( m_pd3dDevice, m_spDxLandMan->GetUsedMaterialSystem(), 1 );
				}
			}

			//// 작업한 m_pShadowSufR32F_2048 를 m_pShadowSufR32F_CSM 에 복제하고 m_pShadowZBuf_2048 값을 삭제한다.
			//{
			//	DxBackupTarget sBackupTarget;
			//	sBackupTarget.Backup( m_pd3dDevice );

			//	m_pd3dDevice->StretchRect( DxSurfaceTex::GetInstance().m_pShadowSufR32F_2048, NULL, DxSurfaceTex::GetInstance().m_pShadowSufR32F_CSM, NULL, D3DTEXF_LINEAR );
			//	m_pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pShadowSufR32F_CSM );
			//	m_pd3dDevice->SetDepthStencilSurface( DxSurfaceTex::GetInstance().m_pShadowZBuf_2048 );
			//	m_pd3dDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);
			//}

			
			break;

		default:
			// SpotLight 셋팅.
			NSShadowLight::FrameMoveTool( m_fElapsedTime );
			break;
		};













		// 환컖E
		//
		PROFILE_BEGIN("Environment");
		switch ( m_ViewType )
		{
		case FRAME_VIEW:
        case PIECE_REPLACE_VIEW:
        case PIECE_SETPOS_VIEW:
        case EFFECT_SET_VIEW:
        case EFFECT_TILE_VIEW:
        case EFFECT_GRASS_VIEW:
		case CAMCOLL_VIEW:
		case WATER_SURF_VIEW:
			DxEnvironment::GetInstance().Render( m_pd3dDevice, g_rSetLandMan->GetFrameMesh(), NULL );	// [Modify] Point Light 여기로 들어간다.
			break;
		case OCTREE_VIEW:
			DxEnvironment::GetInstance().ResetReflectionCV();
			if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && m_spDxLandMan->GetUsedMaterialSystem() && m_ViewType==OCTREE_VIEW )
			{
				// if문은 속도차이를 확인하기 위함.
				if ( m_WaterType == RENDER_WATER_RT_REFLECTION )
				{
					DxEnvironment::GetInstance().RenderReflectionMaterial( m_pd3dDevice, m_spDxLandMan.get() );
				}
			}
			else
			{
				DxEnvironment::GetInstance().Render( m_pd3dDevice, NULL, m_spDxLandMan.get() );
			}
			break;
		}
		PROFILE_END("Environment");

        // 재 Light 셋팅
        DxLightMan::GetInstance()->ReSetLight( m_pd3dDevice, ((m_ViewType==OCTREE_VIEW) ? FALSE : TRUE) );

		switch ( NSLightMapFX::g_emRenderType )
		{
		case NSLIGHTMAPDEF::LMRT_CHECKER:
		case NSLIGHTMAPDEF::LMRT_LIGHTMAP_POINT:
			m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			m_pd3dDevice->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
			m_pd3dDevice->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_POINT );
			break;

		default:
			m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
			m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			m_pd3dDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
			m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			m_pd3dDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
			m_pd3dDevice->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			m_pd3dDevice->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
			break;
		};

		if ( g_rSetLandMan == &m_SetLandManPrefab )
		{
			EDITMESHS::RENDERPLANE ( m_pd3dDevice, D3DXVECTOR3(0,0,0), D3DXVECTOR3(100,0,100), D3DXVECTOR3(10,0,10) );
			EDITMESHS::RENDERARROW ( m_pd3dDevice, D3DXVECTOR3(0,0,0), D3DXVECTOR3(100,0,100), D3DXVECTOR3(10,0,10) );
		}

		// 지?E
		//
		PROFILE_BEGIN("Land");
		switch ( m_ViewType )
		{
		case FRAME_VIEW:
			{
				//DxStaticMeshColor_THREAD::GetInstance().m_bOctreeMode = FALSE;

				RenderDefferedOpaqueForPSF( m_pd3dDevice, colorClear );

				// 선택된 프레임 랜더링, Wire mode로.
				//
				if ( ( !( m_CurType == _CUR_NONE || m_CurType == _CUR_CHAR_MOVE || m_CurType == _CUR_GAME_TEST ) && m_sObjectMRS.IsCollMode() ) )
				{
					g_rSetLandMan->GetFrameMesh()->DrawSelectedFrame ( m_pd3dDevice, &cv, NULL );                    
				}

				if ( m_bFrameMeshWireFrame )
				{
					g_rSetLandMan->DrawWireFrame ( m_pd3dDevice );
				}
			}
			break;

        case PIECE_REPLACE_VIEW:
        case PIECE_SETPOS_VIEW:
        case EFFECT_SET_VIEW:
        case EFFECT_TILE_VIEW:
        case EFFECT_GRASS_VIEW:
		case WATER_SURF_VIEW:
		case PREFAB_VIEW:
			{
				//DxStaticMeshColor_THREAD::GetInstance().m_bOctreeMode = FALSE;
				
				RenderDefferedOpaqueForPSF( m_pd3dDevice, colorClear );

                if ( m_bFrameMeshWireFrame )
                {
                    g_rSetLandMan->DrawWireFrame ( m_pd3dDevice );
                }
			}
			break;
		
		case OCTREE_VIEW:
			//DxStaticMeshColor_THREAD::GetInstance().m_bOctreeMode = TRUE;

			if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && m_RenderType == RENDER_TEXTURE_LIGHTMAP )
			{
				RenderDefferedOpaqueForWLD( m_pd3dDevice, colorClear );
			}
			else
			{
				m_spDxLandMan->Render ( m_pd3dDevice, cv, !g_bFORCE_FOG_OFF );
			}

			break;
			
		case NAVI_VIEW:
			if ( m_spDxLandMan->GetNaviMesh() )
			{
				// 길따라가는거 보여주콅E
				m_pCharacter->RenderActorMove(m_pd3dDevice);

				m_spDxLandMan->GetNaviMesh()->Render ( m_pd3dDevice );
			}
			break;

		case PIECE_VIEW:
			{
				D3DXMATRIX matIdentity;
				D3DXMatrixIdentity( &matIdentity );

				m_ReplacePiece.Render( m_pd3dDevice, &matIdentity, &cv, 1.f );
			}
			break;

		case TILE_GAME_VIEW:
			m_DxMapEditMan.Render ( m_pd3dDevice, TRUE );
			break;

		case TILE_EDIT_VIEW:
			m_DxMapEditMan.Render ( m_pd3dDevice, FALSE );

			switch ( m_SetType )
			{
			case HEIGHT_VIEW:
				m_DxMapEditMan.ModifyHeight_RENDER ( m_pd3dDevice );
				break;

			case TILE_COLOR_VIEW:
				m_DxMapEditMan.ModifyTexUV_RENDER ( m_pd3dDevice );
				break;

			case MAP_PIECE_VIEW:
				m_DxMapEditMan.Render_PieceEDIT ( m_pd3dDevice );
				break;
			};	
			break;
		case FRAMEMESH_VIEW:
		//case STATICMESH_VIEW:
			if ( m_DxPieceEdit.GetFrameMesh() )	
			{
				m_DxPieceEdit.RenderFrame( m_pd3dDevice, cv, matIdentity, TRUE );

				// VisualMaterial의 g_sTranslucent 매터리얼 렌더링
				NSTexEffVisualMaterial::Render( m_pd3dDevice, &NSVMRender::g_sTranslucent );
			}

			// 선택된 프레임 랜더링, Wire mode로.
			//
			if ( !( m_CurType == _CUR_NONE || m_CurType == _CUR_GAME_TEST || g_bFORCE_FOG_OFF ) && m_sObjectMRS.IsCollMode() )
			{
				if ( m_DxPieceEdit.GetFrameMesh() )	
				{
					m_DxPieceEdit.GetFrameMesh()->DrawSelectedFrame ( m_pd3dDevice, &cv, NULL );
				}
			}
			break;

		case STATICMESH_VIEW:
			{
				D3DXMATRIX matIdentity;
				D3DXMatrixIdentity( &matIdentity );
				m_DxPieceEdit.Render( m_pd3dDevice, colorClear, matIdentity );
			}
			break;

		case CAMCOLL_VIEW:	
			{
				//DxStaticMeshColor_THREAD::GetInstance().m_bOctreeMode = FALSE;

				RenderDefferedOpaqueForPSF( m_pd3dDevice, colorClear );

				// 선택된 프레임 랜더링, Wire mode로.
				//
				if ( !( m_CurType == _CUR_NONE || m_CurType == _CUR_CHAR_MOVE || m_CurType == _CUR_GAME_TEST ) && m_sObjectMRS.IsCollMode() )
					g_rSetLandMan->GetFrameMesh()->DrawSelectedMesh ( m_pd3dDevice, &cv );
			}
		};
		PROFILE_END("Land");

		switch ( m_ViewType )
		{
		case PIECE_REPLACE_VIEW:
		case PIECE_SETPOS_VIEW:
			{
				if ( g_pPIECE )
				{
					DWORD dwWireFrame(0L);
					m_pd3dDevice->GetRenderState( D3DRS_FILLMODE, &dwWireFrame );
					m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );

					float fFogStart(1.f);
					float fFogEnd(2.f);
					float fBias = -0.001f;
					DWORD dwBiasPrev(0);
					DWORD dwFogStartPrev(0);
					DWORD dwFogEndPrev(0);
					DWORD dwFogColor(0L);
					DWORD dwFogEnable(0L);
					m_pd3dDevice->GetRenderState ( D3DRS_DEPTHBIAS,	&dwBiasPrev );
					m_pd3dDevice->GetRenderState ( D3DRS_FOGSTART,	&dwFogStartPrev );
					m_pd3dDevice->GetRenderState ( D3DRS_FOGEND,	&dwFogEndPrev );
					m_pd3dDevice->GetRenderState ( D3DRS_FOGCOLOR,	&dwFogColor );
					m_pd3dDevice->GetRenderState ( D3DRS_FOGENABLE,	&dwFogEnable );
					m_pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
					m_pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	*((DWORD *)(&fFogStart)) );
					m_pd3dDevice->SetRenderState ( D3DRS_FOGEND,	*((DWORD *)(&fFogEnd)) );
					m_pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xffffffff );
					m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,	TRUE );

					NSMaterialManager::SetFogColorForEditor( 0xffffffff );

					NSLightMapFX::ShortBlackFog();

					BOOL bBackUpWireFrame = g_bWIREFRAME_VIEW;
					g_bWIREFRAME_VIEW = TRUE;

					g_pPIECE->RenderMS_Opaque( FALSE, FALSE, 0.f, FALSE );
					g_pPIECE->RenderMS_Opaque_Animation( FALSE, FALSE, 0.f, FALSE );
					g_pPIECE->RenderMS_HardAlpha( FALSE, FALSE );
					g_pPIECE->RenderMS_HardAlpha_Animation( FALSE, FALSE );
					g_pPIECE->RenderMS_SoftAlpha( FALSE );

					g_bWIREFRAME_VIEW = bBackUpWireFrame;

					// 밑에 렌더링 되는 것들은 포그를 사용할 필요가 없는 것들이라서 굳이 셋팅을 돌리지는 않는다.
					//NSVERTEXNORTEX_FX::SetFog
					//( 
					//	DxFogMan::GetInstance().GetFogEnd_RealTime(), 
					//	DxFogMan::GetInstance().GetFogEnd_RealTime() - DxFogMan::GetInstance().GetFogStart_RealTime() 
					//);

					m_pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	dwBiasPrev );
					m_pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	dwFogStartPrev );
					m_pd3dDevice->SetRenderState ( D3DRS_FOGEND,	dwFogEndPrev );
					m_pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	dwFogColor );
					m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,	dwFogEnable );

					NSMaterialManager::SetFogColorForEditor( dwFogColor );

					m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,	dwWireFrame );
				}
			}
			break;

		case PREFAB_VIEW:
			{
				if ( g_pPrefab )
				{
					g_pPrefab->Render( GetD3DDevice(), TRUE );
				}
			}
			break;
		};





		//if ( DxSurfaceTex::GetInstance().m_pColorSuf_1st && RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_NM && m_spDxLandMan->GetUsedMaterialSystem() && m_ViewType==OCTREE_VIEW )
		//{
		//	// Note : 
		//	D3DSURFACE_DESC	sDesc;
		//	LPDIRECT3DSURFACEQ pBackBuffer;
		//	m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
		//	pBackBuffer->GetDesc ( &sDesc );
		//	SAFE_RELEASE ( pBackBuffer );

		//	hr = m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorBuffer );

		//	switch ( m_ViewType )
		//	{
		//	case OCTREE_VIEW:
		//		m_spDxLandMan->Render_2Pass ( m_pd3dDevice, cv, m_CurType==_CUR_CHAR_MOVE || m_CurType == _CUR_GAME_TEST );
		//		break;
		//	};
		//}

		// Character Render
		//
		PROFILE_BEGIN("Character");
		if ( m_CurType==_CUR_CHAR_MOVE || m_CurType == _CUR_GAME_TEST )
		{
			//////////////////////////////////////////////////////////////////////////
			//							기본 렌더
			float fFogStart(1.f);
			float fFogEnd(2.f);
			float fBias = -0.001f;
			DWORD dwBiasPrev(0);
			DWORD dwFogStartPrev(0);
			DWORD dwFogEndPrev(0);
			DWORD dwFogColor(0L);
			DWORD dwFogEnable(0L);
			if ( g_bWIREFRAME_VIEW )
			{
				m_pd3dDevice->GetRenderState ( D3DRS_DEPTHBIAS,	&dwBiasPrev );
				m_pd3dDevice->GetRenderState ( D3DRS_FOGSTART,	&dwFogStartPrev );
				m_pd3dDevice->GetRenderState ( D3DRS_FOGEND,	&dwFogEndPrev );
				m_pd3dDevice->GetRenderState ( D3DRS_FOGCOLOR,	&dwFogColor );
				m_pd3dDevice->GetRenderState ( D3DRS_FOGENABLE,	&dwFogEnable );
				m_pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	*((DWORD*)&fBias) );
				m_pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	*((DWORD *)(&fFogStart)) );
				m_pd3dDevice->SetRenderState ( D3DRS_FOGEND,	*((DWORD *)(&fFogEnd)) );
				m_pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	0xffff00ff );
				m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,	TRUE );
			}

			D3DXMATRIX matIdentity;
			D3DXMatrixIdentity( &matIdentity );
			m_pd3dDevice->SetTransform ( D3DTS_WORLD,		&matIdentity );
			m_pd3dDevice->SetTransform ( D3DTS_VIEW,		&DxViewPort::GetInstance().GetMatView() );
			m_pd3dDevice->SetTransform ( D3DTS_PROJECTION,	&DxViewPort::GetInstance().GetMatProj() );
			m_pCharacter->Render ( m_pd3dDevice, m_spDxLandMan.get() );

			if ( g_bWIREFRAME_VIEW )
			{
				m_pd3dDevice->SetRenderState ( D3DRS_DEPTHBIAS,	dwBiasPrev );
				m_pd3dDevice->SetRenderState ( D3DRS_FOGSTART,	dwFogStartPrev );
				m_pd3dDevice->SetRenderState ( D3DRS_FOGEND,	dwFogEndPrev );
				m_pd3dDevice->SetRenderState ( D3DRS_FOGCOLOR,	dwFogColor );
				m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE,	dwFogEnable );
			}


			//////////////////////////////////////////////////////////////////////////
			//							반사 렌더
			if ( m_spDxLandMan->GetRealTimeWaterHeight() != FLT_MAX )
			{
				if ( DxEnvironment::GetInstance().IsRecentReflect() )
				{
					D3DXMATRIX matReflect, matReflectView;
					D3DXMatrixIdentity( &matReflect );
					//matReflect._42 = -(DxEnvironment::GetInstance().GetCenter().y*2.f);
					matReflect._42 = -(m_spDxLandMan->GetRealTimeWaterHeight()*2.f);
					D3DXMatrixMultiply_MIRROR( matReflect );
					D3DXMatrixMultiply( &matReflectView, &matReflect, &DxViewPort::GetInstance().GetMatView() );

					NSMaterialSkinManager::FrameMove( matReflectView, DxViewPort::GetInstance().GetMatProj() );
				}

				m_pCharacter->RenderReflect( m_pd3dDevice, m_spDxLandMan->GetRealTimeWaterHeight() );

				{
					NSMaterialSkinManager::FrameMove( DxViewPort::GetInstance().GetMatView(), DxViewPort::GetInstance().GetMatProj() );
				}
			}
			//DxShadowMap::GetInstance().SetShadowDetail(LAND_QT_HIGH_HIGH);
		}
		else
		{
		//	DxShadowMap::GetInstance().SetShadowDetail(LAND_QT_LOW_LOW);
		}
		PROFILE_END("Character");



		// Note : 
		OPTMManager::GetInstance().Render_HardAlpha();



		//////////////////////////////////////////////////////////////////////////
		// 이전 맵일 경우.
		m_pCharacter->RenderShadow( m_pd3dDevice, TRUE );

		// ShadowMap ImageBlur
		DxShadowMap::GetInstance().LastImageBlur( m_pd3dDevice, m_spDxLandMan->GetUsedMaterialSystem() );






		// Land_EFF
		PROFILE_BEGIN("Land_EFF");
		switch ( m_ViewType )
		{
		case CAMCOLL_VIEW:
		case FRAME_VIEW:
        case PIECE_REPLACE_VIEW:
        case EFFECT_SET_VIEW:
        case EFFECT_TILE_VIEW:
        case EFFECT_GRASS_VIEW:
		case WATER_SURF_VIEW:
		case PREFAB_VIEW:
			{
				RenderDefferedSoftAlphaForPSF( m_pd3dDevice );
				g_rSetLandMan->Render_EFF ( m_pd3dDevice, TRUE );
			}
			break;

        case PIECE_SETPOS_VIEW:
			RenderDefferedSoftAlphaForPSF( m_pd3dDevice );
			g_rSetLandMan->Render_EFF ( m_pd3dDevice, TRUE );
            if( DxPieceManagerPSF::g_emEDITMODE == DxPieceManagerPSF::MODE_INSERT && g_pPIECEEDIT )
			{
				D3DXMatrixIdentity( &matIdentity );
				CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
				
				DWORD dwAlphaBlend(0L);
				DWORD dwSrcBlend(0L);
				DWORD dwDestBlend(0L);
				m_pd3dDevice->GetRenderState ( D3DRS_ALPHABLENDENABLE,	&dwAlphaBlend );
				m_pd3dDevice->GetRenderState ( D3DRS_SRCBLEND,			&dwSrcBlend );
				m_pd3dDevice->GetRenderState ( D3DRS_DESTBLEND,			&dwDestBlend );
				m_pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	TRUE );
				m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA );
				m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			D3DBLEND_INVSRCALPHA );

				D3DXMatrixMultiply(&matIdentity,&matIdentity,&pFrame->m_cDlgEditPIECE.m_cDlgPiecePreView.GetPieceMatrix());
				matIdentity._41 = g_vDYNAMIC_POINT.x;
				matIdentity._42 = g_vDYNAMIC_POINT.y;
				matIdentity._43 = g_vDYNAMIC_POINT.z;

				g_pPIECEEDIT->Render( GetD3DDevice(), colorClear, matIdentity );

				m_pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE,	dwAlphaBlend );
				m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND,			dwSrcBlend );
				m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND,			dwDestBlend );
			}
			break;
		
		case OCTREE_VIEW:
			if ( RENDERPARAM::g_emGroundQulity >= TnL_PS_3_0_DF && m_RenderType == RENDER_TEXTURE_LIGHTMAP )
			{
				RenderDefferedSoftAlphaForWLD( m_pd3dDevice );
			}
			else
			{
				m_spDxLandMan->Render_LOW_SoftAlpha( m_pd3dDevice, cv );
				m_spDxLandMan->Render_LOW_SingleEff( m_pd3dDevice, cv );
			}
			break;
		};
		PROFILE_END("Land_EFF");








		switch ( m_ViewType )
		{
		case WATER_SURF_VIEW:
			NSWaterMainPSF::RenderEdit( m_pd3dDevice );
			break;
		};

		// 하늘 렌더링..!! 후
		DxSkyMan::GetInstance().Render_AFTER( m_pd3dDevice, DxViewPort::GetInstance().GetFOV(), DxViewPort::GetInstance().GetAspect() );



        switch ( m_ViewType )
		{
		case OCTREE_VIEW:
			m_spDxLandMan->Render_LightShafts( m_pd3dDevice );
			break;
		default:
			g_rSetLandMan->Render_LightShafts( m_pd3dDevice );
			break;
		};

		// Note : LightShafts 작업을 한다.
		NSLightShafts::OnRender( m_pd3dDevice );


		// 날씨 메니져
		//
		PROFILE_BEGIN("Weather");
		DxWeatherMan::GetInstance()->Render ( m_pd3dDevice );
		PROFILE_END("Weather");

		// Light Edit 확인용.
		DxLightMan::GetInstance()->Render_Sphere_LightName( m_pd3dDevice );

        // 방향 Edit 시 월드 상에서 알 수 있도록 셋팅함.
		NSDirectionRender::Render( GetD3DDevice(), DxViewPort::GetInstance().GetLookatPt() );

		PROFILE_BEGIN("Editer Mesh");

		// Edit 퓖E
		//
		D3DXMatrixIdentity( &matIdentity );
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );		

		DWORD	dwNormal;
		m_pd3dDevice->GetRenderState( D3DRS_NORMALIZENORMALS, &dwNormal );
		m_pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, TRUE );

		switch ( m_CurType )
		{
		case _CUR_SELECT:
			{
				//CDebugSet::ToView ( 0, "_CUR_SELECT" );

				D3DXVECTOR3 vCollPos(FLT_MAX/100,FLT_MAX/100,FLT_MAX/100);
				D3DXMATRIX matWorld;
				D3DXMatrixIdentity ( &matWorld );
				switch ( m_ViewType )
				{
				case FRAME_VIEW:
				case CAMCOLL_VIEW:
				case WATER_SURF_VIEW:
                    {
                        LPDXFRAME const pDxFrame = g_rSetLandMan->GetCollisionMap()->GetCollisionDetectedFrame();
					    if ( pDxFrame )
                        {
                            vCollPos = g_rSetLandMan->GetCollisionMap()->GetCollisionPos();
                        }
                        else
                        {
                            vCollPos = m_vCollisionPos;
                        }
					    D3DXMatrixTranslation ( &matWorld, vCollPos.x, vCollPos.y, vCollPos.z );
                    }
					break;

				case OCTREE_VIEW:
                case EFFECT_SET_VIEW:
                case PIECE_REPLACE_VIEW:
                case PIECE_SETPOS_VIEW:
                case EFFECT_GRASS_VIEW:
					vCollPos = m_vCollisionPos;
					D3DXMatrixTranslation ( &matWorld, vCollPos.x, vCollPos.y, vCollPos.z );
					break;

				case FRAMEMESH_VIEW:
					vCollPos = m_DxPieceEdit.GetCollisionPos();
					D3DXMatrixTranslation( &matWorld, vCollPos.x, vCollPos.y, vCollPos.z );
					break;
				};

				D3DXVECTOR3 vFromPt = DxViewPort::GetInstance().GetFromPt ();
				vFromPt -= vCollPos;

				float fDist = D3DXVec3Length ( &vFromPt );
				float fScale = DxViewPort::GetInstance().ComputeDistSize ( fDist ) / 800.0f;

				D3DXMATRIX matScale;
				D3DXMatrixScaling ( &matScale, fScale, fScale, fScale );
				D3DXMatrixMultiply ( &matWorld, &matScale, &matWorld );

				m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_WIREFRAME );
				m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_ALWAYS );
				
				m_p3dPosMesh->Render ( m_pd3dDevice, &matWorld, &cv );
				
				m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_SOLID );
				m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_LESSEQUAL );

				m_p3dPosMesh->Render ( m_pd3dDevice, &matWorld, &cv );
			}
			break;

		case _CUR_SELECT_POINT:
			switch ( m_ViewType )
			{
			case FRAME_VIEW:
            case PIECE_REPLACE_VIEW:
            case PIECE_SETPOS_VIEW:
            case EFFECT_SET_VIEW:
			case WATER_SURF_VIEW:
				g_rSetLandMan->GetFrameMesh()->DrawSelectedPoint ( m_pd3dDevice, g_vCLOSED_POINT );
				break;
			case FRAMEMESH_VIEW:
				if ( m_DxPieceEdit.GetFrameMesh() )	m_DxPieceEdit.GetFrameMesh()->DrawSelectedPoint ( m_pd3dDevice, g_vCLOSED_POINT );
				break;
			};
			break;

		case _CUR_ROTATE:
			//CDebugSet::ToView ( 0, "_CUR_ROTATE" );

			m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_WIREFRAME );
			m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_ALWAYS );
			m_pDxEditmat->Render ( m_pd3dDevice );
			m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_SOLID );
			m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_LESSEQUAL );

			m_pDxEditmat->Render ( m_pd3dDevice );
			break;

		case _CUR_MOVE:
			////CDebugSet::ToView ( 0, "_CUR_MOVE" );

			//m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_WIREFRAME );
			//m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_ALWAYS );
			//m_pDxEditmat->Render ( m_pd3dDevice );
			//m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_SOLID );
			//m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_LESSEQUAL );

			//m_pDxEditmat->Render ( m_pd3dDevice );
			break;

		case _CUR_SCALE:
			//CDebugSet::ToView ( 0, "_CUR_MOVE" );

			m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_WIREFRAME );
			m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_ALWAYS );
			m_pDxEditmat->Render ( m_pd3dDevice );
			m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_SOLID );
			m_pd3dDevice->SetRenderState( D3DRS_ZFUNC,		D3DCMP_LESSEQUAL );

			m_pDxEditmat->Render ( m_pd3dDevice );
			break;

		case _CUR_EDIT_BOX:
			m_EditBox.Render ( m_pd3dDevice );
			break;
		case _CUR_TILE_DEL:		// _CUR_TILE_DRAW 이넘이턿E같다.
		case _CUR_TILE_DRAW:
			if ( m_pActiveEffect && Pick() )
			{
				switch(m_pActiveEffect->GetTypeID())
				{
				case DEF_EFFECT_TILING:
					{
						DxEffectTiling *pTiling = (DxEffectTiling *) m_pActiveEffect;
						pTiling->Render_Select ( m_pd3dDevice, TRUE );
					}
					break;
				};
			}
			break;
		case _CUR_TILE_GET:
			if ( m_pActiveEffect && Pick() )
			{
				switch(m_pActiveEffect->GetTypeID())
				{
				case DEF_EFFECT_TILING:
					{
						DxEffectTiling *pTiling = (DxEffectTiling *) m_pActiveEffect;
						pTiling->Render_Select ( m_pd3dDevice, FALSE );
					}
					break;
				};
			}
			break;
		};

		m_pd3dDevice->SetRenderState( D3DRS_NORMALIZENORMALS, dwNormal );

		PROFILE_END("Editer Mesh");

		// Note : Optimize
		OPTMManager::GetInstance().Render();

		// 큐틒E텍스쳐 렌더링..!!
		//
		//DxCubeMap::GetInstance().Render ( m_pd3dDevice );
		NSCubeMapUtil::NSCubeMapBlend::OnFrameMove( m_fElapsedTime );
		switch ( m_ViewType )
		{
		case FRAMEMESH_VIEW:
		case STATICMESH_VIEW:
			NSCubeMapUtil::NSCubeMapBlend::OnRenderForPieceEditor( m_pd3dDevice, NSLightMapFX::GetTextureChecker() );
			NSCubeMapUtil::NSCubeMapForPSF::OnRenderForPieceEditor( m_pd3dDevice, NSLightMapFX::GetTextureChecker() );
			break;

		case OCTREE_VIEW:
			NSCubeMapUtil::NSCubeMapBlend::OnRender( m_pd3dDevice, NSCubeMapCulling::GetCubeMapTexture( DxViewPort::GetInstance().GetLookatPt() ) );
			break;

		case FRAME_VIEW:
		case PIECE_SETPOS_VIEW:
			NSCubeMapUtil::NSCubeMapForPSF::OnFrameMove( m_pd3dDevice, DxViewPort::GetInstance().GetLookatPt(), g_rSetLandMan );
			NSCubeMapUtil::NSCubeMapBlend::OnRender( m_pd3dDevice, NSCubeMapUtil::NSCubeMapForPSF::GetCubeMapTexture() );
			break;

		default:
			break;
		};
		

		// 글로퓖E처리를 하기위해 사퓖E한다.
		DxGlowMan::GetInstance().RenderTex ( m_pd3dDevice );
		DxGlowMan::GetInstance().Render ( m_pd3dDevice );
		DxGlowMan::GetInstance().RenderBURN( m_pd3dDevice );


		//////////////////////////////////////////////////////////////////////////
		//			HDR - Bloom 적용.
		//m_pd3dDevice->StretchRect( DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR, NULL, DxSurfaceTex::GetInstance().m_pColorBuffer, NULL, D3DTEXF_POINT );
		m_pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorBuffer );
		NSBloomHDR::OnRender( m_pd3dDevice );


		// StarGlow 렌더링
		{
			DxBackupTarget sBackupTarget;
			sBackupTarget.Backup( m_pd3dDevice );

			m_pd3dDevice->SetRenderTarget ( 0, DxSurfaceTex::GetInstance().m_pGlowSuf_SRC );
			m_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0L);

			// VisualMaterial의 StarGlow 매터리얼 렌더링
			NSTexEffVisualMaterial::Render( m_pd3dDevice, &NSVMRender::g_sStarGlow );

			// Note : Glow 작업을 한다.
			NSGlow::OnRender( m_pd3dDevice, DxSurfaceTex::GetInstance().m_pColorBuffer );
		}

		if ( CWorldEditView::g_bOcclsionPointRender )
		{
			NSOcclusionCulling::DrawSimulationPoint( m_pd3dDevice );
		}
		if ( CWorldEditView::g_bOcclsionOctreeRender )
		{
			NSOcclusionCulling::DrawOctreeBox( m_pd3dDevice );
		}

		if ( CWorldEditView::g_bCubeMapPointRender )
		{
			NSCubeMapCulling::DrawSimulationPoint( m_pd3dDevice );
		}
		if ( CWorldEditView::g_bCubeMapOctreeRender )
		{
			NSCubeMapCulling::DrawOctreeBox( m_pd3dDevice );
		}

		// 지금까지 그린것을 굴절을 위해 복사해놓는다.
		PROFILE_BEGIN("DxEnvironment::UserToRefraction()");
		DxEnvironment::GetInstance().UserToRefraction ( m_pd3dDevice );
		PROFILE_END("DxEnvironment::UserToRefraction()");

		// Wave 되야 하는것을 저장후 마지막으로 뿌린다.
		//
		PROFILE_BEGIN("DxEnvironment::RenderWave()");
		DxEnvironment::GetInstance().RenderWave ( m_pd3dDevice, DxEffectMan::GetInstance().GetBumpTexture() );
		PROFILE_END("DxEnvironment::RenderWave()");

        if (WorldStateMachine::IsInMouse(this))
        {
		    // 커서 그리콅E
		    CCursor::GetInstance().Render( m_pd3dDevice, DxInputDevice::GetInstance().GetMouseLocateX(), DxInputDevice::GetInstance().GetMouseLocateY() );
        }

		// Note : 커서의 Render및 이동,Rotate, 스케일
		m_sObjectMRS.Render( m_pd3dDevice );

		// Warning : 문제가.. 참...Tile Blend를 할 경우 문제가 있다.
		// Note : 눌렀을 때 m_sObjectMRS 에 걸리는 것이 하나도 없다면 픽킹 모드로 전환한다.
        switch ( m_ViewType )
		{
        case EFFECT_SET_VIEW:
        case PIECE_SETPOS_VIEW:
			DWORD mKeyState = DxInputDevice::GetInstance().GetMouseState ( DXMOUSE_LEFT );
		    if( DxInputDevice::GetInstance().IsMouseInGameScreen( m_bDefWin ) )
		    {
			    if ( mKeyState&(DXKEY_DOWN) )
			    {
				    m_sObjectMRS.NotClick_SetOBECTMRS_I();
			    }
		    }
			break;
		};

		

		// Note : 후처리
	//	DxPostProcess::GetInstance().Render( m_pd3dDevice );

		if( g_bFPS_VISIBLE || g_bCONSOLE_VISIBLE )
		{
			DxOcclusionQuery::DrawSurfaceForDebug();

			PROFILE_BEGIN("Text");
			RenderText();

			if ( g_bCONSOLE_VISIBLE )
			{
				CDebugSet::Render();
			}
			PROFILE_END("Text");
		}		

		// Note : 커서의 Clone 생성
		if( m_sObjectMRS.IsClone() )
		{
			if( m_ViewType != OCTREE_VIEW )
			{
				// Note : 물체를 찾으툈EMatrix도 덤으로 변화시켜 놓는다.
				g_rSetLandMan->Clone_MouseShift( m_pd3dDevice, m_sObjectMRS.GetMatrix() );

				if( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_DOWN )
				{
					CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
					pMainFrame->m_cDlgEdit.UpdatePage_ListChange();
					pMainFrame->m_cDlgEdit.UpdatePage_List_AddLastItem();
				}
			}
		}
		else
		{
			if( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_DOWN )
			{
				CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
				pMainFrame->m_cDlgEdit.UpdatePage_Event( FALSE );
			}
		}

		// Note : SingleEff 에서 AABB 박스의 픸E설정을 위함.
		//			마퓖E?클릭시 AABB 박스 체크를 해서 효과를 찾는다.
		if( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_UP )
		{
			CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			pMainFrame->m_cDlgEdit.UpdatePage_Event( TRUE );
		}

		// Note : 엔터를 치툈E값이 입력이 된다.
		if( (DxInputDevice::GetInstance().GetKeyState( DIK_RETURN ) & DXKEY_UP) ||
			(DxInputDevice::GetInstance().GetKeyState( DIK_NUMPADENTER ) & DXKEY_UP) )
		{
			CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			pMainFrame->m_cDlgEdit.KeyBoard_Enter();
		}

        // WireFrame On/Off
        {
            // Note : 프레임 Mesh 보이게 한다.
		    if( (DxInputDevice::GetInstance().GetKeyState( DIK_F3 ) & DXKEY_UP) )
		    {
                m_bFrameMeshWireFrame = FALSE;
		    }

            // Note : FrameMesh + WireFrame 
            if( (DxInputDevice::GetInstance().GetKeyState( DIK_F4 ) & DXKEY_UP) )
		    {
                m_bFrameMeshWireFrame = TRUE;
		    }
        }

        // 키를 누르는 행동.
        {
            CMainFrame *pMainFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
            pMainFrame->m_cDlgEdit.UpdateKeyBoardMouse();
        }

		m_dwTimeGetTimePrev = timeGetTime();

		// End the scene.
		hr = GetD3DDevice()->EndScene();
		m_pSwapChainMain->Present( NULL, NULL, this->m_hWnd, NULL, 0 );

		if( FAILED(hr) )	CDebugSet::ToFile( "__Test.txt", "EndScene Failed" );
	}

	SAFE_RELEASE ( pSwapChainBuffer );

	PROFILE_END("Render");
	PROFILE_BLOCKEND();
	PROFILE_DUMPOUTPUT();

	OnOcclusionQuery( GetD3DDevice() );

	// 디폴트값 셋팅
	m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,			0x40 );

	return S_OK;
}

void CWorldEditView::OnOcclusionQuery( IDirect3DDevice9* m_pd3dDevice )
{
	if( DxInputDevice::GetInstance().GetKeyState( DIK_LCONTROL ) & DXKEY_PRESSED )
	{
		//if( DxInputDevice::GetInstance().GetKeyState( DIK_BACK ) & DXKEY_UP )
		//{
		//	CHfGeometrySlodPatch::s_bInvisibleRender = !CHfGeometrySlodPatch::s_bInvisibleRender;
		//}

		if( DxInputDevice::GetInstance().GetKeyState( DIK_BACKSLASH ) & DXKEY_UP )
		{
			NSOcclusionCulling::g_bRendering = !NSOcclusionCulling::g_bRendering;
			
			DxOcclusionQuery::g_bForceDisable = TRUE;
		}

		if( DxInputDevice::GetInstance().GetKeyState( DIK_Y ) & DXKEY_UP )
		{
			g_bWIREFRAME_VIEW = !g_bWIREFRAME_VIEW;
		}

		//if( DxInputDevice::GetInstance().GetKeyState( DIK_I ) & DXKEY_UP )
		//{
		//	DxOcclusionQuery::g_bForceDisable = !DxOcclusionQuery::g_bForceDisable;
		//	NSOcclusionCulling::g_bRendering = FALSE;
		//}
	}

	//if ( DxOcclusionQuery::g_bForceDisable )
	//	return;

	//if( DxOcclusionQuery::BeginScene() )
	//{
	//	DxOcclusionQuery::ClearSurface();
	//	DxOcclusionQuery::CaptureState();

	//	//NSHfMain::OcclusionQuery();
	//	m_spDxLandMan->Render_Query( m_pd3dDevice );

	//	DxOcclusionQuery::RollbackState();
	//	DxOcclusionQuery::EndScene();
	//}
}

//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CWorldEditView::RenderText()
{

	D3DCOLOR fontFpsColor	  = D3DCOLOR_ARGB(255,255,255,255);
	D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,255);
    D3DCOLOR fontWarningColor = D3DCOLOR_ARGB(255,0,255,255);
	D3DCOLOR fontRedColor	= D3DCOLOR_ARGB(255,255,0,0);
    TCHAR szMsg[MAX_PATH]	= TEXT("");

	float fNextLine(20.f);
	CD3DFontPar* pD3dFont(NULL);

	// 단순 Fps 만 표시되면 크기를 키운다.
	if ( g_bFPS_VISIBLE && !g_bCONSOLE_VISIBLE )
	{
		fontFpsColor = fontRedColor;
		fNextLine = 0.f;

		pD3dFont = DxFontMan::GetInstance().FindFont ( _DEFAULT_FONT, 24, _DEFAULT_FONT_SHADOW_FLAG );
		if( !pD3dFont )	
		{
			pD3dFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 24, _DEFAULT_FONT_SHADOW_FLAG );
			if( !pD3dFont )	
			{
				return S_OK;
			}
		}
	}
	else
	{
		pD3dFont = DxFontMan::GetInstance().FindFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
		if( !pD3dFont )	
			return S_OK;
	}

	if ( g_bFPS_VISIBLE )
	{
		lstrcpy( szMsg, m_strFrameStats );
		TSTRINGSTREAM ssName;
		ssName << szMsg;
		ssName << ", GPU Delay : ";
		ssName << m_dwDelayGPU;
		//ssName << ' ';
		//ssName << m_fElapsedTime;
		pD3dFont->DrawText( 2, fNextLine, fontFpsColor, ssName.str().c_str() );
		//pD3dFont->DrawText( 2, fNextLine, fontFpsColor, szMsg );
	}

	if ( g_bCONSOLE_VISIBLE )
	{
		lstrcpy( szMsg, m_strDeviceStats );
		pD3dFont->DrawText( 2, 0.f, fontColor, szMsg );

		if ( !DxOcclusionQuery::g_bForceDisable )
		{
			CDebugSet::ToView( 0, "%s", _T("Now : Enable Occlusion Culling Force [SwapKey](LCtrl + O)") );
		}
		else if ( NSOcclusionCulling::g_bRendering )
		{
			CDebugSet::ToView( 0, "%s", _T("Now : Enable Occlusion Culling [SwapKey](LCtrl + \\)") );
		}
		else
		{
			CDebugSet::ToView( 0, "%s", _T("Now : Disable Occlusion Culling [SwapKey](LCtrl + \\)") );
		}

		{
			CDebugSet::ToView( 1, "NSMATERIALMESH::g_nCreateCount : %d", NSMATERIALMESH::g_nCreateCount );
		}
	}

    return S_OK;
}

HRESULT CWorldEditView::InvalidateDeviceObjects()
{
	// Baking 중인데 여기로 들어온다면...
	while ( NSOcclusionCulling::g_bBaking )
	{
		Sleep(1);
	}

	// Baking 중인데 여기로 들어온다면...
	while ( NSBakeCommon::g_bBaking )
	{
		Sleep(1);
	}

	// 각종 장치탛E
	//
	m_pEngineDevice->InvalidateDeviceObjects ();

	m_SetLandMan.InvalidateDeviceObjects( m_pd3dDevice );
	m_SetLandManPrefab.InvalidateDeviceObjects( m_pd3dDevice );
	m_spDxLandMan->InvalidateDeviceObjects ();
	m_ReplacePiece.InvalidateDeviceObjects ();

	m_pCharacter->InvalidateDeviceObjects ();
	
	m_pDxEditmat->InvalidateDeviceObjects ();
	m_p3dPosMesh->InvalidateDeviceObjects( m_pd3dDevice );
	m_sObjectMRS.OnLostDevice( m_pd3dDevice );

	m_DxMapEditMan.InvalidateDeviceObjects ( m_pd3dDevice );
	m_DxPieceEdit.OnLostDevice();

    SAFE_RELEASE( m_pSwapChainMain );
	SAFE_RELEASE( m_pSwapChainPreView );

	NSCubeMapUtil::NSCubeMapForPSF::OnLostDevice();

	return S_OK;
}

HRESULT CWorldEditView::DeleteDeviceObjects ()
{
	// 각종 장치탛E
	//
	m_pEngineDevice->DeleteDeviceObjects ();

	// 커서.
	//
	CCursor::GetInstance().DeleteDeviceObjects ();
	m_EditBox.DeleteDeviceObjects ();

	m_pActiveEffect = NULL;					// 선택된 메쉬 NULL
	m_SetLandMan.DeleteDeviceObjects();
	m_SetLandManPrefab.DeleteDeviceObjects();
	m_spDxLandMan->DeleteDeviceObjects ();
	m_ReplacePiece.DeleteDeviceObjects ();

	m_pDxEditmat->DeleteDeviceObjects();
	m_p3dPosMesh->DeleteDeviceObjects ();
	m_EditBox.DeleteDeviceObjects ();

	m_DxMapEditMan.DeleteDeviceObjects ();
	m_DxPieceEdit.OnDestroyDevice();

	return S_OK;
}

HRESULT CWorldEditView::FinalCleanup()
{
	m_DxPieceEdit.CleanUp();
	m_SetLandMan.FinalCleanup();
	m_SetLandManPrefab.FinalCleanup();

	// 각종 장치탛E
	m_pEngineDevice->FinalCleanup ();

	SAFE_DELETE(m_pDxEditmat);
	SAFE_DELETE(m_p3dPosMesh);

	WorldStateMachine::DestroyWorldStateMachine();

	return S_OK;
}

void CWorldEditView::ResetObjects ()
{
    g_pPIECE = 0;

	g_rSetLandMan->CleanUp( m_pd3dDevice );
	m_spDxLandMan->CleanUp();

	g_rSetLandMan->GetCameraAniMan()->ResetTestCameraAni ();
}

void CWorldEditView::ActiveEditMatrix2( D3DXMATRIX* matWorld )
{
	m_CurType = _CUR_MOVE;

	if( matWorld )
	{
		m_sObjectMRS.SetMatrix( matWorld );
	}
	else
	{
		m_sObjectMRS.SetMatrix( NULL );
	}
}

void CWorldEditView::ActiveEditMatrix ( DXAFFINEMATRIX *pAffineMatrix )
{
	m_CurType = _CUR_MOVE;
	m_pDxEditmat->SetAffineMatrix ( (DXAFFINEMATRIX*)pAffineMatrix );
	m_sObjectMRS.SetMatrix( pAffineMatrix->m_pmatLocal );
}

void CWorldEditView::DeActiveEditMatrix ()
{
	m_CurType = _CUR_SELECT;
	m_pDxEditmat->SetAffineMatrix ( NULL );
	m_sObjectMRS.SetMatrix( NULL );
}

void CWorldEditView::ActiveEditBox ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	m_CurType = _CUR_EDIT_BOX;
	m_EditBox.SetBox ( vMax, vMin );
}

void CWorldEditView::DeActiveEditBox ()
{
	m_CurType = _CUR_SELECT;
}

void CWorldEditView::SetObjRotate90()
{
	g_rSetLandMan->SetObjRotate90();
}

void CWorldEditView::SetCurClick( BOOL bUse )
{
	m_sObjectMRS.SetClick( bUse );
}

void CWorldEditView::SetLandMan()
{
	g_rSetLandMan = &m_SetLandMan;
}

void CWorldEditView::SetLandManPrefab()
{
	g_rSetLandMan = &m_SetLandManPrefab;
}

void CWorldEditView::RefreshPrefab( LPDIRECT3DDEVICEQ m_pd3dDevice )
{
	m_SetLandMan.GetPrefabMan()->RefreshPrefab( m_pd3dDevice );
}

void CWorldEditView::RenderDefferedOpaqueForPSF( IDirect3DDevice9* pd3dDevice, const D3DCOLOR& colorClear )
{
	//////////////////////////////////////////////////////////////////////////
	// Sky Render
	DxSkyBoxMan::GetInstance().Render( pd3dDevice );

	// 하늘 렌더링..!! 본체
	DxSkyMan::GetInstance().Render( pd3dDevice, DxViewPort::GetInstance().GetFOV(), DxViewPort::GetInstance().GetAspect() );



	//////////////////////////////////////////////////////////////////////////
	//							ViewPort
	CLIPVOLUME sCameraCV;
	D3DXVECTOR3 vUP(0.f,1.f,0.f);
	NSDeffered::ComputeClipVolume( sCameraCV, 
									DxViewPort::GetInstance().GetLookatPt(),
									DxViewPort::GetInstance().GetFromPt(),
									vUP,
									DxViewPort::GetInstance().GetNearPlane(),
									DxViewPort::GetInstance().GetWidth(),
									DxViewPort::GetInstance().GetHeight(),
									DxViewPort::GetInstance().GetFOV() );



	//////////////////////////////////////////////////////////////////////////
	//					RenderDefferedOpaqueForPSF
	NSDeffered::RenderDefferedForPSF
	(
		pd3dDevice,
		colorClear,
		g_rSetLandMan,
		DxSurfaceTex::GetInstance().GetWidth(),
		DxSurfaceTex::GetInstance().GetHeight(),
		DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR,
		DxSurfaceTex::GetInstance().m_pColorSuf_1st,
		DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
		DxSurfaceTex::GetInstance().m_pColorSuf_3rd,
		DxSurfaceTex::GetInstance().m_pColorSuf_4th,
		DxSurfaceTex::GetInstance().m_pBurnNewSuf,
		DxSurfaceTex::GetInstance().m_pTexFourCC_INTZ,
		DxSurfaceTex::GetInstance().m_pColorTex_1st,
		DxSurfaceTex::GetInstance().m_pColorTex_2nd, 
		DxSurfaceTex::GetInstance().m_pColorTex_3rd,
		DxSurfaceTex::GetInstance().m_pColorTex_4th,
		DxSurfaceTex::GetInstance().m_pBurnNewTex,
		DxViewPort::GetInstance().GetMatView(),
		DxViewPort::GetInstance().GetMatProj(),
		sCameraCV,
		FALSE
	);
}

void CWorldEditView::RenderDefferedSoftAlphaForPSF( IDirect3DDevice9* pd3dDevice )
{
	//////////////////////////////////////////////////////////////////////////
	//						ViewPort
	CLIPVOLUME sCameraCV;
	D3DXVECTOR3 vUP(0.f,1.f,0.f);
	NSDeffered::ComputeClipVolume( sCameraCV, 
		DxViewPort::GetInstance().GetLookatPt(),
		DxViewPort::GetInstance().GetFromPt(),
		vUP,
		DxViewPort::GetInstance().GetNearPlane(),
		DxViewPort::GetInstance().GetWidth(),
		DxViewPort::GetInstance().GetHeight(),
		DxViewPort::GetInstance().GetFOV() );



	//////////////////////////////////////////////////////////////////////////
	//					RenderDefferedOpaqueForPSF
	D3DCOLOR colorClear = D3DCOLOR_XRGB(0,0,0);
	NSDeffered::RenderDefferedForPSF
	(
		pd3dDevice,
		colorClear,
		g_rSetLandMan,
		DxSurfaceTex::GetInstance().GetWidth(),
		DxSurfaceTex::GetInstance().GetHeight(),
		DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR,
		DxSurfaceTex::GetInstance().m_pColorSuf_1st,
		DxSurfaceTex::GetInstance().m_pColorSuf_2nd,
		DxSurfaceTex::GetInstance().m_pColorSuf_3rd,
		DxSurfaceTex::GetInstance().m_pColorSuf_4th,
		DxSurfaceTex::GetInstance().m_pBurnNewSuf,
		DxSurfaceTex::GetInstance().m_pTexFourCC_INTZ,
		DxSurfaceTex::GetInstance().m_pColorTex_1st,
		DxSurfaceTex::GetInstance().m_pColorTex_2nd, 
		DxSurfaceTex::GetInstance().m_pColorTex_3rd,
		DxSurfaceTex::GetInstance().m_pColorTex_4th,
		DxSurfaceTex::GetInstance().m_pBurnNewTex,
		DxViewPort::GetInstance().GetMatView(),
		DxViewPort::GetInstance().GetMatProj(),
		sCameraCV,
		TRUE
	);
}

void CWorldEditView::RenderDefferedOpaqueForWLD( IDirect3DDevice9* pd3dDevice, const D3DCOLOR& colorClear )
{
	//////////////////////////////////////////////////////////////////////////
	// Sky Render
	pd3dDevice->SetRenderTarget( 0, DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR );



	////////////////////////////////////////////////////////////////////////////
	////							ViewPort
	//CLIPVOLUME sCameraCV;
	//D3DXVECTOR3 vUP(0.f,1.f,0.f);
	//NSDeffered::ComputeClipVolume( sCameraCV, 
	//								DxViewPort::GetInstance().GetLookatPt(),
	//								DxViewPort::GetInstance().GetFromPt(),
	//								vUP,
	//								DxViewPort::GetInstance().GetNearPlane(),
	//								DxViewPort::GetInstance().GetWidth(),
	//								DxViewPort::GetInstance().GetHeight(),
	//								DxViewPort::GetInstance().GetFOV() );



	//////////////////////////////////////////////////////////////////////////
	//					RenderDefferedOpaqueForPSF
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_DF )
	{
		NSDeffered::RenderDefferedForSM30DF
		(
			pd3dDevice,
			colorClear,
			m_spDxLandMan.get(),
			DxSurfaceTex::GetInstance().GetWidth(),
			DxSurfaceTex::GetInstance().GetHeight(),
			DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR,
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
			pd3dDevice,
			colorClear,
			m_spDxLandMan.get(),
			DxSurfaceTex::GetInstance().GetWidth(),
			DxSurfaceTex::GetInstance().GetHeight(),
			DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR,
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
	NSWaterMain::Render( pd3dDevice );



	////////////////////////////////////////////////////////////////////////////
	////							Test Rendering
	//DWORD dwLighting, dwFogEnable;
	//pd3dDevice->GetRenderState( D3DRS_LIGHTING, &dwLighting );
	//pd3dDevice->GetRenderState( D3DRS_FOGENABLE, &dwFogEnable );
	//pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	//pd3dDevice->SetRenderState( D3DRS_FOGENABLE, FALSE );
	//pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG1 );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	//struct VERTEXRHW_CUBE
	//{
	//	D3DXVECTOR4 vPos;
	//	D3DXVECTOR2 vTex;
	//};
	//VERTEXRHW				g_sVertBase[4];
	//g_sVertBase[0].vPos = D3DXVECTOR4 ( 0.f,			0.f,				1.f, 1.f );
	//g_sVertBase[1].vPos = D3DXVECTOR4 ( (float)200.f,	0.f,				1.f, 1.f );
	//g_sVertBase[2].vPos = D3DXVECTOR4 ( 0.f,			(float)200.f,	1.f, 1.f );
	//g_sVertBase[3].vPos = D3DXVECTOR4 ( (float)200.f,	(float)200.f,	1.f, 1.f );
	//g_sVertBase[0].vTex = D3DXVECTOR2 ( 0.f,	0.f );
	//g_sVertBase[1].vTex = D3DXVECTOR2 ( 1.f,	0.f );
	//g_sVertBase[2].vTex = D3DXVECTOR2 ( 0.f,	1.f );
	//g_sVertBase[3].vTex = D3DXVECTOR2 ( 1.f,	1.f );

	//pd3dDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 );
	//pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pTempTex512 );
	////pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pColorTex_1st );
	//pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_sVertBase, sizeof(VERTEXRHW) );

	//g_sVertBase[0].vPos = D3DXVECTOR4 ( 200.f,			0.f,				1.f, 1.f );
	//g_sVertBase[1].vPos = D3DXVECTOR4 ( 400.f,	0.f,				1.f, 1.f );
	//g_sVertBase[2].vPos = D3DXVECTOR4 ( 200.f,			(float)200.f,	1.f, 1.f );
	//g_sVertBase[3].vPos = D3DXVECTOR4 ( 400.f,	(float)200.f,	1.f, 1.f );
	//pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pGlowTex_TEMP );
	////pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pColorTex_2nd );
	//pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_sVertBase, sizeof(VERTEXRHW) );

	//g_sVertBase[0].vPos = D3DXVECTOR4 ( 400.f,			0.f,				1.f, 1.f );
	//g_sVertBase[1].vPos = D3DXVECTOR4 ( 600.f,	0.f,				1.f, 1.f );
	//g_sVertBase[2].vPos = D3DXVECTOR4 ( 400.f,			(float)200.f,	1.f, 1.f );
	//g_sVertBase[3].vPos = D3DXVECTOR4 ( 600.f,	(float)200.f,	1.f, 1.f );
	//pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_p512Tex_4th );
	////pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pColorTex_4th );
	//pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_sVertBase, sizeof(VERTEXRHW) );

	//g_sVertBase[0].vPos = D3DXVECTOR4 ( 600.f,			0.f,				1.f, 1.f );
	//g_sVertBase[1].vPos = D3DXVECTOR4 ( 800.f,	0.f,				1.f, 1.f );
	//g_sVertBase[2].vPos = D3DXVECTOR4 ( 600.f,			(float)200.f,	1.f, 1.f );
	//g_sVertBase[3].vPos = D3DXVECTOR4 ( 800.f,	(float)200.f,	1.f, 1.f );
	//pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pReflectTex );
	////pd3dDevice->SetTexture ( 0, DxSurfaceTex::GetInstance().m_pColorTex_4th );
	//pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, g_sVertBase, sizeof(VERTEXRHW) );

	//pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	//pd3dDevice->SetRenderState( D3DRS_LIGHTING, dwLighting );
	//pd3dDevice->SetRenderState( D3DRS_FOGENABLE, dwFogEnable );
	//pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
}

void CWorldEditView::RenderDefferedSoftAlphaForWLD( IDirect3DDevice9* pd3dDevice )
{
	////////////////////////////////////////////////////////////////////////////
	////						ViewPort
	//CLIPVOLUME sCameraCV;
	//D3DXVECTOR3 vUP(0.f,1.f,0.f);
	//NSDeffered::ComputeClipVolume( sCameraCV, 
	//								DxViewPort::GetInstance().GetLookatPt(),
	//								DxViewPort::GetInstance().GetFromPt(),
	//								vUP,
	//								DxViewPort::GetInstance().GetNearPlane(),
	//								DxViewPort::GetInstance().GetWidth(),
	//								DxViewPort::GetInstance().GetHeight(),
	//								DxViewPort::GetInstance().GetFOV() );



	//////////////////////////////////////////////////////////////////////////
	//					RenderDefferedOpaqueForPSF
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	D3DCOLOR colorClear = D3DCOLOR_XRGB(0,0,0);

	if ( RENDERPARAM::g_emGroundQulity == TnL_PS_3_0_DF )
	{
		NSDeffered::RenderDefferedForSM30DF
		(
			pd3dDevice,
			colorClear,
			m_spDxLandMan.get(),
			DxSurfaceTex::GetInstance().GetWidth(),
			DxSurfaceTex::GetInstance().GetHeight(),
			DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR,
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
			pd3dDevice,
			colorClear,
			m_spDxLandMan.get(),
			DxSurfaceTex::GetInstance().GetWidth(),
			DxSurfaceTex::GetInstance().GetHeight(),
			DxSurfaceTex::GetInstance().m_pColorSuf16F_HDR,
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

	m_spDxLandMan->RenderEff_Deffered( m_pd3dDevice, DxViewPort::GetInstance().GetClipVolume() );
}