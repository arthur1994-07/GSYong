// CollisionMapDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "CollisionMapDlg.h"
#include <process.h>

// CCollisionMapDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CCollisionMapDlg, CDialog)
CCollisionMapDlg::CCollisionMapDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCollisionMapDlg::IDD, pParent)
{
}

CCollisionMapDlg::~CCollisionMapDlg()
{
}

void CCollisionMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCollisionMapDlg, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CCollisionMapDlg �޽��� ó�����Դϴ�.

BOOL CCollisionMapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.	
	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView (); 
	CCollisionMap* const pCollisionMap = pView->GetSetLandMan()->GetCollisionMap();

	ExportProgress::CurPos = 0;
	ExportProgress::EndPos = 1;

	SetTimer ( 0, 100, NULL );
	//m_hThread = CreateThread ( NULL, 0, pCollisionMap->ImportThread, pCollisionMap, 0, &m_ThreadID );
	m_hThread = (HANDLE) ::_beginthreadex(
								NULL,
								0,
								pCollisionMap->ImportThread,
								pCollisionMap,
								0,
								(unsigned int*) &m_ThreadID );

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CCollisionMapDlg::OnTimer(UINT nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CProgressCtrl* const pProgressBar = (CProgressCtrl*) GetDlgItem ( IDC_PROGRESS_COLLISION );

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView (); 
	CCollisionMap* const pCollisionMap = pView->GetSetLandMan()->GetCollisionMap();

	DWORD MaxCount = ExportProgress::EndPos;
	DWORD CurCount = ExportProgress::CurPos;
    int	  percent = ( CurCount * 100 ) / MaxCount;
	
	pProgressBar->SetPos ( percent );

	CWnd* pWnd = GetDlgItem ( IDC_STATIC_PERCENT );
	CString str;
	str.Format ( "%03d%%", percent );
	pWnd->SetWindowText ( str );

	if ( WaitForSingleObject ( m_hThread, 0 ) == WAIT_OBJECT_0 )
	{
		CloseHandle ( m_hThread );
		KillTimer ( 0 );
		OnOK ();
	}

	CDialog::OnTimer(nIDEvent);
}