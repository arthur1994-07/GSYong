// EmulatorView.cpp : CEmulatorView Ŭ������ ����
//
#include "stdafx.h"

#include "../RanLogicServer/Server/AgentServer.h"
#include "../EngineLib/DxResponseMan.h"
#include "../EngineLib/G-Logic/GLogic.h"
#include "../RanLogicClient/Stage/DxGlobalStage.h"
#include "../RanLogicServer/FieldServer/GLGaeaServer.h"

#include "DxDirectMsgServer.h"
#include "Emulator.h"
#include "EmulatorDoc.h"
#include "EmulatorView.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

extern HWND	g_hwndMainDlg;

// CEmulatorView
IMPLEMENT_DYNCREATE(CEmulatorView, CView)

BEGIN_MESSAGE_MAP(CEmulatorView, CView)
	// ǥ�� �μ� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_WM_CREATE()		
	ON_WM_SETCURSOR()
	ON_WM_SIZE()
	ON_COMMAND(ID_APP_PLAY, OnAppPlay)    
    ON_UPDATE_COMMAND_UI(ID_MENU_AUTO, &CEmulatorView::OnUpdateMenuAuto)
    ON_COMMAND(ID_MENU_AUTO, &CEmulatorView::OnMenuAuto)
END_MESSAGE_MAP()

// CEmulatorView ����/�Ҹ�

CEmulatorView::CEmulatorView()
    : CD3DApplication( SP_OFFICE_TEST )
    , CSettingIni(_T("Emulator.ini"), _T("EmulatorView"))
    , m_bCreated( FALSE )
	, m_bEnterGmLevel( false )
    , m_hCursorDefault( NULL )
    , m_pEmulServerField( NULL )
	, m_pServerChar( NULL )
    , m_bAuto(false)
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	m_bUseDepthBuffer	= TRUE;

    CEmulatorApp* pApp = (CEmulatorApp*) AfxGetApp();
	
	GLOGIC::SetGlogicPackFile(FALSE);
	GLOGIC::SetGlogicZipFile(TRUE);
	GLOGIC::SetEngineZipFile(TRUE);

    SetServiceProvider(pApp->GetServiceProvider());
    m_pEngineDevice = new DxResponseMan(m_ServiceProvider, pApp->GetSubPath());
    m_pGlobalStage = new DxGlobalStage(m_pEngineDevice, pApp->GetSubPath(), false);
    m_pGaeaServer = new GLGaeaServer(m_ServiceProvider, pApp->GetSubPath());
    m_pMsgServer = new DxDirectMsgServer(m_pGaeaServer, m_pGlobalStage);    

    int nBool = 0;

    if (Load(_T("AutoPlay"), _T("%d"), &nBool) > 0)
    {
        m_bAuto = (nBool == 0 ? false : true);
    }
}

CEmulatorView::~CEmulatorView()
{
	SAFE_DELETE(m_pAgentServer);

    SAFE_DELETE(m_pGaeaServer);
    SAFE_DELETE(m_pEmulServerField);
    SAFE_DELETE(m_pMsgServer);
    SAFE_DELETE(m_pGlobalStage);
    SAFE_DELETE(m_pEngineDevice);
}


BOOL CEmulatorView::PreCreateWindow(CREATESTRUCT& cs)
{
	CEmulatorApp *pApp = (CEmulatorApp *) AfxGetApp();	

	return CView::PreCreateWindow(cs);
}

int CEmulatorView::OnCreate(LPCREATESTRUCT lpcs)
{
	
// 	m_serverDialogue.Create(IDD_DIALOG_AGENT);
// 	m_serverDialogue.ShowWindow(SW_SHOW);
// 
// 	g_hwndMainDlg = m_serverDialogue.m_hWnd;

	return 0;
}

// CEmulatorView �׸���

void CEmulatorView::OnDraw(CDC* pDC)
{
// 	static bool bInit(false);
// 	if ( bInit == false )
// 	{
// 		bInit = true;		
// 
// 		CEmulatorApp *pApp = (CEmulatorApp *) AfxGetApp();	
// 
// 		CWnd* pWindowConsole(m_serverDialogue.GetDlgItem(IDC_EDIT_CONSOLE_AGENT));
// 		CWnd* pWindowConsoleInfo(m_serverDialogue.GetDlgItem(IDC_EDIT_CONSOLE_INFO));
// 		
// 		m_pAgentServer = new CAgentServer(
// 			m_serverDialogue.m_hWnd,
// 			pWindowConsole->m_hWnd,
// 			pWindowConsoleInfo->m_hWnd,
// 			pApp->GetSubPath(),
// 			pApp->GetServiceProvider(), true);
// 		const int nRetCode(m_pAgentServer->Start());
// 		if ( nRetCode == NET_ERROR )
// 			::PostQuitMessage(0);
// 	}
	CEmulatorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CEmulatorView �μ�

BOOL CEmulatorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �⺻���� �غ�
	return DoPreparePrinting(pInfo);
}

void CEmulatorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ��ϱ� ���� �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
}

void CEmulatorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: �μ� �� ���� �۾��� �߰��մϴ�.
}


// CEmulatorView ����

#ifdef _DEBUG
void CEmulatorView::AssertValid() const
{
	CView::AssertValid();
}

void CEmulatorView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEmulatorDoc* CEmulatorView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEmulatorDoc)));
	return (CEmulatorDoc*)m_pDocument;
}
#endif //_DEBUG


// CEmulatorView �޽��� ó����
