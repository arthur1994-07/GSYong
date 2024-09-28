// MainFrm.cpp : CMainFrame 클래스의 구현
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
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸
CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
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
		TRACE0("도구 모음을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

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

	CString StrTitle1 = CString("Edit Bar");
	if (!m_wndDialogBar.Create(this, &m_DlgUIEdit, StrTitle1 , IDD_DIALOG_MAIN ) )
	//{
	//	TRACE0("대화 상자 모음을 만들지 못했습니다.\n");
	//	return -1;		// 만들지 못했습니다.
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
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	// Window 클래스 또는 스타일을 수정합니다.

	return TRUE;
}


// CMainFrame 진단

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


// CMainFrame 메시지 처리기

