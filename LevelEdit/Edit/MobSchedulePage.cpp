// MobSchedulePage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../LevelEdit.h"
#include "../LevelMainFrm.h"
#include "../LevelEditDoc.h"
#include "../LevelEditView.h"

#include "../../EngineLib/DxTools/DxViewPort.h"

#include "./MobSchedulePage.h"
#include "./LevelSheetWithTab.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMobSchedulePage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMobSchedulePage, CPropertyPage)
CMobSchedulePage::CMobSchedulePage()
	: CPropertyPage(CMobSchedulePage::IDD)
	, m_valTime(_T("")),
	m_bInit(FALSE)
{
	m_pMobActList = NULL;
	m_pMobAction = NULL;

	m_AffineMatrix.UseAffineMatrix ();
}

CMobSchedulePage::~CMobSchedulePage()
{
	if ( m_pMobActList )
	{
		sc::SGLNODE<SMOBACTION>* pHead = m_pMobActList->m_pHead;
		while ( pHead )
		{
			if ( (SMOBACTION*)&pHead->Data == m_pMobAction )
			{
				m_pMobAction = NULL;
				return;
			}

			pHead = pHead->pNext;
		}
	}
	SAFE_DELETE ( m_pMobAction );
}

void CMobSchedulePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ACTION, m_ctrlAction);
	DDX_Text(pDX, IDC_EDIT_TIME, m_valTime);
	DDX_Control(pDX, IDC_LIST_SCHEDULE, m_ctrlSchedule);
}


BEGIN_MESSAGE_MAP(CMobSchedulePage, CPropertyPage)	
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, OnBnClickedButtonCreate)
	ON_BN_CLICKED(IDC_BUTTON_MODIFY, OnBnClickedButtonModify)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_CBN_SELCHANGE(IDC_COMBO_ACTION, OnCbnSelchangeComboAction)
	ON_LBN_DBLCLK(IDC_LIST_SCHEDULE, OnLbnDblclkListSchedule)
	ON_BN_CLICKED(IDC_BUTTON_MOVE, OnBnClickedButtonMove)
END_MESSAGE_MAP()


// CMobSchedulePage 메시지 처리기입니다.

void CMobSchedulePage::SetSchedule(sc::CGLLIST<SMOBACTION>* pMobActList)
{
	m_pMobActList = pMobActList;
	ListingUpdate();
}

BOOL CMobSchedulePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	char	szType[][128] = 
	{
		"Move",
		"Rest"
	};
	for ( int i = 0; i < 2; i++ )
	{
		m_ctrlAction.AddString ( szType[i] );
	}

	ActiveMobScheDuleEdit ( FALSE, FALSE );
	ListingUpdate();


	m_bInit = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMobSchedulePage::ActiveMobScheDuleEdit ( BOOL bShow, BOOL bNew )
{
	int nMobScheduleContrls[] =
	{
		IDC_COMBO_ACTION,
		IDC_STATIC_E,
		IDC_EDIT_TIME,
		IDC_BUTTON_CANCEL,
		IDC_BUTTON_ADD,
	};
	int NumContrl = sizeof(nMobScheduleContrls) / sizeof(int);

	CWnd *pWnd;

	for ( int i=0; i<NumContrl; i++ )
	{
		pWnd = GetDlgItem(nMobScheduleContrls[i]);
		pWnd->ShowWindow ( bShow );
	}

	//if ( bNew )
	//{
	//	pWnd = GetDlgItem ( IDC_EDIT_NAME );
	//	pWnd->EnableWindow ( TRUE );
	//}
	//else
	//{
	//	pWnd = GetDlgItem ( IDC_EDIT_NAME );
	//	pWnd->EnableWindow ( FALSE );
	//}
}

void CMobSchedulePage::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd* pWnd;

	pWnd = GetDlgItem ( IDC_BUTTON_CREATE );
	pWnd->EnableWindow ( TRUE );

	pWnd = GetDlgItem ( IDC_BUTTON_MODIFY );
	pWnd->EnableWindow ( TRUE );

	pWnd = GetDlgItem ( IDC_BUTTON_DELETE );
	pWnd->EnableWindow ( TRUE );

	pWnd = GetDlgItem ( IDC_BUTTON_OK );
	pWnd->EnableWindow ( TRUE );

	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();
	pView->DeActiveEditMatrix();

	ActiveMobScheDuleEdit ( FALSE, FALSE );

	sc::SGLNODE<SMOBACTION>* pHead = m_pMobActList->m_pHead;
    while ( pHead )
	{
		if ( (SMOBACTION*)&pHead->Data == m_pMobAction )
		{
			m_pMobAction = NULL;
			return;
		}

		pHead = pHead->pNext;
	}

	SAFE_DELETE ( m_pMobAction );
}

void CMobSchedulePage::OnBnClickedButtonAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd* pWnd;

	pWnd = GetDlgItem ( IDC_BUTTON_CREATE );
	pWnd->EnableWindow ( TRUE );

	pWnd = GetDlgItem ( IDC_BUTTON_MODIFY );
	pWnd->EnableWindow ( TRUE );

	pWnd = GetDlgItem ( IDC_BUTTON_DELETE );
	pWnd->EnableWindow ( TRUE );

	pWnd = GetDlgItem ( IDC_BUTTON_OK );
	pWnd->EnableWindow ( TRUE );

	UpdateData ( TRUE );

	int nIndex = m_ctrlAction.GetCurSel ();	
	m_pMobAction->emAction = (EMMOBACTIONS) nIndex;

	switch ( nIndex )
	{
	case 0:	//	MOB_ACTMOVE
		{
			CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
			CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();

			D3DXVECTOR3	pos = m_AffineMatrix.m_pAffineParts->vTrans;
			m_pMobAction->vPos = pos;	
			pView->DeActiveEditMatrix();
		}
		break;
	case 1:	//	MOB_ACTSTAY
		{
			if ( !m_valTime.GetLength () )
			{
				MessageBox ( "ERROR : Enter a term of rest." );
				return;
			}

			m_pMobAction->fLife = (float) atof ( m_valTime.GetString () );
		}
		break;
	default:
		{
		}
		break;
	}

	sc::SGLNODE<SMOBACTION>* pHead = m_pMobActList->m_pHead;
    while ( pHead )
	{
		if ( (SMOBACTION*)&pHead->Data == m_pMobAction )
		{
			m_pMobAction = NULL;
			ActiveMobScheDuleEdit ( FALSE, FALSE );
			ListingUpdate ();

			return;
		}

		pHead = pHead->pNext;
	}

	m_pMobActList->ADDTAIL ( *m_pMobAction );
	SAFE_DELETE ( m_pMobAction );

	ActiveMobScheDuleEdit ( FALSE, FALSE );
	ListingUpdate ();
}

void CMobSchedulePage::OnBnClickedButtonCreate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if ( m_pMobAction )
	{
		return;
	}

	CWnd* pWnd;

	pWnd = GetDlgItem ( IDC_BUTTON_CREATE );
	pWnd->EnableWindow ( FALSE );

	pWnd = GetDlgItem ( IDC_BUTTON_MODIFY );
	pWnd->EnableWindow ( FALSE );

	pWnd = GetDlgItem ( IDC_BUTTON_DELETE );
	pWnd->EnableWindow ( FALSE );

	pWnd = GetDlgItem ( IDC_BUTTON_OK );
	pWnd->EnableWindow ( FALSE );

	m_pMobAction = new SMOBACTION;

	m_ctrlAction.SetCurSel ( 0 );
	m_valTime.Format ( "%2.2f", 3.6f );

	UpdateData ( FALSE );

	ActiveMobScheDuleEdit ( TRUE, TRUE );

	OnCbnSelchangeComboAction();
}

void CMobSchedulePage::OnBnClickedButtonModify()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_ctrlSchedule.GetCurSel ();
	if ( nIndex == -1 )
	{
		return;
	}

	CWnd* pWnd;

	pWnd = GetDlgItem ( IDC_BUTTON_CREATE );
	pWnd->EnableWindow ( FALSE );

	pWnd = GetDlgItem ( IDC_BUTTON_MODIFY );
	pWnd->EnableWindow ( FALSE );

	pWnd = GetDlgItem ( IDC_BUTTON_DELETE );
	pWnd->EnableWindow ( FALSE );

	pWnd = GetDlgItem ( IDC_BUTTON_OK );
	pWnd->EnableWindow ( FALSE );

	DWORD_PTR dwPtr = m_ctrlSchedule.GetItemData ( nIndex );
	if ( dwPtr == LB_ERR )
	{
		MessageBox ( "Error! 0xFFFFFFFF" );
		return;
	}
	m_pMobAction = (SMOBACTION*)dwPtr;
 
    int nType = (int)m_pMobAction->emAction;
	m_ctrlAction.SetCurSel ( nType );

	switch ( nType )
	{
	case 0:	//	MOB_ACTMOVE
		{
			DXAFFINEPARTS Affine;
			Affine.vTrans = m_pMobAction->vPos;
			m_AffineMatrix.SetAffineValue ( &Affine );	

			CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
			CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();

			pView->ActiveEditMatrix ( &m_AffineMatrix );
		}
		break;
	case 1:	//	MOB_ACTSTAY
		{
			char szValue[128];
			_gcvt ( m_pMobAction->fLife, 7, szValue );
			m_valTime = szValue;
		}
		break;
	default:
		{
		}
		break;
	}

	UpdateData ( FALSE );

	ActiveMobScheDuleEdit ( TRUE, FALSE );
	ListingUpdate ();
}

void CMobSchedulePage::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_ctrlSchedule.GetCurSel ();
	if ( nIndex == -1 )
	{
		return;
	}

	DWORD_PTR dwPtr = m_ctrlSchedule.GetItemData ( nIndex );
	if ( dwPtr == LB_ERR )
	{
		MessageBox ( "Error! 0xFFFFFFFF" );
		return;
	}
	m_pMobAction = (SMOBACTION*)dwPtr;
	
	sc::SGLNODE<SMOBACTION>* pHead = m_pMobActList->m_pHead;
    while ( pHead )
	{
		if ( (SMOBACTION*)&pHead->Data == m_pMobAction )
		{
			m_pMobActList->DELNODE ( pHead );
			m_ctrlSchedule.DeleteString ( nIndex );
			m_pMobAction = NULL;
			return;
		}

		pHead = pHead->pNext;
	}

	ActiveMobScheDuleEdit ( FALSE, FALSE );
	ListingUpdate ();
}

void CMobSchedulePage::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( MOBSETPAGEINDEX );
	CMobSetPage* pMobSetPage = (CMobSetPage*)m_pSheetTab->GetActivePage ();

	CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
	CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();

	pView->ActiveEditMatrix ( &(pMobSetPage->m_Schedule) );

	//	추가해야함
}

void CMobSchedulePage::OnCbnSelchangeComboAction()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nIndex = m_ctrlAction.GetCurSel ();

	switch ( nIndex )
	{
	case 0:	//	MOB_ACTMOVE
		{
			CWnd *pWnd = (CWnd*) GetDlgItem(IDC_EDIT_TIME);
			pWnd->EnableWindow ( FALSE );

			CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
			CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();

			const D3DXVECTOR3& vCollPos = pView->GetCollisionPos();

			DXAFFINEPARTS Affine;
			Affine.vTrans = vCollPos;
			m_AffineMatrix.SetAffineValue ( &Affine );	

			pView->ActiveEditMatrix ( &m_AffineMatrix );
		}
		break;
	case 1:	//	MOB_ACTSTAY
		{
			CWnd *pWnd = (CWnd*) GetDlgItem(IDC_EDIT_TIME);
			pWnd->EnableWindow ( TRUE );

			CLevelMainFrame *pFrame = (CLevelMainFrame *) AfxGetMainWnd(); //AfxGetApp()->m_pMainWnd;
			CLevelEditView *pView = (CLevelEditView*) pFrame->GetActiveView ();

			pView->DeActiveEditMatrix();
		}
		break;
	default:
		{
		}
		break;
	}	
}

void CMobSchedulePage::ListingUpdate ()
{
	while ( m_ctrlSchedule.GetCount () )
	{
		m_ctrlSchedule.DeleteString ( 0 );
	}

	if ( !m_pMobActList )
	{
		return;
	}

	sc::SGLNODE<SMOBACTION>* pHead = m_pMobActList->m_pHead;
    while ( pHead )
	{
		CString	str;
		SMOBACTION MobAction = pHead->Data;
		int nType = (int)MobAction.emAction;

		switch ( nType )
		{
		case 0:	//	MOB_ACTMOVE
			{
				char szValueX[32];
				char szValueY[32];
				char szValueZ[32];
				_gcvt ( MobAction.vPos.x, 7, szValueX );
				_gcvt ( MobAction.vPos.y, 7, szValueY );
				_gcvt ( MobAction.vPos.z, 7, szValueZ );			

				str.Format ( "[Move] X:%s Y:%s Z:%s", szValueX, szValueY, szValueZ );

				CWnd *pWnd = (CWnd*) GetDlgItem(IDC_EDIT_TIME);
				pWnd->EnableWindow ( FALSE );
			}
			break;
		case 1:	//	MOB_ACTSTAY
			{
				char szValue[32];
				_gcvt ( MobAction.fLife, 7, szValue );

				str.Format ( "[Rest] %s", szValue );

				CWnd *pWnd = (CWnd*) GetDlgItem(IDC_EDIT_TIME);
				pWnd->EnableWindow ( TRUE );
			}
			break;
		default:
			{
			}
			break;
		}

		int nIndex = m_ctrlSchedule.AddString ( str.GetString () );
		m_ctrlSchedule.SetItemData ( nIndex, (DWORD_PTR)&pHead->Data );               
	
		pHead = pHead->pNext;
	}
}

void CMobSchedulePage::UpdatePage ()
{
	if ( !m_bInit )		return;
}

void CMobSchedulePage::OnLbnDblclkListSchedule()
{
	int nIndex = m_ctrlSchedule.GetCurSel ();
	if ( nIndex == -1 )		return;

	DWORD_PTR dwPtr = m_ctrlSchedule.GetItemData ( nIndex );
	if ( dwPtr == LB_ERR )
	{
		MessageBox ( "Error! 0xFFFFFFFF" );
		return;
	}

	SMOBACTION *pMobAction = (SMOBACTION*)dwPtr;
	DxViewPort::GetInstance().CameraJump ( pMobAction->vPos );
}

void CMobSchedulePage::OnBnClickedButtonMove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnLbnDblclkListSchedule();
}
