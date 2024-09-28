
// RandomOptionEdit.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
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
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	CRandomOptionEditDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}

BOOL CRandomOptionEditApp::PreTranslateMessage(MSG* pMsg)
{
//	if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE) return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
}
