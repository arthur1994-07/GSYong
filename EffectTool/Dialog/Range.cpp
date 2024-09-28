// Range.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"
#include "../EffectToolDoc.h"
#include "../EffectToolView.h"

#include "Range.h"


// CRange 대화 상자입니다.

IMPLEMENT_DYNAMIC(CRange, CDialog)
CRange::CRange(CWnd* pParent /*=NULL*/)
	: CDialog(CRange::IDD, pParent),
	m_fPlayTimeS(0.f),
	m_fPlayTimeE(0.f),
	m_fSleepTimeS(0.f),
	m_fSleepTimeE(0.f)
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( FALSE );
}

CRange::~CRange()
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( TRUE );
}

void CRange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRange, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK_RANDOM_DIRECT, OnBnClickedCheckRandomDirect)
END_MESSAGE_MAP()


// CRange 메시지 처리기입니다.

BOOL CRange::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CWnd* pWnd;
	CString Str;

	Str.Format ( "%3.2f", m_fPlayTimeS );
	pWnd = GetDlgItem( IDC_EDIT_PLAY_S );
	pWnd->SetWindowText( Str );

	Str.Format ( "%3.2f", m_fPlayTimeE );
	pWnd = GetDlgItem( IDC_EDIT_PLAY_E );
	pWnd->SetWindowText( Str );

	Str.Format ( "%3.2f", m_fSleepTimeS );
	pWnd = GetDlgItem( IDC_EDIT_SLEEP_S );
	pWnd->SetWindowText( Str );

	Str.Format ( "%3.2f", m_fSleepTimeE );
	pWnd = GetDlgItem( IDC_EDIT_SLEEP_E );
	pWnd->SetWindowText( Str );

	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_RANDOM_DIRECT );
	pButton->SetCheck( m_bRandomDirect );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CRange::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CWnd* pWnd;
	CString Str;

	pWnd = GetDlgItem( IDC_EDIT_PLAY_S );
	pWnd->GetWindowText( Str );
	m_fPlayTimeS = (float)atof( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_PLAY_E );
	pWnd->GetWindowText( Str );
	m_fPlayTimeE = (float)atof( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_SLEEP_S );
	pWnd->GetWindowText( Str );
	m_fSleepTimeS = (float)atof( Str.GetString() );

	pWnd = GetDlgItem( IDC_EDIT_SLEEP_E );
	pWnd->GetWindowText( Str );
	m_fSleepTimeE = (float)atof( Str.GetString() );

	OnOK();
}

void CRange::OnBnClickedCheckRandomDirect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_RANDOM_DIRECT );
	m_bRandomDirect = pButton->GetCheck();
}



