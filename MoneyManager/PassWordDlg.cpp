// PassWordDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "./COdbcManager.h"

#include "./MoneyManager.h"
#include "./PassWordDlg.h"

//				  dls09871234
#define PASSWORD "dls)(*&!@#$"

// CPassWordDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CPassWordDlg, CDialog)
CPassWordDlg::CPassWordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPassWordDlg::IDD, pParent)
{
}

CPassWordDlg::~CPassWordDlg()
{
}

void CPassWordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_LOAD, m_prog_Load);
}


BEGIN_MESSAGE_MAP(CPassWordDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_PAINT()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CPassWordDlg �޽��� ó�����Դϴ�.

BOOL CPassWordDlg::CheckPassWord()
{
	char szTempChar[256];
	strcpy( szTempChar, GetWin_Text( this, IDC_EDIT_PASSWORD ).GetString() );

	if( strcmp( szTempChar, PASSWORD ) != 0 ) return FALSE;



	
	return TRUE;

}
void CPassWordDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if( CheckPassWord() ) 
	{
		SetWindowText( "Loading....." );
		
		m_prog_Load.SetPos( 0 );

		m_setStorageUserNum.clear();
		int nAllUser = COdbcManager::GetInstance()->GetChaSeq();
		int nUserNum;
		for( int i = 0; i <= nAllUser; i++ )
		{
			nUserNum = COdbcManager::GetInstance()->GetSeqUserNum( i, GAMEMONEY_TOOLVER );		
			m_setStorageUserNum.insert(nUserNum);

			int Pos = ((i* 100)/(nAllUser)) ;
			m_prog_Load.SetPos ( Pos );

		}

		KillTimer(1);

			OnOK();
	}else 
	{
		MessageBox( "Wrong password.", "Warning" );
		return OnCancel();
	}
}

BOOL CPassWordDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
	m_prog_Load.SetRange( 0, 100 );
	m_prog_Load.SetPos( 0 );

	SetTimer( 1, 200, NULL );
	m_dwCreateTime = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CPassWordDlg::OnTimer(UINT nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	m_dwCreateTime++;
	m_prog_Load.SetPos ( m_dwCreateTime );

	if( m_dwCreateTime == 100 )
	{
		MessageBox( "Timeout!", "Warning" );
		OnCancel();
	}

	CDialog::OnTimer(nIDEvent);
}
