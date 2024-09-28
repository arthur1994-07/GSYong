// CollisionMapDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WorldEdit.h"
#include "MainFrm.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"
#include "CollisionMapDlg.h"
#include <process.h>

// CCollisionMapDlg 대화 상자입니다.

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


// CCollisionMapDlg 메시지 처리기입니다.

BOOL CCollisionMapDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.	
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
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CCollisionMapDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
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