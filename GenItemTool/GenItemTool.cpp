// SpecialTool.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "../SigmaCore/SigmaCore.h"
#include "../MfcExLib/MinBugTrap.h"
#include "../Enginelib/Common/SUBPATH.h"
#include "../RanLogic/GLogicDataMan.h"

#include "./GenItemTool.h"
#include "./GenItemToolDialogue.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CGenItemToolApplication

BEGIN_MESSAGE_MAP(CGenItemToolApplication, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

// CGenItemToolApplication ����

CGenItemToolApplication::CGenItemToolApplication()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
	sc::initialize();
    GLogicData::GetInstance().LoadCountryFile();
    m_emServiceProvider = GLogicData::GetInstance().GetServiceProvider();
    BUG_TRAP::BugTrapInstall(m_emServiceProvider, std::string(_T("GenItemTool")));
    m_pPath = new SUBPATH(m_emServiceProvider);	
}

CGenItemToolApplication::~CGenItemToolApplication()
{
    delete m_pPath;
    m_pPath = NULL;

	GLogicData::GetInstance().ReleaseInstance();
	sc::finalize();
}


// ������ CGenItemToolApplication ��ü�Դϴ�.

CGenItemToolApplication theApp;

// CGenItemToolApplication �ʱ�ȭ

BOOL CGenItemToolApplication::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControls()�� �ʿ��մϴ�. 
	// InitCommonControls()�� ������� ������ â�� ���� �� �����ϴ�.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));

	
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	CGenItemToolDilogue dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ���⿡ ��ȭ ���ڰ� Ȯ���� ���� �������� ��� ó����
		// �ڵ带 ��ġ�մϴ�.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: ���⿡ ��ȭ ���ڰ� ��Ҹ� ���� �������� ��� ó����
		// �ڵ带 ��ġ�մϴ�.
	}

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�
	// ���� ���α׷��� ���� �� �ֵ��� FALSE�� ��ȯ�մϴ�.
	return FALSE;
}
