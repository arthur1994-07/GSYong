// LevelMainFrm.cpp : CLevelMainFrame Ŭ������ ����
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
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CLevelMainFrame ����/�Ҹ�

CLevelMainFrame::CLevelMainFrame()    
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
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
		TRACE0("���� ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
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
		TRACE0("���� ǥ������ ������ ���߽��ϴ�.\n");
		return -1;      // ������ ���߽��ϴ�.
	}
	// TODO: ���� ������ ��ŷ�� �� ���� �Ϸ��� �� �� ���� �����Ͻʽÿ�.
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

	// Note : ���������� ��θ� ã�Ƽ� �����Ѵ�.
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
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	// Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return TRUE;
}


// CLevelMainFrame ����

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


// CLevelMainFrame �޽��� ó����

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
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CLevelEditView *pView = (CLevelEditView*) GetActiveView ();
	pView->SetCharMoveTo ();
}

void CLevelMainFrame::OnViewSelectMobsch()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	CLevelEditView *pView = (CLevelEditView*) GetActiveView ();
	CLevelEditView::SetCurType ( _CUR_SEL_MOBSCH );
}

void CLevelMainFrame::OnViewAll90()
{
	if( IDYES == AfxMessageBox( "��� ���ð��� 90�� ȸ�� �Ͻðڽ��ϱ� ?", MB_YESNO ) )
	{
		GLLandMan* pGLLandMan = m_pGaeaServer->GetRootMap();
		pGLLandMan->SetObjRotate90();
	}
}

void CLevelMainFrame::OnViewNavigationMesh()
{
	// TODO: ���⿡ ��� ó���� �ڵ带 �߰��մϴ�.
	g_bVIEW_NAVIGATION_MESH_FOR_LEVELEDIT = g_bVIEW_NAVIGATION_MESH_FOR_LEVELEDIT ? FALSE : TRUE;

	CMenu *pMenu = GetMenu ();
	pMenu->CheckMenuItem ( ID_VIEW_NAVIGATION_MESH, g_bVIEW_NAVIGATION_MESH_FOR_LEVELEDIT ? MF_CHECKED : MF_UNCHECKED );
}

