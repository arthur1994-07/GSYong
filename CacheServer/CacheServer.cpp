// CacheServer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <string>
#include "../../RanThirdParty/DirectX/Include/d3d9.h"

#ifndef LPDIRECT3DDEVICEQ
#define LPDIRECT3DDEVICEQ LPDIRECT3DDEVICE9
#endif

#include "../SigmaCore/SigmaCore.h"
#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/gassert.h"
#include "../MfcExLib/MinBugTrap.h"

#include "../EngineLib/G-Logic/GLogic.h"
#include "../EngineLib/Common/SUBPATH.h"

#include "../RanLogic/GLogicDataMan.h"

#include "CacheServer.h"
#include "CacheServerMainFrm.h"

#include "CacheServerDoc.h"
#include "CacheServerView.h"

#include "../ServerController/ServerControllerMsgDefine.h"
//#include "../=VLD/include/vld.h"

//
//mjeon.tbb
//
#ifdef _USE_TBB

#include "../=TBB/include/tbb/tbbmalloc_proxy.h"

#include "../=TBB/include/tbb/task_scheduler_init.h"

#endif


// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CacheServerApp

BEGIN_MESSAGE_MAP(CacheServerApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CacheServerApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CacheServerApp construction

CacheServerApp::CacheServerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
    _set_purecall_handler(smtm_PurecallHandler);
    
	sc::initialize();
    GLogicData::GetInstance().LoadCountryFile();
    m_ServiceProvider = GLogicData::GetInstance().GetServiceProvider();
    BUG_TRAP::BugTrapInstall(m_ServiceProvider, std::string("CacheServer"),true,true,true);
	m_pPath = new SUBPATH(m_ServiceProvider);
}

CacheServerApp::~CacheServerApp()
{
	delete m_pPath;
	m_pPath = NULL;

	GLogicData::GetInstance().ReleaseInstance();
	sc::finalize();
}

// The one and only CacheServerApp object

CacheServerApp theApp;


// CacheServerApp initialization

BOOL CacheServerApp::InitInstance()
{
	//
	//mjeon.tbb
	// all threads that uses tbb MUST init the scheduler.
	//
#ifdef _USE_TBB
	tbb::task_scheduler_init init;	
#endif

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
		RUNTIME_CLASS(CacheServerDoc),
		RUNTIME_CLASS(CacheServerMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CacheServerView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	GLOGIC::SetGlogicPackFile(FALSE);
	GLOGIC::SetGlogicZipFile(TRUE);
	GLOGIC::SetEngineZipFile(TRUE);



	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	//if (!ProcessShellCommand(cmdInfo))
	//	return FALSE;
     OnFileNew();


	std::string strTitle;
	char szTitle[64];
	ZeroMemory(szTitle, sizeof(szTitle));
	
	m_pMainWnd->GetWindowText(szTitle, 64);

	strTitle = std::string(szTitle) + std::string(" - ") + sc::getFileVersion();

	m_pMainWnd->SetWindowText(strTitle.c_str());



	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand

    SERVER_CONTROLLER::checkCmdParameter(AfxGetApp()->m_lpCmdLine);	
    if( SERVER_CONTROLLER::bStart )
    {
        CacheServerMainFrame* pFrame = (CacheServerMainFrame*) AfxGetMainWnd();
        pFrame->StartServer();
    }

	return TRUE;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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

// App command to run the dialog
void CacheServerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CacheServerApp message handlers

