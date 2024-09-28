// Dialog\AmbientDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../CharEdit.h"
#include "AmbientDlg.h"


// CAmbientDlg ��ȭ �����Դϴ�.

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


// CAmbientDlg �޽��� ó�����Դϴ�.

BOOL CAmbientDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
    CString Str;
	Str.Format ( "%d", m_dwAmbient );
    CWnd* pWnd = GetDlgItem( IDC_EDIT_AMBIENT );
    pWnd->SetWindowText( Str );

    CButton* pButton = static_cast<CButton*>( GetDlgItem( IDC_CHECK_AMBIENT_TEST ) );
    pButton->SetCheck( m_bAmbientTest );

    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CAmbientDlg::OnBnClickedOk()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    CString Str;
    CWnd* pWnd = GetDlgItem( IDC_EDIT_AMBIENT );
    pWnd->GetWindowText( Str );
    m_dwAmbient = atoi ( Str.GetString() );

    CButton* pButton = static_cast<CButton*>( GetDlgItem( IDC_CHECK_AMBIENT_TEST ) );
    m_bAmbientTest = pButton->GetCheck();

    OnOK();
}
