// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"

#include "../Enginelib/ServiceProviderDefine.h"
#include "../Enginelib/DxResponseMan.h"

#include "../VisualMaterialLib/ViewPort/DxViewPortVM.h"
#include "../VisualMaterialLib/Preview/NSVM_PREVIEW.h"
#include "../VisualMaterialLib/DxVisualMaterial.h"
#include "Tool_VisualMaterial.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


TSTRING g_strAppPath;
TSTRING CMainFrame::g_strVertexInstructionsInfo = _T("");
TSTRING CMainFrame::g_strPixelInstructionsInfo = _T("");

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(XTP_ID_CUSTOMIZE, OnCustomize)
	ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify)
    ON_UPDATE_COMMAND_UI_RANGE( ID_WINDOW_PROPERTIES, ID_WINDOW_MATERIALEXPRESSIONS, OnUpdateShowPane )
    ON_COMMAND_RANGE( ID_WINDOW_PROPERTIES, ID_WINDOW_MATERIALEXPRESSIONS, OnShowPane )
    ON_COMMAND(ID_EDIT_HOME, OnHome)
	ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_CYLINDER, ID_EDIT_PLANE, OnUpdateViewStyles)
	ON_COMMAND_RANGE(ID_EDIT_CYLINDER, ID_EDIT_PLANE, OnViewStyle)
    ON_UPDATE_COMMAND_UI(ID_BUTTON_LINE_CURVE, OnUpdateLineCurve)
	ON_COMMAND(ID_BUTTON_LINE_CURVE, OnLineCurve)
    ON_UPDATE_COMMAND_UI(ID_BUTTON_GRID, OnUpdateGridToggle)
	ON_COMMAND(ID_BUTTON_GRID, OnGridToggle)
END_MESSAGE_MAP()


static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
    CTool_VisualMaterialApp* pApp = (CTool_VisualMaterialApp*) AfxGetApp();
    SUBPATH* pPath = pApp->GetSubPath();
    m_pEngineDevice = new DxResponseMan(SP_OFFICE_TEST, pPath);
}

CMainFrame::~CMainFrame()
{
    delete m_pEngineDevice;
    m_pEngineDevice = NULL;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// Initialize the command bars
	if (!InitCommandBars())
		return -1;

	// Get a pointer to the command bars object.
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if(pCommandBars == NULL)
	{
		TRACE0("Failed to create command bars object.\n");
		return -1;      // fail to create
	}

	// Add the menu bar
	CXTPCommandBar* pMenuBar = pCommandBars->SetMenu(
		_T("Menu Bar"), IDR_MAINFRAME);
	if(pMenuBar == NULL)
	{
		TRACE0("Failed to create menu bar.\n");
		return -1;      // fail to create
	}

	// Create ToolBar
	CXTPToolBar* pToolBar = (CXTPToolBar*)
		pCommandBars->Add(_T("Standard"), xtpBarTop);
	if (!pToolBar || !pToolBar->LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;
	}
    pToolBar->EnableAnimation();

	// Set Office 2003 Theme
	CXTPPaintManager::SetTheme(xtpThemeVisualStudio2008);

	pCommandBars->GetShortcutManager()->SetAccelerators(IDR_MAINFRAME);

	// Load the previous state for toolbars and menus.
	LoadCommandBars(_T("CommandBars"));

	// Initialize the docking pane manager and set the
	// initial them for the docking panes.  Do this only after all
	// control bars objects have been created and docked.
	m_paneManager.InstallDockingPanes(this);
	// Set Office 2003 Theme
	m_paneManager.SetTheme(xtpPaneThemeVisualStudio2008);
    m_paneManager.SetDockingContextStickerStyle( xtpPaneStickerStyleVisualStudio2005 );
    m_paneManager.SetShowContentsWhileDragging( TRUE );
    m_paneManager.SetShowDockingContextStickers( TRUE );
    m_paneManager.SetAlphaDockingContext( TRUE );


	// Create docking panes.
    CXTPDockingPane* pwndPanePreview;
    CXTPDockingPane* pwndPaneProperties;

    pwndPanePreview = m_paneManager.CreatePane( ID_WINDOW_PREVIEW, 
                                                CRect(0, 0, 300, 120), 
                                                xtpPaneDockLeft );

    m_paneManager.CreatePane( ID_WINDOW_SOURCE, 
                                CRect(0, 0,200, 200), 
                                xtpPaneDockBottom, 
                                pwndPanePreview );

    pwndPaneProperties = m_paneManager.CreatePane( ID_WINDOW_PROPERTIES, 
                                                    CRect(0, 0, 200, 200), 
                                                    xtpPaneDockRight);

    m_paneManager.CreatePane( ID_WINDOW_MATERIALEXPRESSIONS, 
                                CRect(0, 0,200, 200), 
                                xtpPaneDockBottom, 
                                pwndPaneProperties );
	
    // 기본적으로 닫아놓는다.
    m_paneManager.ClosePane( ID_WINDOW_SOURCE );

	// Set the icons for the docking pane tabs.
	int nIDIcons[] = { ID_WINDOW_PROPERTIES, ID_WINDOW_SOURCE, ID_WINDOW_PREVIEW, ID_WINDOW_MATERIALEXPRESSIONS};
	m_paneManager.SetIcons( IDB_BITMAP1, nIDIcons, _countof(nIDIcons), RGB(0, 255, 0) );

	// Load the previous state for docking panes.
	CXTPDockingPaneLayout layoutNormal(&m_paneManager);
	if (layoutNormal.Load(_T("NormalLayout")))
	{
		m_paneManager.SetLayout(&layoutNormal);
	}

    // Note : 실행파일의 경로를 찾아서 저장한다.
    {
	    CString strAppPath = GetCommandLine ();
	    if ( !strAppPath.IsEmpty() )
	    {
		    m_strAppPath = strAppPath.Left ( strAppPath.ReverseFind ( '\\' ) );
		    if ( !m_strAppPath.IsEmpty() )
		    if ( m_strAppPath.GetAt(0) == '"' )
			    m_strAppPath = m_strAppPath.Right ( m_strAppPath.GetLength() - 1 );
	    }
        _tcscpy ( m_szAppPath, m_strAppPath.GetString() );
        g_strAppPath = m_szAppPath;
    }

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.style |= WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers




void CMainFrame::OnClose()
{

	// Save the current state for toolbars and menus.
	SaveCommandBars(_T("CommandBars"));

	// Save the current state for docking panes.
	CXTPDockingPaneLayout layoutNormal(&m_paneManager);
	m_paneManager.GetLayout(&layoutNormal);
	layoutNormal.Save(_T("NormalLayout"));

	CFrameWnd::OnClose();
}


void CMainFrame::OnCustomize()
{
	// Get a pointer to the command bars object.
	CXTPCommandBars* pCommandBars = GetCommandBars();
	if(pCommandBars != NULL)
	{
		// Instanciate the customize dialog object.
		CXTPCustomizeSheet dlg(pCommandBars);

		// Add the keyboard page to the customize dialog.
		CXTPCustomizeKeyboardPage pageKeyboard(&dlg);
		dlg.AddPage(&pageKeyboard);
		pageKeyboard.AddCategories(IDR_MAINFRAME);

		// Add the options page to the customize dialog.
		CXTPCustomizeOptionsPage pageOptions(&dlg);
		dlg.AddPage(&pageOptions);

		// Add the commands page to the customize dialog.
		CXTPCustomizeCommandsPage* pCommands = dlg.GetCommandsPage();
		pCommands->AddCategories(IDR_MAINFRAME);

		// Use the command bar manager to initialize the
		// customize dialog.
		pCommands->InsertAllCommandsCategory();
		pCommands->InsertBuiltInMenus(IDR_MAINFRAME);
		pCommands->InsertNewMenuCategory();

		// Dispaly the dialog.
		dlg.DoModal();
	}
}

LRESULT CMainFrame::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == XTP_DPN_SHOWWINDOW)
	{
		CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;

		if (!pPane->IsValid())
		{
			switch (pPane->GetID())
			{
			case ID_WINDOW_PROPERTIES:
				{
					if (m_sPropertyGridDlg.GetSafeHwnd() == 0)
					{
                        m_sPropertyGridDlg.Create( IDD_PROPERTYGRID_DIALOG, this );
					}
					pPane->Attach(&m_sPropertyGridDlg);
					break;
				}
			case ID_WINDOW_SOURCE:
				{
					if (m_wndSource.GetSafeHwnd() == 0)
					{
                        m_wndSource.Create( WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS |
                                            WS_VSCROLL | WS_HSCROLL |
                                            ES_READONLY | ES_LEFT | ES_MULTILINE,
							                CRect(0, 0, 0, 0), this, ID_WINDOW_SOURCE );
					}
					pPane->Attach(&m_wndSource);
					break;
				}
            case ID_WINDOW_PREVIEW:
				{
					if (m_wndPreview.GetSafeHwnd() == 0)
					{
						m_wndPreview.Create(_T("Preview"),_T("Preview"),
							                WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
							                CRect(0, 0, 0, 0), this, ID_WINDOW_PREVIEW );
					}
					pPane->Attach(&m_wndPreview);
					break;
				}
            case ID_WINDOW_MATERIALEXPRESSIONS:
				{
					if (m_wndMaterialExpressions.GetSafeHwnd() == 0)
					{
                        m_wndMaterialExpressions.Create( WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
							                            CRect(0, 0, 0, 0), this, ID_WINDOW_MATERIALEXPRESSIONS );
					}
					pPane->Attach(&m_wndMaterialExpressions);
					break;
				}
			}
		}
		return TRUE;
	}
	return FALSE;
}

void CMainFrame::OnShowPane( UINT nID )
{
    if( m_paneManager.IsPaneClosed( nID ) )
        m_paneManager.ShowPane( nID );
    else
        m_paneManager.ClosePane( nID );
}

void CMainFrame::OnUpdateShowPane( CCmdUI* pCmdUI )
{
	pCmdUI->SetCheck( m_paneManager.IsPaneClosed( pCmdUI->m_nID ) ? FALSE : TRUE );
}

void CMainFrame::OnHome()
{
    D3DXVECTOR3 vFromPt		= D3DXVECTOR3( 0.0f, 0.0f, -50.0f );
    D3DXVECTOR3 vLookatPt	= D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec		= D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    DxViewPortVM::GetInstance()->SetViewTrans( vFromPt, vLookatPt, vUpVec );
}

void CMainFrame::OnUpdateViewStyles(CCmdUI* pCmdUI)
{
	// TODO: customize or extend this code to handle choices on the
	// View menu.

	// otherwise, use dots to reflect the style of the view
	pCmdUI->Enable();
	BOOL bChecked = FALSE;

    switch ( pCmdUI->m_nID )
    {
    case ID_EDIT_CYLINDER:
        if ( NSVM_PREVIEW::g_nPreviewRenderStyle == NSVM_PREVIEW::MODEL_CYLINDER )
            bChecked = TRUE;
        break;

    case ID_EDIT_CUBE:
        if ( NSVM_PREVIEW::g_nPreviewRenderStyle == NSVM_PREVIEW::MODEL_CUBE )
            bChecked = TRUE;
        break;

    case ID_EDIT_SPHERE:
        if ( NSVM_PREVIEW::g_nPreviewRenderStyle == NSVM_PREVIEW::MODEL_SPHERE )
            bChecked = TRUE;
        break;

    case ID_EDIT_PLANE:
        if ( NSVM_PREVIEW::g_nPreviewRenderStyle == NSVM_PREVIEW::MODEL_PLANE )
            bChecked = TRUE;
        break;
    };

	pCmdUI->SetRadio(bChecked ? 1 : 0);
}

void CMainFrame::OnViewStyle(UINT nCommandID)
{
	// TODO: customize or extend this code to handle choices on the
	// View menu.

    switch ( nCommandID )
    {
    case ID_EDIT_CYLINDER:
        NSVM_PREVIEW::g_nPreviewRenderStyle = NSVM_PREVIEW::MODEL_CYLINDER;
        break;

    case ID_EDIT_CUBE:
        NSVM_PREVIEW::g_nPreviewRenderStyle = NSVM_PREVIEW::MODEL_CUBE;
        break;

    case ID_EDIT_SPHERE:
        NSVM_PREVIEW::g_nPreviewRenderStyle = NSVM_PREVIEW::MODEL_SPHERE;
        break;

    case ID_EDIT_PLANE:
        NSVM_PREVIEW::g_nPreviewRenderStyle = NSVM_PREVIEW::MODEL_PLANE;
        break;
    };
}

void CMainFrame::OnUpdateLineCurve(CCmdUI* pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetRadio( DxVisualMaterial::g_bLineCurve ? 1 : 0 );
}

void CMainFrame::OnLineCurve()
{
    DxVisualMaterial::g_bLineCurve = DxVisualMaterial::g_bLineCurve ? FALSE : TRUE;
}

void CMainFrame::OnUpdateGridToggle(CCmdUI* pCmdUI)
{
    pCmdUI->Enable();
    pCmdUI->SetRadio( NSVM_PREVIEW::g_bGridToggle ? 1 : 0 );
}

void CMainFrame::OnGridToggle()
{
    NSVM_PREVIEW::g_bGridToggle = NSVM_PREVIEW::g_bGridToggle ? FALSE : TRUE;
}

void CMainFrame::SelectCommand( DxVMCommand* pVMCommand )
{
    if ( m_sPropertyGridDlg.GetSafeHwnd() == 0 )
        return;

    m_sPropertyGridDlg.SelectCommand( pVMCommand );
}

void CMainFrame::SetSource( const std::string& strFX, const std::string& strSimpleFX )
{
    static std::string g_strSource("");
    if ( g_strSource != strFX )
    {
        g_strSource = strFX;

        // fx 파일 만들기
        {
            TSTRING strPath = m_strAppPath.GetString();
            strPath += _T("\\~Temp.fx");
            FILE* m_pFile = _tfopen( strPath.c_str(), _T("wt") );
	        if ( m_pFile )
	        {
                fprintf( m_pFile, strFX.c_str() );
                fclose( m_pFile );
            }
        }

        // Vertex InstructionInfo
        {
            TSTRING strPath = _T("fxc.exe /T vs_2_0 /E VS /Fc ");
            strPath += m_strAppPath.GetString();
            strPath += _T("\\~Temp.pso ");
            strPath += m_strAppPath.GetString();
            strPath += _T("\\~Temp.fx");
            g_strVertexInstructionsInfo = _T("Vertex Shader : ");
            GetInstructionInfo( strPath, g_strVertexInstructionsInfo );
        }

        // Pixel InstructionInfo
        {
            TSTRING strPath = _T("fxc.exe /T ps_2_0 /E PS /Fc ");
            strPath += m_strAppPath.GetString();
            strPath += _T("\\~Temp.pso ");
            strPath += m_strAppPath.GetString();
            strPath += _T("\\~Temp.fx");
            g_strPixelInstructionsInfo = _T("Pixel Shader : ");
            GetInstructionInfo( strPath, g_strPixelInstructionsInfo );
        }

        // 파일 삭제
        {
            TSTRING strPath = m_strAppPath.GetString();
            strPath += _T("\\~Temp.fx");
            remove( strPath.c_str() );

            strPath = m_strAppPath.GetString();
            strPath += _T("\\~Temp.pso");
            remove( strPath.c_str() );
        }
    }


    if ( m_wndSource.GetSafeHwnd() == 0 )
        return;

    // 이전 HLSL과 같다면 Update 필요 없음.
    CString strNamePrev;
    m_wndSource.GetWindowText( strNamePrev );
    if ( strSimpleFX == strNamePrev.GetString() )
        return;

    // HLSL 업데이트
    m_wndSource.SetWindowText( strSimpleFX.c_str() );
}

void CMainFrame::GetInstructionInfo( const TSTRING& strCommand, TSTRING& strInstructionInfo )
{
    // fxc 실행시키기
    {
        DWORD dwExitCode = 0; 

        STARTUPINFO siStartupInfo; 
        PROCESS_INFORMATION piProcessInfo; 
        memset(&siStartupInfo, 0, sizeof(siStartupInfo)); 
        memset(&piProcessInfo, 0, sizeof(piProcessInfo)); 
        siStartupInfo.cb = sizeof(siStartupInfo); 

        if (CreateProcess( NULL, 
                            const_cast<LPSTR>(strCommand.c_str()), 
                            0, 
                            0, 
                            FALSE,         //pwszParam
                            CREATE_NO_WINDOW, 
                            0, 
                            m_strAppPath.GetString(), 
                            &siStartupInfo, 
                            &piProcessInfo) != false) 
        { 
             /* Watch the process. */ 
            dwExitCode = WaitForSingleObject( piProcessInfo.hProcess, 100 ); 
        }

        CloseHandle(piProcessInfo.hProcess); 
        CloseHandle(piProcessInfo.hThread);    
    }

    // pso 확인
    {
        #define MAX_COLS 32768
        char szText[MAX_COLS]; // 행이 1줄씩 임시로 저장될 버퍼
        TSTRING strText;

        std::vector<int>    vecInt;
        TSTRING strPath = m_strAppPath.GetString();
        strPath += _T("\\~Temp.pso");
        FILE* m_pFile = _tfopen( strPath.c_str(), _T("rt") );
	    if ( m_pFile )
        {
            // 읽기 시작: 프로그램의 본체
            while ( fgets( szText, MAX_COLS, m_pFile ) != NULL) 
            {
                vecInt.clear();
                strText = szText;

                std::string::size_type idx = strText.find( "approximately" );
                if ( idx != std::string::npos )
                {
                    strInstructionInfo += strText;
                }
            }

            fclose( m_pFile );
        }
    }
}

HRESULT CMainFrame::OnCreateDevice( IDirect3DDevice9* pd3dDevice )
{
    return m_wndPreview.OnCreateDevice( pd3dDevice );
}

HRESULT CMainFrame::OnResetDevice( IDirect3DDevice9* pd3dDevice )
{
    return m_wndPreview.OnResetDevice( pd3dDevice );
}

void CMainFrame::OnLostDevice()
{
    m_wndPreview.OnLostDevice();
}

void CMainFrame::OnFrameMove( float fElapsedTime )
{
    m_wndPreview.OnFrameMove( fElapsedTime );
}

HRESULT CMainFrame::PreviewRender()
{
    return m_wndPreview.Render();
}
