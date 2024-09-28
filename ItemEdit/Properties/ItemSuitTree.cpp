// AllItemView.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include <vector>
#include "../ItemEdit.h"
#include "SheetWithTab.h"
#include "ItemSuitTree.h"

#include "../../RanLogic/GLStringTable.h"
#include "../../RanLogic/TooltipMan/GLItemTooltipMan.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../../EngineLib/GUInterface/UITextControl.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../SigmaCore/File/Excel/MinExcel.h"

// CItemSuitTree ��ȭ �����Դϴ�.

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC(CItemSuitTree, CPropertyPage)
CItemSuitTree::CItemSuitTree( LOGFONT logfont )
	: CPropertyPage(CItemSuitTree::IDD),
	m_pFont( NULL )
{
	m_bDlgInit = FALSE;
	m_pDummyItemNode = new CItemNode;	
	m_bDummyHasItem = FALSE;

	
	
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CItemSuitTree::~CItemSuitTree()
{
	SAFE_DELETE ( m_pDummyItemNode );
	SAFE_DELETE ( m_pFont );
}

void CItemSuitTree::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ITEM, m_ctrlItemTree);
}


BEGIN_MESSAGE_MAP(CItemSuitTree, CPropertyPage)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_ITEM, OnNMRclickTreeItem)
	ON_NOTIFY(NM_CLICK, IDC_TREE_ITEM, OnNMClickTreeItem)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_ITEM, OnNMDblclkTreeItem)
END_MESSAGE_MAP()


// CItemSuitTree �޽��� ó�����Դϴ�.

BOOL CItemSuitTree::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_bDlgInit = TRUE;
	UpdateTree ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CItemSuitTree::OnNMRclickTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;	

	POINT OriginPoint;
	GetCursorPos ( &OriginPoint );

	//	��Ŭ�� ���� ����
	ForceSelectOnRightClick ( OriginPoint );

	//	�޴� ���� & ���� �޴� ����
	RunSelectMenu ( GetSelectMenu ( OriginPoint ) );
}

void CItemSuitTree::OnNMDblclkTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();

	PITEMNODE pItemNode = NULL;
	if ( hItem )
	{
		pItemNode = (PITEMNODE)m_ctrlItemTree.GetItemData ( hItem );
		if( pItemNode ) 
		{
			SetSelectItem ( pItemNode );
			m_pSheetTab->ActiveItemParentPage ( ITEMSUITTREEPAGE, &pItemNode->m_sItem );
		}
	}	
}

void CItemSuitTree::RunSelectMenu ( int nSelect )
{
	HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();

	PITEMNODE pItemNode = NULL;
	if ( hItem )
	{
		pItemNode = (PITEMNODE)m_ctrlItemTree.GetItemData ( hItem );
	}

	switch ( nSelect )
	{
	case ID_MENUITEM_PARENTMODIFY:
		{
			if ( GetSelectItem () )
			{				
				m_pSheetTab->ActiveItemParentPage ( ITEMSUITTREEPAGE, &pItemNode->m_sItem );
			}			
		}
		break;
	case ID_MENUITEM_SUITMODIFY:
		{
			if ( GetSelectItem () )
			{
				m_pSheetTab->ActiveItemSuitPage ( ITEMSUITTREEPAGE, &pItemNode->m_sItem );
			}			
		}
		break;
	case ID_MENUITEM_ADD:
		{
			AddItem ();
		}
		break;
	case ID_MENUITEM_DEL:
		{
			if ( GetSelectItem () )
			{
				if ( MessageBox ( "Do you want to delete?", "Question", MB_YESNO ) == IDYES )
				{
					DelItem();
				}
			}
		}
		break;
	case ID_MENUITEM_COPY:
		{
			CopyItem ();
		}
		break;
	case ID_MENUITEM_PASTE_NEW:
		{
			PasteItemNew ();
		}
		break;
	case ID_MENUITEM_PASTE_VALUE:
		{
			PasteItemValue ();
		}
		break;
	};
}

int	 CItemSuitTree::GetSelectMenu ( POINT pt )
{
	//	<--	���� �۵� �޴� ����
	int pMenuID = 0;
	CMenu Menu;

	int SelectionMade = -1;
	//switch ( m_SelectMenu )
	//{
	//case 0:
	//	{
		VERIFY ( Menu.LoadMenu ( IDR_MENU_ITEMSUIT_POPUP ) );

		CMenu* pPopup = Menu.GetSubMenu ( pMenuID );
		ASSERT ( pPopup != NULL );	

		SetReadMode( pPopup );

		SelectionMade = pPopup->TrackPopupMenu (
			TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
			pt.x, pt.y, this );

		pPopup->DestroyMenu ();
	//	}
	//	break;
	//}
	//	-->	

	return SelectionMade;
}

bool CItemSuitTree::ExportExcelDetail_CheckItem (
	std::string& strSaveFileName,
	boost::function<void (int)> PercentCallbackFunc )
{
	// Ȯ���ڸ� ������;
	std::string ExcelFileName( "item" );
	sc::Excel Xls( ExcelFileName, strSaveFileName, 1, true );

	// ------------------------------------------------------------------------
	int Col = 0;
	int Row = 0;
	int SheetNum = 0;

	Xls.SetCell( SheetNum, Row, Col++, "ID" );
	Xls.SetCell( SheetNum, Row, Col++, "Name" );
	Xls.SetCell( SheetNum, Row, Col++, "Description" );
	// ------------------------------------------------------------------------

	int nPercentCount = 0;
	int nMaxItemCount = GLItemMan::MAX_MID*GLItemMan::MAX_SID;
	for ( int MID = 0; MID < GLItemMan::MAX_MID; MID++ )
	{
		for ( int SID = 0; SID < GLItemMan::MAX_SID; SID++ )
		{
			Col = 0;

			HTREEITEM hItem = m_TreeItemList[MID][SID];
			BOOL bChecked = m_ctrlItemTree.GetCheck( hItem );

			// Export;
			if ( bChecked )
			{
				SITEM* pItem = GLogicData::GetInstance().GetItem ( MID, SID );

				if ( pItem )
				{
					Row++;

					// ID;
					Xls.SetCell( SheetNum, Row, Col++, pItem->sBasicOp.strName );

					// Name;
					Xls.SetCell( SheetNum, Row, Col++, pItem->GetName() );

					// Info;
					Tooltip::GLItemTooltipMan::Instance()->SetItem( pItem );
					Tooltip::GLItemTooltipMan::Instance()->GenerateItemTooltip();
					Xls.SetCell( SheetNum, Row, Col++,
						Tooltip::GLItemTooltipMan::Instance()->GetItemDescription() );
					Tooltip::GLItemTooltipMan::Instance()->Clear();
				}
			}

			if ( PercentCallbackFunc )
			{
				nPercentCount++;
				if ( nPercentCount && nMaxItemCount )
				{
					int Percentage = static_cast<int>(
						(nPercentCount*100)/nMaxItemCount);
					PercentCallbackFunc(Percentage);
				}
			}
		}
	}

	if ( PercentCallbackFunc )
		PercentCallbackFunc(100);

	return true;
}

BOOL	CItemSuitTree::UpdateTree ()
{	
	if ( !m_bDlgInit )
	{
		return FALSE;
	}

	CleanAllItem ();

	for ( int MID = 0; MID < GLItemMan::MAX_MID; MID++ )
	{
		HTREEITEM hMainItem= NULL;
		HTREEITEM hSubItem = NULL;

		for ( int SID = 0; SID < GLItemMan::MAX_SID; SID++ )
		{        		
			SITEM* pItem = GLogicData::GetInstance().GetItem ( MID, SID );

			if ( pItem )
			{
				CString strItem = GetFormatName ( pItem );
				if ( !hMainItem )
				{
					hMainItem = m_ctrlItemTree.InsertItem ( strItem, m_TreeRoot );
					m_ctrlItemTree.SetItemData ( hMainItem, (DWORD_PTR)pItem );

					m_TreeItemList[MID][SID] = hMainItem;
				}
				else
				{
					hSubItem = m_ctrlItemTree.InsertItem ( strItem, hMainItem );
					m_ctrlItemTree.SetItemData ( hSubItem, (DWORD_PTR)pItem );

					m_TreeItemList[MID][SID] = hSubItem;
				}				
			}
		}
	}

	return TRUE;
}



void CItemSuitTree::CleanAllItem ()
{
	if ( !m_bDlgInit )
	{
		return ;
	}

	m_ctrlItemTree.DeleteAllItems ();
	m_TreeRoot = m_ctrlItemTree.InsertItem ( "Root Node" );
}

BOOL	CItemSuitTree::CopyItem ()
{
	HTREEITEM hMainItem = m_ctrlItemTree.GetSelectedItem ();
	if ( hMainItem && hMainItem != m_TreeRoot )
	{
		*m_pDummyItemNode = *((PITEMNODE)m_ctrlItemTree.GetItemData ( hMainItem ));
		m_bDummyHasItem = TRUE;
		return TRUE;
	}

	return FALSE;
}

BOOL	CItemSuitTree::PasteItemNew ()
{
	if ( !m_bDummyHasItem )
	{
		return FALSE;
	}

	WORD MID, SID;
	CItemNode sItemNode;
	HTREEITEM hMainItem = m_ctrlItemTree.GetSelectedItem ();
	if ( hMainItem && GetSelectItem () )	//	����� �߰���
	{
		MID = GetSelectItem()->m_sItem.sBasicOp.sNativeID.wMainID;
		SID = GLItemMan::GetInstance().FindFreeSID ( MID );
		if ( SID == ITEMID_NOTFOUND )
		{
			return FALSE;
		}

		sItemNode = *m_pDummyItemNode;
		sItemNode.m_sItem.sBasicOp.sNativeID.wMainID= MID;
		sItemNode.m_sItem.sBasicOp.sNativeID.wSubID = SID;

		GLItemMan::GetInstance().InsertItem ( MID, SID, &sItemNode );
		SITEM *pITEM = GLogicData::GetInstance().GetItem ( MID, SID );

		HTREEITEM hParentItem = m_ctrlItemTree.GetParentItem ( hMainItem );
		if ( hParentItem != m_TreeRoot )
		{
			UpdateItem (pITEM, hParentItem );
		}
		else
		{
			UpdateItem ( pITEM, hMainItem );
		}
	}
	else	//	�������� �߰���
	{
		MID = GLItemMan::GetInstance().FindFreeMID ();
		if ( MID == ITEMID_NOTFOUND )
		{
			return FALSE;
		}
		SID = GLItemMan::GetInstance().FindFreeSID ( MID );
		if ( SID == ITEMID_NOTFOUND )
		{
			return FALSE;
		}
		
		CItemNode sItemNode;
		sItemNode = *m_pDummyItemNode;
		sItemNode.m_sItem.sBasicOp.sNativeID.wMainID = MID;
		sItemNode.m_sItem.sBasicOp.sNativeID.wSubID = SID;

		GLItemMan::GetInstance().InsertItem ( MID, SID, &sItemNode );
		SITEM *pITEM = GLogicData::GetInstance().GetItem ( MID, SID );

		UpdateItem ( pITEM, NULL );
	}

	char szNewString[ITEM_SZNAME] = {0};
	sprintf ( szNewString, "IN_%03d_%03d", MID, SID );
	sItemNode.m_sItem.sBasicOp.strName = szNewString;	
	GLStringTable::GetInstance().InsertString(sItemNode.m_sItem.sBasicOp.strName
		, static_cast<std::string>(GLStringTable::GetInstance().GetItemString(m_pDummyItemNode->m_sItem.sBasicOp.strName))
		, GLStringTable::ITEM);

	sprintf ( szNewString, "ID_%03d_%03d", MID, SID );
	sItemNode.m_sItem.sBasicOp.strComment = szNewString;
	GLStringTable::GetInstance().InsertString(sItemNode.m_sItem.sBasicOp.strComment
		, static_cast<std::string>(GLStringTable::GetInstance().GetItemString(m_pDummyItemNode->m_sItem.sBasicOp.strComment))
		, GLStringTable::ITEM);
	return TRUE;
}

BOOL	CItemSuitTree::PasteItemValue ()
{
	if ( !m_bDummyHasItem )
	{
		return FALSE;
	}

	HTREEITEM hMainItem = m_ctrlItemTree.GetSelectedItem ();
	if ( hMainItem && GetSelectItem () )
	{		
		WORD MID = GetSelectItem()->m_sItem.sBasicOp.sNativeID.wMainID;
		WORD SID = GetSelectItem()->m_sItem.sBasicOp.sNativeID.wSubID;

		*(GetSelectItem()) = *m_pDummyItemNode;

		GetSelectItem()->m_sItem.sBasicOp.sNativeID.wMainID= MID;
		GetSelectItem()->m_sItem.sBasicOp.sNativeID.wSubID = SID;

		m_ctrlItemTree.SetItemText ( hMainItem, GetFormatName ( &GetSelectItem()->m_sItem ) );
		
		return TRUE;
	}

	return FALSE;
}

BOOL CItemSuitTree::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	switch ( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			if ( pMsg->wParam == VK_RETURN )
			{
				HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();
				if ( !hItem ) return TRUE;

				PITEMNODE pItemNode = (PITEMNODE)m_ctrlItemTree.GetItemData ( hItem );
				if ( !pItemNode )
				{
					return TRUE;
				}

				m_pSheetTab->ActiveItemParentPage ( ITEMSUITTREEPAGE, &pItemNode->m_sItem );
			}

			if ( ::GetKeyState ( VK_CONTROL ) < 0 )
			{
				if ( pMsg->wParam == 'C' )
				{
					RunSelectMenu ( ID_MENUITEM_COPY );
				}
			}

			if ( ::GetKeyState ( VK_CONTROL ) < 0 )
			{
				if ( pMsg->wParam == 'V' )
				{
					HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();
					PITEMNODE pItemNode = (PITEMNODE)m_ctrlItemTree.GetItemData ( hItem );
					SetSelectItem ( pItemNode );

					RunSelectMenu ( ID_MENUITEM_PASTE_NEW );
				}
			}

			if ( ::GetKeyState ( VK_CONTROL ) < 0 )
			{
				if ( pMsg->wParam == 'B' )
				{
					HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();
					PITEMNODE pItemNode = (PITEMNODE)m_ctrlItemTree.GetItemData ( hItem );
					SetSelectItem ( pItemNode );

					RunSelectMenu ( ID_MENUITEM_PASTE_VALUE );
				}
			}

		}
		break;
	}

	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CItemSuitTree::SetSelectItem ( PITEMNODE pItemNode )
{
	m_pItemNode = pItemNode;
}

PITEMNODE CItemSuitTree::GetSelectItem ()
{
	return m_pItemNode;
}

void CItemSuitTree::CheckChildItems ( HTREEITEM hItem )
{
	HTREEITEM hChildItem = m_ctrlItemTree.GetChildItem( hItem );

	while ( hChildItem != NULL )
	{
		m_ctrlItemTree.SetCheck( hChildItem, TRUE );
		if ( m_ctrlItemTree.ItemHasChildren( hChildItem ) )
		{
			CheckChildItems( hChildItem );
		}

		hChildItem = m_ctrlItemTree.GetNextItem( hChildItem, TVGN_NEXT );
	}
}

void CItemSuitTree::UnCheckChildItems ( HTREEITEM hItem )
{
	HTREEITEM hChildItem = m_ctrlItemTree.GetChildItem( hItem );

	while ( hChildItem != NULL )
	{
		m_ctrlItemTree.SetCheck( hChildItem, FALSE );
		if ( m_ctrlItemTree.ItemHasChildren( hChildItem ) )
		{
			UnCheckChildItems( hChildItem );
		}

		hChildItem = m_ctrlItemTree.GetNextItem( hChildItem, TVGN_NEXT );
	}
}

void CItemSuitTree::OnNMClickTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	CPoint point;
	UINT nFlags = 0;

	GetCursorPos( &point );
	::ScreenToClient( m_ctrlItemTree.m_hWnd, &point );

	// ������ Item;
	HTREEITEM hItem = m_ctrlItemTree.HitTest( point, &nFlags );
	if ( hItem && ( nFlags & TVHT_ONITEMSTATEICON ) )
	{
		if ( m_ctrlItemTree.GetCheck( hItem ) )
		{
			UnCheckChildItems( hItem );
		}
		else
		{
			CheckChildItems( hItem );
		}
	}
}


void CItemSuitTree::SetReadMode( CMenu* pMenu )
{
//  �б����� ����϶��� �����Ѵ�. 
//  ���ҽ� �߰��� ���� ��� 
#ifdef READTOOL_PARAM

	pMenu->EnableMenuItem ( ID_MENUITEM_ADD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem ( ID_MENUITEM_DEL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem ( ID_MENUITEM_COPY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem ( ID_MENUITEM_PASTE_NEW, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem ( ID_MENUITEM_PASTE_VALUE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );

#endif
}

