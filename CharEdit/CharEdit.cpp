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
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControls()�� �ʿ��մϴ�. 
	// InitCommonControls()�� ������� ������ â�� ���� �� �����ϴ�.
	InitCommonControls();

	CWinApp::InitInstance();
	

	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox("OLE �ʱ�ȭ�� �����Ͽ����ϴ�.");
		return FALSE;
	}
	AfxEnableControlContainer();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));
	LoadStdProfileSettings(4);  // MRU�� �����Ͽ� ǥ�� INI ���� �ɼ��� �ε��մϴ�.

	// ���� ���α׷��� ���� ���ø��� ����մϴ�. ���� ���ø���
	// ����, ������ â �� �� ������ ���� ������ �մϴ�.
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CCharEditDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CCharEditView));
	AddDocTemplate(pDocTemplate);

	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// ����ٿ� ������ ����� ����ġ�մϴ�. ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// â �ϳ��� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// ���̻簡 ���� ��쿡�� DragAcceptFiles�� ȣ���մϴ�.
	// SDI ���� ���α׷������� ProcessShellCommand �Ŀ� �̷��� ȣ���� �߻��ؾ� �մϴ�.

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
				//	Note : D3D APP ������Ʈ.
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

