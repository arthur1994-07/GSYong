#include "stdafx.h"
#include "../InternalCommonLib/dxstdafx.h"

#include "../EngineLib/Common/SUBPATH.h"
#include "../EngineLib/DxBase/RenderInterface.h"
#include "../EngineLib/DxTools/DxInputDevice.h"
#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/TextTexture/TextUtil.h"

#include "./Frame.h"
#include "./Editor.h"
#include "./TargetView.h"
#include "./VisualScript.h"


#include "../SigmaCore/DebugInclude.h"


namespace
{
    const COLORREF c_ClearWnd = RGB( 0, 0, 0 );
    const DWORD c_ClearD3D = D3DCOLOR_XRGB( 0, 0, 0 );
}

BEGIN_MESSAGE_MAP( CTargetView, CClientView )
    ON_COMMAND( ID_BUTTON_OPEN, OnButtonLoad )
END_MESSAGE_MAP()

CTargetView::CTargetView()
{
    m_bActive = FALSE;
    m_nToolBarID = IDR_TARGETVIEW;
}

CTargetView::~CTargetView()
{
}

BOOL CTargetView::PreCreateWindow( CREATESTRUCT& cs )
{
	if( !CWnd::PreCreateWindow( cs ) )
		return FALSE;

	cs.lpszClass = AfxRegisterWndClass(
        CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
        LoadCursor( NULL, IDC_ARROW ),
        (HBRUSH)CreateSolidBrush( c_ClearWnd ),
        NULL );

	return TRUE;
}

VOID CTargetView::OnButtonLoad()
{
    CFileDialog cDlg(
        TRUE,
        _T( ".lev" ),
        NULL,
		OFN_HIDEREADONLY,
        _T( "level ( *.lev ) |*.lev|" ),
		this );

    CVisualScriptApp* pApp = (CVisualScriptApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

    CString strPath = pPath->AppRoot().c_str(); //SUBPATH::APP_ROOT;
    strPath.Append( pPath->GLogicPath().c_str() ); //SUBPATH::GLOGIC_FILE );
    cDlg.m_ofn.lpstrInitialDir = strPath.GetString();

    if( cDlg.DoModal() != IDOK )
        return;
}

VOID CTargetView::ProcessInput( FLOAT fElapsedTime )
{
}

VOID CTargetView::OnFrameMove(
    IDirect3DDevice9* pd3dDevice,
    DOUBLE fTime,
    FLOAT fElapsedTime )
{
    if( !IsVisible() )
        return;

    if( IsActive() )
        ProcessInput( fElapsedTime );
}

VOID CTargetView::OnFrameRender(
    IDirect3DDevice9* pd3dDevice,
    DOUBLE fTime,
    FLOAT fElapsedTime )
{
    BeginScene( pd3dDevice, c_ClearD3D );

    // Target
    g_pEditor->RenderTarget( fElapsedTime );

    CTextUtil::Get()->Render( TRUE );

    // State
    {
        CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont(
            _DEFAULT_FONT,
            9,
            _DEFAULT_FONT_SHADOW_FLAG );

        if( pFont )
        {
            CString strText;
            strText.Format(
                _T( "%s [Origin %.1f %.1f %.1f]" ),
                IsActive() ? _T( "Active" ) : _T( "DeActive" ),
                0.0f,
                0.0f,
                0.0f );

            pFont->DrawText(
                2.0f,
                2.0f,
                D3DCOLOR_XRGB( 255, 255, 255 ),
                strText.GetString() );
        }
    }

    CTextUtil::Get()->Render( FALSE );

    EndScene( pd3dDevice );
}