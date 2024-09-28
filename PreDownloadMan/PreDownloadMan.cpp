#include "stdafx.h"
#include "../SigmaCore/File/FileUtil.h"
#include "../SigmaCore/SigmaCore.h"

#include "../Enginelib/Common/SUBPATH.h"

#include "../MfcExLib/MinBugTrap.h"

#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLogicDataMan.h"

#include "PreDownloadMan.h"
#include "PreDownloadManDlg.h"

// ----------------------------------------------------------------------------
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ----------------------------------------------------------------------------

// CPreDownloadManApp

BEGIN_MESSAGE_MAP(CPreDownloadManApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

// CPreDownloadManApp »ý¼º

CPreDownloadManApp::CPreDownloadManApp()
{
#ifdef _DEBUG
	m_memState = NULL;
#endif

	sc::initialize();
	GLogicData::GetInstance().LoadCountryFile();
	m_ServiceProvider = GLogicData::GetInstance().GetServiceProvider();
	BUG_TRAP::BugTrapInstall(m_ServiceProvider, std::string(_T("PreDownloder")));
	m_pPath = new SUBPATH(m_ServiceProvider);

	TCHAR szPROFILE[MAX_PATH] = {0};
	SHGetSpecialFolderPath(NULL, szPROFILE, CSIDL_PERSONAL, FALSE);

	std::string DirPath(szPROFILE);
	DirPath.append(m_pPath->SaveRoot());
	sc::file::createDirectory(DirPath);

}

CPreDownloadManApp::~CPreDownloadManApp()
{
	delete m_pPath;
	m_pPath = NULL;

#ifdef _DEBUG
	if (m_memState != NULL)
	{
		m_memState->DumpAllObjectsSince();

		delete m_memState;
	}
#endif

	GLogicData::GetInstance().ReleaseInstance();
	sc::finalize();
}

CPreDownloadManApp theApp;

BOOL CPreDownloadManApp::InitInstance()
{
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

#if _MFC_VER < 0x0700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

#ifdef _DEBUG
	m_memState = new CMemoryState;
	m_memState->Checkpoint();
#endif

	CPreDownloadManDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	return FALSE;
}

BOOL CPreDownloadManApp::InitApplication()
{
	if ( FindWindow ( NULL, "PreDownloder" ) )
	{		
		return FALSE;
	}

	return CWinApp::InitApplication();
}
