// SequenceNumberDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"
#include "../EffectToolDoc.h"
#include "../EffectToolView.h"

#include "SequenceNumberDlg.h"


// CSequenceNumberDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSequenceNumberDlg, CDialog)
CSequenceNumberDlg::CSequenceNumberDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSequenceNumberDlg::IDD, pParent)
{
	m_nTexSize = 0;
	m_nNumber = 0;
	m_bRepeat = FALSE;

	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( FALSE );
}

CSequenceNumberDlg::~CSequenceNumberDlg()
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( TRUE );
}

void CSequenceNumberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSequenceNumberDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO_NUMBER16, OnBnClickedRadioNumber16)
	ON_BN_CLICKED(IDC_RADIO_NUMBER32, OnBnClickedRadioNumber32)
	ON_BN_CLICKED(IDC_RADIO_NUMBER64, OnBnClickedRadioNumber64)
	ON_BN_CLICKED(IDC_RADIO_TEXSIZE128, OnBnClickedRadioTexsize128)
	ON_BN_CLICKED(IDC_RADIO_TEXSIZE256, OnBnClickedRadioTexsize256)
	ON_BN_CLICKED(IDC_CHECK_REPEAT, OnBnClickedCheckRepeat)
END_MESSAGE_MAP()


// CSequenceNumberDlg 메시지 처리기입니다.
BOOL CSequenceNumberDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CButton* pButton128 = (CButton*) GetDlgItem( IDC_RADIO_TEXSIZE128 );
	CButton* pButton256 = (CButton*) GetDlgItem( IDC_RADIO_TEXSIZE256 );

	pButton128->SetCheck( FALSE );
	pButton256->SetCheck( FALSE );

	if( m_nTexSize == 0 )		pButton128->SetCheck( TRUE );
	else if( m_nTexSize == 1 )	pButton256->SetCheck( TRUE );


	CButton* pButton16 = (CButton*) GetDlgItem( IDC_RADIO_NUMBER16 );
	CButton* pButton32 = (CButton*) GetDlgItem( IDC_RADIO_NUMBER32 );
	CButton* pButton64 = (CButton*) GetDlgItem( IDC_RADIO_NUMBER64 );

	pButton16->SetCheck( FALSE );
	pButton32->SetCheck( FALSE );
	pButton64->SetCheck( FALSE );

	if( m_nNumber == 0 )		pButton16->SetCheck( TRUE );
	else if( m_nNumber == 1 )	pButton32->SetCheck( TRUE );
	else if( m_nNumber == 2 )	pButton64->SetCheck( TRUE );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CSequenceNumberDlg::OnBnClickedRadioNumber16()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nNumber = 0;
}

void CSequenceNumberDlg::OnBnClickedRadioNumber32()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nNumber = 1;
}

void CSequenceNumberDlg::OnBnClickedRadioNumber64()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nNumber = 2;
}

void CSequenceNumberDlg::OnBnClickedRadioTexsize128()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nTexSize = 0;
	ChangeNumber();
}

void CSequenceNumberDlg::OnBnClickedRadioTexsize256()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nTexSize = 1;
	ChangeNumber();
}

void CSequenceNumberDlg::ChangeNumber()
{
	CButton* pButton16 = (CButton*) GetDlgItem( IDC_RADIO_NUMBER16 );
	CButton* pButton32 = (CButton*) GetDlgItem( IDC_RADIO_NUMBER32 );
	CButton* pButton64 = (CButton*) GetDlgItem( IDC_RADIO_NUMBER64 );

	if( m_nTexSize==0 )
	{
		pButton16->SetWindowText( "16" );
		pButton32->SetWindowText( "32" );
		pButton64->SetWindowText( "64" );
	}
	else if( m_nTexSize==1 )
	{
		pButton16->SetWindowText( "16" );
		pButton32->SetWindowText( "25" );
		pButton64->SetWindowText( "36" );
	}
}
void CSequenceNumberDlg::OnBnClickedCheckRepeat()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bRepeat = (m_bRepeat) ? FALSE : TRUE;
}


