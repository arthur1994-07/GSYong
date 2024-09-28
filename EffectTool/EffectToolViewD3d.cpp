#include "stdafx.h"

#include "../EngineLib/DxMeshs/FrameMesh/DxFrameMesh.h"
#include "../EngineLib/DxTools/DebugSet.h"
#include "../EngineLib/DxTools/DxInputDevice.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxTools/dxfontman.h"
#include "../EngineLib/DxTools/DxRenderStates.h"
#include "../EngineLib/DxTools/Light/DxLightMan.h"
#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxEffect/NSEffectThread.h"
#include "../EngineLib/DxEffect/DxLoadShader.h"
#include "../EngineLib/Common/subpath.h"
#include "../EngineLib/Common/profile.h"
#include "../EngineLib/GUInterface/Cursor.h"
#include "../EngineLib/DxTools/DxEditMat.h"
#include "../EngineLib/DxTools/EditMeshs.h"

#include "../EngineLib/DxTools/DxSurfaceTex.h"
#include "../EngineLib/DxTools/DxEnvironment.h"
#include "../EngineLib/DxTools/DxShadowMap.h"
#include "../EngineLib/DxTools/DxGlowMan.h"
#include "../EngineLib/DxTools/DxCubeMap.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"

#include "../EngineLib/DxEffect/DxPostEffect.h"
#include "../EngineLib/DxEffect/Frame/DxEffectMan.h"
#include "../EngineLib/DxEffect/EffProj/DxEffProj.h"
#include "../EngineLib/DxEffect/Single/DxEffSingleMan.h"
#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"
#include "../EngineLib/DxEffect/Single/DxEffGroupPlayer.h"
#include "../EngineLib/DxEffect/EffectLua/NSEffectLuaPlay.h"
#include "../EngineLib/DxMeshs/dxsimplemeshman.h"
#include "../EngineLib/DxMeshs/SkinMesh/NSSkinAniThread.h"
#include "../EngineLib/DxTools/DxEffToTexMan.h"

#include "../EngineLib/DxResponseMan.h"
#include "../EngineLib/G-Logic/glperiod.h"
#include "../EngineLib/DxTools/DxDynamicVB.h"
#include "../EngineLib/DxTools/CubeMap/NSCubeMapCulling.h"
#include "../EngineLib/DxTools/CubeMap/NSCubeMapUtil.h"

#include "../RanLogic/RANPARAM.h"
#include "../RanLogicClient/Char/GLCharacter.h"

#include "EffectTool.h"
#include "MainFrm.h"
#include "EffectToolDoc.h"
#include "EffectToolView.h"
#include "EffectTool.h"
#include "Resource.h"

#include "./Properties/EffGroupPage.h"

BOOL g_bBGTex;

_CUR_TYPE	CEffectToolView::m_curType;
BOOL CEffectToolView::m_bCharView = FALSE;
BOOL CEffectToolView::m_bMapView = TRUE;
int	 CEffectToolView::m_iSheetTap = 0;

extern BOOL				g_bFRAME_LIMIT;
extern BOOL				g_bEFFECT_TOOL;
extern BOOL				g_bEFFtoTEX;

HRESULT CEffectToolView::ConfirmDevice ( D3DCAPSQ* pCaps, DWORD dwBehavior, D3DFORMAT Format )
{
	if( dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING )
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectToolView::OneTimeSceneInit()
{
	//CMainFrame* pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    EngineDeviceMan* pEngineDevice = EngineSingletone::GetInstance()->EngineDevice();
    CEffectToolApp* pApp = (CEffectToolApp*) AfxGetApp();
	RANPARAM::LOAD(pFrame->m_szAppPath, pApp->GetServiceProvider(), true, false);

	// 각종 장치들.
	pEngineDevice->OneTimeSceneInit(
        pFrame->m_szAppPath,
        this,
        RANPARAM::strFontType,
        FALSE,
        RENDERPARAM::emLangSet,
        RANPARAM::strGDIFont);

	g_bEFFECT_TOOL = TRUE;

	return S_OK;
}

HRESULT CEffectToolView::InitDeviceObjects()
{
	DxFontMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	CD3DFontPar* pD3dFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

	//	Note	:	디버그셋 경로 설정 및 초기화
	CDebugSet::InitDeviceObjects( pD3dFont9 );


	// 각종 장치들
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    EngineDeviceMan* pEngineDevice = EngineSingletone::GetInstance()->EngineDevice();
	pEngineDevice->InitDeviceObjects ( m_pd3dDevice );

	// 마우스 커서.
	//
	CCursor::GetInstance().InitDeviceObjects ();

	D3DXVECTOR3 vFromPt		= D3DXVECTOR3( 0.0f, 70.0f, -70.0f );
	D3DXVECTOR3 vLookatPt	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	DxViewPort::GetInstance().SetViewTrans ( vFromPt, vLookatPt, vUpVec );

	// 옥트리 메니져 초기화.
	char szFileName[MAX_PATH] = "Eff_Test.wld";
	BOOL bFalse = m_spDxLandMan->LoadFile ( szFileName, m_pd3dDevice );

	m_spDxLandMan->InitDeviceObjects( m_pd3dDevice );
	m_spDxLandMan->ActiveMap_Tool( m_pd3dDevice, m_spDxLandMan );

	// 캐릭터
	//
	D3DCAPSQ d3dCaps;
	m_pd3dDevice->GetDeviceCaps( &d3dCaps );
	m_pCharacter->Create ( m_spDxLandMan->GetNaviMesh(), NULL, NULL, 0, m_pd3dDevice, m_spDxLandMan.get() );

	CMainFrame *pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( pMainFrame )
	{
		pMainFrame->GetEffGroupPage()->SetCharAnimList();		
	}

    TextureManager::GetInstance().EnableTextureAlphaTypeDebug();

	m_pBackGroundTex = NULL;
	g_bBGTex = FALSE;

	return S_OK;
}


HRESULT CEffectToolView::RestoreDeviceObjects()
{
	HRESULT hr;

	// 각종 장치들.
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    EngineDeviceMan* pEngineDevice = EngineSingletone::GetInstance()->EngineDevice();
	pEngineDevice->RestoreDeviceObjects ();

	// 뷰포트
	//
	DxViewPort::GetInstance().SetProjection ( D3DX_PI/4, (float)m_d3dsdBackBuffer.Width, 
		(float)m_d3dsdBackBuffer.Height, 5.0f, 3000.0f );
	
	hr = DxViewPort::GetInstance().SetViewPort ();
	if ( FAILED(hr) )	return hr;

	D3DXMatrixIdentity( &matParent );
	m_sObjectMRS.OnResetDevice( m_pd3dDevice );

	// 옥트리 메니져 
	m_spDxLandMan->RestoreDeviceObjects ( m_pd3dDevice );

	m_pCharacter->RestoreDeviceObjects ( m_pd3dDevice );

	// 글로우 처리를 하기위해 사용 한다.
	DxGlowMan::GetInstance().SetProjectActiveON();

	// Note : Tex 변환
	DxEffToTexMan::GetInstance().OnResetDevice( m_pd3dDevice );

	DxPostEffect::GetInstance().RestoreDeviceObjects( m_pd3dDevice );

	return S_OK;
}

void CEffectToolView::LoadBGTexture( const char *pszFileName )
{
	SAFE_RELEASE( m_pBackGroundTex );
	D3DXCreateTextureFromFile( m_pd3dDevice, pszFileName, &m_pBackGroundTex );
}


HRESULT CEffectToolView::FrameMove( BOOL bNotRendering )
{
	PROFILE_BLOCKSTART();
	PROFILE_BEGIN("FrameMove");


	DxViewPort::GetInstance().SetToolCamera();

	// Note : 
	CMainFrame *pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	m_sObjectMRS.SetWVL( pMainFrame->m_wndToolBar.m_comboMatrix.GetCurSel() );
	if (m_sObjectMRS.IsCollMode())
        m_curType = _CUR_NONE;

	// 각종 장치들.    
    EngineDeviceMan* pEngineDevice = EngineSingletone::GetInstance()->EngineDevice();
	pEngineDevice->FrameMove( m_fTime, m_fElapsedTime, m_bDefWin );

	GLPeriod::GetInstance().SetHour ( 1 );
	GLPeriod::GetInstance().SetTIMETOVBR ( 0 );

	// 뷰포트
	DxViewPort::GetInstance().FrameMoveRAN ( m_fTime, m_fElapsedTime );

	if ( m_bCharView && !m_iSheetTap )
	{
		// 케릭터 이동에 따른 카메라 제어.
		D3DXVECTOR3 vCamera = m_pCharacter->GetPosition();
		vCamera.y += 10.0f;
		DxViewPort::GetInstance().CameraJump ( vCamera );
		m_pCharacter->FrameMoveForTool( m_fTime, m_fElapsedTime );
	}

	// 옥트리
	if( m_bMapView )	
		m_spDxLandMan->FrameMove ( m_fTime, m_fElapsedTime, DxViewPort::GetInstance().GetClipVolume(), bNotRendering );

	// Note : 화면 효과
	DxEffProjMan::GetInstance().FrameMove( m_fElapsedTime );

	if( DxInputDevice::GetInstance().GetKeyState(DIK_P)&DXKEY_UP )
	{
		DxEffProjMan::GetInstance().EnableGetItemEFF( TRUE );
	}
	if( DxInputDevice::GetInstance().GetKeyState(DIK_S)&DXKEY_UP )
	{
		DxEffProjMan::GetInstance().EnableGetItemEFF( FALSE );
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

void CEffectToolView::RenderBackGround()
{
	VERTEXRHW sVertex[4];

	sVertex[0].vPos = D3DXVECTOR4( 0.f, 0.f, 1.f, 1.f );
	sVertex[1].vPos = D3DXVECTOR4( (float)m_d3dsdBackBuffer.Width, 0.f, 1.f, 1.f );
	sVertex[2].vPos = D3DXVECTOR4( 0.f,							   (float)m_d3dsdBackBuffer.Height, 1.f, 1.f );
	sVertex[3].vPos = D3DXVECTOR4( (float)m_d3dsdBackBuffer.Width, (float)m_d3dsdBackBuffer.Height, 1.f, 1.f );

	sVertex[0].vPos.w = sVertex[1].vPos.w = sVertex[2].vPos.w = sVertex[3].vPos.w = 1.0f;
	sVertex[0].vPos.z = sVertex[1].vPos.z = sVertex[2].vPos.z = sVertex[3].vPos.z = 1.0f;

	sVertex[0].vTex = D3DXVECTOR2( 0.f, 0.f );
	sVertex[1].vTex = D3DXVECTOR2( 1.f, 0.f );
	sVertex[2].vTex = D3DXVECTOR2( 0.f, 1.f );
	sVertex[3].vTex = D3DXVECTOR2( 1.f, 1.f );

	m_pd3dDevice->SetFVF( VERTEXRHW::FVF );
	m_pd3dDevice->SetTexture( 0, m_pBackGroundTex );
	m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, sVertex, sizeof(VERTEXRHW) );
	m_pd3dDevice->SetTexture( 0, NULL );
}

HRESULT CEffectToolView::Render()
{
	PROFILE_BEGIN("Render");

	// Shadow Map Clear
	//
	DxShadowMap::GetInstance().ClearShadow ( m_pd3dDevice, m_spDxLandMan->GetUsedMaterialSystem() );

	// Clear the viewport
	HRESULT hr = m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						D3DCOLOR_XRGB(0,0,0), 1.0f, 0L );//D3DCOLOR_XRGB(89,135,179)

	// Note : 그림자 셋
	DxShadowMap::GetInstance().SetShadowDetail(LAND_QT_LOW_LOW);

	RENDERPARAM::g_emCharacterQulity = TnL_CHAR_PS_2_0;
	//RENDERPARAM::g_emGroundQulity = TnL_PS_1_1;

	// Begin the scene
	if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
	{
		// Note : Post Efffect를 위해 렌더 타겟을 바꾼다.
		DxPostEffect::GetInstance().ChangeFinalTarget( m_pd3dDevice, TRUE );

		// Glow 
		DxGlowMan::GetInstance().RenderTex ( m_pd3dDevice );

		// Light 설정.
		//
		DxLightMan::GetInstance()->Render ( m_pd3dDevice, FALSE, m_spDxLandMan->GetUsedMaterialSystem() );

		// 버텍스 쉐이더 콘스탄트 설정.
		//
		DXLIGHT &Light = *DxLightMan::GetInstance()->GetDirectLight ();
		m_pd3dDevice->SetVertexShaderConstantF( VSC_LIGHTDIRECT, (float*)&Light.m_Light.Direction, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( VSC_LIGHTDIFFUSE, (float*)&Light.m_Light.Diffuse, 1 );
		m_pd3dDevice->SetVertexShaderConstantF( VSC_LIGHTAMBIENT, (float*)&Light.m_Light.Ambient, 1 );

		D3DXVECTOR3 &vFromPt = DxViewPort::GetInstance().GetFromPt ();
		m_pd3dDevice->SetVertexShaderConstantF( VSC_CAMERAPOSITION, (float*)&vFromPt, 1 );

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
			m_pd3dDevice->SetVertexShaderConstantF(i*2+VSC_PLIGHTPOS01, (float*)&vPointPos, 1);
			m_pd3dDevice->SetVertexShaderConstantF(i*2+VSC_PLIGHTDIFF01, (float*)&vPointDiff, 1);
		}

		D3DXMATRIX matView = DxViewPort::GetInstance().GetMatView();
		D3DXMATRIX matProj = DxViewPort::GetInstance().GetMatProj();

		m_pd3dDevice->SetVertexShaderConstantF( VSC_SKIN_DEFAULT, (float*)&D3DXVECTOR4 (1.f, 0.5f, 0.f, 765.01f), 1 );

		D3DXVECTOR3	vLightVector = DxLightMan::GetInstance()->GetDirectLight()->m_Light.Direction;
		D3DXVec3TransformNormal ( &vLightVector, &vLightVector, &matView );
		D3DXVec3Normalize ( &vLightVector, &vLightVector);
		vLightVector = -vLightVector;
		m_pd3dDevice->SetVertexShaderConstantF( VSC_LIGHTDIRECT_VIEW, (float*)&vLightVector, 1 );

		D3DXMatrixTranspose( &matView, &matView );
		D3DXMatrixTranspose( &matProj, &matProj );

		m_pd3dDevice->SetVertexShaderConstantF( VSC_MATWVP_01, (float*)&matView, 4 );
		m_pd3dDevice->SetVertexShaderConstantF( VSC_MATWVP_02, (float*)&matProj, 4 );	

		D3DXVECTOR3	vDirect;
		vDirect.x	=  -Light.m_Light.Direction.x;
		vDirect.y	=  -Light.m_Light.Direction.y;
		vDirect.z	=  -Light.m_Light.Direction.z;
		D3DXVec3Normalize ( &vDirect, &vDirect );
		m_pd3dDevice->SetPixelShaderConstantF( 0, (float*)&vDirect, 1 );				// 빛 방향
		m_pd3dDevice->SetPixelShaderConstantF( 1, (float*)&Light.m_Light.Diffuse, 1 );

		CLIPVOLUME cv = DxViewPort::GetInstance().GetClipVolume ();

		if( g_bBGTex )
		{
			RenderBackGround();
		}

		if( !g_bEFFtoTEX )
		{
			// 환경
			PROFILE_BEGIN("Environment");
			if ( m_bMapView )
			{
				DxEnvironment::GetInstance().Render ( m_pd3dDevice, NULL, m_spDxLandMan.get() );
			}
			PROFILE_END("Environment");

			if ( m_bMapView )
			{
				// 옥트리
				m_spDxLandMan->Render ( m_pd3dDevice, cv );
				//m_spDxLandMan->Render_2Pass ( m_pd3dDevice, cv, TRUE );
			}
			else
			{
				EDITMESHS::RENDERPLANE ( m_pd3dDevice, D3DXVECTOR3(0,0,0), D3DXVECTOR3(100,0,100), D3DXVECTOR3(10,0,10) );
				EDITMESHS::RENDERARROW ( m_pd3dDevice, D3DXVECTOR3(0,0,0), D3DXVECTOR3(100,0,100), D3DXVECTOR3(10,0,10) );
			}

			if ( m_bCharView )
			{
				m_pCharacter->Render( m_pd3dDevice, m_spDxLandMan.get() );
				m_pCharacter->RenderShadow( m_pd3dDevice, FALSE );
			}

			// ShadowMap ImageBlur
			DxShadowMap::GetInstance().LastImageBlur( m_pd3dDevice, m_spDxLandMan->GetUsedMaterialSystem() );


			// 옥트리
			if( m_bMapView )
			{
				m_spDxLandMan->Render_LOW_SoftAlpha( m_pd3dDevice, cv );
				m_spDxLandMan->Render_LOW_SingleEff( m_pd3dDevice, cv );
			}
		}

		// DxEffGroupPlayer Render.
		DxEffGroupPlayer::GetInstance().Render ( m_pd3dDevice );

		// Note : Optimize
		OPTMManager::GetInstance().Render_HardAlpha();
		OPTMManager::GetInstance().Render();

		// 큐브 텍스쳐 렌더링..!!
		//DxCubeMap::GetInstance().Render ( m_pd3dDevice );
		NSCubeMapUtil::NSCubeMapBlend::OnFrameMove( m_fElapsedTime );
		NSCubeMapUtil::NSCubeMapBlend::OnRender( m_pd3dDevice, NSCubeMapCulling::GetCubeMapTexture( DxViewPort::GetInstance().GetLookatPt() ) );

		// 글로우 처리를 하기위해 사용 한다.
		DxGlowMan::GetInstance().RenderTex ( m_pd3dDevice );
		DxGlowMan::GetInstance().Render ( m_pd3dDevice );
		DxGlowMan::GetInstance().RenderBURN( m_pd3dDevice );

		// 지금까지 그린것을 굴절을 위해 복사해놓는다.
		PROFILE_BEGIN("DxEnvironment::UserToRefraction()");
		DxEnvironment::GetInstance().UserToRefraction ( m_pd3dDevice );
		PROFILE_END("DxEnvironment::UserToRefraction()");

		// Note : 포스트 이펙트.
		//			작업되면 DxSurfaceTex::GetInstance().m_pColorBuffer 값이 됨.
		DxPostEffect::GetInstance().RenderPostEffect( m_pd3dDevice ); // by luxes.


		// Wave 되야 하는것을 저장후 마지막으로 뿌린다.
		//
		PROFILE_BEGIN("DxEnvironment::RenderWave()");
		DxEnvironment::GetInstance().RenderWave ( m_pd3dDevice, DxEffectMan::GetInstance().GetBumpTexture() );
		PROFILE_END("DxEnvironment::RenderWave()");

		// Note : 화면 효과
		DxEffProjMan::GetInstance().Render( m_pd3dDevice );

		if( g_bEFFtoTEX )
		{
			// Note : Eff를 Tex로 변화시키기 위함.
			DxEffToTexMan::GetInstance().InsertData( m_pd3dDevice );
		}
		else
		{
			m_sObjectMRS.Render_Temp( m_pd3dDevice, matParent );
			m_sObjectMRS.Render( m_pd3dDevice );

			PROFILE_BEGIN("debug-text");
			RenderText();
			CDebugSet::Render ();
			PROFILE_END("debug-text");
		}

		// 여기서 

		NSEffectPlay::RenderPostEffect( m_pd3dDevice );

		// End the scene.
		//m_pd3dDevice->EndScene();
		while(1)
		{
			if( SUCCEEDED( m_pd3dDevice->EndScene() ) )	break;
		}
	}

	PROFILE_END("Render");
	PROFILE_BLOCKEND();
	PROFILE_DUMPOUTPUT();

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CEffectToolView::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,255);
    D3DCOLOR fontWarningColor = D3DCOLOR_ARGB(255,0,255,255);
    TCHAR szMsg[MAX_PATH] = TEXT("");

	// Output display stats
	CD3DFontPar* pD3dFont = DxFontMan::GetInstance().FindFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	if ( !pD3dFont )	return S_OK;

	// Output display stats
	FLOAT fNextLine;

	lstrcpy( szMsg, m_strDeviceStats );
	fNextLine = 1.0f;
	pD3dFont->DrawText( 2, fNextLine, fontColor, szMsg );

	lstrcpy( szMsg, m_strFrameStats );
	fNextLine = 14.0f;
	pD3dFont->DrawText( 2, fNextLine, fontColor, szMsg );

    return S_OK;
}

HRESULT CEffectToolView::InvalidateDeviceObjects()
{
	// 각종 장치들
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    EngineDeviceMan* pEngineDevice = EngineSingletone::GetInstance()->EngineDevice();
	pEngineDevice->InvalidateDeviceObjects ();

	m_sObjectMRS.OnLostDevice( m_pd3dDevice );

	// 옥트리
	m_spDxLandMan->InvalidateDeviceObjects ();

	m_pCharacter->InvalidateDeviceObjects ();

	// Note : Tex 변환
	DxEffToTexMan::GetInstance().OnLostDevice();

	DxPostEffect::GetInstance().InvalidateDeviceObjects();

	return S_OK;
}

HRESULT CEffectToolView::DeleteDeviceObjects()
{
	// 각종 장치들
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    EngineDeviceMan* pEngineDevice = EngineSingletone::GetInstance()->EngineDevice();
	pEngineDevice->DeleteDeviceObjects();

	// 마우스 커서.
	CCursor::GetInstance().DeleteDeviceObjects ();

	// 옥트리
	m_spDxLandMan->DeleteDeviceObjects ();

	return S_OK;
}

HRESULT CEffectToolView::FinalCleanup()
{
	// 각종 장치들
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    EngineDeviceMan* pEngineDevice = EngineSingletone::GetInstance()->EngineDevice();
	pEngineDevice->FinalCleanup();
	return S_OK;
}

void CEffectToolView::SetCurType ( _CUR_TYPE cType )
{
	switch( cType )
	{
	case _CUR_NONE:
		m_sObjectMRS.SetMode( OBECTMRS_I );
		break;
	case _CUR_MOVE:
		m_sObjectMRS.SetMode( OBECTMRS_M );
		break;
	case _CUR_ROTATE:
		m_sObjectMRS.SetMode( OBECTMRS_R );
		break;
	case _CUR_SCALE:
		m_sObjectMRS.SetMode( OBECTMRS_S );
		break;
	case _CUR_EDIT_BOX:
		m_sObjectMRS.SetMode( OBECTMRS_I );
		break;
	};
}

void CEffectToolView::ActiveEditMatrix( D3DXMATRIX* matWorld )
{
	m_curType = _CUR_MOVE;

	if( matWorld )	m_sObjectMRS.SetMatrix( matWorld );
	else			m_sObjectMRS.SetMatrix( NULL );
}

void CEffectToolView::DeActiveEditMatrix()
{
	m_curType = _CUR_NONE;
	m_sObjectMRS.SetMatrix( NULL );

	D3DXMatrixIdentity( &matParent );
}

void CEffectToolView::ActiveEditBox ( D3DXVECTOR3 &vMax, D3DXVECTOR3 &vMin )
{
	m_curType = _CUR_EDIT_BOX;
	//m_EditBox.SetBox ( vMax, vMin );
}

void CEffectToolView::DeActiveEditBox ()
{
	m_curType = _CUR_NONE;
}

void CEffectToolView::SetCurClick( BOOL bUse )
{
	m_sObjectMRS.SetClick( bUse );
}



