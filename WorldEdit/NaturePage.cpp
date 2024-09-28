// NaturePage.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "MainFrm.h"

#include "../EngineLib/DxTools/DxMethods.h"

#include "afxdlgs.h"
#include "sheetWithTab.h"

#include "NaturePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CNaturePage 대화 상자입니다.


IMPLEMENT_DYNAMIC(CNaturePage, CPropertyPage)
CNaturePage::CNaturePage()
	: CPropertyPage(CNaturePage::IDD)
{
}

CNaturePage::~CNaturePage()
{
}

void CNaturePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EFFECT, m_pListEffect);
	DDX_Control(pDX, IDC_COMBO_EFFECT, m_pComboEffect);
}


BEGIN_MESSAGE_MAP(CNaturePage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_EFFECTCHANGEADD, OnBnClickedButtonEffectchangeadd)
	ON_BN_CLICKED(IDC_BUTTON_EFFECTSTATE, OnBnClickedButtonEffectstate)
	ON_BN_CLICKED(IDC_BUTTON_EFFECTDELETE, OnBnClickedButtonEffectdelete)
END_MESSAGE_MAP()


// CNaturePage 메시지 처리기입니다.
BOOL CNaturePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	DWORD nIndex;
	nIndex = m_pComboEffect.AddString ( g_strEditSheet[NATUREPAGEINDEX] );
	m_pComboEffect.SetItemData ( nIndex, NATUREPAGEINDEX );

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	//CWnd *pWnd = GetDlgItem ( IDC_EDIT_FILENAME );
	//pWnd->SetWindowText ( "이름없음.msf" );


	//ActiveMobSetEdit( FALSE, FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

/*
void CNaturePage::ActiveMobSetEdit ( BOOL bShow, BOOL bNew )
{
	int nMobSetContrls[] =
	{
		//IDC_STATIC_S,
		//IDC_LIST_MOBSET,
		//IDC_BUTTON_DEL,
		//IDC_BUTTON_MODIFY,
		//IDC_BUTTON_CREATE,
		IDC_STATIC_E,
		IDC_BUTTON5,
		IDC_EDIT_MOBNAME,
		IDC_BUTTON_SCHEDULE,
		IDC_BUTTON_CANCEL,
		IDC_BUTTON_ADD,
		IDC_BUTTON_SKIN,
		IDC_EDIT_SKIN,
		IDC_BUTTON_REGEN,
		IDC_EDIT_REGEN,
	};
	int NumContrl = sizeof(nMobSetContrls) / sizeof(int);

	CWnd *pWnd;

	for ( int i=0; i<NumContrl; i++ )
	{
		pWnd = GetDlgItem(nMobSetContrls[i]);
		pWnd->ShowWindow ( bShow );
	}

	if ( bNew )
	{
		pWnd = GetDlgItem ( IDC_EDIT_MOBNAME );
		pWnd->EnableWindow ( TRUE );
	}
	else
	{
		pWnd = GetDlgItem ( IDC_EDIT_MOBNAME );
		pWnd->EnableWindow ( FALSE );
	}
}

void CNaturePage::OnBnClickedButtonCreate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( m_pSchedule )
	{
		return;
	}

	CWnd* pWnd;

	pWnd = GetDlgItem ( IDC_BUTTON_CREATE );
	pWnd->EnableWindow ( FALSE );

	pWnd = GetDlgItem ( IDC_BUTTON_MODIFY );
	pWnd->EnableWindow ( FALSE );

	pWnd = GetDlgItem ( IDC_BUTTON_DEL );
	pWnd->EnableWindow ( FALSE );

	m_pSchedule = new GLMobSchedule;

	m_valName = "";
	m_valSkinName = "";
	m_valReGenTime.Format ( "%2.2f", 0.0f );
	UpdateData ( FALSE );


	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	const D3DXVECTOR3& vCollPos = pView->GetCollisionPos();

	DXAFFINEPARTS Affine;
	Affine.vTrans = vCollPos;
	m_pSchedule->SetAffineValue ( &Affine );

	pView->ActiveEditMatrix ( m_pSchedule );

	ActiveMobSetEdit ( TRUE, TRUE ); 
}

void CNaturePage::OnBnClickedButtonDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_ctrlMobList.GetCurSel ();
	if ( nIndex == -1 )
	{
		return;
	}

	DWORD dwPtr = m_ctrlMobList.GetItemData ( nIndex );
	if ( dwPtr == LB_ERR )
	{
		MessageBox ( "오류! 0xFFFFFFFF 만났다." );
		return;
	}
	m_pSchedule = (GLMobSchedule *)dwPtr;
	
	m_pMobSchMan->DelMobSch ( m_pSchedule->m_szName );
	m_ctrlMobList.DeleteString ( nIndex );

	m_pSchedule = NULL;
    
	ActiveMobSetEdit ( FALSE, FALSE );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->DeActiveEditMatrix ();
}

void CNaturePage::OnBnClickedButtonModify()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_ctrlMobList.GetCurSel ();
	if ( nIndex == -1 )
	{
		return;
	}

	CWnd* pWnd;

	pWnd = GetDlgItem ( IDC_BUTTON_CREATE );
	pWnd->EnableWindow ( FALSE );

	pWnd = GetDlgItem ( IDC_BUTTON_MODIFY );
	pWnd->EnableWindow ( FALSE );

	pWnd = GetDlgItem ( IDC_BUTTON_DEL );
	pWnd->EnableWindow ( FALSE );

	
	DWORD dwPtr = m_ctrlMobList.GetItemData ( nIndex );
	if ( dwPtr == LB_ERR )
	{
		MessageBox ( "오류! 0xFFFFFFFF 만났다." );
		return;
	}
	m_pSchedule = (GLMobSchedule *)dwPtr;

	m_valName = m_pSchedule->m_szName;
	m_valSkinName = m_pSchedule->m_szSkinFile;

	char szValue[128];
	_gcvt ( m_pSchedule->m_fReGenTime, 7, szValue );
	m_valReGenTime = szValue;
	UpdateData ( FALSE );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();

	pView->ActiveEditMatrix ( m_pSchedule );

	ActiveMobSetEdit ( TRUE, FALSE );
}

void CNaturePage::OnBnClickedButtonSchedule()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	
	pView->m_CurType = _CUR_SELECT;
	pView->DeActiveEditMatrix ();

	m_pSheetTab->ActiveMobSchedulePage ( &m_pSchedule->m_sMobActList );	
}

void CNaturePage::OnBnClickedButtonAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData ( TRUE );

	if ( !m_valName.GetLength () ||
		 !m_valSkinName.GetLength () ||
		 !m_valReGenTime.GetLength () )
	{
		MessageBox ( "값 좀 채워 주세요.. 네~~~?" );
		return;
	}

	CWnd* pWnd;

	pWnd = GetDlgItem ( IDC_BUTTON_CREATE );
	pWnd->EnableWindow ( TRUE );

	pWnd = GetDlgItem ( IDC_BUTTON_MODIFY );
	pWnd->EnableWindow ( TRUE );

	pWnd = GetDlgItem ( IDC_BUTTON_DEL );
	pWnd->EnableWindow ( TRUE );

	strcpy ( m_pSchedule->m_szName, m_valName.GetString () );
	strcpy ( m_pSchedule->m_szSkinFile, m_valSkinName.GetString () );
	m_pSchedule->m_fReGenTime = (float) atof ( m_valReGenTime.GetString () );

	//	<--	새로 등록되는 놈일때	-->	//
	if ( !m_pMobSchMan->FindMobSch ( m_pSchedule->m_szName ) )
	{
		int nIndex = m_ctrlMobList.AddString ( m_valName.GetString () );
		m_ctrlMobList.SetItemData ( nIndex, (DWORD_PTR)m_pSchedule );
		m_ctrlMobList.SetCurSel ( nIndex );

		m_pMobSchMan->AddMobSch ( m_pSchedule );
		m_pSchedule = NULL;

		CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
		CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
		pView->DeActiveEditMatrix ();

		ActiveMobSetEdit ( FALSE, FALSE );

		return;
	}

	//	<--	수정된 놈일때	-->	//
    sc::CGLLIST<GLMobSchedule*>* pMobScheduleList = m_pMobSchMan->GetMobSchList();
	sc::SGLNODE<GLMobSchedule*>* pHead = pMobScheduleList->m_pHead;
	while ( pHead )
	{
		if ( (GLMobSchedule*)pHead->Data == m_pSchedule )
		{
			m_pSchedule = NULL;

			CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
			CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
			pView->DeActiveEditMatrix ();

			ActiveMobSetEdit ( FALSE, FALSE );

			return;
		}
		pHead = pHead->pNext;
	}

	MessageBox ( "머라카노~ 같은 이름 아이가..." );
}

void CNaturePage::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData ( TRUE );

	CWnd* pWnd;

	pWnd = GetDlgItem ( IDC_BUTTON_CREATE );
	pWnd->EnableWindow ( TRUE );

	pWnd = GetDlgItem ( IDC_BUTTON_MODIFY );
	pWnd->EnableWindow ( TRUE );

	pWnd = GetDlgItem ( IDC_BUTTON_DEL );
	pWnd->EnableWindow ( TRUE );

	if ( !m_pMobSchMan->FindMobSch ( m_pSchedule->m_szName ) )
	{
		SAFE_DELETE ( m_pSchedule );
	}
	m_pSchedule = NULL;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	pView->DeActiveEditMatrix ();

	ActiveMobSetEdit ( FALSE, FALSE );
}

void CNaturePage::OnBnClickedButtonLoadfile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Mob Set File (*.msf)|*.msf|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".msf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CNaturePage*)this);

	dlg.m_ofn.lpstrInitialDir = GLMobScheduleMan::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();
		char *szFileName = sFileName.GetString ( sFileName.GetLength()+1 );

        m_pMobSchMan->CleanUp ();
		m_pMobSchMan->LoadFile ( szFileName );
		ListingUpdate ();
	}

	ActiveMobSetEdit( FALSE, FALSE );
}

void CNaturePage::OnBnClickedButtonSavefile()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Mob Set File (*.msf)|*.msf|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".msf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CNaturePage*)this);

	dlg.m_ofn.lpstrInitialDir = GLMobScheduleMan::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();
		char *szFileName = sFileName.GetString ( sFileName.GetLength()+1);

		m_pMobSchMan->SaveFile ( szFileName );
	}
}

void CNaturePage::OnBnClickedButtonNew()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pMobSchMan->CleanUp ();
	m_pSchedule = NULL;

	CWnd *pWnd = GetDlgItem ( IDC_EDIT_FILENAME );
	pWnd->SetWindowText ( "이름없음.msf" );
}

void CNaturePage::OnBnClickedButtonSkin()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString szFilter = "Mob skin File (*.chf)|*.chf|";
	
	UpdateData ( TRUE );

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".chf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CNaturePage*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkinCharDataContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();
		char *szFileName = sFileName.GetString ( sFileName.GetLength()+1);

		//	Note : 케릭터 파일 읽기.
		//
		m_valSkinName = szFileName;
		UpdateData ( FALSE );
	}
}

void CNaturePage::ListingUpdate ()
{
	while ( m_ctrlMobList.GetCount () )
	{
		m_ctrlMobList.DeleteString ( 0 );
	}

	if ( !m_pMobSchMan )
	{
		return ;
	}

	sc::CGLLIST<GLMobSchedule*>* pMobScheduleList = m_pMobSchMan->GetMobSchList();	
	sc::SGLNODE<GLMobSchedule*>* pHead = pMobScheduleList->m_pHead;

	if ( !pHead )
	{
		return ;
	}

	while ( pHead )
	{
		m_pSchedule = (GLMobSchedule*)pHead->Data;

		int nIndex = m_ctrlMobList.AddString ( m_pSchedule->m_szName );
		m_ctrlMobList.SetItemData ( nIndex, (DWORD_PTR)m_pSchedule );
		m_ctrlMobList.SetCurSel ( nIndex );

		pHead = pHead->pNext;
	}

	m_pSchedule = NULL;
}

void CNaturePage::OnBnClickedButtonTest()
{
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	GLLandManClient* pLandManClient = pView->GetLandManClient ();

	pLandManClient->SetMobSchMan ( &g_Man );
}
*/
void CNaturePage::OnBnClickedButtonEffectchangeadd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CNaturePage::OnBnClickedButtonEffectstate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CNaturePage::OnBnClickedButtonEffectdelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
