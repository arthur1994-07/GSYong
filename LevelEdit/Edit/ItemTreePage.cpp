// ItemTreePage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../LevelEdit.h"
#include "./ItemTreePage.h"

#include "./LevelSheetWithTab.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"

// CItemTreePage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CItemTreePage, CPropertyPage)
CItemTreePage::CItemTreePage()
	: CPropertyPage(CItemTreePage::IDD)
{
	m_CalledPage = -1;
	m_bDlgInit = FALSE;	
	m_bNativeIDInit = FALSE;
	m_BtnCallNum = 0;
}

CItemTreePage::~CItemTreePage()
{
}

void CItemTreePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ITEM, m_ctrlItemTree);
}


BEGIN_MESSAGE_MAP(CItemTreePage, CPropertyPage)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
END_MESSAGE_MAP()


// CItemTreePage �޽��� ó�����Դϴ�.

BOOL CItemTreePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_bDlgInit = TRUE;
	UpdateTreeItem ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CItemTreePage::UpdateTreeItem ()
{
	if ( !m_bDlgInit )
	{
		return FALSE;
	}

	m_ctrlItemTree.DeleteAllItems ();

    HTREEITEM hSelectItem = NULL;
	for ( WORD MID = 0; MID < GLItemMan::MAX_MID; MID++ )
	{
		HTREEITEM hMainItem = NULL;
		HTREEITEM hSubItem = NULL;

		for ( WORD SID = 0; SID < GLItemMan::MAX_SID; SID++ )
		{
			SNATIVEID sCUR_NID(MID,SID);
			const SITEM* pItem = GLogicData::GetInstance().GetItem ( sCUR_NID );

			if ( pItem )
			{
				if ( !hMainItem )
				{
					hMainItem = m_ctrlItemTree.InsertItem ( pItem->GetName() );
					m_ctrlItemTree.SetItemData ( hMainItem, (DWORD_PTR)pItem );					

					if ( m_bNativeIDInit && m_NativeID == sCUR_NID )
					{
						hSelectItem = hMainItem;
					}
				}
				else
				{
					hSubItem = m_ctrlItemTree.InsertItem ( pItem->GetName(), hMainItem );
					m_ctrlItemTree.SetItemData ( hSubItem, (DWORD_PTR)pItem );

					if ( m_bNativeIDInit && m_NativeID == sCUR_NID )
					{
						hSelectItem = hSubItem;
					}
				}
			}
		}
	}

	m_ctrlItemTree.SelectItem ( hSelectItem );

	return TRUE;
}

void CItemTreePage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyPage::OnShowWindow(bShow, nStatus);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	if ( bShow )
	{
		UpdateTreeItem ();
	}
}

void CItemTreePage::CleanAllItem ()
{
	if ( !m_bDlgInit )
	{
		return ;
	}

	m_ctrlItemTree.DeleteAllItems ();
}

void CItemTreePage::SetPropertyData ( int CalledPage, SNATIVEID NativeID, int iBtnNum )
{
	m_CalledPage = CalledPage;
	m_NativeID = NativeID;

	m_bNativeIDInit = TRUE;
	m_BtnCallNum    = iBtnNum;
}

void CItemTreePage::OnBnClickedButtonCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->SetActivePage ( m_CalledPage );
	m_bNativeIDInit = FALSE;
}

void CItemTreePage::OnBnClickedButtonOk()
{
	SNATIVEID	NativeID;

	HTREEITEM hTreeItem = m_ctrlItemTree.GetSelectedItem ();
	//PCROWDATA pItem = (PCROWDATA)m_ctrlItemTree.GetItemData ( hTreeItem );	
	SITEM* pItem = (SITEM*)m_ctrlItemTree.GetItemData ( hTreeItem );	

	if ( m_CalledPage == REQUIREINDEX )
	{
		//SNATIVEID NativeID = pItem->sNativeID;
		SNATIVEID NativeID = pItem->sBasicOp.sNativeID;
		m_pSheetTab->m_RequirePage.SetReqItem ( NativeID, m_BtnCallNum );
	}

	m_pSheetTab->SetActivePage ( m_CalledPage );
	m_bNativeIDInit = FALSE;
}
