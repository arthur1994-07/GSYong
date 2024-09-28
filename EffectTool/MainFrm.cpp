// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/File/FileUtil.h"
#include "../EngineLib/DxResponseMan.h"
#include "../EngineLib/Common/SUBPATH.h"

#include "./Properties/EffSheetWithTab.h"
#include "EffectTool.h"
#include "EffectToolDoc.h"
#include "EffectToolView.h"
#include "./DlgLog.h"
#include "MainFrm.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern BOOL		g_bEffectAABBBox;
extern	BOOL	g_bDebugParticleRange;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CURNONE, OnViewCurnone)
	ON_COMMAND(ID_VIEW_MOVE, OnViewMove)
	ON_COMMAND(ID_VIEW_ROTATE, OnViewRotate)
	ON_COMMAND(ID_VIEW_SCALE, OnViewScale)
	ON_COMMAND(ID_VIEW_CHAR, OnViewChar)
	ON_COMMAND(ID_VIEW_MAP, OnViewMap)
	ON_COMMAND(ID_VIEW_EDITBOX	,OnViewEditbox )
	ON_COMMAND(ID_VIEW_PARTICLERANGE	,OnViewParticleRange )
	ON_COMMAND(ID_VIEW_BGTEX, OnViewBgtex)
	ON_COMMAND(ID_LOAD_BGTEX, OnLoadBgtex)
	ON_UPDATE_COMMAND_UI( ID_VIEW_BGTEX, OnUpdateViewBgtex )
	ON_COMMAND(ID_ALL_FILE_LOADING, OnAllFileLoading)

	//}}AFX_MSG_MAP
    ON_WM_CLOSE()
    ON_COMMAND(ID_MENU_LOG_WINDOW, &CMainFrame::OnMenuLogWindow)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

EngineSingletone* EngineSingletone::m_pEngineSingletone;
EngineSingletone::EngineSingletone()
{
    m_pEngineDevice = NULL;
}

EngineSingletone::~EngineSingletone()
{
    if (m_pEngineDevice)
        delete m_pEngineDevice;
}

void EngineSingletone::CreateDevice(EMSERVICE_PROVIDER ServiceProvier, SUBPATH* pPath)
{
    if (!m_pEngineDevice)
        m_pEngineDevice = new DxResponseMan(ServiceProvier, pPath);
}

EngineSingletone* EngineSingletone::GetInstance()
{
    if (!m_pEngineSingletone)
        m_pEngineSingletone = new EngineSingletone;
    return m_pEngineSingletone;    
}

void EngineSingletone::ReleaseInstance()
{
    delete m_pEngineSingletone;
}

EngineDeviceMan* EngineSingletone::EngineDevice()
{
    return m_pEngineDevice;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
    : m_pLogDlg(NULL)
{
	g_bDebugParticleRange = TRUE;
	g_bEffectAABBBox = TRUE;
    CEffectToolApp* pApp = (CEffectToolApp*) AfxGetApp();
    EngineSingletone::GetInstance()->CreateDevice(pApp->GetServiceProvider(), pApp->GetSubPath());
}

CMainFrame::~CMainFrame()
{
    EngineSingletone::GetInstance()->ReleaseInstance();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{ 
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndToolBar.CreateComboBox(m_wndToolBar.m_comboMatrix, 10, ID_COMBO_MATRIX, 65, 100))
	{
		TRACE0("Failed to create toolbar's combo box\n");
		return -1;      // fail to create
	}

	m_wndToolBar.m_comboMatrix.InsertString( 0, "World" );
	m_wndToolBar.m_comboMatrix.InsertString( 1, "View" );
	m_wndToolBar.m_comboMatrix.InsertString( 2, "Local" );
	m_wndToolBar.m_comboMatrix.SetCurSel(0);

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);


	//	Note : Edit Bar
	//
	CString StrTitle1 = CString("EditBar");
	if ( !m_wndDialogBar1.Create ( this, &m_DlgEditBar, StrTitle1, IDD_DIALOG_EDIT ) )
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;      // fail to create

	}


    m_wndDialogBar1.SetBarStyle(m_wndDialogBar1.GetBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC);
	m_wndDialogBar1.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndDialogBar1,AFX_IDW_DOCKBAR_RIGHT);

	//	Note : Effect Lift-Time Line Bar
	//
	CString StrTitle2 = CString("Effect Lift Time Line");
	if ( !m_wndDialogBar2.Create ( this, &m_DlgLiftTimeBar, StrTitle2, IDD_DIALOG_EFFLIFETIME ) )
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;      // fail to create

	}

    m_wndDialogBar2.SetBarStyle(m_wndDialogBar1.GetBarStyle()|CBRS_TOOLTIPS|CBRS_FLYBY|CBRS_SIZE_DYNAMIC);
	m_wndDialogBar2.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndDialogBar2,AFX_IDW_DOCKBAR_BOTTOM);
	
	
	// Note : 실행파일의 경로를 찾아서 저장한다.
	//
	CString strAppPath;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strAppPath = szPath;

	if ( !strAppPath.IsEmpty() )
	{
		m_strAppPath = strAppPath.Left ( strAppPath.ReverseFind ( '\\' ) );
		
		if ( !m_strAppPath.IsEmpty() )
		if ( m_strAppPath.GetAt(0) == '"' )
			m_strAppPath = m_strAppPath.Right ( m_strAppPath.GetLength() - 1 );
	}

	strcpy ( m_szAppPath, m_strAppPath.GetString() );

	CMenu *pMenu = GetMenu();
	pMenu->CheckMenuItem ( ID_VIEW_BGTEX, g_bBGTex ? MF_CHECKED : MF_UNCHECKED );

    m_pLogDlg = new DlgLog;
    m_pLogDlg->Create(IDD_DIALOG_LOG);
    m_pLogDlg->ShowWindow(SW_SHOW);
    sc::LogMan::getInstance()->attachEditControl(m_pLogDlg->GetEditWindow(), m_pLogDlg->GetEditWindow());

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
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

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers

void CMainFrame::OnViewCurnone()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CEffectToolView *pView = (CEffectToolView*)GetActiveView ();
	if ( pView )	pView->SetCurType( _CUR_NONE );
}

void CMainFrame::OnViewMove()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CEffectToolView *pView = (CEffectToolView*)GetActiveView ();
	if ( pView )	pView->SetCurType( _CUR_MOVE );
}

void CMainFrame::OnViewRotate()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CEffectToolView *pView = (CEffectToolView*)GetActiveView ();
	if ( pView )	pView->SetCurType( _CUR_ROTATE );
}

void CMainFrame::OnViewScale()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CEffectToolView *pView = (CEffectToolView*)GetActiveView ();
	if ( pView )	pView->SetCurType( _CUR_SCALE );
}

void CMainFrame::OnViewChar()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CEffectToolView::m_bCharView =  CEffectToolView::m_bCharView ? FALSE : TRUE;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuItem ( ID_VIEW_CHAR, CEffectToolView::m_bCharView ? MF_CHECKED : MF_UNCHECKED );
}

void CMainFrame::OnViewMap()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CEffectToolView::m_bMapView =  CEffectToolView::m_bMapView ? FALSE : TRUE;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuItem ( ID_VIEW_MAP, CEffectToolView::m_bMapView ? MF_CHECKED : MF_UNCHECKED );
}

void CMainFrame::OnViewEditbox()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CEffectToolView::m_curType = _CUR_EDIT_BOX;
}

void CMainFrame::OnViewParticleRange()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_bDebugParticleRange =  g_bDebugParticleRange ? FALSE : TRUE;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuItem ( ID_VIEW_PARTICLERANGE, g_bDebugParticleRange ? MF_CHECKED : MF_UNCHECKED );

	g_bEffectAABBBox = g_bDebugParticleRange;
}

void CMainFrame::OnLoadBgtex()
{
	CEffectToolView *pView = (CEffectToolView*) GetActiveView ();

	CString szFilter = "Texture Image (*.bmp,*.dds,*.tga,*.jpg)|*.bmp;*.dds;*.tga;*.jpg|";
	CFileDialog dlg ( TRUE, ".BMP", NULL, OFN_HIDEREADONLY, szFilter, this );
	dlg.m_ofn.lpstrInitialDir = TextureManager::GetInstance().GetPath().c_str();

	if ( dlg.DoModal() == IDOK )
	{
		CString FilePath = dlg.GetPathName();

		pView->LoadBGTexture( FilePath.GetString() );
	}

}

void CMainFrame::OnViewBgtex()
{
	g_bBGTex = !g_bBGTex;
}

void CMainFrame::OnUpdateViewBgtex(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
	pCmdUI->SetCheck( g_bBGTex );
}



CEffGroupPage* CMainFrame::GetEffGroupPage ()
{
	return &m_DlgEditBar.m_psheetWithTab->m_EffGroupPage;
}


void CMainFrame::OnClose()
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    sc::LogMan::getInstance()->releaseInstance();
    m_pLogDlg->ShowWindow(SW_HIDE);
    delete m_pLogDlg;
    m_pLogDlg = NULL;

    CFrameWnd::OnClose();
}

void CMainFrame::OnMenuLogWindow()
{
    // TODO: 여기에 명령 처리기 코드를 추가합니다.
    if (m_pLogDlg)
        m_pLogDlg->ShowWindow(SW_SHOW);
}

//
static void FindDontHaveTexture( const TCHAR* FileName, const TCHAR* pExt )
{
	TSTRING strName = FileName;
	std::transform( strName.begin(), strName.end(), strName.begin(), tolower );

	strName = sc::file::getFileExt( strName );
	if ( strName != pExt )
		return;

	DxEffSinglePropGMan::GetInstance().LoadEffectGProp( FileName, FALSE, FALSE );
}

void CMainFrame::OnAllFileLoading()
{
	sc::CFileFindTree	m_sFileTree;
	m_sFileTree.CreateTree( DxEffSinglePropGMan::GetInstance().GetPath(), FALSE );
	m_sFileTree.SomeWorkSameExt( FindDontHaveTexture, "egp" );
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	//switch(pMsg->message)
	//{
	//case WM_UPDATE_LOG_CONSOLE:
	//	{
	//		char *pBuffer = (char *)pMsg->lParam;
	//		if(pBuffer)
	//		{
	//			SAFE_DELETE_ARRAY(pBuffer);
	//		}
	//	}
	//	break;
	//default:
	//	break;
	//}

	return __super::PreTranslateMessage(pMsg);
}