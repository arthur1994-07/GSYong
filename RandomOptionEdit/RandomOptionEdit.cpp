
// RandomOptionEdit.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"

#include "../MfcExLib/MinBugTrap.h"
#include "../Enginelib/Common/SUBPATH.h"
#include "../RanLogic/GLogicDataMan.h"

#include "RandomOptionEdit.h"
#include "RandomOptionEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRandomOptionEditApp

BEGIN_MESSAGE_MAP(CRandomOptionEditApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


CRandomOptionEditApp::CRandomOptionEditApp()
{
	GLogicData::GetInstance().LoadCountryFile();
	m_emServiceProvider = GLogicData::GetInstance().GetServiceProvider();
	BUG_TRAP::BugTrapInstall(m_emServiceProvider, std::string(_T("GenItemTool")));
	m_pPath = new SUBPATH(m_emServiceProvider);	
}

CRandomOptionEditApp::~CRandomOptionEditApp()
{
	delete m_pPath;
	m_pPath = NULL;
}

CRandomOptionEditApp theApp;

BOOL CRandomOptionEditApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�.
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));

	CRandomOptionEditDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ���⿡ [Ȯ��]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ���⿡ [���]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}

BOOL CRandomOptionEditApp::PreTranslateMessage(MSG* pMsg)
{
//	if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
}
