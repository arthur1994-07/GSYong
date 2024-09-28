// AllItemView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../LevelEdit.h"
#include "./LevelSheetWithTab.h"
#include "./CrowTree.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"

// CCrowTree ��ȭ �����Դϴ�.

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CCrowTree, CPropertyPage)
CCrowTree::CCrowTree()
	: CPropertyPage(CCrowTree::IDD)
{
	m_CalledPage = -1;
	m_bDlgInit = FALSE;	
	m_bNativeIDInit = FALSE;
}

CCrowTree::~CCrowTree()
{	
}

void CCrowTree::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ITEM, m_ctrlItemTree);
}


BEGIN_MESSAGE_MAP(CCrowTree, CPropertyPage)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// CCrowTree �޽��� ó�����Դϴ�.

BOOL CCrowTree::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_bDlgInit = TRUE;
	UpdateTreeItem ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

BOOL CCrowTree::UpdateTreeItem ()
{
	if ( !m_bDlgInit )
	{
		return FALSE;
	}

	m_ctrlItemTree.DeleteAllItems ();

    HTREEITEM hSelectItem = NULL;
	for ( int MID = 0; MID < GLCrowDataMan::MAX_CROW_MID; MID++ )
	{
		HTREEITEM hMainItem = NULL;
		HTREEITEM hSubItem = NULL;

		for ( int SID = 0; SID < GLCrowDataMan::MAX_CROW_SID; SID++ )
		{        		
			PCROWDATA pItem = GLCrowDataMan::GetInstance().GetCrowData ( MID, SID );

			if ( pItem )
			{
				if ( !hMainItem )
				{
					hMainItem = m_ctrlItemTree.InsertItem ( pItem->GetName() );
					m_ctrlItemTree.SetItemData ( hMainItem, (DWORD_PTR)pItem );					

					if ( m_bNativeIDInit &&
						( m_NativeID.wMainID == WORD(MID) && m_NativeID.wSubID == WORD(SID) ) )
					{
						hSelectItem = hMainItem;
					}
				}
				else
				{
					hSubItem = m_ctrlItemTree.InsertItem ( pItem->GetName(), hMainItem );
					m_ctrlItemTree.SetItemData ( hSubItem, (DWORD_PTR)pItem );

					if ( m_bNativeIDInit &&
						( m_NativeID.wMainID == WORD(MID) && m_NativeID.wSubID == WORD(SID) ) )
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

void CCrowTree::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyPage::OnShowWindow(bShow, nStatus);

	if ( bShow )
	{
		UpdateTreeItem ();
	}
}

void CCrowTree::CleanAllItem ()
{
	if ( !m_bDlgInit )	return;

	m_ctrlItemTree.DeleteAllItems ();
}

void CCrowTree::OnBnClickedButtonOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	SNATIVEID	NativeID;

	HTREEITEM hTreeItem = m_ctrlItemTree.GetSelectedItem ();
	PCROWDATA pItem = (PCROWDATA)m_ctrlItemTree.GetItemData ( hTreeItem );	

	if ( m_CalledPage == MOBSETPAGEINDEX )
	{
		SNATIVEID NativeID = pItem->sNativeID;
		m_pSheetTab->m_MobSetPage.SetNewID ( NativeID.wMainID, NativeID.wSubID );
	}

	m_pSheetTab->SetActivePage ( m_CalledPage );
	m_bNativeIDInit = FALSE;
}

void CCrowTree::OnBnClickedButtonCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_pSheetTab->SetActivePage ( m_CalledPage );
	m_bNativeIDInit = FALSE;
}

void CCrowTree::SetPropertyData ( int CalledPage, SNATIVEID NativeID )
{
	m_CalledPage = CalledPage;
	m_NativeID = NativeID;

	m_bNativeIDInit = TRUE;
}