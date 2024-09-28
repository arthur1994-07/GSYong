#include "stdafx.h"

#include "../RanLogic/RANPARAM.h"

#include "../VisualMaterialLib/Preview/NSVM_PREVIEW.h"
#include "../VisualMaterialLib/ViewPort/DxViewPortVM.h"
#include "../VisualMaterialLib/Edit/DxEditVM.h"
#include "../VisualMaterialLib/DxVisualMaterial.h"

#include "../EngineLib/Common/SubPath.h"
#include "../EngineLib/texttexture/TextUtil.h"
#include "../EngineLib/DxTools/TextureManager.h"
#include "../EngineLib/DxTools/DxInputDevice.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../EngineLib/DxTools/DxViewPort.h"
#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/DxCommon/dxutil.h"
#include "../EngineLib/DxResponseMan.h"

#include "./MainFrm.h"

#include "Tool_VisualMaterialView.h"

extern BOOL	g_bFRAME_LIMIT;

//--------------------------------------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------------------------------------
HRESULT CTool_VisualMaterialView::ConfirmDevice( D3DCAPSQ* pCaps, DWORD dwBehavior, D3DFORMAT Format )
{
	if ( dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING )
		return E_FAIL;

    return S_OK;
}

HRESULT CTool_VisualMaterialView::OneTimeSceneInit()
{
    return S_OK;
}

HRESULT CTool_VisualMaterialView::InitDeviceObjects()
{
    D3DSURFACE_DESC		d3dsdBackBuffer;
    LPDIRECT3DSURFACEQ pBackBuffer;
	m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
    pBackBuffer->GetDesc( &d3dsdBackBuffer );
    pBackBuffer->Release();

    OnCreateDevice( m_pd3dDevice, &d3dsdBackBuffer );

    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    if ( !pFrame )
        return S_OK;

    pFrame->OnCreateDevice( m_pd3dDevice );

    return S_OK;
}

HRESULT CTool_VisualMaterialView::RestoreDeviceObjects()
{
    D3DSURFACE_DESC		d3dsdBackBuffer;
    LPDIRECT3DSURFACEQ pBackBuffer;
	m_pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
    pBackBuffer->GetDesc( &d3dsdBackBuffer );
    pBackBuffer->Release();

    OnResetDevice( m_pd3dDevice, &d3dsdBackBuffer );

    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    if ( !pFrame )
        return S_OK;

    pFrame->OnResetDevice( m_pd3dDevice );

    return S_OK;
}

HRESULT CTool_VisualMaterialView::FrameMove( BOOL bNotRendering )
{
    OnFrameMove( m_pd3dDevice, m_fTime, m_fElapsedTime );

    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    if ( !pFrame )
        return S_OK;

    pFrame->OnFrameMove( m_fElapsedTime );

    return S_OK;
}

HRESULT CTool_VisualMaterialView::Render()
{
    OnFrameRender( m_pd3dDevice, m_fTime, m_fElapsedTime );

    return S_OK;
}

HRESULT CTool_VisualMaterialView::InvalidateDeviceObjects()
{
    OnLostDevice();

    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
    if ( !pFrame )
        return S_OK;
        
    pFrame->OnLostDevice();

    return S_OK;
}

HRESULT CTool_VisualMaterialView::DeleteDeviceObjects()
{
    OnDestroyDevice();

    return S_OK;
}

HRESULT CTool_VisualMaterialView::FinalCleanup()
{
    // Release Instance
    DxViewPortVM::GetInstance()->ReleaseInstance();

    return S_OK;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------------------------------------
HRESULT CTool_VisualMaterialView::OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    //-----------------------------------------------------------------
	// Note : OnInit Data
	//-----------------------------------------------------------------
	CMainFrame *pFrame = (CMainFrame*) AfxGetMainWnd(); // AfxGetApp()->m_pMainWnd;
    EngineDeviceMan* pEngineDevice = pFrame->GetEngineDevice();

	//	Note : 각종 장치들.
	pEngineDevice->OneTimeSceneInit(
        pFrame->m_szAppPath,
        this,
        RANPARAM::strFontType,
        false,
        RENDERPARAM::emLangSet,
        RANPARAM::strGDIFont);

	//-----------------------------------------------------------------
	// Note : Init Data
	//-----------------------------------------------------------------
	//	Note : 각종 장치들.
	//
	pEngineDevice->InitDeviceObjects ( pd3dDevice, FALSE );

    // DxViewPortVM 관련 초기화
    {
        DxViewPortVM::GetInstance()->OnCreateDevice( pd3dDevice, m_hWnd );
        DxViewPortVM::GetInstance()->SetViewPort();

        D3DXVECTOR3 vFromPt		= D3DXVECTOR3( 0.0f, 0.0f, -50.0f );
	    D3DXVECTOR3 vLookatPt	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	    D3DXVECTOR3 vUpVec		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
        DxViewPortVM::GetInstance()->SetViewTrans( vFromPt, vLookatPt, vUpVec );

        DxViewPortVM::GetInstance()->SetProjection
        ( 
            D3DX_PI/4, 
            (float)m_d3dpp.BackBufferWidth, 
            (float)m_d3dpp.BackBufferHeight, 
            5.f,
            3000.0f 
        );
    }

    DxFontMan::GetInstance().InitDeviceObjects ( m_pd3dDevice );
	CD3DFontPar* pD3dFont9 = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );

    // VisualMaterial Main 쪽 생성
    m_pVisualMaterial->Create( pd3dDevice );

    return S_OK;
}

void CTool_VisualMaterialView::OnDestroyDevice()
{
	//-----------------------------------------------------------------
	// Note : Delete
	//-----------------------------------------------------------------
	//	Note : 각종 장치들.
	//
    CMainFrame *pFrame = (CMainFrame*) AfxGetMainWnd(); // AfxGetApp()->m_pMainWnd;
    EngineDeviceMan* pEngineDevice = pFrame->GetEngineDevice();
	pEngineDevice->DeleteDeviceObjects ();

	//-----------------------------------------------------------------
	// Note : Final
	//-----------------------------------------------------------------
	HRESULT hr(S_OK);

	//	Note : 각종 장치들.
	//
	pEngineDevice->FinalCleanup ();
}

HRESULT CTool_VisualMaterialView::OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
	//	Note : 각종 장치들.
	//
    CMainFrame *pFrame = (CMainFrame*) AfxGetMainWnd(); // AfxGetApp()->m_pMainWnd;
    EngineDeviceMan* pEngineDevice = pFrame->GetEngineDevice();
	pEngineDevice->RestoreDeviceObjects ();

    // VisualMaterial
    m_pVisualMaterial->OnResetDevice();

    // ViewPort
    DxViewPortVM::GetInstance()->SetViewPort();
    DxViewPortVM::GetInstance()->SetProjection
    ( 
        D3DX_PI/4, 
        (float)m_d3dpp.BackBufferWidth, 
        (float)m_d3dpp.BackBufferHeight, 
        5.f,
        3000.0f 
    );

    SAFE_RELEASE( m_pSwapChainMain );
	GetD3DDevice()->GetSwapChain( 0, &m_pSwapChainMain );

    DxEditVM::OnResetDeviceSTATIC( pd3dDevice );

    return S_OK;
}

void CTool_VisualMaterialView::OnLostDevice()
{
	//	Note : 각종 장치들.
	//
    CMainFrame *pFrame = (CMainFrame*) AfxGetMainWnd(); // AfxGetApp()->m_pMainWnd;
    EngineDeviceMan* pEngineDevice = pFrame->GetEngineDevice();
	pEngineDevice->InvalidateDeviceObjects ();

    // VisualMaterial
    m_pVisualMaterial->OnLostDevice();

    SAFE_RELEASE( m_pSwapChainMain );

    DxEditVM::OnLostDeviceSTATIC();
}

void CTool_VisualMaterialView::OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{

	//	Note : 각종 장치들.
	//
    CMainFrame *pFrame = (CMainFrame*) AfxGetMainWnd(); // AfxGetApp()->m_pMainWnd;
    EngineDeviceMan* pEngineDevice = pFrame->GetEngineDevice();
	pEngineDevice->FrameMove( (float)fTime, fElapsedTime, TRUE );

    // 인터페이스 작업
    if( DxInputDevice::GetInstance().IsMouseInGameScreen( TRUE ) )
	{
	    if( DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT ) & DXKEY_UP )
	    {
		    m_pVisualMaterial->MouseUp();
	    }
	}

    // VisualMaterial
    m_pVisualMaterial->FrameMove( fElapsedTime );
    
    // Edit 중이 아닐 경우만 카메라를 움직일 수 있다.
    if ( m_pVisualMaterial->IsCameraMove() )
    {
        // 카메라 정보
        DxViewPortVM::GetInstance()->FrameMove( fElapsedTime );
    }

    // 선택된 물체가 바뀌었을 경우    
    pFrame->SelectCommand( m_pVisualMaterial->GetCommand() );
    pFrame->SetSource( NSVM_PREVIEW::GetSourceString(), NSVM_PREVIEW::GetSourceString_Simple() );
}

void CTool_VisualMaterialView::OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();

    LPDIRECT3DSURFACE9 pBackBuffer = NULL;
    LPDIRECT3DSURFACE9 pZBuffer = NULL;
	m_pSwapChainMain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	pd3dDevice->SetRenderTarget( 0, pBackBuffer );
    {
	    // Clear the viewport
        D3DCOLOR colorClear(0xffaaaaaa);
	    HRESULT hr = pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
						    colorClear, 1.0f, 0L );

	    // Begin the scene
	    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	    {
            D3DLIGHT9 sLight;
            sLight.Type = D3DLIGHT_DIRECTIONAL;
            sLight.Diffuse.r = sLight.Diffuse.g = sLight.Diffuse.b = sLight.Diffuse.a = 1.f;
            sLight.Direction.x = sLight.Direction.y = sLight.Direction.z = 1.f;
            sLight.Range = 10.f;
            pd3dDevice->SetLight ( 0, &sLight );
		    pd3dDevice->LightEnable ( 0, FALSE );

            // Device에 카메라 셋팅 정보를 등록한다.
            DxViewPortVM::GetInstance()->SetTransform();

            // VisualMaterial Render
            m_pVisualMaterial->Render();

            // Render Text
            RenderText();

		    // End the scene.
		    pd3dDevice->EndScene();

            // 
            m_pSwapChainMain->Present( NULL, NULL, this->m_hWnd, NULL, 0 );
	    }
    }
    pBackBuffer->Release();
}

//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CTool_VisualMaterialView::RenderText()
{
	D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,255);
    D3DCOLOR fontWarningColor = D3DCOLOR_ARGB(255,0,255,255);
    TCHAR szMsg[MAX_PATH] = TEXT("");

	CD3DFontPar* pD3dFont = DxFontMan::GetInstance().FindFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	if( !pD3dFont )	
        return S_OK;

    // Output display stats
    FLOAT fNextLine = 40.0f; 

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine = 0.0f;
    pD3dFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine += 20.0f;
    pD3dFont->DrawText( 2, fNextLine, fontColor, szMsg );

    fNextLine += 20.0f;
    pD3dFont->DrawText( 2, fNextLine, fontColor, CMainFrame::g_strVertexInstructionsInfo.c_str() );

    fNextLine += 20.0f;
    pD3dFont->DrawText( 2, fNextLine, fontColor, CMainFrame::g_strPixelInstructionsInfo.c_str() );

    return S_OK;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------------------------------------

//BOOL CTool_VisualMaterialView::Render3DEnvironment()
//{
//	if( DXUTGetD3DDevice() != NULL )
//        DXUTRender3DEnvironment();
//
//	return TRUE;
//}

BOOL CTool_VisualMaterialView::RenderApp()
{
    //if ( FAILED ( Render3DEnvironment() ) )
    //    return FALSE;
    //return TRUE;

	HRESULT hr;

    // Test the cooperative level to see if it's okay to render
    if( FAILED( hr = m_pd3dDevice->TestCooperativeLevel() ) )
    {
        // If the device was lost, do not render until we get it back
        if( D3DERR_DEVICELOST == hr )
		{
            //sc::writeLogError(std::string("Reset(), D3DERR_DEVICELOST _ Render3DEnvironment() FAILED"));
			Sleep( 50 );
            return TRUE;
		}

        // Check if the device needs to be resized.
        if( D3DERR_DEVICENOTRESET == hr )
        {
            // If we are windowed, read the desktop mode and use the same format for
            // the back buffer
            if( m_bDefWin )
            {
                D3DAdapterInfo* pAdapterInfo = &m_Adapters[m_dwAdapter];
                m_pD3D->GetAdapterDisplayMode( m_dwAdapter, &pAdapterInfo->d3ddmDesktop );
                m_d3dpp.BackBufferFormat = pAdapterInfo->d3ddmDesktop.Format;
            }

            if( FAILED( hr = Resize3DEnvironment() ) )		return FALSE;
        }
        return FALSE;
    }

	//static double fBeforeTime = 0.0f;
	//static double fCurrentTime = 0.0f;
	//static double fTimeDelta = 0.0f;

	//if( !m_bDefWin || g_bFRAME_LIMIT )
	//{
	//	// 현재 시간을 얻어옴
	//	fCurrentTime = DXUtil_Timer( TIMER_GETAPPTIME );

	//	// 다음 랜더링 될 시점을 구함
	//	fTimeDelta = fBeforeTime+0.033f;

	//	// 만약 현재가 첫 프레임이나 timeGetTime()이 랜더링될 시점의 시간보다 크면 랜더링
	//	if( fBeforeTime == 0.0f || fCurrentTime >= fTimeDelta )	
	//	{
	//		// 메인루프 처리
	//		// 현재 시간을 g_BeforeTime에 넣음
	//		fBeforeTime = fCurrentTime;
	//	}
	//	else
	//	{
	//		Sleep(0);
	//		return TRUE;
	//	}
	//}

	// Get the app's time, in seconds. Skip rendering if no time elapsed
    FLOAT fAppTime        = DXUtil_Timer( TIMER_GETAPPTIME );
    FLOAT fElapsedAppTime = DXUtil_Timer( TIMER_GETELAPSEDTIME );

	if( ( 0.0f == fElapsedAppTime ) && m_bFrameMoving )
        return TRUE;
	
	// FrameMove (animate) the scene
	if( m_bFrameMoving || m_bSingleStep )
	{
		// Store the time for the app
		m_fTime        = fAppTime;
		m_fElapsedTime = fElapsedAppTime;
	
		// Frame move the scene
		if( FAILED( hr = FrameMove( FALSE ) ) )
			return FALSE;
		
		m_bSingleStep = FALSE;
	}

	// Render the scene as normal
	if( FAILED( hr = Render() ) )
		return FALSE;

	return TRUE;
}

