// ProgressBarDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include <process.h>

#include "MainFrm.h"
#include "WorldEdit.h"
#include "WorldEditDoc.h"
#include "WorldEditView.h"

#include "../EngineLib/DxLand/ExportXFile/NSExportXFile.h"

#include "ProgressBarDlg.h"


// CProgressBarDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CProgressBarDlg, CDialog)

CProgressBarDlg::CProgressBarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressBarDlg::IDD, pParent)
{

}

CProgressBarDlg::~CProgressBarDlg()
{
}

void CProgressBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
}


BEGIN_MESSAGE_MAP(CProgressBarDlg, CDialog)
	ON_WM_TIMER()
END_MESSAGE_MAP()

static	const	int	ID_EXPORT			=	1;

// CProgressBarDlg 메시지 처리기입니다.

unsigned int WINAPI ExportXFile( LPVOID pParam )
{
	HRESULT hr = S_OK;

	SParam	*pExportParam = (SParam *)pParam;

	CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
	CWorldEditView* pView = (CWorldEditView *) pFrame->GetActiveView ();

	//	Note : 디버그 절대 경로 가져오기
	CWorldEditApp* pApp = (CWorldEditApp*) AfxGetApp();
	const SUBPATH* pPath = pApp->GetSubPath();

	NSExportXFile::ExportXFile( pView->GetD3DDevice(), pExportParam->m_strExportName.c_str(), pView->GetLandMan() );

	return 0;
}

BOOL CProgressBarDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_ctrlProgress.SetRange ( 0, 100 );
	m_ctrlProgress.SetPos ( 0 );

	//	Note	:	0 나누기 방지
	ExportProgress::EndPos = 4;
	ExportProgress::CurPos = 0;

	::_beginthreadex(
		NULL,
		0,
		ExportXFile,
		&m_sParam,
		0,
		NULL );

	SetTimer ( ID_EXPORT, 5, NULL );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CProgressBarDlg::Create( const TSTRING& strName )
{
	m_sParam.m_strExportName = strName;
}

void CProgressBarDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	switch ( nIDEvent )
	{
	case ID_EXPORT:
		{			
			if ( ExportProgress::EndPos <= 0 )
			{
				ExportProgress::EndPos = 1;
			}
			int Pos = (int)( ( ExportProgress::CurPos * 100 ) / ExportProgress::EndPos );
			m_ctrlProgress.SetPos ( Pos );

			if ( ExportProgress::CurPos+1 >= ExportProgress::EndPos  )
			{
				KillTimer ( ID_EXPORT );
				EndDialog ( 0 );
			}

			break;
		}
	}

	CDialog::OnTimer(nIDEvent);
}

BOOL CProgressBarDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	switch(pMsg->message)
	{
	case WM_KEYDOWN:
		{
			switch(pMsg->wParam)
			{
			case VK_ESCAPE:
			case VK_RETURN:
				return TRUE;
			}
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}
