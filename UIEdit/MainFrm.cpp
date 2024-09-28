// MainFrm.cpp : CMainFrame Ŭ������ ����
//
#include "stdafx.h"
#include "../EngineLib/DxResponseMan.h"
#include "../EngineLib/Common/SUBPATH.h"
#include "../RanLogic/GLogicDataMan.h"

#include "UIEdit.h"
#include "UIEditDoc.h"
#include "UIEditView.h"
#include "MainFrm.h"
#include "MainUI/InterfaceAdmin.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/�Ҹ�
CMainFrame::CMainFrame()
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
    GLogicData::GetInstance().LoadCountryFile();
    m_pPath = new SUBPATH(GLogicData::GetInstance().GetServiceProvider());
    m_pEngineDevice = new DxResponseMan(GLogicData::GetInstance().GetServiceProvider(), m_pPath);
    m_pInterfaceAdmin = new CInterfaceAdmin(m_pEngineDevice);
}

CMainFrame::~CMainFrame()
{
    delete m_pInterfaceAdmin;
    m_pInterfaceAdmin = NULL;

    delete m_pEngineDevice;
    m_pEngineDevice = NULL;

    delete m_pPath;
    m_pPath = NULL;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

	CString StrTitle1 = CString("Edit Bar");
	if (!m_wndDialogBar.Create(this, &m_DlgUIEdit, StrTitle1 , IDD_DIALOG_MAIN ) )
	//{
	//	TRACE0("��ȭ ���� ������ ������ ���߽��ϴ�.\n");
	//	return -1;		// ������ ���߽��ϴ�.
	//}

	m_wndDialogBar.SetBarStyle(m_wndDialogBar.GetBarStyle()|CBRS_TOOLTIPS|CBRS_SIZE_DYNAMIC);
	m_wndDialogBar.EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
	DockControlBar(&m_wndDialogBar,AFX_IDW_DOCKBAR_RIGHT);


	m_cDlgEditText.Create( CDlgTextureInfoEdit::IDD, GetDesktopWindow() );
	m_cDlgMainView.Create( CDlgMainView::IDD, GetDesktopWindow() );

	m_pInterfaceAdmin->Init(&m_DlgUIEdit);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	// Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return TRUE;
}


// CMainFrame ����

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


// CMainFrame �޽��� ó����

