// NaturePage.cpp : ���� �����Դϴ�.
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

// CNaturePage ��ȭ �����Դϴ�.


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


// CNaturePage �޽��� ó�����Դϴ�.
BOOL CNaturePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	DWORD nIndex;
	nIndex = m_pComboEffect.AddString ( g_strEditSheet[NATUREPAGEINDEX] );
	m_pComboEffect.SetItemData ( nIndex, NATUREPAGEINDEX );

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	//CWnd *pWnd = GetDlgItem ( IDC_EDIT_FILENAME );
	//pWnd->SetWindowText ( "�̸�����.msf" );


	//ActiveMobSetEdit( FALSE, FALSE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nIndex = m_ctrlMobList.GetCurSel ();
	if ( nIndex == -1 )
	{
		return;
	}

	DWORD dwPtr = m_ctrlMobList.GetItemData ( nIndex );
	if ( dwPtr == LB_ERR )
	{
		MessageBox ( "����! 0xFFFFFFFF ������." );
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
		MessageBox ( "����! 0xFFFFFFFF ������." );
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView *pView = (CWorldEditView*) pFrame->GetActiveView ();
	
	pView->m_CurType = _CUR_SELECT;
	pView->DeActiveEditMatrix ();

	m_pSheetTab->ActiveMobSchedulePage ( &m_pSchedule->m_sMobActList );	
}

void CNaturePage::OnBnClickedButtonAdd()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData ( TRUE );

	if ( !m_valName.GetLength () ||
		 !m_valSkinName.GetLength () ||
		 !m_valReGenTime.GetLength () )
	{
		MessageBox ( "�� �� ä�� �ּ���.. ��~~~?" );
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

	//	<--	���� ��ϵǴ� ���϶�	-->	//
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

	//	<--	������ ���϶�	-->	//
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

	MessageBox ( "�Ӷ�ī��~ ���� �̸� ���̰�..." );
}

void CNaturePage::OnBnClickedButtonCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Mob Set File (*.msf)|*.msf|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Mob Set File (*.msf)|*.msf|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pMobSchMan->CleanUp ();
	m_pSchedule = NULL;

	CWnd *pWnd = GetDlgItem ( IDC_EDIT_FILENAME );
	pWnd->SetWindowText ( "�̸�����.msf" );
}

void CNaturePage::OnBnClickedButtonSkin()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString szFilter = "Mob skin File (*.chf)|*.chf|";
	
	UpdateData ( TRUE );

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".chf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CNaturePage*)this);

	dlg.m_ofn.lpstrInitialDir = DxSkinCharDataContainer::GetInstance().GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		CString sFileName = dlg.GetFileName();
		char *szFileName = sFileName.GetString ( sFileName.GetLength()+1);

		//	Note : �ɸ��� ���� �б�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CNaturePage::OnBnClickedButtonEffectstate()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}

void CNaturePage::OnBnClickedButtonEffectdelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}
