// ItemEdit.cpp : Defines class behavior for an application..
//

#include "stdafx.h"
#include <vector>

#include "../SigmaCore/SigmaCore.h"
#include "../MfcExLib/MinBugTrap.h"
#include "../Enginelib/Common/SUBPATH.h"
#include "../RanLogic/GLogicDataMan.h"
#include "./Dialog/SearchDialog.h"
#include "ItemEdit.h"
#include "ItemEditDlg.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CItemEditApp

BEGIN_MESSAGE_MAP(CItemEditApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CItemEditApp generation

CItemEditApp::CItemEditApp()
{
	// TODO: Add the generation code here.
	// InitInstance Place all important initialization tasks in .
	sc::initialize();
    GLogicData::GetInstance().LoadCountryFile();
    m_emServiceProvider = GLogicData::GetInstance().GetServiceProvider();
    BUG_TRAP::BugTrapInstall(m_emServiceProvider, std::string(_T("ItemEdit")));
    m_pPath = new SUBPATH(m_emServiceProvider);
}

CItemEditApp::~CItemEditApp()
{
    delete m_pPath;
    m_pPath = NULL;

	GLogicData::GetInstance().ReleaseInstance();
	sc::finalize();
}

// only CItemEditApp It's an object.
CItemEditApp theApp;

// CItemEditApp reset

BOOL CItemEditApp::InitInstance()
{
	// The application manifest uses ComCtl32.dll version 6 or later to provide visual styles.
	// If you specify to use it, InitCommonControls() is absolutely required on Windows XP.
	// You cannot create a window without using InitCommonControls().
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	CItemEditDlg dlg;


	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Here is what to do if the dialog box disappears when you click OK
		// Deploy the code.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Here is what to do if the dialog box disappears when you press cancel.
		// Deploy the code.
	}

	// The dialog box is closed, so the application's message pump is not started.
	// Returns FALSE to allow the application to exit.
	return FALSE;
}
