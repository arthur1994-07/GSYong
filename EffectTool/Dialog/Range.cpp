// Range.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"
#include "../EffectToolDoc.h"
#include "../EffectToolView.h"

#include "Range.h"


// CRange ��ȭ �����Դϴ�.

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


// CRange �޽��� ó�����Դϴ�.

BOOL CRange::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
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
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CRange::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton* pButton = (CButton*)GetDlgItem( IDC_CHECK_RANDOM_DIRECT );
	m_bRandomDirect = pButton->GetCheck();
}



