
// PatchPrimeManNew.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PatchPrimeManNew.h"
#include "PatchPrimeManNewDlg.h"

#include <strsafe.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern int VERSION;
extern int SGAMEVER;

// CPatchPrimeManNewApp

BEGIN_MESSAGE_MAP(CPatchPrimeManNewApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPatchPrimeManNewApp construction

CPatchPrimeManNewApp::CPatchPrimeManNewApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CPatchPrimeManNewApp object

CPatchPrimeManNewApp theApp;


// CPatchPrimeManNewApp initialization

BOOL CPatchPrimeManNewApp::InitInstance()
{
	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	if ( m_lpCmdLine != NULL )
	{
		TCHAR szCmdLine[MAX_PATH] = { 0, };
		StringCchCopy( szCmdLine, MAX_PATH, m_lpCmdLine );

		TCHAR* token( NULL );
		token = _tcstok( szCmdLine, _T(" ") );
		if ( token == NULL )
			return FALSE;
		VERSION = _ttoi( token );

		token = _tcstok( NULL, _T(" ") );
		if ( token == NULL )
			return FALSE;
		SGAMEVER = _ttoi( token );
	}
	else
	{
		//	잘못된 인자
		VERSION = -1; // 인자 없이 실행할 경우, 런쳐에서 값이 넘어오지 않은 경우
	}

	CPatchPrimeManNewDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
