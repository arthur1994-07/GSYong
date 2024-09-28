// LevelMainFrm.cpp : CLevelMainFrame 클래스의 구현
//

#include "stdafx.h"

#include "./LevelEdit.h"

#include "./LevelEditDoc.h"
#include "./LevelEditView.h"

#include "../EngineLib/DxTools/DxEditMat.h"

#include "./LevelMainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLevelMainFrame

BOOL g_bVIEW_NAVIGATION_MESH_FOR_LEVELEDIT(FALSE);

IMPLEMENT_DYNCREATE(CLevelMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CLevelMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CURNONE, OnViewCurnone)
	ON_COMMAND(ID_VIEW_SELECT, OnViewSelect)
	ON_COMMAND(ID_VIEW_ROTATE, OnViewRotate)
	ON_COMMAND(ID_VIEW_SCALE, OnViewScale)
	ON_COMMAND(ID_VIEW_MOVE, OnViewMove)
	ON_COMMAND(ID_VIEW_CAMERA_CHAR, OnViewCameraChar)
	ON_COMMAND(ID_VIEW_CAMERA_TO, OnViewCameraTo)
	ON_COMMAND(ID_VIEW_SELECT_MOBSCH, OnViewSelectMobsch)
	ON_COMMAND(ID_VIEW_ALL_90, OnViewAll90)
	ON_COMMAND(ID_VIEW_NAVIGATION_MESH, OnViewNavigationMesh)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CLevelMainFrame 생성/소멸

CLevelMainFrame::CLevelMainFrame()    
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
    CLevelEditApp* pApp = (CLevelEditApp*) AfxGetApp();
    m_pGaeaServer = new GLGaeaServer(pApp->GetServiceProvider(), pApp->GetSubPath());
}

CLevelMainFrame::~CLevelMainFrame()
{
    //delete m_pGaeaServer;
	SAFE_DELETE(m_pGaeaServer);
    m_pGaeaServer = NULL;
}


int CLevelMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if (!m_wndToolBar.CreateComboBox(m_wndToolBar.m_comboMatrix, 14, ID_COMBO_MATRIX, 65, 100))
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
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}
	// TODO: 도구 모음을 도킹할 수 없게 하려면 이 세 줄을 삭제하십시오.
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	//	Note : Edit Bar
	//
	CString StrTitle1 = CString("Edit Bar");
	if ( !m_wndDialogBar1.Create ( this, &m_DlgLevelEdit, StrTitle1, IDD_DIALOG_LEVELEDIT ) )
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;      // fail to create

	}

    m_wndDialogBar1.SetBarStyle(m_wndDialogBar1.GetBarStyle()|CBRS_TOOLTIPS|CBRS_SIZE_DYNAMIC);
	m_wndDialogBar1.EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndDialogBar1,AFX_IDW_DOCKBAR_RIGHT);

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

	return 0;
}

BOOL CLevelMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}


// CLevelMainFrame 진단

#ifdef _DEBUG
void CLevelMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CLevelMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CLevelMainFrame 메시지 처리기

void CLevelMainFrame::OnViewCurnone()
{
	CLevelEditView *pView = (CLevelEditView*) GetActiveView ();

	CLevelEditView::SetCurType ( _CUR_NONE );
}

void CLevelMainFrame::OnViewSelect()
{
	CLevelEditView *pView = (CLevelEditView*) GetActiveView ();

	CLevelEditView::SetCurType ( _CUR_SELECT );
}

void CLevelMainFrame::OnViewMove()
{
	CLevelEditView *pView = (CLevelEditView*) GetActiveView ();

	CLevelEditView::SetCurType ( _CUR_MOVE );
	if ( pView )	pView->SetCurEditType ( TRANS_MAT );
}

void CLevelMainFrame::OnViewRotate()
{
	CLevelEditView *pView = (CLevelEditView*) GetActiveView ();

	CLevelEditView::SetCurType ( _CUR_ROTATE );
	if ( pView )	pView->SetCurEditType ( ROTATE_MAT );
}

void CLevelMainFrame::OnViewScale()
{
	CLevelEditView *pView = (CLevelEditView*) GetActiveView ();

	CLevelEditView::SetCurType ( _CUR_SCALE );
	if ( pView )	pView->SetCurEditType ( SCALE_MAT );
}

void CLevelMainFrame::OnViewCameraChar()
{
	
}

void CLevelMainFrame::OnViewCameraTo()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CLevelEditView *pView = (CLevelEditView*) GetActiveView ();
	pView->SetCharMoveTo ();
}

void CLevelMainFrame::OnViewSelectMobsch()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CLevelEditView *pView = (CLevelEditView*) GetActiveView ();
	CLevelEditView::SetCurType ( _CUR_SEL_MOBSCH );
}

void CLevelMainFrame::OnViewAll90()
{
	if( IDYES == AfxMessageBox( "모든 셋팅값을 90도 회전 하시겠습니까 ?", MB_YESNO ) )
	{
		GLLandMan* pGLLandMan = m_pGaeaServer->GetRootMap();
		pGLLandMan->SetObjRotate90();
	}
}

void CLevelMainFrame::OnViewNavigationMesh()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	g_bVIEW_NAVIGATION_MESH_FOR_LEVELEDIT = g_bVIEW_NAVIGATION_MESH_FOR_LEVELEDIT ? FALSE : TRUE;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuItem ( ID_VIEW_NAVIGATION_MESH, g_bVIEW_NAVIGATION_MESH_FOR_LEVELEDIT ? MF_CHECKED : MF_UNCHECKED );
}

