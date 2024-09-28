// Tool_VisualMaterial.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "../Enginelib/Common/SUBPATH.h"
#include "../MfcExLib/MinBugTrap.h"
#include "../VisualMaterialLib/Define/DxVMDefine.h"

#include "Tool_VisualMaterial.h"
#include "MainFrm.h"
#include "Tool_VisualMaterialDoc.h"
#include "Tool_VisualMaterialView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTool_VisualMaterialApp

BEGIN_MESSAGE_MAP(CTool_VisualMaterialApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CTool_VisualMaterialApp::OnAppAbout)
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// ǥ�� �μ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CTool_VisualMaterialApp ����

CTool_VisualMaterialApp::CTool_VisualMaterialApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
    //GLogicData::GetInstance().LoadCountryFile();
    BUG_TRAP::BugTrapInstall(SP_OFFICE_TEST, std::string(_T("Tool_VisualMaterial")));
    m_pPath = new SUBPATH(SP_OFFICE_TEST);
}

CTool_VisualMaterialApp::~CTool_VisualMaterialApp()
{
    delete m_pPath;
    m_pPath = NULL;
}

// ������ CTool_VisualMaterialApp ��ü�Դϴ�.

CTool_VisualMaterialApp theApp;


// CTool_VisualMaterialApp �ʱ�ȭ

BOOL CTool_VisualMaterialApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CTool_VisualMaterialDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CTool_VisualMaterialView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand

	return TRUE;
}



// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// ��ȭ ���ڸ� �����ϱ� ���� ���� ���α׷� ����Դϴ�.
void CTool_VisualMaterialApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CTool_VisualMaterialApp �޽��� ó����


int CTool_VisualMaterialApp::Run()
{
    // TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

    MSG msg;

	while (1)
	{
		if ( PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) )
		{
			if ( GetMessage(&msg, NULL, 0, 0) == FALSE )
			{
				ExitInstance();
				return (int) msg.wParam;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);

            // ���콺 ���� ��ư�� ���� ������ Command�� ��������.
            switch ( msg.message )
            {
            case WM_LBUTTONUP:
                NSVISUAL_MATERIAL::g_nSelectCommandVM = -1;
                break;
            }
		}
		else
		{
			if ( m_bIsActive == TRUE )
			{
				//	Note : D3D APP ������Ʈ.
				//
				CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
				CTool_VisualMaterialView *pView = (CTool_VisualMaterialView*) pFrame->GetActiveView ();

				if ( SUCCEEDED ( pView->RenderApp() )  )
                {
                    if ( FAILED ( pFrame->PreviewRender() ) )
                    {
                        AfxGetMainWnd()->PostMessage ( WM_CLOSE, 0, 0 );
                    }
                }
                else
				{
					AfxGetMainWnd()->PostMessage ( WM_CLOSE, 0, 0 );
				}
			}		
			else
			{
				WaitMessage();
			}
		}
	}

    //return CWinApp::Run();
}