// GateListPage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../LevelEdit.h"
#include "./GateListPage.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "./LevelSheetWithTab.h"
#include "../../RanLogicServer/FieldServer/GLGaeaServer.h"
#include "LevelMainFrm.h"

// CGateListPage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CGateListPage, CPropertyPage)
CGateListPage::CGateListPage()
	: CPropertyPage(CGateListPage::IDD)
{
	m_CalledPage = -1;
	m_bDlgInit = FALSE;	
	m_bGateIDInit = FALSE;
}

CGateListPage::~CGateListPage()
{
}

void CGateListPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GATE, m_listboxGate);	
}


BEGIN_MESSAGE_MAP(CGateListPage, CPropertyPage)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// CGateListPage 메시지 처리기입니다.

BOOL CGateListPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_bDlgInit = TRUE;
	UpdateTreeItem ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CGateListPage::UpdateTreeItem ()
{
	if (!m_bDlgInit)
        return FALSE;

    CLevelMainFrame* pFrame = (CLevelMainFrame*) AfxGetMainWnd();
    GLGaeaServer* pGaeaServer = pFrame->GetGLGaeaServer();

	m_listboxGate.ResetContent ();

	GLLandMan* pGLLandMan = pGaeaServer->GetRootMap();

	DxLandGateMan *pLandGateMan = pGLLandMan->GetLandGateMan();
	if ( pLandGateMan == NULL )
		return TRUE;

	PDXLANDGATE pCur_Gate = pLandGateMan->GetListLandGate();
	while ( pCur_Gate )
	{
		int nIndex(0);
		nIndex = m_listboxGate.AddString ( pCur_Gate->GetName() );
		m_listboxGate.SetItemData ( nIndex, pCur_Gate->GetGateID() );

		if ( m_bGateIDInit && m_dwGateID == pCur_Gate->GetGateID() )
		{
			if ( !(pCur_Gate->GetFlags() & DxLandGate::GATE_IN) )
                continue;
			m_listboxGate.SetCurSel ( nIndex );
		}

		pCur_Gate = pCur_Gate->m_pNext;
	};

	return TRUE;
}

void CGateListPage::CleanAllItem ()
{
	if (!m_bDlgInit)
        return;
	m_listboxGate.ResetContent ();
}

void CGateListPage::SetPropertyData ( int CalledPage, DWORD dwGateID )
{
	m_CalledPage = CalledPage;
	m_dwGateID = dwGateID;

	m_bGateIDInit = TRUE;
}

void CGateListPage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyPage::OnShowWindow(bShow, nStatus);

	if ( bShow )
	{
		UpdateTreeItem ();
	}
}

void CGateListPage::OnBnClickedButtonOk()
{
	int nIndex = m_listboxGate.GetCurSel ();
	if ( nIndex==LB_ERR )
        return;

	DWORD dwGateID = (DWORD) m_listboxGate.GetItemData ( nIndex );
	if ( m_CalledPage == MOBSETPAGEINDEX )
	{
		m_pSheetTab->m_MobSetPage.SetPC_GATE_REGEN ( dwGateID );
	}

	m_pSheetTab->SetActivePage ( m_CalledPage );
	m_bGateIDInit = FALSE;
}

void CGateListPage::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( m_CalledPage );
	m_bGateIDInit = FALSE;
}
