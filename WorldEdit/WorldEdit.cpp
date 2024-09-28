// WorldEdit.cpp : Defines the class behaviors for the application.
//
 
#include "stdafx.h"

#include "../SigmaCore/SigmaCore.h"
#include "../EngineLib/DxEffect/Frame/DxEffectFrame.h"
#include "../EngineLib/DxEffect/Frame/DxEffectMan.h"
#include "../EngineLib/Common/SUBPATH.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../MfcExLib/MinBugTrap.h"

#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "State/WorldStateMachine.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

DO_RTC_CATCH _rtc_catch;

/////////////////////////////////////////////////////////////////////////////
// CWorldEditApp

BEGIN_MESSAGE_MAP(CWorldEditApp, CWinApp)
	//{{AFX_MSG_MAP(CWorldEditApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorldEditApp construction

CWorldEditApp::CWorldEditApp()
    : m_bIsActive(TRUE)
	, m_bRedrawScreen(FALSE)
    , m_bShutDown(FALSE)
{
	// 순수 가상함수 호출 감지 핸들러.
	_set_purecall_handler(smtm_PurecallHandler);
	sc::initialize();
	GLogicData::GetInstance().LoadCountryFile();
    m_emServiceProvider = GLogicData::GetInstance().GetServiceProvider();
    BUG_TRAP::BugTrapInstall(m_emServiceProvider, std::string(_T("WorldEdit")));
    m_pPath = new SUBPATH(m_emServiceProvider);

	// Place all significant initialization in InitInstance
}

CWorldEditApp::~CWorldEditApp()
{
    delete m_pPath;
    m_pPath = NULL;

	GLogicData::GetInstance().ReleaseInstance();
	sc::finalize();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWorldEditApp object

CWorldEditApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWorldEditApp initialization

BOOL CWorldEditApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControls()가 필요합니다. 
	// InitCommonControls()를 사용하지 않으면 창을 만들 수 없습니다.
	InitCommonControls();

	CWinApp::InitInstance();

	// OLE 라이브러리를 초기화합니다.
	if (!AfxOleInit())
	{
		AfxMessageBox("OLE 초기화에 실패하였습니다.");
		return FALSE;
	}
	AfxEnableControlContainer();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));
	LoadStdProfileSettings(4);  // MRU를 포함하여 표준 INI 파일 옵션을 로드합니다.

	// 응용 프로그램의 문서 템플릿을 등록합니다. 문서 템플릿은
	// 문서, 프레임 창 및 뷰 사이의 연결 역할을 합니다.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CWorldEditDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CWorldEditView));
	AddDocTemplate(pDocTemplate);

	// 표준 셸 명령, DDE, 파일 열기에 대한 명령줄을 구문 분석합니다.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// 명령줄에 지정된 명령을 디스패치합니다. 응용 프로그램이 /RegServer, /Register, /Unregserver 또는 /Unregister로 시작된 경우 FALSE를 반환합니다.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// 창 하나만 초기화되었으므로 이를 표시하고 업데이트합니다.

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// 접미사가 있을 경우에만 DragAcceptFiles를 호출합니다.
	// SDI 응용 프로그램에서는 ProcessShellCommand 후에 이러한 호출이 발생해야 합니다.

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnUpdateViewPiececheck(CCmdUI *pCmdUI);
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//	Note : 효과 타입들을 등록.
	//
	//
	CString strText, strVer;
	DxEffectType *pCurEffectType = DxEffectMan::GetInstance().GetEffectList();
	while ( pCurEffectType )
	{
		DxEffectBase* pEffBase = DxEffectMan::GetInstance().CreateEffInstance ( pCurEffectType->TYPEID );

		PBYTE pBuff;
		DWORD dwSize, dwVer;
		pEffBase->GetProperty ( pBuff, dwSize, dwVer );
		SAFE_DELETE(pEffBase);

		strVer.Format ( "V[%04x/%04d]", dwVer, dwSize );
		strText += strVer;
		strText += pCurEffectType->NAME;
		strText += "\r\n";
		pCurEffectType = pCurEffectType->pNextType;
	}

	CWnd *pWnd = GetDlgItem ( IDC_EDIT_VER );
	pWnd->SetWindowText ( strText );


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

// App command to run the dialog
void CWorldEditApp::OnAppAbout()
{
	//CAboutDlg aboutDlg;
	//aboutDlg.DoModal();

    ShellExecute(0, _T("OPEN"), _T("WorldEdit.chm"), 0,  0, SW_SHOWNORMAL);
}

/////////////////////////////////////////////////////////////////////////////
// CWorldEditApp message handlers


int CWorldEditApp::Run() 
{
	MSG msg;

	while (1)
	{
        if ( m_bShutDown == TRUE || AfxGetMainWnd() == NULL )
		{
			ExitInstance ();
			return 0;
		}

		if ( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			if ( GetMessage(&msg, NULL, 0, 0) == FALSE )
			{
				ExitInstance();
				return msg.wParam;
			}

             if (m_pMainWnd && m_pMainWnd->PreTranslateMessage(&msg))
             {
                 continue;
             }

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			WorldStateMachine::Update();

            if ( m_bIsActive == FALSE )
                continue;

            if ( m_bShutDown == TRUE )
			{
				//AfxGetMainWnd()->PostMessage( WM_CLOSE, 0, 0 );
				return 0;
			}

			CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

			if ( FAILED ( pView->Render3DEnvironment() ) )
			{
				AfxGetMainWnd()->PostMessage ( WM_CLOSE, 0, 0 );
			}

			//if ( !m_bIsActive && !m_bShutDown )
			//{
			//	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			//	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView();

			//	if ( FAILED ( pView->Render3DEnvironment() ) )
			//	{
			//		AfxGetMainWnd()->PostMessage ( WM_CLOSE, 0, 0 );
			//	}
			//}
			//else if ( m_bIsActive == TRUE )
			//{
			//	//	Note : D3D APP 업데이트.
			//	//
			//	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			//	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

			//	if ( FAILED ( pView->Render3DEnvironment() ) )
			//	{
			//		AfxGetMainWnd()->PostMessage ( WM_CLOSE, 0, 0 );
			//	}
			//}
			//else
			//	WaitMessage();		
		}
	}
}
