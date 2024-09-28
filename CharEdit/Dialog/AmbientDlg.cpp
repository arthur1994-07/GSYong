// Dialog\AmbientDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../CharEdit.h"
#include "AmbientDlg.h"


// CAmbientDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAmbientDlg, CDialog)

CAmbientDlg::CAmbientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAmbientDlg::IDD, pParent)
    , m_bAmbientTest(FALSE)
    , m_dwAmbient(0)
{

}

CAmbientDlg::~CAmbientDlg()
{
}

void CAmbientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAmbientDlg, CDialog)
    ON_BN_CLICKED(IDOK, &CAmbientDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAmbientDlg 메시지 처리기입니다.

BOOL CAmbientDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    CString Str;
	Str.Format ( "%d", m_dwAmbient );
    CWnd* pWnd = GetDlgItem( IDC_EDIT_AMBIENT );
    pWnd->SetWindowText( Str );

    CButton* pButton = static_cast<CButton*>( GetDlgItem( IDC_CHECK_AMBIENT_TEST ) );
    pButton->SetCheck( m_bAmbientTest );

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CAmbientDlg::OnBnClickedOk()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    CString Str;
    CWnd* pWnd = GetDlgItem( IDC_EDIT_AMBIENT );
    pWnd->GetWindowText( Str );
    m_dwAmbient = atoi ( Str.GetString() );

    CButton* pButton = static_cast<CButton*>( GetDlgItem( IDC_CHECK_AMBIENT_TEST ) );
    m_bAmbientTest = pButton->GetCheck();

    OnOK();
}
