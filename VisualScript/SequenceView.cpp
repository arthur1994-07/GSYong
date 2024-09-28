#include "stdafx.h"
#include "../InternalCommonLib/dxstdafx.h"

#include "../InternalCommonLib/Core/CommonUtil.h"

#include "../VisualScriptLib/Logic/SequenceEvent.h"
#include "../VisualScriptLib/Logic/SequenceVariable.h"
#include "../VisualScriptLib/Logic/SequenceAction.h"
#include "../VisualScriptLib/Logic/SequenceFrame.h"

#include "../EngineLib/Common/SUBPATH.h"
#include "../EngineLib/DxBase/RenderDevice.h"
#include "../EngineLib/DxBase/RenderInterface.h"
#include "../EngineLib/DxTools/DxInputDevice.h"
#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/TextTexture/TextUtil.h"

#include "./Frame.h"
#include "./Editor.h"
#include "./InputDlg.h"
#include "./SequenceMenuBackground.h"
#include "./SequenceMenuObject.h"
#include "./SequenceMenuConnector.h"
#include "./SequenceView.h"
#include "./VisualScript.h"


#include "../SigmaCore/DebugInclude.h"


namespace
{
    const COLORREF c_ClearWnd = RGB( 171, 171, 171 );
    const DWORD c_ClearD3D = D3DCOLOR_XRGB( 171, 171, 171 );
}

BEGIN_MESSAGE_MAP( CSequenceView, CClientView )
    ON_COMMAND( ID_BUTTON_NEW, OnButtonNew )
    ON_COMMAND( ID_BUTTON_OPEN, OnButtonLoad )
    ON_COMMAND( ID_BUTTON_SAVE, OnButtonSave )
    ON_COMMAND( ID_BUTTON_ZOOM, OnButtonZoom )
    ON_COMMAND( ID_BUTTON_TEST, OnButtonTest )
END_MESSAGE_MAP()

CSequenceView::CSequenceView()
    : m_bMakingLine( FALSE )
    , m_pConnSeqOp( NULL )
    , m_nConnType( INT_MIN )
    , m_nConnIndex( INT_MIN )

    , m_bSpecialDrag( FALSE )
    , m_pSpecialFrame( NULL )

    , m_bBoxSelecting( FALSE )
{
    m_bActive = TRUE;
    m_nToolBarID = IDR_MAINFRAME;
}

CSequenceView::~CSequenceView()
{
}

BOOL CSequenceView::PreCreateWindow( CREATESTRUCT& cs )
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

VOID CSequenceView::ChangeActiveSequnce( vs::CSequence* pSequence )
{
    if( !pSequence )
        return;

    if( g_pEditor->m_pSequence == pSequence )
        return;

    if( g_pEditor->m_pSequence )
        g_pEditor->m_pSequence->UpdateConnectors();

    g_pEditor->m_pSequence = pSequence;

    m_sOrigin = pSequence->sDefaultView;
    m_fZoom = pSequence->fDefaultViewZoom;

    m_bMakingLine = FALSE;
    m_pConnSeqOp = NULL;

    m_bSpecialDrag = FALSE;
    m_pSpecialFrame = NULL;

    m_bBoxSelecting = FALSE;

    g_pEditor->SetFocusInfo( NULL, INT_MIN, INT_MIN );

    g_pEditor->EmptySelectedSequence();

    g_pEditor->m_pSequence->UpdateConnectors();
}

VOID CSequenceView::OnButtonNew()
{
    g_pEditor->NewSequenceRoot();

    ChangeActiveSequnce( g_pEditor->m_pSequenceRoot );

    m_pFrame->RefreshPropertiesControl();
    m_pFrame->RebuildTreeControl();
}

VOID CSequenceView::OnButtonLoad()
{
    CFileDialog cDlg(
        TRUE,
        _T( ".seq" ),
        NULL,
		OFN_HIDEREADONLY,
        _T( "sequence ( *.seq, *.xml ) |*.seq;*.xml|" ),
		this );

    CVisualScriptApp* pApp = (CVisualScriptApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

    CString strPath = pPath->AppRoot().c_str(); //SUBPATH::APP_ROOT;
    strPath.Append( pPath->GLogicPath().c_str() ); //SUBPATH::GLOGIC_FILE );
    cDlg.m_ofn.lpstrInitialDir = strPath.GetString();

    if( cDlg.DoModal() != IDOK )
        return;

    if( !g_pEditor->LoadSequence( cDlg.GetPathName().GetString() ) )
        return;

    ChangeActiveSequnce( g_pEditor->m_pSequenceRoot );

    m_pFrame->RefreshPropertiesControl();
    m_pFrame->RebuildTreeControl();
}

VOID CSequenceView::OnButtonSave()
{
    CFileDialog cDlg(
        FALSE,
        _T( ".seq" ),
        NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        _T( "sequence ( *.seq, *.xml ) |*.seq;*.xml|" ),
		this );

    CVisualScriptApp* pApp = (CVisualScriptApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

    CString strPath = pPath->AppRoot().c_str(); //SUBPATH::APP_ROOT;
    strPath.Append( pPath->GLogicPath().c_str() ); //SUBPATH::GLOGIC_FILE );
    cDlg.m_ofn.lpstrInitialDir = strPath.GetString();

    if( cDlg.DoModal() != IDOK )
        return;

    g_pEditor->SaveSequence( cDlg.GetPathName().GetString() );
}

VOID CSequenceView::OnButtonZoom()
{
    ic::Int4 sBox( SHRT_MAX, SHRT_MAX, -SHRT_MAX, -SHRT_MAX );

    if( g_pEditor->m_vecSequenceSelected.empty() )
        g_pEditor->CalcBoundingBox( g_pEditor->m_pSequence->mapObjects, sBox );
    else
        g_pEditor->CalcBoundingBox( g_pEditor->m_vecSequenceSelected, sBox );

    if( sBox.x == SHRT_MAX || sBox.y == SHRT_MAX || sBox.z == -SHRT_MAX || sBox.w == -SHRT_MAX )
        return;

    const INT LittlePadding = 20;

    ic::Int2 sExtent;
    sExtent.x = INT( FLOAT( sBox.z - sBox.x ) * 0.5f ) + LittlePadding;
    sExtent.y = INT( FLOAT( sBox.w - sBox.y ) * 0.5f ) + LittlePadding;

    ic::Int2 sCenter;
    sCenter.x = sBox.x + sExtent.x;
    sCenter.y = sBox.y + sExtent.y;

    const CRect& cDestRect = GetDestRect();

	FLOAT fNewZoomX = FLOAT( cDestRect.Width() ) / FLOAT( sExtent.x * 2 );
	FLOAT fNewZoomY = FLOAT( cDestRect.Height() ) / FLOAT( sExtent.y * 2 );
	m_fZoom = ic::Clamp< FLOAT >( min( fNewZoomX, fNewZoomY ), 0.1f, 1.0f );

    D3DXVECTOR2 sOriginDraw( FLOAT( sCenter.x ), FLOAT( sCenter.y ) );
	sOriginDraw.x -= FLOAT( cDestRect.Width() ) * 0.5f / m_fZoom;
	sOriginDraw.y -= FLOAT( cDestRect.Height() ) * 0.5f / m_fZoom;

	m_sOrigin.x = -INT( sOriginDraw.x * m_fZoom );
	m_sOrigin.y = -INT( sOriginDraw.y * m_fZoom );

    OnViewChanged();
}

VOID CSequenceView::OnButtonTest()
{
    const INT GapX = 30;
    const INT GapY = 50;

    CRect cDestRect = GetDestRect();
    cDestRect.right -= 100;

    ic::Int2 sPos;
    INT nHeight = 0;

    for( size_t i=0; i<ic::CRttiMgr::Count(); ++i )
    {
        ic::CBaseObject* pStatic = ic::CRttiMgr::FindStatic( i );
        vs::CSequenceObject* pObject = dynamic_cast< vs::CSequenceObject* >( pStatic );
        if( !pObject )
            continue;

        if( pObject->VFlags( ic::EInternal ) )
            continue;

        pObject = g_pEditor->NewSequenceObject( pObject->VIndex(), sPos );
        if( !pObject )
            continue;

        pObject->DrawObject( g_pRI, FALSE, FALSE, 0, 0 );
        const ic::Int4 sObjectBox = pObject->GetBoundingBox();
        nHeight = max( nHeight, ( sObjectBox.w - sObjectBox.y ) );

        sPos.x += ( sObjectBox.z - sObjectBox.x ) + GapX;
        if( sPos.x >= cDestRect.Width() )
        {
            sPos.x = 0;
            sPos.y += nHeight + GapY;
            nHeight = 0;
        }
    }

    g_pEditor->EmptySelectedSequence();
}

VOID CSequenceView::OnViewChanged()
{
    g_pEditor->m_pSequence->sDefaultView = m_sOrigin;
    g_pEditor->m_pSequence->fDefaultViewZoom = m_fZoom;
}

VOID CSequenceView::CaptureMousePos()
{
    m_sMouseDownPos.x = DxInputDevice::GetInstance().GetMouseLocateX();
    m_sMouseDownPos.y = DxInputDevice::GetInstance().GetMouseLocateY();
}

BOOL CSequenceView::DiffCaptureMousePosVsCurrent()
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

VOID CSequenceView::ProcessInput( FLOAT fElapsedTime )
{
    BOOL bAlt = DxInputDevice::GetInstance().IsAltDown();
    BOOL bCtrl = DxInputDevice::GetInstance().IsCtrlDown();
    BOOL bShift = DxInputDevice::GetInstance().IsShiftDown();

    DWORD nML = DxInputDevice::GetInstance().GetMouseState( DXMOUSE_LEFT );
    DWORD nMR = DxInputDevice::GetInstance().GetMouseState( DXMOUSE_RIGHT );

    if( ( nML & DXKEY_DOWN ) || ( nMR & DXKEY_DOWN ) )
        CaptureMousePos();

    if( CRenderDevice::s_pHitProxyBuffer )
    {
        CRenderDevice::s_pHitProxyViewport = NULL;
        CRenderDevice::GetHitProxy( this, GetClientMousePos() );
    }

    ApplyWheelToZoom();

    if( nMR & DXKEY_DRAG )
        ApplyDragToOrigin();

    if( !bAlt && bCtrl && !bShift )
        ProcessCutCopyPaste();

    if( DxInputDevice::GetInstance().GetKeyState( DIK_DELETE ) & DXKEY_UP )
        DeleteSelected();

    if( ( nML & DXKEY_DOWN ) && bAlt && bCtrl )
        BeginBoxSelecting();

    if( m_bBoxSelecting )
    {
        ContinueBoxSelecting();

        if( nML & ( DXKEY_IDLE | DXKEY_UP ) )
            EndBoxSelecting();
    }
    else if( m_bMakingLine )
    {
        DoScrollBorder( fElapsedTime );

        CaptureMousePos();

        if( nML & ( DXKEY_IDLE | DXKEY_UP ) )
            EndMakingLine();

        UpdateMouseOverObject();

        if( nML & DXKEY_DUP )
            ActionObject();
    }
    else if( m_bSpecialDrag )
    {
        DoScrollBorder( fElapsedTime );

        ContinueSpecialDrag();

        if( nML & ( DXKEY_IDLE | DXKEY_UP ) )
            EndSpecialDrag();
    }
    else
    {
        if( nML & DXKEY_DOWN )
        {
            SelectConnector();
            SelectSpecial();
        }

        if( ( nML & DXKEY_DRAG ) && bCtrl )
        {
            DoScrollBorder( fElapsedTime );

            MoveSelected();
        }

        if( nML & DXKEY_UP )
            SelectObject();

        UpdateMouseOverObject();

        if( nMR & DXKEY_UP )
            OpenMenu();

        if( nML & DXKEY_DUP )
            ActionObject();
    }
}

VOID CSequenceView::ApplyDragToOrigin()
{
    m_sOrigin.x += DxInputDevice::GetInstance().GetMouseMoveX();
    m_sOrigin.x = ic::Clamp< INT >( m_sOrigin.x, -SHRT_MAX, SHRT_MAX );

    m_sOrigin.y += DxInputDevice::GetInstance().GetMouseMoveY();
    m_sOrigin.y = ic::Clamp< INT >( m_sOrigin.y, -SHRT_MAX, SHRT_MAX );

    OnViewChanged();
}

VOID CSequenceView::ApplyWheelToZoom()
{
    const FLOAT ZoomOffset = 1200.0f;
    const FLOAT ZoomMax = 1.0f;
    const FLOAT ZoomMin = 0.1f;

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

VOID CSequenceView::ProcessCutCopyPaste()
{
    if( DxInputDevice::GetInstance().GetKeyState( DIK_X ) & DXKEY_DOWN )
    {
        if( g_pEditor->CopyObjects( m_sOrigin ) )
            DeleteSelected();
    }

    if( DxInputDevice::GetInstance().GetKeyState( DIK_C ) & DXKEY_DOWN )
        g_pEditor->CopyObjects( m_sOrigin );

    if( DxInputDevice::GetInstance().GetKeyState( DIK_V ) & DXKEY_DOWN )
    {
        if( g_pEditor->PasteObjects( m_sOrigin ) )
        {
            m_pFrame->RefreshPropertiesControl();
            m_pFrame->RebuildTreeControl();
        }
    }
}

VOID CSequenceView::OpenMenu()
{
    if( !InViewClientMousePos() )
        return;

    if( DiffCaptureMousePosVsCurrent() )
        return;

    ic::SHitProxy* pHitProxy = CRenderDevice::GetHitProxy( this, GetClientMousePos() );
    if( !pHitProxy )
    {
        OpenSequenceBackground();
        return;
    }

    ic::SLinkedObjProxy* pObject = dynamic_cast< ic::SLinkedObjProxy* >( pHitProxy );
    if( pObject )
    {
        OpenSequenceObject();
        return;
    }

    ic::SLinkedObjConnectorProxy* pConnector = dynamic_cast< ic::SLinkedObjConnectorProxy* >( pHitProxy );
    if( pConnector )
    {
        OpenSequenceConnector();
        return;
    }
}

VOID CSequenceView::OpenSequenceBackground()
{
    CSequenceMenuBackground cMenu;
    CSequenceMenuBackground::SSelect sSelect = cMenu.Open( m_sMouseDownPos, this );
    if( sSelect.nIndex <= 0 )
        return;

    ic::Int2 sClient = GetClientPos( m_sMouseDownPos );
    sClient.x = INT( ( sClient.x - m_sOrigin.x ) / m_fZoom );
    sClient.y = INT( ( sClient.y - m_sOrigin.y ) / m_fZoom );

    switch( sSelect.nIndex )
    {
    case CSequenceMenuBackground::ENewComment:
        {
            CInputDlg cInputDlg;
            cInputDlg.m_strTitle = _T( "New Comment" );
            cInputDlg.m_strComment = _T( "Comment Text:" );
            cInputDlg.m_strValue = _T( "Comment" );

            if( cInputDlg.DoModal() == IDOK )
            {
                vs::CSequenceObject* pObject = g_pEditor->NewSequenceObject(
                    vs::CSequenceFrame::Rtti()->Index(),
                    sClient,
                    cInputDlg.m_strValue.GetString() );

                if( pObject )
                {
                    m_pFrame->RefreshPropertiesControl();
                    m_pFrame->RebuildTreeControl();
                }
            }
        }
        return;

    case CSequenceMenuBackground::ECreateNewSequence:
        {
            CInputDlg cInputDlg;
            cInputDlg.m_strTitle = _T( "Name Sequence" );
            cInputDlg.m_strComment = _T( "Sequence name:" );
            cInputDlg.m_strValue = _T( "Sequence" );

            if( cInputDlg.DoModal() == IDOK )
            {
                vs::CSequenceObject* pObject = g_pEditor->NewSequenceObject(
                    vs::CSequence::Rtti()->Index(),
                    sClient,
                    cInputDlg.m_strValue.GetString() );

                if( pObject )
                {
                    m_pFrame->RefreshPropertiesControl();
                    m_pFrame->RebuildTreeControl();
                }
            }
        }
        return;

    case CSequenceMenuBackground::EPasteHere:
        {
            if( g_pEditor->PasteObjects( m_sOrigin, &sClient ) )
            {
                m_pFrame->RefreshPropertiesControl();
                m_pFrame->RebuildTreeControl();
            }
        }
        return;
    }

    vs::CSequenceObject* pObject = g_pEditor->NewSequenceObject( sSelect.nIndex, sClient );
    if( pObject )
    {
        m_pFrame->RefreshPropertiesControl();
        m_pFrame->RebuildTreeControl();
    }
}

VOID CSequenceView::OpenSequenceObject()
{
    if( !g_pEditor->m_pFocusObject )
        return;

    CSequenceMenuObject cMenu;
    CSequenceMenuObject::SSelect sSelect = cMenu.Open( m_sMouseDownPos, this );
    if( sSelect.nIndex <= 0 )
        return;

    switch( sSelect.nIndex )
    {
    case CSequenceMenuObject::EBreakAllLinks:
        {
            g_pEditor->BreakAllLinksOfFocusObject();
        }
        return;

    case CSequenceMenuObject::EEnumProperty:
        {
            m_pFrame->OpenEnumPropertyDlg( g_pEditor->m_pFocusObject );
        }
        return;
    }
}

VOID CSequenceView::OpenSequenceConnector()
{
    if( !g_pEditor->m_pFocusObject )
        return;
    if( g_pEditor->m_nFocusConnType < 0 )
        return;
    if( g_pEditor->m_nFocusConnIndex < 0 )
        return;

    CSequenceMenuConnector cMenu;
    CSequenceMenuConnector::SSelect sSelect = cMenu.Open( m_sMouseDownPos, this );
    if( sSelect.nIndex <= 0 )
        return;

    switch( sSelect.nIndex )
    {
    case CSequenceMenuConnector::EBreakAllLinks:
        {
            g_pEditor->BreakLinksOfFocusConnector();
        }
        return;

    case CSequenceMenuConnector::ESetActivateDelay:
        {
            vs::CSequenceOp* pOp = dynamic_cast< vs::CSequenceOp* >( g_pEditor->m_pFocusObject );
            __gassert_( pOp );

            switch( g_pEditor->m_nFocusConnType )
            {
            case ic::EInput:
                {
                    vs::SSeqOpInputLink& rLink = pOp->vecInputLinks[ g_pEditor->m_nFocusConnIndex ];

                    CInputDlg cInputDlg;
                    cInputDlg.m_strTitle = _T( "Set Input Delay" );
                    cInputDlg.m_strComment = _T( "Delay:" );
                    cInputDlg.m_strValue.Format( _T( "%.2f" ), rLink.fDelay );

                    if( cInputDlg.DoModal() == IDOK )
                    {
                        DOUBLE fDelay = atof( cInputDlg.m_strValue.GetString() );
                        rLink.fDelay = static_cast< float >( fDelay );
                        rLink.fDelay = max( rLink.fDelay, 0.0f );
                    }
                }
                break;

            case ic::EOutput:
                {
                    vs::SSeqOpOutputLink& rLink = pOp->vecOutputLinks[ g_pEditor->m_nFocusConnIndex ];

                    CInputDlg cInputDlg;
                    cInputDlg.m_strTitle = _T( "Set Output Delay" );
                    cInputDlg.m_strComment = _T( "Delay:" );
                    cInputDlg.m_strValue.Format( _T( "%.2f" ), rLink.fDelay );

                    if( cInputDlg.DoModal() == IDOK )
                    {
                        DOUBLE fDelay = atof( cInputDlg.m_strValue.GetString() );
                        rLink.fDelay = static_cast< float >( fDelay );
                        rLink.fDelay = max( rLink.fDelay, 0.0f );
                    }
                }
                break;
            }
        }
        return;

    case CSequenceMenuConnector::ESetActiveEvent:
        {
            g_pEditor->ActionObject(
                g_pEditor->m_pFocusObject,
                g_pEditor->m_nFocusConnType,
                g_pEditor->m_nFocusConnIndex );
        }
        return;
    }

    if( sSelect.nIndex >= CSequenceMenuConnector::EBreakLinkTo &&
        sSelect.nIndex < CSequenceMenuConnector::EBreakLinkToEnd )
    {
        g_pEditor->BreakLinksOfFocusConnector(
            sSelect.nIndex - CSequenceMenuConnector::EBreakLinkTo );
    }
}

VOID CSequenceView::ActionObject()
{
    if( !InViewClientMousePos() )
        return;

    ic::SHitProxy* pHitProxy = CRenderDevice::GetHitProxy( this, GetClientMousePos() );
    if( !pHitProxy )
        return;

    ic::SLinkedObjProxy* pObject = dynamic_cast< ic::SLinkedObjProxy* >( pHitProxy );
    if( pObject )
    {
        vs::CSequence* pSequence = dynamic_cast< vs::CSequence* >( pObject->pObject );
        if( pSequence )
        {
            ChangeActiveSequnce( pSequence );
            m_pFrame->RefreshPropertiesControl();
            m_pFrame->RebuildTreeControl();
        }

        vs::CSeqVar_Named* pNamedVar = dynamic_cast< vs::CSeqVar_Named* >( pObject->pObject );
        if( pNamedVar )
        {
            if( g_pEditor->ActionNamedVar( pObject->pObject ) )
            {
                OnButtonZoom();
                m_pFrame->RefreshPropertiesControl();
            }
        }

        return;
    }

    ic::SLinkedObjConnectorProxy* pConnector = dynamic_cast< ic::SLinkedObjConnectorProxy* >( pHitProxy );
    if( pConnector )
    {
        g_pEditor->ActionObject(
            dynamic_cast< vs::CSequenceObject* >( pConnector->pObject ),
            pConnector->nConnType,
            pConnector->nConnIndex );
        return;
    }
}

VOID CSequenceView::SelectObject()
{
    if( !InViewClientMousePos() )
        return;

    if( DiffCaptureMousePosVsCurrent() )
        return;

    ic::SHitProxy* pHitProxy = CRenderDevice::GetHitProxy( this, GetClientMousePos() );
    if( !pHitProxy )
        return;

    ic::SLinkedObjProxy* pObject = dynamic_cast< ic::SLinkedObjProxy* >( pHitProxy );
    if( !pObject )
        return;

    vs::CSequenceObject* pSequence = dynamic_cast< vs::CSequenceObject* >( pObject->pObject );
    if( !pSequence )
        return;

    if( !DxInputDevice::GetInstance().IsCtrlDown() )
        g_pEditor->EmptySelectedSequence();

    if( g_pEditor->IsSelectedSequence( pSequence ) )
        g_pEditor->EraseSelectedSequence( pSequence );
    else
        g_pEditor->InsertSelectedSequence( pSequence );

    m_pFrame->RefreshPropertiesControl();
}

VOID CSequenceView::SelectConnector()
{
    if( !InViewClientMousePos() )
        return;

    ic::SHitProxy* pHitProxy = CRenderDevice::GetHitProxy( this, GetClientMousePos() );
    if( !pHitProxy )
    {
        if( !DxInputDevice::GetInstance().IsCtrlDown() )
        {
            g_pEditor->EmptySelectedSequence();
            m_pFrame->RefreshPropertiesControl();
        }
        return;
    }

    ic::SLinkedObjConnectorProxy* pConnector = dynamic_cast< ic::SLinkedObjConnectorProxy* >( pHitProxy );
    if( !pConnector )
        return;

    g_pEditor->EmptySelectedSequence();

    SetSelectedConnector(
        pConnector->pObject,
        pConnector->nConnType,
        pConnector->nConnIndex );

    m_pFrame->RefreshPropertiesControl();
}

VOID CSequenceView::SelectSpecial()
{
    if( !InViewClientMousePos() )
        return;

    ic::SHitProxy* pHitProxy = CRenderDevice::GetHitProxy( this, GetClientMousePos() );
    if( !pHitProxy )
        return;

    ic::SLinkedObjProxySpecial* pSpecial = dynamic_cast< ic::SLinkedObjProxySpecial* >( pHitProxy );
    if( !pSpecial )
        return;

    vs::CSequenceFrame* pSeqFrame = dynamic_cast< vs::CSequenceFrame* >( pSpecial->pObject );
    if( !pSeqFrame )
        return;

    g_pEditor->EmptySelectedSequence();

    g_pEditor->InsertSelectedSequence( pSeqFrame );

    BeginSpecialDrag( pSeqFrame );

    m_pFrame->RefreshPropertiesControl();
}

VOID CSequenceView::MoveSelected()
{
    ic::Int2 sMove;
    sMove.x = DxInputDevice::GetInstance().GetMouseMoveX();
    sMove.y = DxInputDevice::GetInstance().GetMouseMoveY();

    if( m_sScrollBorder.x != 0.0f )
        sMove.x -= INT( m_sScrollBorder.x );
    if( m_sScrollBorder.y != 0.0f )
        sMove.y -= INT( m_sScrollBorder.y );

    ic::Int2 sTrans;
    sTrans.x = INT( FLOAT( sMove.x ) / m_fZoom );
    sTrans.y = INT( FLOAT( sMove.y ) / m_fZoom );

    g_pEditor->MoveSelectedSequence( sTrans );
}

VOID CSequenceView::DeleteSelected()
{
    g_pEditor->DeleteSelectedSequence();
    m_pFrame->RefreshPropertiesControl();
    m_pFrame->RebuildTreeControl();
}

VOID CSequenceView::SetSelectedConnector( ic::CBaseObject* pObject, INT nConnType, INT nConnIndex )
{
    if( !pObject->IsChildOf( vs::CSequenceOp::Rtti() ) )
        return;

    m_bMakingLine = TRUE;

    m_pConnSeqOp = dynamic_cast< vs::CSequenceOp* >( pObject );
	m_nConnType = nConnType;
	m_nConnIndex = nConnIndex;
}

ic::Int2 CSequenceView::GetSelectedConnLocation()
{
    if( !m_pConnSeqOp )
		return ic::Int2( 0, 0 );

	return m_pConnSeqOp->GetConnectionLocation( m_nConnType, m_nConnIndex );
}

DWORD CSequenceView::GetMakingLinkColor()
{
    switch( m_nConnType )
    {
    case ic::EInput:
    case ic::EOutput:
        {
            return D3DCOLOR_XRGB( 0, 0, 0 );
        }
        break;

    case ic::EVariable:
        {
            D3DXCOLOR sColor( m_pConnSeqOp->GetVarConnectorColor( m_nConnIndex ) );
            sColor *= 0.8f;
		    return sColor;
        }
        break;
    }

    return D3DCOLOR_XRGB( 0, 0, 0 );
}

VOID CSequenceView::EndMakingLine()
{
    m_bMakingLine = FALSE;

    ic::SHitProxy* pHitProxy = CRenderDevice::GetHitProxy( this, GetClientMousePos() );
    if( !pHitProxy )
        return;

    ic::SLinkedObjProxy* pObject = dynamic_cast< ic::SLinkedObjProxy* >( pHitProxy );
    if( pObject )
    {
        g_pEditor->MakeConnectionToObject(
            m_pConnSeqOp,
            m_nConnType,
            m_nConnIndex,
            pObject->pObject );
        return;
    }

    ic::SLinkedObjConnectorProxy* pConnector = dynamic_cast< ic::SLinkedObjConnectorProxy* >( pHitProxy );
    if( pConnector )
    {
        g_pEditor->MakeConnectionToConnector(
            m_pConnSeqOp,
            m_nConnType,
            m_nConnIndex,
            pConnector->pObject,
            pConnector->nConnType,
            pConnector->nConnIndex );
        return;
    }
}

VOID CSequenceView::DrawMakingLine()
{
    ic::Int2 sStartPoint = GetSelectedConnLocation();

    ic::Int2 sEndPoint = GetClientPos( m_sMouseDownPos );
    sEndPoint.x = INT( ( sEndPoint.x - m_sOrigin.x ) / m_fZoom );
    sEndPoint.y = INT( ( sEndPoint.y - m_sOrigin.y ) / m_fZoom );

	DWORD nLinkColor = GetMakingLinkColor();

	FLOAT fTension;
	if( m_nConnType == ic::EInput || m_nConnType == ic::EOutput )
		fTension = FLOAT( abs( sStartPoint.x - sEndPoint.x ) );
	else
		fTension = FLOAT( abs( sStartPoint.y - sEndPoint.y ) );

    switch( m_nConnType )
    {
    case ic::EInput:
        {
            ic::CLinkedObjDrawUtils::DrawSpline(
                g_pRI,
                sStartPoint,
                D3DXVECTOR2( -fTension, 0.0f ),
                sEndPoint,
                D3DXVECTOR2( -fTension, 0.0f ),
                nLinkColor,
                FALSE );
        }
        break;

    case ic::EOutput:
        {
            ic::CLinkedObjDrawUtils::DrawSpline(
                g_pRI,
                sStartPoint,
                D3DXVECTOR2( fTension, 0.0f ),
                sEndPoint,
                D3DXVECTOR2( fTension, 0.0f ),
                nLinkColor,
                FALSE );
        }
        break;

    case ic::EVariable:
        {
            ic::CLinkedObjDrawUtils::DrawSpline(
                g_pRI,
                sStartPoint,
                D3DXVECTOR2( 0.0f, fTension ),
                sEndPoint,
                D3DXVECTOR2( 0.0f, 0.0f ),
                nLinkColor,
                FALSE );
        }
        break;
    }
}

VOID CSequenceView::BeginSpecialDrag( vs::CSequenceFrame* pSeqFrame )
{
    m_bSpecialDrag = TRUE;
    m_pSpecialFrame = pSeqFrame;
    m_sSpecialSize = m_pSpecialFrame->sSize;
    m_sSpecialBegin = GetClientMousePos();
}

VOID CSequenceView::ContinueSpecialDrag()
{
    const INT MinSize = 64;

    if( m_sScrollBorder.x != 0.0f )
        m_sSpecialBegin.x += INT( m_sScrollBorder.x );
    if( m_sScrollBorder.y != 0.0f )
        m_sSpecialBegin.y += INT( m_sScrollBorder.y );
 
    ic::Int2 sClient = GetClientMousePos();
    ic::Int2 sOffset;
    sOffset.x = INT( FLOAT( sClient.x - m_sSpecialBegin.x ) / m_fZoom );
    sOffset.y = INT( FLOAT( sClient.y - m_sSpecialBegin.y ) / m_fZoom );

    m_pSpecialFrame->sSize = m_sSpecialSize + sOffset;
    m_pSpecialFrame->sSize.x = max( m_pSpecialFrame->sSize.x, MinSize );
    m_pSpecialFrame->sSize.y = max( m_pSpecialFrame->sSize.y, MinSize );
}

VOID CSequenceView::EndSpecialDrag()
{
    m_bSpecialDrag = FALSE;

    m_pFrame->RefreshPropertiesControl();
}

VOID CSequenceView::BeginBoxSelecting()
{
    if( !InViewClientMousePos() )
        return;

    m_bBoxSelecting = TRUE;

    ic::Int2 sClient = GetClientMousePos();
    m_sBoxRect.x = m_sBoxRect.z = sClient.x;
    m_sBoxRect.y = m_sBoxRect.w = sClient.y;
}

VOID CSequenceView::ContinueBoxSelecting()
{
    ic::Int2 sClient = GetClientMousePos();
    m_sBoxRect.z = sClient.x;
    m_sBoxRect.w = sClient.y;
}

VOID CSequenceView::EndBoxSelecting()
{
    m_bBoxSelecting = FALSE;

    if( !DxInputDevice::GetInstance().IsShiftDown() )
        g_pEditor->EmptySelectedSequence();

    ic::Int2 sMin;
    sMin.x = min( m_sBoxRect.x, m_sBoxRect.z );
    sMin.y = min( m_sBoxRect.y, m_sBoxRect.w );

    ic::Int2 sMax;
    sMax.x = max( m_sBoxRect.x, m_sBoxRect.z ) + 1;
    sMax.y = max( m_sBoxRect.y, m_sBoxRect.w ) + 1;

    CRenderDevice::HitProxyVec vecProxyMap;
	CRenderDevice::GetHitProxyMap( this, sMin, sMax, vecProxyMap );

    ic::Int2 sTestSize( sMax - sMin );

    for( INT y=0; y<sTestSize.y; ++y )
    {
	    for( INT x=0; x<sTestSize.x; ++x )
	    {
		    ic::SHitProxy* pHitProxy = vecProxyMap[ x + y * sTestSize.x ];
            g_pEditor->InsertSelectedSequence( pHitProxy );
	    }
    }

    m_pFrame->RefreshPropertiesControl();
}

VOID CSequenceView::DrawBoxSelecting()
{
    const DWORD ColorSelect = D3DCOLOR_XRGB( 255, 0, 0 );

    INT nMinX = INT( ( min( m_sBoxRect.x, m_sBoxRect.z ) - m_sOrigin.x ) / m_fZoom );
	INT nMinY = INT( ( min( m_sBoxRect.y, m_sBoxRect.w ) - m_sOrigin.y ) / m_fZoom );
	INT nMaxX = INT( ( max( m_sBoxRect.x, m_sBoxRect.z ) - m_sOrigin.x ) / m_fZoom );
	INT nMaxY = INT( ( max( m_sBoxRect.y, m_sBoxRect.w ) - m_sOrigin.y ) / m_fZoom );

    g_pRI->DrawLine( ic::Int2( nMinX, nMinY ), ic::Int2( nMaxX, nMinY ), ColorSelect );
	g_pRI->DrawLine( ic::Int2( nMaxX, nMinY ), ic::Int2( nMaxX, nMaxY ), ColorSelect );
	g_pRI->DrawLine( ic::Int2( nMaxX, nMaxY ), ic::Int2( nMinX, nMaxY ), ColorSelect );
	g_pRI->DrawLine( ic::Int2( nMinX, nMaxY ), ic::Int2( nMinX, nMinY ), ColorSelect );
}

BOOL CSequenceView::ShouldHighlightConnector( ic::CBaseObject* pObject, INT nConnType, INT nConnIndex )
{
    if( !m_bMakingLine )
        return TRUE;

    if( m_nConnType == ic::EVariable )
        return FALSE;

    if( nConnType == ic::EVariable )
        return FALSE;

    if( m_nConnType == nConnType )
        return FALSE;

    if( m_pConnSeqOp == pObject &&
        m_nConnType == nConnType &&
        m_nConnIndex == nConnIndex )
    {
        return FALSE;
    }

    return TRUE;
}

VOID CSequenceView::UpdateMouseOverObject()
{
    if( !InViewClientMousePos() )
        return;

    ic::SHitProxy* pHitProxy = CRenderDevice::GetHitProxy( this, GetClientMousePos() );
    if( !pHitProxy )
    {
        g_pEditor->SetFocusInfo(
            NULL,
            INT_MIN,
            INT_MIN );
        return;
    }

    ic::SLinkedObjProxy* pObject = dynamic_cast< ic::SLinkedObjProxy* >( pHitProxy );
    if( pObject )
    {
        g_pEditor->SetFocusInfo(
            dynamic_cast< vs::CSequenceObject* >( pObject->pObject ),
            INT_MIN,
            INT_MIN );
        return;
    }

    ic::SLinkedObjConnectorProxy* pConnector = dynamic_cast< ic::SLinkedObjConnectorProxy* >( pHitProxy );
    if( pConnector )
    {
        BOOL bHighlightConnector = ShouldHighlightConnector(
            pConnector->pObject,
            pConnector->nConnType,
            pConnector->nConnIndex );

        g_pEditor->SetFocusInfo(
            dynamic_cast< vs::CSequenceObject* >( pConnector->pObject ),
            bHighlightConnector ? pConnector->nConnType : INT_MIN,
            bHighlightConnector ? pConnector->nConnIndex : INT_MIN );
        return;
    }
}

VOID CSequenceView::DoScrollBorder( FLOAT fElapsedTime )
{
    const FLOAT ScrollBorderSize = 20.0f;
    const FLOAT ScrollBorderSpeed = 400.f;

    const CRect& cSourceRect = GetSourceRect();
    D3DXVECTOR2 sRect( FLOAT( cSourceRect.Width() ), FLOAT( cSourceRect.Height() ) );

    ic::Int2 sClient = GetClientMousePos();
    D3DXVECTOR2 sPos( FLOAT( sClient.x ), FLOAT( sClient.y ) );

	if( sPos.x < ScrollBorderSize )
	{
        FLOAT fOffset = 1.0f - sPos.x / ScrollBorderSize;
		m_sScrollAccum.x += fOffset * ScrollBorderSpeed * fElapsedTime;
	}
	else if( sPos.x > sRect.x - ScrollBorderSize )
	{
        FLOAT fOffset = ( sPos.x - sRect.x + ScrollBorderSize ) / ScrollBorderSize;
		m_sScrollAccum.x -= fOffset * ScrollBorderSpeed * fElapsedTime;
	}
	else
	{
		m_sScrollAccum.x = 0.0f;
	}

	if( sPos.y < ScrollBorderSize )
	{
        FLOAT fOffset = 1.0f - sPos.y / ScrollBorderSize;
		m_sScrollAccum.y += fOffset * ScrollBorderSpeed * fElapsedTime;
	}
	else if( sPos.y > sRect.y - ScrollBorderSize )
	{
        FLOAT fOffset = ( sPos.y - sRect.y + ScrollBorderSize ) / ScrollBorderSize;
		m_sScrollAccum.y -= fOffset * ScrollBorderSpeed * fElapsedTime;
	}
	else
	{
		m_sScrollAccum.y = 0.0f;
	}

	m_sScrollBorder.x = floorf( m_sScrollAccum.x );
	m_sOrigin.x += INT( m_sScrollBorder.x );
	m_sScrollAccum.x -= m_sScrollBorder.x;

	m_sScrollBorder.y = floorf( m_sScrollAccum.y );
	m_sOrigin.y += INT( m_sScrollBorder.y );
	m_sScrollAccum.y -= m_sScrollBorder.y;

    OnViewChanged();
}

VOID CSequenceView::OnFrameMove(
    IDirect3DDevice9* pd3dDevice,
    DOUBLE fTime,
    FLOAT fElapsedTime )
{
    if( !IsVisible() )
        return;

    if( IsActive() )
        ProcessInput( fElapsedTime );

    g_pEditor->UpdateSequence( fElapsedTime );
}

VOID CSequenceView::OnFrameRender(
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

    // Sequence
    g_pEditor->DrawSequence( fElapsedTime );

    // New Line
    if( !bHitTesting && m_bMakingLine )
        DrawMakingLine();

    // Select Box
    if( !bHitTesting && m_bBoxSelecting )
        DrawBoxSelecting();

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