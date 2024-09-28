// AllItemView.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../SkillEdit.h"
#include "../SkillEditDlg.h"

#include "./SheetWithTab.h"
#include "./SkillTree.h"
#include "../../Enginelib/GUInterface/GameTextControl.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/GLStringTable.h"
#include "../../RanLogic/GLSkillToolTip.h"

#include "../../SigmaCore/File/Excel/MinExcel.h"

// CSkillTree 대화 상자입니다.

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace
{
	CString GetFormatName ( GLSKILL *pItemNode )
	{
		CString strSkillName;
		strSkillName.Format ( "[%04d/%02d]  %s", pItemNode->m_sBASIC.sNATIVEID.wMainID, pItemNode->m_sBASIC.sNATIVEID.wSubID,
			pItemNode->GetName() );

		CString strNewName;
		strNewName.Format( "SN_%03d_%03d", pItemNode->m_sBASIC.sNATIVEID.wMainID, pItemNode->m_sBASIC.sNATIVEID.wSubID );
		StringCchCopy( pItemNode->m_sBASIC.szNAME, SKILL::MAX_SZNAME, strNewName );
		strNewName.Format( "SD_%03d_%03d", pItemNode->m_sBASIC.sNATIVEID.wMainID, pItemNode->m_sBASIC.sNATIVEID.wSubID );
		pItemNode->m_sEXT_DATA.strCOMMENTS = strNewName;

		return strSkillName;
	}
};

IMPLEMENT_DYNAMIC(CSkillTree, CPropertyPage)
CSkillTree::CSkillTree(  LOGFONT logfont )
	: CPropertyPage(CSkillTree::IDD)
	, m_pFont ( NULL )
	, m_pDefaultCharacter ( NULL )
{
//	m_pComboBox = NULL;
	m_ItemNodeID = eNOT_FOUND;
	m_bDlgInit = FALSE;
	m_DummyItemNodeID = eNOT_FOUND;
	m_bDummyHasItem = FALSE;

	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);

	m_pDefaultCharacter = new GLCHARLOGIC();
}

CSkillTree::~CSkillTree()
{
    m_DummyItemNodeID = eNOT_FOUND;	
	SAFE_DELETE ( m_pFont );
}

void CSkillTree::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ITEM, m_ctrlItemTree);
}


BEGIN_MESSAGE_MAP(CSkillTree, CPropertyPage)
	ON_NOTIFY(NM_CLICK, IDC_TREE_ITEM, OnNMClickTreeItem)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_ITEM, OnNMRclickTreeItem)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_ITEM, OnNMDblclkTreeItem)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ITEM, OnTVNLclickTreeItem)	
END_MESSAGE_MAP()


// CSkillTree 메시지 처리기입니다.

BOOL CSkillTree::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );
	m_bDlgInit = TRUE;
	UpdateTreeItem ();    

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CSkillTree::CheckChildItems ( HTREEITEM hItem )
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

void CSkillTree::UnCheckChildItems ( HTREEITEM hItem )
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

void CSkillTree::OnNMClickTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	CPoint point;
	UINT nFlags = 0;

	GetCursorPos( &point );
	::ScreenToClient( m_ctrlItemTree.m_hWnd, &point );

	// 선택한 Item;
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

void CSkillTree::OnNMRclickTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	switch ( GetSelectMenu() )
	{
	case ID_MENUITEM_SKILL_MODIFY:
		{
			if ( m_ItemNodeID == eNOT_FOUND || ISROOT() )
			{
				return;
			}

			m_pSheetTab->ActiveSkillDataPage ( SKILLTREEPAGE, m_ItemNodeID );
//			m_pComboBox->EnableWindow ( FALSE );
		}
		break;
	case ID_MENUITEM_SKILL_LEVEL_DATA_MODIFY:
		{
			if ( m_ItemNodeID == eNOT_FOUND || ISROOT() )
			{
				return;
			}
			m_pSheetTab->ActiveSkillLevelDataPage ( SKILLTREEPAGE, m_ItemNodeID );
//			m_pComboBox->EnableWindow ( FALSE );
		}
		break;
	case ID_MENUITEM_ADD:
		{
//			if ( !ISROOT() )	return;

			AddItem ();
		}
		break;
	case ID_MENUITEM_DEL:
		{
			if ( m_ItemNodeID == eNOT_FOUND || ISROOT() )
			{
				return;
			}

			if ( MessageBox ( "Do you want to delete?", "Question", MB_YESNO ) == IDNO )
			{
				return;
			}

			DelItem ();
		}
		break;
	case ID_MENUITEM_COPY:
		{
			if ( m_ItemNodeID == eNOT_FOUND || ISROOT() )
			{
				return;
			}

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
			if ( m_ItemNodeID == eNOT_FOUND || ISROOT() )
			{
				return;
			}

			PasteItemValue ();
		}
		break;
	};	
}

void CSkillTree::UpdateName ()
{
	HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();
	if ( !hItem )	return;


	WORD MID = (WORD)((m_ItemNodeID & eMASK_0xFFFF0000) >> eSHIFT_16BIT);
	WORD SID = (WORD)((m_ItemNodeID & eMASK_0x0000FFFF) /*>> eBIT_16BITSHIFT*/);

	//	Note : 스킬 제거.
	PGLSKILL pSkill = GLSkillMan::GetInstance().GetData ( MID, SID );
	if ( !pSkill )	return;

	m_ctrlItemTree.SetItemText ( hItem, GetFormatName ( pSkill ) );
}

BOOL	CSkillTree::AddItem ()
{
	HTREEITEM hMainItem = m_ctrlItemTree.GetSelectedItem ();    
	if ( hMainItem && ( m_ItemNodeID != eNOT_FOUND ) )	//	서브로 추가시
	{
		WORD MID = (WORD)((m_ItemNodeID & eMASK_0xFFFF0000) >> eSHIFT_16BIT);
		WORD SID = GLSkillMan::GetInstance().FindFreeSkillIndex ( MID );

		GLSKILL ItemNode;
		ItemNode.m_sBASIC.sNATIVEID.wMainID = MID;
		ItemNode.m_sBASIC.sNATIVEID.wSubID = SID;

		char szNewName[ITEM_SZNAME];
		sprintf ( szNewName, "SN_%03d_%03d", MID, SID );
		strcpy ( ItemNode.m_sBASIC.szNAME, szNewName );
		sprintf ( szNewName, "SD_%03d_%03d", MID, SID );
		ItemNode.m_sEXT_DATA.strCOMMENTS = szNewName;
		GLSkillMan::GetInstance().SetData ( MID, SID, new GLSKILL(ItemNode) );
		PGLSKILL pSKILL = GLSkillMan::GetInstance().GetData ( MID, SID );

		CString strSkill = GetFormatName ( pSKILL );

		HTREEITEM hSubItem = NULL;

		if ( ISROOT() )
		{
			hSubItem = m_ctrlItemTree.InsertItem ( strSkill, hMainItem );
		}
		else
		{
			hSubItem = m_ctrlItemTree.InsertItem ( strSkill, m_ctrlItemTree.GetParentItem(hMainItem) );
		}

		DWORD CombineID =
		(
			(DWORD)(MID & eMASK_0x0000FFFF) << eSHIFT_16BIT |
			(DWORD)(SID & eMASK_0x0000FFFF) /*<< eSHIFT_16BIT |*/
		);
		m_ctrlItemTree.SetItemData ( hSubItem, (DWORD_PTR)CombineID );

		m_ItemNodeID = eNOT_FOUND;
		
		return TRUE;
	}
	

	return FALSE;
}

BOOL	CSkillTree::DelItem ()
{
	if ( m_ItemNodeID != eNOT_FOUND )
	{
		WORD MID = (WORD)((m_ItemNodeID & eMASK_0xFFFF0000) >> eSHIFT_16BIT);
		WORD SID = (WORD)((m_ItemNodeID & eMASK_0x0000FFFF) /*>> eBIT_16BITSHIFT*/);

		//	Note : 스킬 제거.
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			CString strId;
			strId.Format( "IN_%03d_%03d", MID, SID );
			CGameTextMan::GetInstance().DeleteString( strId.GetBuffer(), CGameTextMan::EM_SKILL_TEXT );
			strId.Format( "ID_%03d_%03d", MID, SID );
			CGameTextMan::GetInstance().DeleteString( strId.GetBuffer(), CGameTextMan::EM_SKILL_TEXT );
		}
		else
		{
			GLStringTable::GetInstance().DeleteString( MID, SID, GLStringTable::SKILL );
		}
		GLSkillMan::GetInstance().DeleteSkill ( MID, SID );
		m_ItemNodeID = eNOT_FOUND;

		HTREEITEM htreeitem = m_ctrlItemTree.GetSelectedItem ();
		if ( htreeitem != NULL )	m_ctrlItemTree.DeleteItem ( htreeitem );

		return TRUE;	
	}

	return FALSE;
}

int		CSkillTree::GetSelectMenu ()
{
	POINT OriginPoint, ClientPoint;

	//	<--	우클릭으로 포커스 옮기기
	GetCursorPos ( &OriginPoint );
	ClientPoint = OriginPoint;

	m_ctrlItemTree.ScreenToClient(&ClientPoint);

	UINT uFlag = 0;
	HTREEITEM hItem = m_ctrlItemTree.HitTest ( ClientPoint, &uFlag );
	m_ctrlItemTree.SelectItem ( hItem );

	if ( hItem && uFlag & TVHT_ONITEMLABEL )
	{		
		m_ItemNodeID = (DWORD)m_ctrlItemTree.GetItemData ( hItem );
	}
	else
	{
		m_ItemNodeID = eNOT_FOUND;
	}
	//	-->

	//	<--	실제 작동 메뉴 따기
	int pMenuID = 0;
	CMenu Menu;

	DWORD SelectionMade = -1;
	//switch ( m_SelectMenu )
	//{
	//case 0:
	//	{
		VERIFY ( Menu.LoadMenu ( IDR_MENU_SKILL_POPUP ) );

		CMenu* pPopup = Menu.GetSubMenu ( pMenuID );
		ASSERT ( pPopup != NULL );	

		SetReadMode( pPopup );

		SelectionMade = pPopup->TrackPopupMenu (
			TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
			OriginPoint.x, OriginPoint.y, this );

		pPopup->DestroyMenu ();
	//	}
	//	break;
	//}
	//	-->

	return SelectionMade;
}

bool CSkillTree::ExportExcelDetail_CheckItem (
	std::string& strSaveFileName,
	boost::function<void (int)> PercentCallbackFunc )
{
	// 확장자를 없애자;
	std::string ExcelFileName( "skill" );
	sc::Excel Xls( ExcelFileName, strSaveFileName, 1, true );

	// ------------------------------------------------------------------------
	int Col = 0;
	int Row = 0;
	int SheetNum = 0;

	Xls.SetCell( SheetNum, Row, Col++, "ID" );
	Xls.SetCell( SheetNum, Row, Col++, "Name" );

	for ( int i=0; i<9; ++i )
	{
		CSkillEditDlg* pSkillEditDlg =
			static_cast< CSkillEditDlg* >( theApp.m_pMainWnd );
		if ( pSkillEditDlg )
		{
			if ( !pSkillEditDlg->IsSkillLevelChecked( i ) )
				continue;
		}

		Xls.SetCell( SheetNum, Row, Col++, sc::string::format( "Basic Skill(%1%Lv)", i+1 ) );
		//Xls.SetCell( SheetNum, Row, Col++, sc::string::format( "Multi Skill(%1%Lv)", i+1 ) );
		//Xls.SetCell( SheetNum, Row, Col++, sc::string::format( "Link Skill(%1%Lv)", i+1 ) );
	}
	// ------------------------------------------------------------------------

	WORD MaxMID, MaxSID;
	GLSkillMan::GetInstance().GetMaxSkill ( MaxMID, MaxSID );
	int nPercentCount = 0;
	int nMaxSkillCount = MaxMID*MaxSID;
	for ( int MID = 0; MID < MaxMID; MID++ )
	{
		for ( int SID = 0; SID < MaxSID; SID++ )
		{
			Col = 0;

			HTREEITEM hItem = m_TreeItemList[MID][SID];
			BOOL bChecked = m_ctrlItemTree.GetCheck( hItem );

			// Export;
			if ( bChecked )
			{
				GLSKILL* pSkill = GLSkillMan::GetInstance().GetData ( MID, SID );
				if ( pSkill )
				{
					Row++;

					// ID;
					Xls.SetCell( SheetNum, Row, Col++, sc::string::format( "%s", pSkill->m_sBASIC.szNAME ) );

					// Name;
					Xls.SetCell( SheetNum, Row, Col++, pSkill->GetName() );

					for ( int iLevel=0; iLevel<9; ++iLevel )
					{
						CSkillEditDlg* pSkillEditDlg =
							static_cast< CSkillEditDlg* >( theApp.m_pMainWnd );
						if ( pSkillEditDlg )
						{
							if ( !pSkillEditDlg->IsSkillLevelChecked( iLevel ) )
								continue;
						}

						// Basic Skill;
						std::string strSkillInfo = GetSkillInfoText( pSkill, iLevel );

						GLSKILL* pMultiSkill = pSkill->GetSecondSkill();
						if ( pMultiSkill )
							strSkillInfo += sc::string::format( "\nMulti Skill : %1%", pMultiSkill->GetName() );
						
						for ( int i=0; i<pSkill->m_sAPPLY.vecSPEC.size(); ++i )
						{
							if ( pSkill->m_sAPPLY.vecSPEC[ i ].emSPEC ==
								EMSPECA_SKILL_LINK )
							{
								// 링크 스킬 아이디 획득;
								SNATIVEID sLinkID = pSkill->m_sAPPLY.vecSPEC[ i ].sSPEC[ iLevel ].dwLinkID;

								// 링크 스킬 가져오기;
								GLSKILL* pLinkSkill = GLSkillMan::GetInstance().GetData(
									sLinkID.wMainID, sLinkID.wSubID );
								if ( pLinkSkill )
								{
									strSkillInfo += sc::string::format( "\nLink Skill : %1%", pLinkSkill->GetName() );
								}
							}
						}

						Xls.SetCell( SheetNum, Row, Col++, strSkillInfo );

						// Multi Skill;
						//Xls.SetCell( SheetNum, Row, Col++,
						//	GetSkillInfoText( pSkill->GetSecondSkill(), iLevel ) );

						// Link Skill;
						//std::string tempLinkSkillInfo = "";
						//for ( int i=0; i<pSkill->m_sAPPLY.vecSPEC.size(); ++i )
						//{
						//	if ( pSkill->m_sAPPLY.vecSPEC[ i ].emSPEC ==
						//		EMSPECA_SKILL_LINK )
						//	{
								// 링크 스킬 아이디 획득;
						//		SNATIVEID sLinkID = pSkill->m_sAPPLY.vecSPEC[ i ].sSPEC[ iLevel ].dwLinkID;

								// 링크 스킬 가져오기;
						//		GLSKILL* pLinkSkill = GLSkillMan::GetInstance().GetData(
						//			sLinkID.wMainID, sLinkID.wSubID );
						//		if ( pLinkSkill )
						//		{
						//			tempLinkSkillInfo += GetSkillInfoText( pLinkSkill, iLevel );
						//		}
						//	}
						//}

						//Xls.SetCell( SheetNum, Row, Col++, tempLinkSkillInfo );
					}
				}
			}

			if ( PercentCallbackFunc )
			{
				nPercentCount++;
				if ( nPercentCount && nMaxSkillCount )
				{
					int Percentage = static_cast<int>(
						(nPercentCount*100)/nMaxSkillCount);
					PercentCallbackFunc(Percentage);
				}
			}
		}
	}

	if ( PercentCallbackFunc )
		PercentCallbackFunc(100);

	return true;
}

std::string CSkillTree::GetSkillInfoText ( GLSKILL* pSkill, WORD wLevel )
{
	std::string strInfo = "NULL";
	std::string strTempInfo = "";

	if ( !pSkill )
		return strInfo;

	// 이름;
	strInfo = sc::string::format( "%1% : %2%",
		ID2GAMEWORD( "SKILL_BASIC_INFO", 0 ),
		pSkill->GetName() );
	strInfo += "\n";

	// 등급/속성;
	if ( GLSkillToolTip::MakeTooltipString_In_Skill_Grade(
		strTempInfo, pSkill ) )
	{
		strTempInfo = sc::string::format( "%1%/%2% : %3%",
			ID2GAMEWORD( "SKILL_BASIC_INFO", 1 ),
			ID2GAMEWORD( "SKILL_BASIC_INFO", 2 ),
			strTempInfo );
		strInfo += strTempInfo;
		strInfo += "\n";
	}

	// 타입;
	if ( GLSkillToolTip::MakeTooltipString_In_Skill_ApplyType(
		strTempInfo, pSkill ) )
	{
		strTempInfo = sc::string::format( "%1% : %2%",
			ID2GAMEWORD( "SKILL_BASIC_INFO", 5 ),
			strTempInfo );
		strInfo += strTempInfo;
		strInfo += "\n";
	}

	// 사용 무기;
	if ( GLSkillToolTip::MakeTooltipString_In_Skill_UseWeapon(
		strTempInfo, pSkill ) )
	{
		strTempInfo = sc::string::format( "%1% : %2%",
			ID2GAMEWORD( "SKILL_BASIC_INFO", 9 ),
			strTempInfo );
		strInfo += strTempInfo;
		strInfo += "\n";
	}

	// 소모 조건;
	if ( GLSkillToolTip::MakeTooltipString_In_Skill_Requirement(
		strTempInfo, pSkill, wLevel ) )
	{
		strTempInfo = sc::string::format( "%1% : %2%",
			ID2GAMEWORD( "SKILL_BASIC_INFO", 10 ),
			strTempInfo );
		strInfo += strTempInfo;
		strInfo += "\n";
	}

	// 딜레이;
	if ( GLSkillToolTip::MakeTooltipString_In_Skill_ReuseTime(
		strTempInfo, pSkill, wLevel ) )
	{
		strTempInfo = sc::string::format( "%1% : %2%",
			ID2GAMEWORD( "SKILL_CDATA_CVL_BASIC", 0 ),
			strTempInfo );
		strInfo += strTempInfo;
		strInfo += "\n\n";
	}

	// 스킬 영역;
	if ( GLSkillToolTip::MakeTooltipString_In_Skill_SkillScope(
		strTempInfo, pSkill, wLevel, m_pDefaultCharacter ) )
	{
		strTempInfo = sc::string::format( "%1%\r\n%2%",
			ID2GAMEWORD( "SKILL_BASIC_INFO", 12 ),
			strTempInfo );
		strInfo += strTempInfo;
		strInfo += "\n\n";
	}

	// 스킬 효과;
	if ( GLSkillToolTip::MakeTooltipString_In_Skill_SkillEffect(
		strTempInfo, pSkill, wLevel, m_pDefaultCharacter ) )
	{
		strTempInfo = sc::string::format( "%1%\r\n%2%",
			ID2GAMEWORD( "SKILL_BASIC_INFO", 13 ),
			strTempInfo );
		strInfo += strTempInfo;
	}

	// 상태 이상 효과;
	if ( GLSkillToolTip::MakeTooltipString_In_Skill_Blow(
		strTempInfo, pSkill, wLevel ) )
	{
		strTempInfo = sc::string::format( "%1%\r\n%2%",
			ID2GAMEWORD( "SKILL_BASIC_INFO", 13 ),
			strTempInfo );
		strInfo += strTempInfo;
	}


	return strInfo;
}

BOOL	CSkillTree::UpdateTreeItem ()
{
	if ( !m_bDlgInit )
	{
		return FALSE;
	}

	m_ctrlItemTree.DeleteAllItems ();

	WORD MaxMID, MaxSID;
	GLSkillMan::GetInstance().GetMaxSkill ( MaxMID, MaxSID );

	for ( WORD MID = 0; MID < MaxMID; MID++ )
	{
		HTREEITEM hMainItem = NULL;
		HTREEITEM hSubItem = NULL;

		//	MID를 상위 16비트로 올리고, 하위 16비트를 0xFFFF으로 채운다.
		//	하위 16비트를 0xFFFF로 채우는 것은 사용되지 않는 부분임을 표시하기 위한 것이다.
		DWORD	UpperID = (DWORD)(MID & eMASK_0x0000FFFF) << eSHIFT_16BIT | eMASK_0x0000FFFF;
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		{
			hMainItem = m_ctrlItemTree.InsertItem (CGameTextMan::GetInstance().GetCommentText("SKILLCLASS",MID).GetString());
		}
		else
		{
			hMainItem = m_ctrlItemTree.InsertItem ( COMMENT::SKILLCLASS[MID].c_str() );
		}
		m_ctrlItemTree.SetItemData ( hMainItem,	(DWORD_PTR)UpperID );

		for ( WORD SID = 0; SID < MaxSID; SID++ )
		{			
			GLSKILL *pItemNode = GLSkillMan::GetInstance().GetData ( MID, SID );

			if ( pItemNode )
			{
				if ( !hMainItem )
				{
					m_TreeItemList[MID][SID] = hMainItem;
					continue;
				}

				DWORD CombineID =
				(
					(DWORD)(MID & eMASK_0x0000FFFF) << eSHIFT_16BIT |
					(DWORD)(SID & eMASK_0x0000FFFF) /*<< eSHIFT_16BIT |*/
				);

				CString strSkillName = GetFormatName ( pItemNode );
				hSubItem = m_ctrlItemTree.InsertItem ( strSkillName, hMainItem );
				m_ctrlItemTree.SetItemData ( hSubItem, (DWORD_PTR)CombineID );

				m_TreeItemList[MID][SID] = hSubItem;
			}
		}
	}

	return TRUE;
}

void CSkillTree::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyPage::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if ( bShow )
	{
		//UpdateTreeItem ();
	}
}

void CSkillTree::CleanAllItem ()
{
	if ( !m_bDlgInit )
	{
		return ;
	}

	m_ctrlItemTree.DeleteAllItems ();
}

BOOL CSkillTree::CopyItem ()
{
	HTREEITEM hMainItem = m_ctrlItemTree.GetSelectedItem ();    
	if ( hMainItem )
	{
		m_DummyItemNodeID = m_ItemNodeID;
		m_bDummyHasItem = TRUE;
		return TRUE;
	}

	return FALSE;
}

BOOL CSkillTree::PasteItemNew ()
{
	if ( !m_bDummyHasItem )
	{
		return FALSE;
	}	

	HTREEITEM hMainItem = m_ctrlItemTree.GetSelectedItem ();
	if ( hMainItem && ( m_ItemNodeID != eNOT_FOUND ) )	//	서브로 추가시
	{
		//	MID 가져 오기 ( 상위비트에 있는 값 끄집어 내림 )
		WORD MID = (WORD)((m_ItemNodeID & eMASK_0xFFFF0000) >> eSHIFT_16BIT);
		WORD SID = GLSkillMan::GetInstance().FindFreeSkillIndex ( MID );

		//	MID와 SID로 분리
		WORD DummyMID = (WORD)((m_DummyItemNodeID & eMASK_0xFFFF0000) >> eSHIFT_16BIT);
		WORD DummySID = (WORD)(m_DummyItemNodeID & eMASK_0x0000FFFF);

		GLSKILL ItemNode = *GLSkillMan::GetInstance().GetData ( DummyMID, DummySID );
		ItemNode.m_sBASIC.sNATIVEID.wMainID = MID;
		ItemNode.m_sBASIC.sNATIVEID.wSubID = SID;
				
		GLSkillMan::GetInstance().SetData ( MID, SID, new GLSKILL(ItemNode) );

		UpdateTreeItem ();
		m_ItemNodeID = eNOT_FOUND;
		return TRUE;
	}
	else	//	메인으로 추가시
	{
		//WORD MID = GLCrowDataMan::GetInstance().FindFreeCrowDataMID ();
		//if ( MID == ITEMID_NOTFOUND )
		//{
		//	return FALSE;
		//}
		//WORD SID = GLCrowDataMan::GetInstance().FindFreeCrowDataSID ( MID );
		//if ( SID == ITEMID_NOTFOUND )
		//{
		//	return FALSE;
		//}
		//
		//m_pItemNode = new SCROWDATA;

		//*m_pItemNode = *m_pDummyItemNode;

		//m_pItemNode->sNativeID.wMainID = MID;
		//m_pItemNode->sNativeID.wSubID = SID;		
		//GLCrowDataMan::GetInstance().InsertCrowData ( MID, SID, m_pItemNode );

		//UpdateTreeItem ();
		//m_pItemNode = NULL;
		//return TRUE;
	}

	return FALSE;
}

BOOL	CSkillTree::PasteItemValue ()
{
	if ( !m_bDummyHasItem )
	{
		return FALSE;
	}

	WORD MID = (WORD)((m_ItemNodeID & eMASK_0xFFFF0000) >> eSHIFT_16BIT);
	WORD SID = (WORD)(m_ItemNodeID & eMASK_0x0000FFFF);

	WORD DummyMID = (WORD)((m_DummyItemNodeID & eMASK_0xFFFF0000) >> eSHIFT_16BIT);
	WORD DummySID = (WORD)(m_DummyItemNodeID & eMASK_0x0000FFFF);

	GLSKILL ItemNode = *GLSkillMan::GetInstance().GetData ( DummyMID, DummySID );
	ItemNode.m_sBASIC.sNATIVEID.wMainID = MID;
	ItemNode.m_sBASIC.sNATIVEID.wSubID = SID;
			
	GLSkillMan::GetInstance().SetData ( MID, SID, new GLSKILL(ItemNode) );

	UpdateTreeItem ();
	m_ItemNodeID = eNOT_FOUND;	

	return FALSE;
}

BOOL	CSkillTree::ISROOT ()
{
	DWORD MID = (WORD)((m_ItemNodeID & eMASK_0xFFFF0000) >> eSHIFT_16BIT);
	DWORD SID = (WORD)(m_ItemNodeID & eMASK_0x0000FFFF);

	if ( SID == eMASK_0x0000FFFF )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CSkillTree::SetReadMode( CMenu* pMenu )
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	pMenu->EnableMenuItem ( ID_MENUITEM_ADD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem ( ID_MENUITEM_DEL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem ( ID_MENUITEM_COPY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem ( ID_MENUITEM_PASTE_NEW, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	pMenu->EnableMenuItem ( ID_MENUITEM_PASTE_VALUE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );

#endif
}

void CSkillTree::OnNMDblclkTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();

	m_ItemNodeID = eNOT_FOUND;
	if ( hItem )
	{
		m_ItemNodeID = (DWORD)m_ctrlItemTree.GetItemData ( hItem );
		if( m_ItemNodeID != eNOT_FOUND && !ISROOT() ) 
		{
			CSkillEditDlg* pDlg = (CSkillEditDlg*)m_pSheetTab->m_pWndParent;
			WORD MID = (WORD)((m_ItemNodeID & eMASK_0xFFFF0000) >> eSHIFT_16BIT);
			WORD SID = (WORD)((m_ItemNodeID & eMASK_0x0000FFFF) /*>> eBIT_16BITSHIFT*/);

			GLSKILL pSkill = *GLSkillMan::GetInstance().GetData ( MID, SID );
			if( pDlg )
				pDlg->SetSkillView( &pSkill );

			m_pSheetTab->ActiveSkillDataPage ( SKILLTREEPAGE, m_ItemNodeID );
		}
	}	
}

void CSkillTree::OnTVNLclickTreeItem( NMHDR *pNMHDR, LRESULT *pResult )
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	// 선택한 Item;
	HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();

	m_ItemNodeID = eNOT_FOUND;
	if ( hItem )
	{
		m_ItemNodeID = (DWORD)m_ctrlItemTree.GetItemData ( hItem );
		if( m_ItemNodeID != eNOT_FOUND && !ISROOT() ) 
		{
			CSkillEditDlg* pDlg = (CSkillEditDlg*)m_pSheetTab->m_pWndParent;
			WORD MID = (WORD)((m_ItemNodeID & eMASK_0xFFFF0000) >> eSHIFT_16BIT);
			WORD SID = (WORD)((m_ItemNodeID & eMASK_0x0000FFFF) /*>> eBIT_16BITSHIFT*/);

			GLSKILL pSkill = *GLSkillMan::GetInstance().GetData ( MID, SID );
			if( pDlg )
				pDlg->SetSkillView( &pSkill );

			//m_pSheetTab->ActiveSkillDataPage ( SKILLTREEPAGE, m_ItemNodeID );
		}
	}	
}