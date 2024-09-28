// SequenceNumberDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"
#include "../EffectToolDoc.h"
#include "../EffectToolView.h"

#include "SequenceNumberDlg.h"


// CSequenceNumberDlg ��ȭ �����Դϴ�.

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


// CSequenceNumberDlg �޽��� ó�����Դϴ�.
BOOL CSequenceNumberDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
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
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CSequenceNumberDlg::OnBnClickedRadioNumber16()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_nNumber = 0;
}

void CSequenceNumberDlg::OnBnClickedRadioNumber32()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_nNumber = 1;
}

void CSequenceNumberDlg::OnBnClickedRadioNumber64()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_nNumber = 2;
}

void CSequenceNumberDlg::OnBnClickedRadioTexsize128()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_nTexSize = 0;
	ChangeNumber();
}

void CSequenceNumberDlg::OnBnClickedRadioTexsize256()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_bRepeat = (m_bRepeat) ? FALSE : TRUE;
}


