// CharEdit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#include "../EngineLib/Common/SUBPATH.h"
#include "../MfcExLib/MinBugTrap.h"

#include "CharEdit.h"
#include "MainFrm.h"
#include "CharEditDoc.h"
#include "CharEditView.h"
#include "State/CharStateMachine.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CCharEditApp

BEGIN_MESSAGE_MAP(CCharEditApp, CWinApp)
	//{{AFX_MSG_MAP(CCharEditApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCharEditApp construction

CCharEditApp::CCharEditApp()
    : m_bIsActive(TRUE)
	, m_bShutDown(FALSE)
{
    //GLogicData::GetInstance().LoadCountryFile();
    BUG_TRAP::BugTrapInstall(SP_OFFICE_TEST, std::string(_T("CharEdit")));
    m_pPath = new SUBPATH(SP_OFFICE_TEST);	
}

CCharEditApp::~CCharEditApp()
{
    delete m_pPath;
    m_pPath = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CCharEditApp object

CCharEditApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCharEditApp initialization

BOOL CCharEditApp::InitInstance()
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
		RUNTIME_CLASS(CCharEditDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CCharEditView));
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

	CharStateMachine::InitRefresher();

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

// App command to run the dialog
void CCharEditApp::OnAppAbout()
{
	//CAboutDlg aboutDlg;
	//aboutDlg.DoModal();

    ShellExecute(0, _T("OPEN"), _T("CharEdit.chm"), 0,  0, SW_SHOWNORMAL);
}

/////////////////////////////////////////////////////////////////////////////
// CCharEditApp message handlers


int CCharEditApp::Run() 
{
	// TODO: Add your specialized code here and/or call the base class
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
			if ( !m_bIsActive && !m_bShutDown )
			{
                if ( m_bOnColorPicker )
                {
				    CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
				    CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

				    if ( FAILED ( pView->Render3DEnvironment() ) )
				    {
					    AfxGetMainWnd()->PostMessage ( WM_CLOSE, 0, 0 );
				    }
                }
			}
			else if ( m_bIsActive == TRUE )
			{
				//	Note : D3D APP 업데이트.
				//
				CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
				CCharEditView *pView = (CCharEditView*) pFrame->GetActiveView();

				if ( FAILED ( pView->Render3DEnvironment() ) )
				{
					AfxGetMainWnd()->PostMessage ( WM_CLOSE, 0, 0 );
				}
			}
			else
				WaitMessage();

			CharStateMachine::Update();
		}
	}
	
//	return CWinApp::Run();
}

int CCharEditApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	CharStateMachine::DestroyRefresher();
	return CWinApp::ExitInstance();
}

