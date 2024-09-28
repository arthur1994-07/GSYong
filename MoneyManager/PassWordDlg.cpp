// PassWordDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "./COdbcManager.h"

#include "./MoneyManager.h"
#include "./PassWordDlg.h"

//				  dls09871234
#define PASSWORD "dls)(*&!@#$"

// CPassWordDlg 대화 상자입니다.

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


// CPassWordDlg 메시지 처리기입니다.

BOOL CPassWordDlg::CheckPassWord()
{
	char szTempChar[256];
	strcpy( szTempChar, GetWin_Text( this, IDC_EDIT_PASSWORD ).GetString() );

	if( strcmp( szTempChar, PASSWORD ) != 0 ) return FALSE;



	
	return TRUE;

}
void CPassWordDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	GetDlgItem(IDC_EDIT_PASSWORD)->SetFocus();
	m_prog_Load.SetRange( 0, 100 );
	m_prog_Load.SetPos( 0 );

	SetTimer( 1, 200, NULL );
	m_dwCreateTime = 0;

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CPassWordDlg::OnTimer(UINT nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_dwCreateTime++;
	m_prog_Load.SetPos ( m_dwCreateTime );

	if( m_dwCreateTime == 100 )
	{
		MessageBox( "Timeout!", "Warning" );
		OnCancel();
	}

	CDialog::OnTimer(nIDEvent);
}
