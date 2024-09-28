// IDCheckDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../RanLogic/Item/GLItemMan.h"
#include "../RanLogic/Item/GLItem.h"

#include "ItemDataMergerTool.h"
#include "../MfcExLib/ExLibs/EtcFunction.h"

#include "IDCheckDlg.h"

// CIDCheckDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CIDCheckDlg, CDialog)
CIDCheckDlg::CIDCheckDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIDCheckDlg::IDD, pParent)
{
}

CIDCheckDlg::CIDCheckDlg(const SNATIVEID sNativeID, const CString strItemName, GLItemMan *pItemMan )
	: CDialog(CIDCheckDlg::IDD, NULL)
	, m_sItemID(sNativeID)
	, m_pItemMan(pItemMan)
	, m_strItemName(strItemName)
{
}

CIDCheckDlg::~CIDCheckDlg()
{
}

void CIDCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CIDCheckDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_SEARCH_BUTTON, OnBnClickedSearchButton)
END_MESSAGE_MAP()


// CIDCheckDlg �޽��� ó�����Դϴ�.

BOOL CIDCheckDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

	SetWin_Num_int( this, IDC_MID_EDIT, m_sItemID.wMainID ); 
	SetWin_Num_int( this, IDC_SID_EDIT, m_sItemID.wSubID );

	SetWin_Text( this, IDC_NAME_STATIC, m_strItemName.GetString() );


	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CIDCheckDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_sItemID.wMainID = GetWin_Num_int( this, IDC_MID_EDIT ); 
	m_sItemID.wSubID  = GetWin_Num_int( this, IDC_SID_EDIT );
	OnOK();
}

void CIDCheckDlg::OnBnClickedSearchButton()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_sItemID.wMainID = GetWin_Num_int( this, IDC_MID_EDIT ); 
	m_sItemID.wSubID  = GetWin_Num_int( this, IDC_SID_EDIT );
	SITEM *pItem = m_pItemMan->GetItem( m_sItemID );
	if( pItem )
	{
		CString strTempChar;
		strTempChar.Format( "%s �������� �̹� ����ϰ� �ִ� ���̵� �Դϴ�.", pItem->GetName() );
		SetWin_Text( this, IDC_MSG_STATIC, strTempChar.GetString() );
	}else{
		SetWin_Text( this, IDC_MSG_STATIC, "����ϰ� ���� ���� ���̵� �Դϴ�." );		
	}

}
