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
	// ���� �����Լ� ȣ�� ���� �ڵ鷯.
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
		RUNTIME_CLASS(CWorldEditDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CWorldEditView));
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

	//	Note : ȿ�� Ÿ�Ե��� ���.
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
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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
			//	//	Note : D3D APP ������Ʈ.
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
