// SetData.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "CommentTool.h"
#include "SetData.h"
#include ".\setdata.h"

#include "../MfcExLib/ExLibs/EtcFunction.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// SetData ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(SetDataDlg, CDialog)
SetDataDlg::SetDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SetDataDlg::IDD, pParent)
{
}

SetDataDlg::~SetDataDlg()
{
}

void SetDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SetDataDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// SetData �޽��� ó�����Դϴ�.

BOOL SetDataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	SetWin_Text ( this, IDC_INPUT_TEXT, m_strData.c_str() );

	GetDlgItem ( IDC_INPUT_TEXT )->SetFocus();


	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void SetDataDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString str = GetWin_Text( this, IDC_INPUT_TEXT );
	m_strData = str.GetBuffer( 0 );
	
	OnOK();
}
