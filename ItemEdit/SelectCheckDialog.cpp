// SelectDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ItemEdit.h"
#include "SelectCheckDialog.h"
#include "../RanLogic/Item/GLItemMan.h"

// CSelectCheckDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSelectCheckDlg, CDialog)
CSelectCheckDlg::CSelectCheckDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectCheckDlg::IDD, pParent)
	, m_nSelect( -1 )
	, m_dwData ( 0 )
	, m_dwCheckList( 0 )
{
}

CSelectCheckDlg::~CSelectCheckDlg()
{
}

void CSelectCheckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_CHECK_BOX    , m_SelectList);
}


BEGIN_MESSAGE_MAP(CSelectCheckDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_LBN_SELCHANGE(IDC_LIST_CHECK_BOX, OnSelchangeList)
	ON_CLBN_CHKCHANGE(IDC_LIST_CHECK_BOX, OnCheckchangeList)
END_MESSAGE_MAP()

BOOL CSelectCheckDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_SelectList.ResetContent();
	m_SelectList.SetCheckStyle(BS_AUTOCHECKBOX);

	for( int i = 0; i < ITEM_DRUG_ER_SIZE; ++i )
	{
		DWORD dwBit = 1 << i;

		bool bResult = (m_dwCheckList & dwBit) > 0 ? 1 : 0;

		m_SelectList.SetCheck( m_SelectList.AddString(COMMENT::ITEMDRUG_ER[i].c_str()), bResult );
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CSelectCheckDlg::OnOK()
{
	CDialog::OnOK();
}

void CSelectCheckDlg::OnCheckchangeList()
{
	int iIndex = m_SelectList.GetCurSel();

	DWORD dwBit = 1 << iIndex;

	if( m_SelectList.GetCheck(iIndex) > 0 )
	{	
		m_dwCheckList |= dwBit;
	}
	else
	{
		m_dwCheckList ^= dwBit;
	}
}

void CSelectCheckDlg::OnSelchangeList()
{


}
void CSelectCheckDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}