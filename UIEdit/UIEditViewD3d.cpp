// UIEditView.cpp : CUIEditView 클래스의 구현
//

#include "stdafx.h"
#include "../EngineLib/DxResponseMan.h"
#include "../EngineLib/GUInterface/InterfaceCfg.h"
#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/TextTexture/TextUtil.h"
#include "../EngineLib/GUInterface/GameTextControl.h"

#include "../RanLogic/RANPARAM.h"

#include "./MainUI/InterfaceAdmin.h"
#include "MainFrm.h"
#include "UIEdit.h"
#include "UIEditDoc.h"
#include "UIEditView.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

VOID CUIEditView::CreateMain()
{
	SAFE_RELEASE( m_pSwapChainMain );
	GetD3dDevice()->GetSwapChain( 0, &m_pSwapChainMain );
}

VOID CUIEditView::CreateTextureView()
{
	D3DPRESENT_PARAMETERS presentParams;
	ZeroMemory( &presentParams, sizeof( presentParams ) );

	presentParams.Windowed = TRUE;
	presentParams.SwapEffect = D3DSWAPEFFECT_COPY;
	presentParams.BackBufferWidth = 1024;
	presentParams.BackBufferHeight = 920;
	presentParams.BackBufferFormat = D3DFORMAT( D3DADAPTER_DEFAULT );

	SAFE_RELEASE( m_pSwapChainTextureView );
	GetD3dDevice()->CreateAdditionalSwapChain( &presentParams, &m_pSwapChainTextureView );
}

VOID CUIEditView::CreateMagnifyingGlass()
{
	D3DPRESENT_PARAMETERS presentParams;
	ZeroMemory( &presentParams, sizeof( presentParams ) );

	presentParams.Windowed = TRUE;
	presentParams.SwapEffect = D3DSWAPEFFECT_COPY;
	presentParams.BackBufferWidth = 512;
	presentParams.BackBufferHeight = 512;
	presentParams.BackBufferFormat = D3DFORMAT( D3DADAPTER_DEFAULT );

	SAFE_RELEASE( m_pSwapChainMagnifyingGlass );
	GetD3dDevice()->CreateAdditionalSwapChain( &presentParams, &m_pSwapChainMagnifyingGlass );
}

VOID CUIEditView::CreateAniPreView()
{
	D3DPRESENT_PARAMETERS presentParams;
	ZeroMemory( &presentParams, sizeof( presentParams ) );

	presentParams.Windowed = TRUE;
	presentParams.SwapEffect = D3DSWAPEFFECT_COPY;
	presentParams.BackBufferWidth = 200;
	presentParams.BackBufferHeight = 200;
	presentParams.BackBufferFormat = D3DFORMAT( D3DADAPTER_DEFAULT );

	SAFE_RELEASE( m_pSwapChainAniPreView );
	GetD3dDevice()->CreateAdditionalSwapChain( &presentParams, &m_pSwapChainAniPreView );
}

void CUIEditView::ResetAllMode(int nBeforeMode)
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();    

	switch(nBeforeMode)
	{
	case UI_EDIT_MODE: 
		{
			pInterfaceAdmin->InvalidateDeviceObjects();
			pInterfaceAdmin->DeleteDeviceObjects();
			pInterfaceAdmin->FinalCleanup();
			pInterfaceAdmin->DeleteAllControl ();
		}
		break;
	case UI_INNER_MODE:
		{
			//CInnerInterface::GetInstance()->InvalidateDeviceObjects();
			//CInnerInterface::GetInstance()->DeleteDeviceObjects();
			//CInnerInterface::GetInstance()->FinalCleanup();
			//CInnerInterface::GetInstance()->DeleteAllControl ();
		}
		break;
	case UI_OUTER_MODE:
		{
			//COuterInterface::GetInstance()->InvalidateDeviceObjects();
			//COuterInterface::GetInstance()->DeleteDeviceObjects();
			//COuterInterface::GetInstance()->FinalCleanup();
			//COuterInterface::GetInstance()->DeleteAllControl ();
		}
		break;
	case UI_LOADING_MODE:
		{
			//CLoadingInterface::GetInstance()->InvalidateDeviceObjects();
			//CLoadingInterface::GetInstance()->DeleteDeviceObjects();
			//CLoadingInterface::GetInstance()->FinalCleanup();
			//CLoadingInterface::GetInstance()->DeleteAllControl();
		}
		break;
	}

	CInterfaceCfg::GetInstance().Clear();
}

void CUIEditView::SetEditMode(int nBeforeMode)
{
	if (nBeforeMode != -1)
        ResetAllMode(nBeforeMode);

	//CInterfaceCfg::GetInstance().LoadText( _T("testcfg.xml") );
	CInterfaceCfg::GetInstance().LoadText( _T("testcfg.xml"), RANPARAM::strCountry);

	RECT lpRectWindow;
	GetClientRect(&lpRectWindow);

    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	pInterfaceAdmin->SetResolution(MAKELONG(lpRectWindow.bottom,lpRectWindow.right));
	pInterfaceAdmin->OneTimeSceneInit ();
	pInterfaceAdmin->InitDeviceObjects(GetD3dDevice()); 
	pInterfaceAdmin->RestoreDeviceObjects(GetD3dDevice()); 
	pInterfaceAdmin->ClearUIList();
	pInterfaceAdmin->SetViewSize(m_nViewWidth, m_nViewHeight);
}

HRESULT CUIEditView::OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
	HRESULT hr(S_OK);

	WORD wWidth = 800;
	WORD wHeight = 600;

	//	Note : 현재해상도 받아오기.
	//
	D3DSURFACE_DESC		d3dsdBackBuffer;
	LPDIRECT3DSURFACEQ	pBackBuffer = NULL;
	pd3dDevice->GetBackBuffer ( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );

	if ( pBackBuffer )
	{

		pBackBuffer->GetDesc( &d3dsdBackBuffer );
		pBackBuffer->Release();

		wWidth = WORD ( d3dsdBackBuffer.Width );
		wHeight = WORD ( d3dsdBackBuffer.Height );
	}

	//RECT m_rcWindow;
	//-----------------------------------------------------------------
	// Note : OnInit Data
	//-----------------------------------------------------------------
	CUIEditApp *pApp = (CUIEditApp *) AfxGetApp();

	TCHAR szFullPath[MAX_PATH] = {0};

	StringCchCopy ( szFullPath, MAX_PATH, pApp->m_szAppPath );
	StringCchCat ( szFullPath, MAX_PATH, _T("\\data\\gui\\"));

	CInterfaceCfg::GetInstance().SetPath(szFullPath);
	CGameTextMan::GetInstance().SetPath(szFullPath);

	StringCchCopy(
		szFullPath,
		MAX_PATH,
		pApp->m_szAppPath );

	//	Note : 각종 장치들.
	//CORE_COMMON::getFileSystem()->readFileList();
	//CORE_COMMON::getFileSystem()->setMode( true );
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    EngineDeviceMan* pEngineDevice = pFrame->GetEngineDevice();
    hr = pEngineDevice->OneTimeSceneInit(pApp->m_szAppPath, this, FALSE, false, language::DEFAULT, _T("돋움체"));
	if (FAILED(hr))
		return E_FAIL;

	pEngineDevice->InitDeviceObjects ( GetD3dDevice(), TRUE );

	SetEditMode();

	CreateMain();
	CreateTextureView();
	CreateAniPreView();
	CreateMagnifyingGlass();

	return S_OK;
}

void CUIEditView::OnDestroyDevice()
{
	//-----------------------------------------------------------------
	// Note : Delete
	//-----------------------------------------------------------------

#ifdef _DEBUG
	_CrtCheckMemory();
#endif //_DEBUG

	if (GetActiveUIMan())
        GetActiveUIMan()->DeleteDeviceObjects ();
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    EngineDeviceMan* pEngineDevice = pFrame->GetEngineDevice();
	pEngineDevice->DeleteDeviceObjects();
	if (GetActiveUIMan())
        GetActiveUIMan()->FinalCleanup();
}

HRESULT CUIEditView::OnResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc )
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    EngineDeviceMan* pEngineDevice = pFrame->GetEngineDevice();
	pEngineDevice->RestoreDeviceObjects();

	if( GetActiveUIMan())
		GetActiveUIMan()->RestoreDeviceObjects(pd3dDevice);

	CreateMain();
	CreateTextureView();
	CreateMagnifyingGlass();

	return S_OK;
}

void CUIEditView::OnLostDevice()
{
	//cLine.Destroy();
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    EngineDeviceMan* pEngineDevice = pFrame->GetEngineDevice();
	pEngineDevice->InvalidateDeviceObjects();

	if( GetActiveUIMan())
		GetActiveUIMan()->InvalidateDeviceObjects();

	SAFE_RELEASE( m_pSwapChainMain );
	SAFE_RELEASE( m_pSwapChainTextureView );
	SAFE_RELEASE( m_pSwapChainMagnifyingGlass );
	SAFE_RELEASE( m_pSwapChainAniPreView );
}

void CUIEditView::OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
	if (GetActiveUIMan())
    {
        //GetActiveUIMan()->FrameMove(pd3dDevice,fElapsedTime);
    }
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    EngineDeviceMan* pEngineDevice = pFrame->GetEngineDevice();
	pEngineDevice->FrameMove((float)fTime, fElapsedTime, DXUTIsWindowed());
}

void CUIEditView::OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime )
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();

	CreateMain();
	CreateTextureView();
	CreateMagnifyingGlass();
	CreateAniPreView();

	// 텍스쳐에 문자열 작업
	//CTextUtil::Get()->Render( TRUE );
	LPDIRECT3DSURFACE9 pBackBuffer = NULL;
	m_pSwapChainMain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
	GetD3dDevice()->SetRenderTarget( 0, pBackBuffer );

	// 클리어 색 결졍
	D3DCOLOR colorClear = D3DCOLOR_XRGB(89,135,179);
	D3DCOLOR colorView  = D3DCOLOR_XRGB(0,0,0);

	switch( GetActiveMode() )
	{
	case UI_EDIT_MODE: colorClear = D3DCOLOR_XRGB(89,135,179); break;
	case UI_INNER_MODE: colorClear = D3DCOLOR_XRGB(89,135,179); break;
	case UI_OUTER_MODE: colorClear = D3DCOLOR_XRGB(89,135,179); break;
	case UI_LOADING_MODE: colorClear = D3DCOLOR_XRGB(0,0,0); break;
	}

	// Clear the viewport
	HRESULT hr = GetD3dDevice()->Clear(
        0L,
        NULL,
        D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		colorClear,
        1.0f,
        0L);

	//// Begin the scene
	if( SUCCEEDED( GetD3dDevice()->BeginScene() ) )
	{
		//CTextUtil::Get()->SetStateBlock( TRUE );
		switch(GetActiveMode())
		{
		case UI_EDIT_MODE: 
			{
				pInterfaceAdmin->SetViewSize(m_nViewWidth, m_nViewHeight);
			
				if(pInterfaceAdmin->IsGraphVisible())
				{
					DrawGraphPaper();		
					colorView = colorClear;
				}
				else
					colorView  = D3DCOLOR_XRGB(0,0,0);
			}
			break;
		case UI_INNER_MODE: 
			{
				if(GetActiveUIMan()) GetActiveUIMan()->SetResolution(MAKELONG(m_nViewHeight,m_nViewWidth));
				colorView  = D3DCOLOR_XRGB(0,0,0);
			}
			break;
		case UI_OUTER_MODE: 
			{
				if(GetActiveUIMan()) GetActiveUIMan()->SetResolution(MAKELONG(m_nViewHeight,m_nViewWidth));
				colorView  = D3DCOLOR_XRGB(0,0,0);
			}
			break;
		case UI_LOADING_MODE:
			{
				if(GetActiveUIMan()) GetActiveUIMan()->SetResolution(MAKELONG(m_nViewHeight,m_nViewWidth));
				colorView  = D3DCOLOR_XRGB(255,255,255);
			}
			break;
		}

		DrawViewSizeLine( m_nViewWidth, m_nViewHeight, colorView );	

		//if(GetActiveUIMan()) GetActiveUIMan()->BGRender(GetD3dDevice());
		if(GetActiveUIMan()) GetActiveUIMan()->Render(GetD3dDevice());

		GetD3dDevice()->EndScene();
		m_pSwapChainMain->Present( NULL, NULL, this->m_hWnd, NULL, 0 );
	}

	SAFE_RELEASE( pBackBuffer );
}

BOOL CUIEditView::Render()
{
	if( DXUTGetD3DDevice() != NULL )
		DXUTRender3DEnvironment();

	return TRUE;
}

void CUIEditView::PostNcDestroy()
{
	SAFE_RELEASE( m_pSwapChainMain );
	SAFE_RELEASE( m_pSwapChainTextureView );
	SAFE_RELEASE( m_pSwapChainMagnifyingGlass );
	SAFE_RELEASE( m_pSwapChainAniPreView );
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    EngineDeviceMan* pEngineDevice = pFrame->GetEngineDevice();
	pEngineDevice->DeleteDeviceObjects();
	pEngineDevice->FinalCleanup();
	CView::PostNcDestroy();
}

void CUIEditView::DrawGraphPaper()
{
    CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
    CInterfaceAdmin* pInterfaceAdmin = pFrame->GetInterfaceAdmin();
	int	nLineinterval = pInterfaceAdmin->GetLineInterval();

	RECT rcWindow;
	GetClientRect( &rcWindow );

	int	nMaxWidthLine  = (rcWindow.right  / nLineinterval) + 1;
	int	nMaxHeightLine = (rcWindow.bottom / nLineinterval) + 1;

	D3DCOLOR LineColor = D3DCOLOR_ARGB(
		pInterfaceAdmin->GetGraphLineColor().a,
		pInterfaceAdmin->GetGraphLineColor().r,
		pInterfaceAdmin->GetGraphLineColor().g,
		pInterfaceAdmin->GetGraphLineColor().b);

	for(int nWidth=0;nWidth<nMaxWidthLine;nWidth++)
	{
		DrawLine(D3DXVECTOR2(nWidth * nLineinterval, rcWindow.top) , D3DXVECTOR2(nWidth * nLineinterval,rcWindow.bottom) , LineColor);
	}

	for(int nHeight=0;nHeight<nMaxHeightLine;nHeight++)
	{
		DrawLine(D3DXVECTOR2(rcWindow.left ,nHeight * nLineinterval) , D3DXVECTOR2(rcWindow.right ,nHeight * nLineinterval) , LineColor);
	}
}

void CUIEditView::DrawViewSizeLine(int nWidth,int nHeight , const D3DCOLOR& _diffuse )
{

	DrawLine(D3DXVECTOR2(0.0f, 0.0f) , D3DXVECTOR2(nWidth,0.0f) , _diffuse);
	DrawLine(D3DXVECTOR2(nWidth,0.0f) , D3DXVECTOR2(nWidth,nHeight) , _diffuse);
	DrawLine(D3DXVECTOR2(nWidth,nHeight) , D3DXVECTOR2(0.0f,nHeight) , _diffuse);
	DrawLine(D3DXVECTOR2(0.0f,nHeight) , D3DXVECTOR2(0.0f,0.0f) , _diffuse);

	CString strPos;
	strPos.Format(_T("(%d,%d)"),nWidth, nHeight);

	CD3DFontPar*			m_pFont;
	m_pFont = DxFontMan::GetInstance().LoadDxFont ( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG );
	m_pFont->DrawText( nWidth, nHeight, _diffuse, strPos, 0, FALSE );

}

void CUIEditView::DrawLine(D3DXVECTOR2 vStart, D3DXVECTOR2 vEnd, const D3DCOLOR& _diffuse )
{
	UIVERTEX stLine[ 2 ];
	for( INT i=0; i<2; ++i )
		stLine[ i ].SetDiffuse(_diffuse);

	stLine[ 0 ].SetPos( vStart );
	stLine[ 1 ].SetPos( vEnd );

	GetD3dDevice()->SetTexture( 0, NULL );
	GetD3dDevice()->SetFVF( UIVERTEX::D3DFVF_UIVERTEX );
	GetD3dDevice()->DrawPrimitiveUP( D3DPT_LINESTRIP, 1, stLine, sizeof( UIVERTEX ) );
}
