// InputDataDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "ItemEdit.h"
#include "InputDataDlg.h"
#include ".\inputdatadlg.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"


// CInputDataDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CInputDataDlg, CDialog)
CInputDataDlg::CInputDataDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInputDataDlg::IDD, pParent)
	, m_bNum ( false )
	, m_dwNum ( 1 )
	, m_fRate ( 0.0f )
	, m_wMainID( -1 )
	, m_wSubID( -1 )
{
}

CInputDataDlg::~CInputDataDlg()
{
}

void CInputDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CInputDataDlg, CDialog)
END_MESSAGE_MAP()


// CInputDataDlg �޽��� ó�����Դϴ�.

BOOL CInputDataDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	CString strTemp;

	if ( m_bNum )
	{
		strTemp.Format( "%d/%d %s Num", m_wMainID, m_wSubID, m_strItemName );
		SetWin_Text( this, IDC_ITEM_STATIC, strTemp );
		SetWin_Num_int ( this, IDC_EDIT_NUM, m_dwNum );
	}
	else
	{
		strTemp.Format( "%d/%d %s Rate", m_wMainID, m_wSubID, m_strItemName );
		SetWin_Text( this, IDC_ITEM_STATIC, strTemp );
		SetWin_Num_float ( this, IDC_EDIT_NUM, m_fRate );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CInputDataDlg::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

	

	if ( m_bNum )
	{
		m_dwNum = GetWin_Num_int ( this, IDC_EDIT_NUM );
		if ( m_dwNum <= 0 ) 
		{
			MessageBox ( " Num Error " );
			return;
		}

	}
	else
	{
		m_fRate = GetWin_Num_float ( this, IDC_EDIT_NUM );		
		if ( m_fRate <= 0.0f || m_fRate > 100.0f ) 
		{
			MessageBox ( " Rate Error " );
			return;
		}
		m_dwNum = static_cast<DWORD>( m_fRate * 100 );
	}

	CDialog::OnOK();
}

