
//

#include "stdafx.h"
#include "TexEncrypt.h"
#include "TexEncryptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTexEncryptApp

BEGIN_MESSAGE_MAP(CTexEncryptApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()



CTexEncryptApp::CTexEncryptApp()
{

}



CTexEncryptApp theApp;



BOOL CTexEncryptApp::InitInstance()
{
	InitCommonControls();

	CWinApp::InitInstance();


	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성한 응용 프로그램"));

	CTexEncryptDlg dlg;
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
