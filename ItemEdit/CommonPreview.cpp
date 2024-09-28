#include "stdafx.h"

#include "./CommonPreview.h"

#include "../EngineLib/DxBase/RenderInterface.h"
#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/TextTexture/TextUtil.h"

#include "../SigmaCore/DebugInclude.h"

CCommonPreview& CCommonPreview::GetInstance()
{
	static CCommonPreview s_cInstance;
	return s_cInstance;
}

bool CCommonPreview::Create( CWnd* pParent )
{
	int nScreenW = GetSystemMetrics( SM_CXSCREEN );
	int nScreenH = GetSystemMetrics( SM_CYSCREEN );

	CRect cRect( 0, 0, 400, 400 );
	LONG nWindowStyle = WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	AdjustWindowRectEx( &cRect, nWindowStyle, FALSE, WS_EX_APPWINDOW );

	cRect = CRect(
		nScreenW - ( cRect.right - cRect.left ),
		0,
		nScreenW,
		cRect.bottom - cRect.top );

	BOOL bCreate = CWnd::CreateEx(
		WS_EX_APPWINDOW,
		NULL,
		AfxGetAppName(),
		nWindowStyle,
		cRect,
		pParent,
		NULL,
		NULL );

	return bCreate ? true : false;
}

void CCommonPreview::Destroy()
{
	UnregisterClass( AfxGetAppName(), AfxGetInstanceHandle() );
}

BOOL CCommonPreview::PreCreateWindow( CREATESTRUCT& cs )
{
	if( !CWnd::PreCreateWindow( cs ) )
		return FALSE;

	cs.lpszClass = AfxRegisterWndClass(
        CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
        LoadCursor( NULL, IDC_ARROW ),
        (HBRUSH)CreateSolidBrush( RGB( 0, 0, 0 ) ),
        NULL );

	return TRUE;
}

LRESULT CCommonPreview::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
    LRESULT lr = CWnd::WindowProc( message, wParam, lParam );

	lr |= CD3DViewport::WindowProc( message, wParam, lParam );

    return lr;
}

VOID CCommonPreview::OnFrameMove(
    IDirect3DDevice9* pd3dDevice,
    DOUBLE fTime,
    FLOAT fElapsedTime )
{
    if( !IsVisible() )
        return;
}

VOID CCommonPreview::OnFrameRender(
    IDirect3DDevice9* pd3dDevice,
    DOUBLE fTime,
    FLOAT fElapsedTime )
{
	if( !IsVisible() )
        return;

	const CRect& cSourceRect = GetSourceRect();

    BeginScene( pd3dDevice, D3DCOLOR_XRGB( 0, 0, 0 ) );

	CTextUtil::Get()->Render( TRUE );

	g_pRI->SetOrigin( m_sOrigin );
    g_pRI->SetZoom( m_fZoom );

    g_pRI->SetViewport(
        pd3dDevice,
        cSourceRect.left,
        cSourceRect.top,
        cSourceRect.Width(),
        cSourceRect.Height() );

	CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont(
		_DEFAULT_FONT,
		9,
		_DEFAULT_FONT_SHADOW_FLAG );

	if( pFont )
	{
		CString strText;
		strText.Format( _T( "%f" ), fElapsedTime );

		pFont->DrawText(
			2.0f,
			72.0f,
			D3DCOLOR_XRGB( 255, 255, 255 ),
			strText.GetString() );
	}

	CTextUtil::Get()->Render( FALSE );

    EndScene( pd3dDevice );
}