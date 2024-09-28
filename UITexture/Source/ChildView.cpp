#include "stdafx.h"

#include "../../InternalCommonLib/dxstdafx.h"
#include "../../InternalCommonLib/Core/CommonUtil.h"

#include "../../EngineLib/Common/SUBPATH.h"
#include "../../EngineLib/DxBase/RenderDevice.h"
#include "../../EngineLib/DxBase/RenderInterface.h"
#include "../../EngineLib/DxTools/DxInputDevice.h"
#include "../../EngineLib/DxTools/DxFontMan.h"
#include "../../EngineLib/DxTools/TextureManager.h"
#include "../../EngineLib/TextTexture/TextUtil.h"

#include "./UITexture.h"
#include "./Frame.h"
#include "./Editor.h"
#include "./ChildView.h"

#include "../../SigmaCore/DebugInclude.h"

namespace
{
    const COLORREF c_ClearWnd = RGB( 171, 171, 171 );
    DWORD c_ClearD3D = D3DCOLOR_XRGB( 171, 171, 171 );
}

BEGIN_MESSAGE_MAP( CChildView, CWnd )
	ON_COMMAND( ID_BUTTON_NEW, OnButtonNew )
    ON_COMMAND( ID_BUTTON_OPEN, OnButtonLoad )
    ON_COMMAND( ID_BUTTON_SAVE, OnButtonSave )
    ON_COMMAND( ID_BUTTON_OPEN_PSD, OnButtonPsdLoad )
    ON_COMMAND( ID_BUTTON_USEALPHA, OnButtonUseAlpha )
    ON_COMMAND( ID_BUTTON_BGCOLOR, OnButtonBgColor )
    ON_COMMAND( ID_BUTTON_ADD, OnButtonAdd )
    ON_COMMAND( ID_BUTTON_DEL, OnButtonDel )
END_MESSAGE_MAP()

CChildView::CChildView()
{
	m_bActive = TRUE;

	m_sOrigin.x = 50;
	m_sOrigin.y = 50;
}

CChildView::~CChildView()
{
}

VOID CChildView::FindControl()
{
    CRect rcWindow;
    GetWindowRect(&rcWindow);

    ic::Int2 sCilent;
    ic::Int2 mouse = GetClientMousePos(); 

    D3DXVECTOR2 vPos;
    vPos.x = (mouse.x - m_sOrigin.x) / m_fZoom;
    vPos.y = (mouse.y - m_sOrigin.y) / m_fZoom;

    std::string strOutItemName;
    if( g_pEditor->FindControl( vPos, strOutItemName  ) == true )
    {
        m_pFrame->FindItem( strOutItemName );
    }
}

BOOL CChildView::PreCreateWindow( CREATESTRUCT& cs )
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

VOID CChildView::OnButtonNew()
{
	g_pEditor->Reset();

	m_pFrame->RefreshAll();
}

VOID CChildView::OnButtonLoad()
{
	CFileDialog cDlg(
        TRUE,
        _T( ".xml" ),
        NULL,
		OFN_HIDEREADONLY,
        _T( "TextureList ( *.xml ) |*.xml|" ),
		this );

    const SUBPATH* pPath = theApp.GetSubPath();
	CString strPath = pPath->AppRoot().c_str();
    strPath.Append( pPath->GuiFileRoot().c_str() );
    cDlg.m_ofn.lpstrInitialDir = strPath.GetString();

    if( cDlg.DoModal() != IDOK )
        return;

    if( !g_pEditor->LoadXml( cDlg.GetPathName().GetString() ) )
        return;

	m_pFrame->RefreshAll();
}

VOID CChildView::OnButtonSave()
{
	CFileDialog cDlg(
        FALSE,
        _T( ".xml" ),
        NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        _T( "TextureList ( *.xml ) |*.xml|" ),
		this );

    const SUBPATH* pPath = theApp.GetSubPath();
	CString strPath = pPath->AppRoot().c_str();
    strPath.Append( pPath->GuiFileRoot().c_str() );
    cDlg.m_ofn.lpstrInitialDir = strPath.GetString();

    if( cDlg.DoModal() != IDOK )
        return;

    g_pEditor->SaveXml( cDlg.GetPathName().GetString() );
}

VOID CChildView::OnButtonPsdLoad()
{
    if( g_pEditor->IsLoaded() == FALSE )
    {
        MessageBox( _T("uiinnerTextureList.xml 파일을 우선 열어 주십시오."), _T("MessageBox"), MB_OK );
        return;
    }
    const SUBPATH* pPath = theApp.GetSubPath();
    CString strPath = pPath->AppRoot().c_str();


    CString strPsdParserPath;
    strPsdParserPath = strPath + "\\psd_parser.exe";
    CFile f;
    BOOL bIsOpen = f.Open( strPsdParserPath, CFile::modeRead );
    f.Close();

    if( bIsOpen == FALSE )
    {
        //실행파일 선택
        CFileDialog cDlgEXE(
            TRUE,
            _T( "psd_parser" ),
            NULL,
            OFN_HIDEREADONLY,
            _T( "Parser Program ( psd_parser.exe ) |psd_parser.exe|" ),
            this );
        cDlgEXE.m_ofn.lpstrInitialDir = strPath.GetString();
        if( cDlgEXE.DoModal() != IDOK )
            return;

        strPsdParserPath = cDlgEXE.GetPathName().GetString();
    }

    //PSD파일 선택
    CFileDialog cDlgPSD(
        TRUE,
        _T( ".psd" ),
        NULL,
        OFN_HIDEREADONLY,
        _T( "PSD File ( *.psd ) |*.psd|" ),
        this );
    cDlgPSD.m_ofn.lpstrInitialDir = strPath.GetString();
    if( cDlgPSD.DoModal() != IDOK )
        return;

   
    // 파일이름 확장자 제거
    std::string strFilename = cDlgPSD.GetFileName();
    strFilename.erase(strFilename.end()-3,strFilename.end());
    
    // 임시XML파일 경로와 파일이름
    CString strXMLFilePath;
    strXMLFilePath.Format( "%s\\%sxml", strPath.GetString(), strFilename.c_str() );
    
    // 콘솔로 임시 XML 파일 생성
    CString strCommand;
    strCommand.Format( "%s %s > %s",strPsdParserPath.GetString(),  cDlgPSD.GetPathName().GetString(), strXMLFilePath.GetString() );
    system( strCommand.GetString() );

    // 프로그램에 XML추가
    g_pEditor->AddXml( strXMLFilePath.GetString() );
    
    // 임시 XML 파일 삭제
    ::DeleteFile( strXMLFilePath.GetString() );
    //system( strCommand.GetString() );
    
    // 임시 XCF 파일 삭제
    strXMLFilePath.Format( "%s\\%sxcf", strPath.GetString(), strFilename.c_str() );
    ::DeleteFile( strXMLFilePath.GetString() );
    //system( strCommand.GetString() );

    m_pFrame->RefreshAll();
}

VOID CChildView::OnButtonUseAlpha()
{
    m_bAlphaChannelUse = !m_bAlphaChannelUse;
}

VOID CChildView::OnButtonBgColor()
{
    CColorDialog dlg(RGB(0,0,0), CC_FULLOPEN);
    if(dlg.DoModal() == IDOK)
    {
        COLORREF color = dlg.GetColor();
        c_ClearD3D = D3DCOLOR_XRGB( GetRValue(color), GetGValue(color), GetBValue(color) );
    }

 /* 메서드 설명
 RGB : RGB 색상 설정
 GetColor : 컬러 반환 메서드
 Get(R,G,B)Value : 해당 요소의 컬러를 반환하는 메서드
 */
    
}


VOID CChildView::OnButtonAdd()
{
    g_pEditor->AddNewTemp();
    
    m_pFrame->RefreshAll();
}

VOID CChildView::OnButtonDel()
{
    if( MessageBox( _T("[정말 삭제하시겠습니까?"), _T("MessageBox"), MB_YESNO ) == IDYES )
    {
        if( g_pEditor->DelControl() == false )
            MessageBox( _T("삭제할 수 없습니다."), _T("MessageBox"), MB_OK );
    }
    
    m_pFrame->RefreshAll();
}

VOID CChildView::OnViewChanged()
{
}

VOID CChildView::CaptureMousePos()
{
    m_sMouseDownPos.x = DxInputDevice::GetInstance().GetMouseLocateX();
    m_sMouseDownPos.y = DxInputDevice::GetInstance().GetMouseLocateY();
}

BOOL CChildView::DiffCaptureMousePosVsCurrent()
{
    const INT MousePosOffset = 3;

    ic::Int2 sPos;
    sPos.x = DxInputDevice::GetInstance().GetMouseLocateX();
    sPos.y = DxInputDevice::GetInstance().GetMouseLocateY();

    if( abs( m_sMouseDownPos.x - sPos.x ) > MousePosOffset ||
        abs( m_sMouseDownPos.y - sPos.y ) > MousePosOffset )
    {
        return TRUE;
    }

    return FALSE;
}

VOID CChildView::ProcessInput( FLOAT fElapsedTime )
{
    BOOL bAlt = DxInputDevice::GetInstance().IsAltDown();
    BOOL bCtrl = DxInputDevice::GetInstance().IsCtrlDown();
    BOOL bShift = DxInputDevice::GetInstance().IsShiftDown();

    DWORD nML = DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT );
    DWORD nMR = DxInputDevice::GetInstance().GetMouseState( DXMOUSE_RIGHT );

    if( ( nML & DXKEY_DOWN ) || ( nMR & DXKEY_DOWN ) )
    {
        CaptureMousePos();
        if( nML == DXKEY_DOWN )
            FindControl();
    }

    if( CRenderDevice::s_pHitProxyBuffer )
    {
        CRenderDevice::s_pHitProxyViewport = NULL;
        CRenderDevice::GetHitProxy( this, GetClientMousePos() );
    }

    ApplyWheelToZoom();

    if( nMR & DXKEY_DRAG )
        ApplyDragToOrigin();
}

VOID CChildView::ApplyDragToOrigin()
{
    m_sOrigin.x += DxInputDevice::GetInstance().GetMouseMoveX();
    m_sOrigin.x = ic::Clamp< INT >( m_sOrigin.x, -SHRT_MAX, SHRT_MAX );

    m_sOrigin.y += DxInputDevice::GetInstance().GetMouseMoveY();
    m_sOrigin.y = ic::Clamp< INT >( m_sOrigin.y, -SHRT_MAX, SHRT_MAX );

    OnViewChanged();
}

VOID CChildView::ApplyWheelToZoom()
{
    const FLOAT ZoomOffset = 120.0f;
    const FLOAT ZoomMax = 5.0f;
    const FLOAT ZoomMin = 1.0f;

    INT nMouseZ = DxInputDevice::GetInstance().GetMouseMoveZ();
    if( nMouseZ == 0 )
        return;

    ic::Int2 sClient = GetClientMousePos();
    ic::Int2 sPosLast;
    sPosLast.x = INT( ( sClient.x - m_sOrigin.x ) / m_fZoom );
    sPosLast.y = INT( ( sClient.y - m_sOrigin.y ) / m_fZoom );

    m_fZoom += FLOAT( nMouseZ ) / ZoomOffset;
    m_fZoom = ic::Clamp< FLOAT >( m_fZoom, ZoomMin, ZoomMax );

    ic::Int2 sPosNew;
    sPosNew.x = INT( ( sClient.x - m_sOrigin.x ) / m_fZoom );
    sPosNew.y = INT( ( sClient.y - m_sOrigin.y ) / m_fZoom );

	m_sOrigin.x += INT( ( sPosNew.x - sPosLast.x ) * m_fZoom );
	m_sOrigin.y += INT( ( sPosNew.y - sPosLast.y ) * m_fZoom );

    OnViewChanged();
}

VOID CChildView::DrawTextureControl( FLOAT fElapsedTime )
{
	const std::string& strSelectedID = g_pEditor->GetSelectedID();
	UITextureList::ControlMap& mapControl = g_pEditor->GetControlMap();

	UITextureList::ControlMapIter it = mapControl.find( strSelectedID );
	if( it == mapControl.end() )
		return;

	const UITextureList::SControl& sControl = it->second;

	// 텍스쳐
	{
		TextureResource textureRes = TextureManager::GetInstance().LoadTexture(
			sControl.m_strFileName,
			false,
			TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
			0,
			D3DFMT_UNKNOWN,
			TEXTURE_RESIZE_NONE,
			TEXTURE_UI,
			false,
			FALSE );

		const D3DXIMAGE_INFO& sImageInfo = textureRes.GetInfo();
     
     
		g_pRI->DrawTile(
			ic::Int2( 0, 0 ),
			ic::Int2( sImageInfo.Width, sImageInfo.Height ),
			D3DXVECTOR2( 0.0f, 0.0f ),
			D3DXVECTOR2( 1.0f, 1.0f ),
			D3DCOLOR_XRGB( 255, 255, 255 ),
			textureRes.GetTexture(),
			m_bAlphaChannelUse );
	}

	// 깜빡이게 하려고 시간 계산
	const FLOAT c_fMax = 0.5f;
	const FLOAT c_fVisible = 0.35f;
	static FLOAT s_fCount = 0.0f;
	s_fCount += fElapsedTime;
	if( s_fCount > c_fMax )
		s_fCount = 0.0f;

	// 컨트롤 영역
	{
		if( s_fCount < c_fVisible )
		{
			const DWORD nColor = D3DCOLOR_XRGB( 255, 0, 255 );

			g_pRI->DrawLineBox(
				ic::Int2( INT( sControl.m_vPos.x ), INT( sControl.m_vPos.y ) ),
				ic::Int2( INT( sControl.m_vPos.z ), INT( sControl.m_vPos.w ) ),
				nColor );
		}
	}

	// 볼더 영역
	if( sControl.m_vBorder.x != 0.0f ||
		sControl.m_vBorder.y != 0.0f ||
		sControl.m_vBorder.z != 0.0f ||
		sControl.m_vBorder.w != 0.0f )
	{
		if( s_fCount < c_fVisible )
		{
			const DWORD nColor = D3DCOLOR_XRGB( 0, 255, 255 );

			D3DXVECTOR2 vPos(
				sControl.m_vPos.x + sControl.m_vBorder.x,
				sControl.m_vPos.y + sControl.m_vBorder.y );

			D3DXVECTOR2 vSize(
				sControl.m_vPos.z - sControl.m_vBorder.x - sControl.m_vBorder.z,
				sControl.m_vPos.w - sControl.m_vBorder.y - sControl.m_vBorder.w );

			g_pRI->DrawLineBox(
				ic::Int2( INT( vPos.x ), INT( vPos.y ) ),
				ic::Int2( INT( vSize.x ), INT( vSize.y ) ),
				nColor );
		}
	}
}

VOID CChildView::OnFrameMove(
    IDirect3DDevice9* pd3dDevice,
    DOUBLE fTime,
    FLOAT fElapsedTime )
{
    if( !IsVisible() )
        return;

	if( IsActive() )
        ProcessInput( fElapsedTime );
}

VOID CChildView::OnFrameRender(
    IDirect3DDevice9* pd3dDevice,
    DOUBLE fTime,
    FLOAT fElapsedTime )
{
    BOOL bHitTesting = g_pRI->IsHitTesting();
    const CRect& cSourceRect = GetSourceRect();

    BeginScene( pd3dDevice, c_ClearD3D );

    g_pRI->SetOrigin( m_sOrigin );
    g_pRI->SetZoom( m_fZoom );

    g_pRI->SetViewport(
        pd3dDevice,
        cSourceRect.left,
        cSourceRect.top,
        cSourceRect.Width(),
        cSourceRect.Height() );

    CTextUtil::Get()->Render( TRUE );

	// Draw
	DrawTextureControl( fElapsedTime );

    g_pRI->FlushBatchedPrimitives();

    // State
    if( !bHitTesting )
    {
        CD3DFontPar* pFont = DxFontMan::GetInstance().LoadDxFont(
            _DEFAULT_FONT,
            9,
            _DEFAULT_FONT_SHADOW_FLAG );

        if( pFont )
        {
            CString strText;
            strText.Format(
                _T( "%s [Origin %d %d], [Zoom %3.1f]" ),
                IsActive() ? _T( "Active" ) : _T( "DeActive" ),
                m_sOrigin.x,
                m_sOrigin.y,
                m_fZoom );

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

