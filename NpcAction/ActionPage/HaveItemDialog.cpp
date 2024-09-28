// HaveItemDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../NpcAction.h"
#include "./HaveItemDialog.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLStringTable.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "./FindItem.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CHaveItemDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CHaveItemDialog, CDialog)
CHaveItemDialog::CHaveItemDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CHaveItemDialog::IDD, pParent),
	m_dwItemID(UINT_MAX)
	, m_nNum ( 1 ) 
{
}

CHaveItemDialog::~CHaveItemDialog()
{
}

void CHaveItemDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHaveItemDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnBnClickedButtonFind)
END_MESSAGE_MAP()


// CHaveItemDialog �޽��� ó�����Դϴ�.

void CHaveItemDialog::OnBnClickedButtonCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_dwItemID = UINT_MAX;
	m_nNum = 1;
	OnCancel ();
}

void CHaveItemDialog::OnBnClickedButtonOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_bConfirm = TRUE;
	CListBox* pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ITEM );
	int nIndex = pListBox->GetCurSel ();
	if ( nIndex==LB_ERR )	return;

	m_nNum = GetDlgItemInt( IDC_EDIT_NUM );
	if ( m_nNum < 1)
	{
		MessageBox( "Input Num" );
		return;
	}

	m_dwItemID = (DWORD) pListBox->GetItemData ( nIndex );

	OnOK ();
}

void CHaveItemDialog::OnBnClickedButtonFind()
{	
	int nMainID, nSubID;

	nMainID = GetWin_Num_int( this, IDC_EDIT_FINDMID );
	nSubID = GetWin_Num_int( this, IDC_EDIT_FINDSID );

	CListBox* pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ITEM );
	int nSize = pListBox->GetCount();

	SNATIVEID sNativeID;

	for ( int i = 0; i < nSize; ++i )
	{
		sNativeID = (DWORD) pListBox->GetItemData( i );

		if ( nMainID == sNativeID.wMainID && nSubID == sNativeID.wSubID )
		{
			pListBox->SetCurSel( i );
			return;
		}
	}
}

BOOL CHaveItemDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_bConfirm = FALSE;	
	m_nNum = 1;

	SetDlgItemInt( IDC_EDIT_NUM, m_nNum );

	//	���� ������ �巯�� �����۵� ��� �ε��ϱ�
	CListBox* pListBox = (CListBox*) GetDlgItem ( IDC_LIST_ITEM );
	for ( WORD i=0; i<GLItemMan::MAX_MID; ++i )
	for ( WORD j=0; j<GLItemMan::MAX_SID; ++j )
	{
		const SITEM *pItem = GLogicData::GetInstance().GetItem ( i, j );
		if ( !pItem )	continue;

		CString strTemp;
		strTemp.Format ( "[%03d/%03d] %s", pItem->sBasicOp.sNativeID.wMainID,
			pItem->sBasicOp.sNativeID.wSubID, pItem->GetName() );
		
		int nIndex = pListBox->AddString ( strTemp.GetString() );
		pListBox->SetItemData ( nIndex, pItem->sBasicOp.sNativeID.dwID );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

DWORD CHaveItemDialog::GetItemID ()
{
	assert ( m_bConfirm && "������ ���� �Ѿ�ϴ�." );

	return m_dwItemID;
}
