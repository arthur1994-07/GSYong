#include "stdafx.h"

#include "../../InternalCommonLib/dxstdafx.h"

#include "../../EngineLib/DxBase/RenderDevice.h"
#include "../../EngineLib/DxBase/RenderInterface.h"

#include "./Editor.h"
#include "./Frame.h"

#include "../../SigmaCore/DebugInclude.h"

IMPLEMENT_DYNAMIC( CFrame, CFrameWnd )

BEGIN_MESSAGE_MAP( CFrame, CFrameWnd )
	ON_WM_CREATE()
    ON_WM_CLOSE()
    ON_WM_SIZE()
	ON_WM_SETFOCUS()
    ON_WM_ACTIVATE()
	ON_COMMAND( XTP_ID_CUSTOMIZE, OnCustomize )
	ON_MESSAGE( XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify )
    ON_COMMAND_RANGE( IDR_PANE_LISTBOX, IDR_PANE_PROPERTIES, OnShowPane )
    ON_UPDATE_COMMAND_UI_RANGE( IDR_PANE_LISTBOX, IDR_PANE_PROPERTIES, OnUpdateShowPane )
END_MESSAGE_MAP()

CFrame::CFrame()
	: m_wndEditList( NULL )
{
}

CFrame::~CFrame()
{
}

BOOL CFrame::PreCreateWindow( CREATESTRUCT& cs )
{
	if( !CFrameWnd::PreCreateWindow( cs ) )
		return FALSE;

    // TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE |
        WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass( 0 );
	cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

    return TRUE;
}

BOOL CFrame::OnCmdMsg( UINT nID, INT nCode, VOID* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo )
{
	// let the view have first crack at the command
	if( m_wndView.OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) )
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
}

#ifdef _DEBUG
VOID CFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

VOID CFrame::Dump( CDumpContext& dc ) const
{
	CFrameWnd::Dump( dc );
}
#endif //_DEBUG

INT CFrame::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if( CFrameWnd::OnCreate( lpCreateStruct ) == -1 )
		return -1;

	// create a view to occupy the client area of the frame
	BOOL bReturn = m_wndView.Create(
		NULL,
		NULL,
		AFX_WS_DEFAULT_VIEW,
		CRect( 0, 0, 0, 0 ),
		this,
		AFX_IDW_PANE_FIRST,
		NULL);

	if( !bReturn )
	{
		TRACE0( "Failed to create view window\n" );
		return -1;
	}

    UINT indicators[] =
    {
	    ID_SEPARATOR,           // status line indicator
	    ID_INDICATOR_CAPS,
	    ID_INDICATOR_NUM,
	    ID_INDICATOR_SCRL,
    };

	if( !m_wndStatusBar.Create( this ) ||
		!m_wndStatusBar.SetIndicators( indicators, sizeof( indicators ) / sizeof( UINT ) ) )
	{
		TRACE0( "Failed to create status bar\n" );
		return -1;      // fail to create
	}

	// Initialize the command bars
	if( !InitCommandBars() )
		return -1;

	// Get a pointer to the command bars object.
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if( pCommandBars == NULL )
	{
		TRACE0( "Failed to create command bars object.\n" );
		return -1;      // fail to create
	}

	// Add the menu bar
	CXTPCommandBar* pMenuBar = pCommandBars->SetMenu( _T( "Menu Bar" ), IDR_MAINFRAME );
    if( pMenuBar == NULL )
	{
		TRACE0( "Failed to create menu bar.\n" );
		return -1;      // fail to create
	}

	// Create ToolBar
	CXTPToolBar* pToolBar = (CXTPToolBar*)pCommandBars->Add( _T( "Standard" ), xtpBarTop );
	if( !pToolBar || !pToolBar->LoadToolBar( IDR_MAINFRAME ) )
	{
		TRACE0( "Failed to create toolbar\n" );
		return -1;
	}

	// Initialize the docking pane manager and set the
	// initial them for the docking panes.  Do this only after all
	// control bars objects have been created and docked.
	m_paneManager.InstallDockingPanes( this );

	// Create docking panes.
	CXTPDockingPane* pTextureID = m_paneManager.CreatePane(
		IDR_PANE_LISTBOX,
		CRect( 0, 0, 400, 600 ),
		xtpPaneDockRight );

	CXTPDockingPane* pProperties = m_paneManager.CreatePane(
		IDR_PANE_PROPERTIES,
		CRect( 0, 0, 400, 600 ),
		xtpPaneDockBottom,
		pTextureID );

	// Set Style
    CXTPPaintManager::SetTheme( xtpThemeVisualStudio2008 );
	m_paneManager.SetTheme( xtpPaneThemeVisualStudio2008 );
    m_paneManager.SetDockingContextStickerStyle( xtpPaneStickerStyleVisualStudio2005 );
    m_paneManager.SetDefaultPaneOptions( xtpPaneNoHideable );
    m_paneManager.SetShowContentsWhileDragging( TRUE );
    m_paneManager.SetShowDockingContextStickers( TRUE );
    m_paneManager.SetAlphaDockingContext( TRUE );
    m_paneManager.UseSplitterTracker( TRUE );

	// Load the previous state for toolbars and menus.
	//LoadCommandBars( _T( "CommandBars" ) );

	// Load the previous state for docking panes.
	//CXTPDockingPaneLayout cPaneLayout( &m_paneManager );
	//if( cPaneLayout.Load( _T( "PaneLayout" ) ) )
	//	m_paneManager.SetLayout( &cPaneLayout );

    // The one and only window has been initialized, so show and update it
    ShowWindow( SW_SHOWMAXIMIZED );
	UpdateWindow();

    OnCreateFrame();

	return 0;
}

VOID CFrame::OnClose()
{
	// Save the current state for toolbars and menus.
	//SaveCommandBars( _T( "CommandBars" ) );

	// Save the current state for docking panes.
	CXTPDockingPaneLayout cPaneLayout( &m_paneManager );
	m_paneManager.GetLayout( &cPaneLayout );
	cPaneLayout.Save( _T( "PaneLayout" ) );

	CFrameWnd::OnClose();

    OnDestroyFrame();
}

VOID CFrame::OnSize( UINT nType, INT cx, INT cy )
{
    CFrameWnd::OnSize( nType, cx, cy );

    CRenderDevice::OnSize( nType, cx, cy );
}

VOID CFrame::OnSetFocus( CWnd* /*pOldWnd*/ )
{
	m_wndView.SetFocus();
}

VOID CFrame::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
	CFrameWnd::OnActivate( nState, pWndOther, bMinimized );

    CRenderDevice::OnActivate( nState, bMinimized );
}

VOID CFrame::OnCustomize()
{
	// Get a pointer to the command bars object.
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if( pCommandBars == NULL )
        return;

	// Instanciate the customize dialog object.
	CXTPCustomizeSheet dlg( pCommandBars );

	// Add the keyboard page to the customize dialog.
	CXTPCustomizeKeyboardPage pageKeyboard( &dlg );
	dlg.AddPage( &pageKeyboard );
	pageKeyboard.AddCategories( IDR_MAINFRAME );

	// Add the options page to the customize dialog.
	CXTPCustomizeOptionsPage pageOptions( &dlg );
	dlg.AddPage( &pageOptions );

	// Add the commands page to the customize dialog.
	CXTPCustomizeCommandsPage* pCommands = dlg.GetCommandsPage();
	pCommands->AddCategories( IDR_MAINFRAME );

	// Use the command bar manager to initialize the
	// customize dialog.
	pCommands->InsertAllCommandsCategory();
	pCommands->InsertBuiltInMenus( IDR_MAINFRAME );
	pCommands->InsertNewMenuCategory();

	// Dispaly the dialog.
	dlg.DoModal();
}

LRESULT CFrame::OnDockingPaneNotify( WPARAM wParam, LPARAM lParam )
{
    switch( wParam )
    {
    case XTP_DPN_SHOWWINDOW:
        {
            return OnPaneNotifyShowWindow( lParam );
        }
        break;

    case XTP_DPN_CLOSEPANE:
        {
            return OnPaneNotifyClosePane( lParam );
        }
        break;
	}

    return FALSE;
}

VOID CFrame::OnShowPane( UINT nID )
{
    if( m_paneManager.IsPaneClosed( nID ) )
        m_paneManager.ShowPane( nID );
    else
        m_paneManager.ClosePane( nID );
}

VOID CFrame::OnUpdateShowPane( CCmdUI* pCmdUI )
{
	pCmdUI->SetCheck( m_paneManager.IsPaneClosed( pCmdUI->m_nID ) ? FALSE : TRUE );
}

BOOL CFrame::OnPaneNotifyShowWindow( LPARAM lParam )
{
    CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;
    if( !pPane )
        return FALSE;

    switch( pPane->GetID() )
    {
    case IDR_PANE_LISTBOX:
        {
            if( !pPane->IsValid() && !m_wndEditList )
		    {
				m_wndEditList = (CEditListPage*)CEditListPage::CreateObject();
				m_wndEditList->Create(
					NULL,
					NULL,
					WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
					CRect( 0, 0, 0, 0 ),
					this,
					NULL );
				pPane->Attach( m_wndEditList );
		    }
        }
        break;

    case IDR_PANE_PROPERTIES:
	    {
			if( !pPane->IsValid() && !m_wndProperties.GetSafeHwnd() )
			{
				m_wndProperties.Create(
					CRect( 0, 0, 0, 0 ),
					this,
					NULL );
				pPane->Attach( &m_wndProperties );
			}
	    }
        break;
    }

    return TRUE;
}

BOOL CFrame::OnPaneNotifyClosePane( LPARAM lParam )
{
    CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;
    if( !pPane )
        return FALSE;

    return TRUE;
}

VOID CFrame::OnCreateFrame()
{
	g_pEditor = new CEditor;
    g_pRI = new CRenderInterface;

    CRenderDevice::OnCreate(
        GetSafeHwnd(),
        GetSafeHwnd(),
        GetSafeHwnd(),
        false,
        false );

	CRenderDevice::CreateObject( g_pEditor );
    CRenderDevice::AttachObject( g_pEditor, CRenderDevice::EM_INSERT_FRONT );

    CRenderDevice::CreateObject( g_pRI );
    CRenderDevice::AttachObject( g_pRI, CRenderDevice::EM_INSERT_FRONT );
}

VOID CFrame::OnDestroyFrame()
{
    CRenderDevice::DetachObject( g_pRI );
    CRenderDevice::DestroyObject( g_pRI );

	CRenderDevice::DetachObject( g_pEditor );
    CRenderDevice::DestroyObject( g_pEditor );

    CRenderDevice::OnDestroy();

    SAFE_DELETE( g_pRI );
	SAFE_DELETE( g_pEditor );
}

BOOL CFrame::OnLoop()
{
    return CRenderDevice::OnLoop();
}

void CFrame::FindItem( const std::string strItem )
{
    m_wndEditList->SetSelectItem( strItem );
}

VOID CFrame::ChangeActiveView( HWND hWnd )
{
    if( m_wndView.GetSafeHwnd() == hWnd )
	{
		m_wndEditList->SetFocus();
        m_wndView.SetActive( TRUE );
	}
    else
	{
        m_wndView.SetActive( FALSE );
	}
}

VOID CFrame::RefreshAll()
{
	RefreshEditListPage();
	RefreshControlProperties();
}

VOID CFrame::RefreshEditListPage()
{
	m_wndEditList->Refresh();
}

VOID CFrame::RefreshControlProperties()
{
	m_wndProperties.Refresh();
}