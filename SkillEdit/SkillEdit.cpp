// SkillEdit.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "../MfcExLib/MinBugTrap.h"
#include "../SigmaCore/SigmaCore.h"
#include "../Enginelib/Common/SUBPATH.h"
#include "../RanLogic/GLogicDataMan.h"
#include "./SkillEdit.h"
#include "./SkillEditDlg.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CSkillEditApp

BEGIN_MESSAGE_MAP(CSkillEditApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSkillEditApp ����

CSkillEditApp::CSkillEditApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
	sc::initialize();
    GLogicData::GetInstance().LoadCountryFile();
    m_emServiceProvider = GLogicData::GetInstance().GetServiceProvider();
    BUG_TRAP::BugTrapInstall(m_emServiceProvider, std::string(_T("SkillEdit")));
    m_pPath = new SUBPATH(m_emServiceProvider);
}

CSkillEditApp::~CSkillEditApp()
{
    delete m_pPath;
    m_pPath = NULL;

	GLogicData::GetInstance().ReleaseInstance();
	sc::finalize();
}

// ������ CSkillEditApp ��ü�Դϴ�.

CSkillEditApp theApp;


// CSkillEditApp �ʱ�ȭ

BOOL CSkillEditApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControls()�� �ʿ��մϴ�. 
	// InitCommonControls()�� ������� ������ â�� ���� �� �����ϴ�.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();


	CSkillEditDlg dlg;
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
