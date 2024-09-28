// Tool_VisualMaterial.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
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
	// 표준 파일을 기초로 하는 문서 명령입니다.
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
	// 표준 인쇄 설정 명령입니다.
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CTool_VisualMaterialApp 생성

CTool_VisualMaterialApp::CTool_VisualMaterialApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
    //GLogicData::GetInstance().LoadCountryFile();
    BUG_TRAP::BugTrapInstall(SP_OFFICE_TEST, std::string(_T("Tool_VisualMaterial")));
    m_pPath = new SUBPATH(SP_OFFICE_TEST);
}

CTool_VisualMaterialApp::~CTool_VisualMaterialApp()
{
    delete m_pPath;
    m_pPath = NULL;
}

// 유일한 CTool_VisualMaterialApp 개체입니다.

CTool_VisualMaterialApp theApp;


// CTool_VisualMaterialApp 초기화

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



// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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

// 대화 상자를 실행하기 위한 응용 프로그램 명령입니다.
void CTool_VisualMaterialApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CTool_VisualMaterialApp 메시지 처리기


int CTool_VisualMaterialApp::Run()
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

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

            // 마우스 왼쪽 버튼을 떼면 선택한 Command는 지워주자.
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
				//	Note : D3D APP 업데이트.
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