// UIEdit.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "UIEdit.h"

#include "UIEditDoc.h"
#include "UIEditView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUIEditApp

BEGIN_MESSAGE_MAP(CUIEditApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// ǥ�� ������ ���ʷ� �ϴ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// ǥ�� �μ� ���� ����Դϴ�.
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CUIEditApp ����

CUIEditApp::CUIEditApp()
: m_bIsActive(TRUE)
, m_bIsShutDown(FALSE)
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
	
	memset(m_szAppPath, 0, sizeof(TCHAR) * (MAX_PATH));	
}


// ������ CUIEditApp ��ü�Դϴ�.

CUIEditApp theApp;

// CUIEditApp �ʱ�ȭ

BOOL CUIEditApp::InitInstance()
{
	CString strAppPath = GetCommandLine ();

	if ( !strAppPath.IsEmpty() )
	{
		DWORD dwFind = strAppPath.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			m_strAppPath = strAppPath.Left ( dwFind );

			if ( !m_strAppPath.IsEmpty() )
				if ( m_strAppPath.GetAt(0) == '"' )
					m_strAppPath = m_strAppPath.Right ( m_strAppPath.GetLength()-1 );

			::StringCchCopy( m_szAppPath, MAX_PATH, m_strAppPath.GetString() );
		}
	}

	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControls()�� �ʿ��մϴ�. 
	// InitCommonControls()�� ������� ������ â�� ���� �� �����ϴ�.
	InitCommonControls();

	CWinApp::InitInstance();

	// OLE ���̺귯���� �ʱ�ȭ�մϴ�.
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
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
		RUNTIME_CLASS(CUIEditDoc),
		RUNTIME_CLASS(CMainFrame),       // �� SDI ������ â�Դϴ�.
		RUNTIME_CLASS(CUIEditView));
	AddDocTemplate(pDocTemplate);
	// ǥ�� �� ���, DDE, ���� ���⿡ ���� ������� ���� �м��մϴ�.
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// ����ٿ� ������ ����� ����ġ�մϴ�. ���� ���α׷��� /RegServer, /Register, /Unregserver �Ǵ� /Unregister�� ���۵� ��� FALSE�� ��ȯ�մϴ�.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// â �ϳ��� �ʱ�ȭ�Ǿ����Ƿ� �̸� ǥ���ϰ� ������Ʈ�մϴ�.
	m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
	m_pMainWnd->UpdateWindow();
	m_pMainWnd->SetWindowText( _T("UIEdit") );
	// ���̻簡 ���� ��쿡�� DragAcceptFiles�� ȣ���մϴ�.
	// SDI ���� ���α׷������� ProcessShellCommand �Ŀ� �̷��� ȣ���� �߻��ؾ� �մϴ�.
	return TRUE;
}



// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� ������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ����

// ����
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
void CUIEditApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CUIEditApp �޽��� ó����

int CUIEditApp::Run()
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
				return ( INT ) msg.wParam;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if ( m_bIsActive == TRUE )
			{
				//	Note : D3D APP ������Ʈ.
				//
				CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
				CUIEditView *pView = (CUIEditView*) pFrame->GetActiveView ();

				if ( FAILED ( pView->Render() ) )
				{
					AfxGetMainWnd()->PostMessage ( WM_CLOSE, 0, 0 );
				}	
			}		
			else
			{
				if(m_bIsShutDown)
				{
					//AfxGetMainWnd()->PostMessage( WM_CLOSE, 0, 0 );
					return 0;
				}
				else
					WaitMessage();
			}
		}
	}
}


