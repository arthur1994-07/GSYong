// AllItemView.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../MobNpcEdit.h"
#include "./SheetWithTab.h"
#include "../LogDialog.h"
#include "./CrowTree.h"

#include "../../enginelib/GUInterface/GameTextControl.h"
#include "../../EngineLib/DxTools/DxFontDefine.h"
#include "../../EngineLib/DxTools/DxFontMan.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../RanLogic/RANPARAM.h"

#include "../../RanLogic/GLStringTable.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/Xml/2.5.2/tinyxml.h"

// CCrowTree 대화 상자입니다.

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CCrowTree, CPropertyPage)
CCrowTree::CCrowTree( LOGFONT logfont )
	: CPropertyPage(CCrowTree::IDD)
	, m_pFont ( NULL )
{
//	m_pComboBox = NULL;
	m_pItemNode = NULL;
	m_bDlgInit = FALSE;
	m_pDummyItemNode = new SCROWDATA;
	m_bDummyHasItem = FALSE;
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CCrowTree::~CCrowTree()
{
	SAFE_DELETE ( m_pDummyItemNode );
	SAFE_DELETE ( m_pFont );
	
}

void CCrowTree::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ITEM, m_ctrlItemTree);
}


BEGIN_MESSAGE_MAP(CCrowTree, CPropertyPage)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_ITEM, OnNMRclickTreeItem)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE_SYNC, OnBnClickedButtonUpdateSync)
	ON_BN_CLICKED(IDC_BUTTON_STRTBL_SYNC, OnBnClickedButtonStrtblSync)
	ON_BN_CLICKED(IDC_BTN_CSV_SAVE, OnBnClickedBtnCsvSave)
	ON_BN_CLICKED(IDC_BTN_CSV_LOAD, OnBnClickedBtnCsvLoad)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_ITEM, OnNMDblclkTreeItem)
	ON_BN_CLICKED(IDC_BTN_EXCEL_STRTBL_IMPORT, OnBnClickedBtnExcelStrTblImport)
	ON_BN_CLICKED(IDC_BTN_EXCEL_STRTBL_EXPORT, OnBnClickedBtnExcelStrTblExport)
	ON_BN_CLICKED(IDC_BUTTON_XMLCONVERT, OnBnClickedBtnXmlConvert)
END_MESSAGE_MAP()


// CCrowTree 메시지 처리기입니다.

BOOL CCrowTree::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	// PropertyPage 폰트 강제 변경
	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	m_bDlgInit = TRUE;
	UpdateTree ();

	SetReadMode ( NULL );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
void CCrowTree::OnNMRclickTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;	

	POINT OriginPoint;
	GetCursorPos ( &OriginPoint );

	//	우클릭 강제 설정
	ForceSelectOnRightClick ( OriginPoint );

	//	메뉴 선택 & 선택 메뉴 실행
	RunSelectMenu ( GetSelectMenu ( OriginPoint ) );
}

BOOL	CCrowTree::DelItem ()
{
	if ( GetSelectItem() )
	{
		WORD WID = GetSelectItem()->sNativeID.wMainID;
		WORD SID = GetSelectItem()->sNativeID.wSubID;

		if ( SID == 0 )	// 루트를 삭제하는 것이면...
		{
			for ( int i = 0; i < GLCrowDataMan::MAX_CROW_SID; i++ )
			{
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					CString strId;
					strId.Format( "CN_%03d_%03d", WID, SID );
					CGameTextMan::GetInstance().DeleteString( strId.GetBuffer(), CGameTextMan::EM_ITEM_TEXT );
				}
				else
				{
					GLStringTable::GetInstance().DeleteString( WID, i, GLStringTable::CROW );
					GLCrowDataMan::GetInstance().DeleteCrowData ( WID, i );
				}
			}
		}
		else
		{
			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			{
				CString strId;
				strId.Format( "CN_%03d_%03d", WID, SID );
				CGameTextMan::GetInstance().DeleteString( strId.GetBuffer(), CGameTextMan::EM_ITEM_TEXT );
			}
			else
			{
				GLStringTable::GetInstance().DeleteString( std::string(m_pItemNode->m_szName), std::string(), GLStringTable::CROW );
				GLCrowDataMan::GetInstance().DeleteCrowData ( m_pItemNode->sNativeID );
			}
		}
		
		m_ctrlItemTree.DeleteItem ( m_ctrlItemTree.GetSelectedItem () );

		return TRUE;	
	}

	return FALSE;
}

void CCrowTree::CleanAllItem ()
{
	if ( !m_bDlgInit )
	{
		return ;
	}

	m_ctrlItemTree.DeleteAllItems ();
	m_TreeRoot = m_ctrlItemTree.InsertItem ( "Root Node" );
}

void CCrowTree::OnBnClickedButtonUpdateSync()
{
	GLCrowDataMan::GetInstance().SyncUpdateData ();

	CLogDialog LogDlg;
	LogDlg.SetLogFile ( GLCrowDataMan::_LOGFILE );
	LogDlg.DoModal ();
}


BOOL CCrowTree::UpdateTree ()
{	
	if ( !m_bDlgInit )
	{
		return FALSE;
	}

	CleanAllItem ();

	for ( int MID = 0; MID < GLCrowDataMan::MAX_CROW_MID; MID++ )
	{
		HTREEITEM hMainItem= NULL;
		HTREEITEM hSubItem = NULL;

		for ( int SID = 0; SID < GLCrowDataMan::MAX_CROW_SID; SID++ )
		{        		
			PCROWDATA pItem = GLCrowDataMan::GetInstance().GetCrowData ( MID, SID );

			if ( pItem )
			{
				CString strItem = GetFormatName ( pItem );
				if ( !hMainItem )
				{
					hMainItem = m_ctrlItemTree.InsertItem ( strItem, m_TreeRoot );
					m_ctrlItemTree.SetItemData ( hMainItem, (DWORD_PTR)pItem );
				}
				else
				{
					hSubItem = m_ctrlItemTree.InsertItem ( strItem, hMainItem );
					m_ctrlItemTree.SetItemData ( hSubItem, (DWORD_PTR)pItem );
				}				
			}
		}
	}

	return TRUE;
}

void CCrowTree::UpdateName ()
{
	HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();
	GASSERT ( hItem );
	PCROWDATA pItemNode = (PCROWDATA)m_ctrlItemTree.GetItemData ( hItem );
	GASSERT ( pItemNode );

	m_ctrlItemTree.SetItemText ( hItem, GetFormatName ( pItemNode ) );
}

BOOL CCrowTree::UpdateItem ( PCROWDATA pItem, HTREEITEM hMainItem )
{
	CString strItem;
	strItem = GetFormatName ( pItem );
	
	if ( !hMainItem )
	{
		hMainItem = m_ctrlItemTree.InsertItem ( strItem, m_TreeRoot );
		m_ctrlItemTree.SetItemData ( hMainItem, (DWORD_PTR)pItem );
	}
	else
	{
		HTREEITEM hSubItem = m_ctrlItemTree.InsertItem ( strItem, hMainItem );
		m_ctrlItemTree.SetItemData ( hSubItem, (DWORD_PTR)pItem );
	}

	return TRUE;
}

const char* CCrowTree::GetFormatName ( PCROWDATA pItem )
{
	GASSERT ( pItem );

	static	CString strItem;
	//if ( pItem->dwReqCharClass==GLCC_ALL )
	//{
	//	strItem.Format ( "[%04d/%02d]  %s  [{%s}]",
	//		pItem->sNativeID.wMainID, pItem->sNativeID.wSubID, pItem->szName, "공용" );
	//}
	//else
	//{
	//	strItem.Format ( "[%04d/%02d]  %s  [",
	//		pItem->sNativeID.wMainID, pItem->sNativeID.wSubID, pItem->szName );

	//	if ( pItem->dwReqCharClass&GLCC_FIGHTER )	strItem += "격투|";
	//	if ( pItem->dwReqCharClass&GLCC_ARMS )		strItem += "검도|";
	//	if ( pItem->dwReqCharClass&GLCC_ARCHER )	strItem += "양궁|";
	//	if ( pItem->dwReqCharClass&GLCC_SPIRIT )	strItem += "기예|";
	//	if ( pItem->dwReqCharClass&GLCC_EXTREME_M )	strItem += "극강부남|";
	//	if ( pItem->dwReqCharClass&GLCC_EXTREME_W )	strItem += "극강부여";
	//}
	strItem.Format ( "[%04d/%02d] %s", pItem->sNativeID.wMainID, pItem->sNativeID.wSubID, pItem->GetName() );

	CString strNewName;
	strNewName.Format( "CN_%03d_%03d", pItem->sNativeID.wMainID, pItem->sNativeID.wSubID );
	StringCchCopy( pItem->m_szName, CHAR_SZNAME, strNewName );

	return strItem.GetString ();
}

void CCrowTree::SetSelectItem ( PCROWDATA pItemNode )
{
	m_pItemNode = pItemNode;
}

PCROWDATA CCrowTree::GetSelectItem ()
{
	return m_pItemNode;
}

BOOL	CCrowTree::CopyItem ()
{
	HTREEITEM hMainItem = m_ctrlItemTree.GetSelectedItem ();
	if ( hMainItem && hMainItem != m_TreeRoot )
	{
		m_pDummyItemNode->Assign ( *((PCROWDATA)m_ctrlItemTree.GetItemData ( hMainItem )) );
		m_bDummyHasItem = TRUE;
		return TRUE;
	}

	return FALSE;
}

BOOL	CCrowTree::PasteItemNew ()
{
	if ( !m_bDummyHasItem )
	{
		return FALSE;
	}

	HTREEITEM hMainItem = m_ctrlItemTree.GetSelectedItem ();
	if ( hMainItem && GetSelectItem () )	//	서브로 추가시
	{
		WORD MID = GetSelectItem ()->sNativeID.wMainID;
		WORD SID = GLCrowDataMan::GetInstance().FindFreeCrowDataSID ( MID );
		if ( SID == ITEMID_NOTFOUND )
		{
			return FALSE;
		}

		SCROWDATA pItemNode;

		pItemNode.Assign ( *m_pDummyItemNode );

		pItemNode.sNativeID.wMainID = MID;
		pItemNode.sNativeID.wSubID = SID;
		GLCrowDataMan::GetInstance().InsertCrowData ( MID, SID, &pItemNode );
		PCROWDATA pCROWDATA = GLCrowDataMan::GetInstance().GetCrowData ( MID, SID );

		HTREEITEM hParentItem = m_ctrlItemTree.GetParentItem ( hMainItem );
		if ( hParentItem != m_TreeRoot )
		{
			UpdateItem ( pCROWDATA, hParentItem );
		}
		else
		{
			UpdateItem ( pCROWDATA, hMainItem );
		}
		
		return TRUE;
	}
	else	//	메인으로 추가시
	{
		WORD MID = GLCrowDataMan::GetInstance().FindFreeCrowDataMID ();
		if ( MID == ITEMID_NOTFOUND )
		{
			return FALSE;
		}
		WORD SID = GLCrowDataMan::GetInstance().FindFreeCrowDataSID ( MID );
		if ( SID == ITEMID_NOTFOUND )
		{
			return FALSE;
		}
		
		PCROWDATA pItemNode = new SCROWDATA;

		pItemNode->Assign ( *m_pDummyItemNode );

		pItemNode->sNativeID.wMainID = MID;
		pItemNode->sNativeID.wSubID = SID;		
		GLCrowDataMan::GetInstance().InsertCrowData ( MID, SID, pItemNode );
		PCROWDATA pCROWDATA = GLCrowDataMan::GetInstance().GetCrowData ( MID, SID );

		UpdateItem ( pCROWDATA, NULL );

		return TRUE;
	}
}

BOOL	CCrowTree::PasteItemValue ()
{
	if ( !m_bDummyHasItem )
	{
		return FALSE;
	}

	HTREEITEM hMainItem = m_ctrlItemTree.GetSelectedItem ();
	if ( hMainItem && GetSelectItem() )	//	서브로 추가시
	{
		WORD MID = GetSelectItem()->sNativeID.wMainID;
		WORD SID = GetSelectItem()->sNativeID.wSubID;

		GetSelectItem()->Assign ( *m_pDummyItemNode );

		GetSelectItem()->sNativeID.wMainID = MID;
		GetSelectItem()->sNativeID.wSubID = SID;

		m_ctrlItemTree.SetItemText ( hMainItem, GetFormatName ( GetSelectItem() ) );
		
		return TRUE;
	}

	return FALSE;
}

int	 CCrowTree::GetSelectMenu ( POINT pt )
{
	//	<--	실제 작동 메뉴 따기
	int pMenuID = 0;
	CMenu Menu;

	int SelectionMade = -1;
	//switch ( m_SelectMenu )
	//{
	//case 0:
	//	{
		VERIFY ( Menu.LoadMenu ( IDR_MENU_CROW_POPUP ) );

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

void CCrowTree::ForceSelectOnRightClick ( POINT pt )
{		
	m_ctrlItemTree.ScreenToClient( &pt );

	UINT uFlag = 0;
	PCROWDATA pItemNode = NULL;
	HTREEITEM hItem = m_ctrlItemTree.HitTest ( CPoint(pt), &uFlag );

    SetSelectItem ( NULL );	
	if ( hItem )
	{
		m_ctrlItemTree.SelectItem ( hItem );
		if ( uFlag & TVHT_ONITEMLABEL )
		{			
			SetSelectItem ( (PCROWDATA)m_ctrlItemTree.GetItemData ( hItem ) );
		}
	}
}

void CCrowTree::RunSelectMenu ( int nSelect )
{
	HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();

	PCROWDATA pItemNode = NULL;
	if ( hItem )
	{
		pItemNode = (PCROWDATA)m_ctrlItemTree.GetItemData ( hItem );
	}

	switch ( nSelect )
	{
	case ID_MENUITEM_CROWDATAMODIFY:
		{
			if ( GetSelectItem () )
			{
				m_pSheetTab->ActiveCrowDataPage ( CROWTREEPAGE, pItemNode );
			}
		}
		break;
	case ID_MENUITEM_CROWATTACKMODIFY:
		{
			if ( GetSelectItem () )
			{
				m_pSheetTab->ActiveCrowAttackTreePage ( CROWTREEPAGE, pItemNode );
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
					DelItem ();
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

BOOL CCrowTree::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	switch ( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			if ( pMsg->wParam == VK_RETURN )
			{
				HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();
				if ( !hItem ) return TRUE;

				PCROWDATA pItemNode = (PCROWDATA)m_ctrlItemTree.GetItemData ( hItem );
				if ( !pItemNode )
				{
					return TRUE;
				}

				m_pSheetTab->ActiveCrowDataPage ( CROWTREEPAGE, pItemNode );
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
					PCROWDATA pItemNode = (PCROWDATA)m_ctrlItemTree.GetItemData ( hItem );
					SetSelectItem ( pItemNode );

					RunSelectMenu ( ID_MENUITEM_PASTE_NEW );
				}
			}

			if ( ::GetKeyState ( VK_CONTROL ) < 0 )
			{
				if ( pMsg->wParam == 'B' )
				{
					HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();
					PCROWDATA pItemNode = (PCROWDATA)m_ctrlItemTree.GetItemData ( hItem );
					SetSelectItem ( pItemNode );

					RunSelectMenu ( ID_MENUITEM_PASTE_VALUE );
				}
			}

		}
		break;
	}

	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CCrowTree::OnBnClickedButtonReloadStrtbl()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 아이템 스트링테이블을 다시 로드한다. by 경대
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		CGameTextMan::GetInstance().Clear(CGameTextMan::EM_CROW_TEXT);
		CGameTextMan::GetInstance().LoadText(RANPARAM::strCrowText, CGameTextMan::EM_CROW_TEXT, RANPARAM::strCountry, CGameTextMan::GAMETEXTMAN_LOADOP_REFACT/*너무 느리다bToolMode*/ );
	}
	else
	{
		GLStringTable::GetInstance().CLEAR();
		GLStringTable::GetInstance().LOADFILE( GLCrowDataMan::_STRINGTABLE, GLStringTable::CROW );
	}
}

void CCrowTree::OnBnClickedButtonStrtblSync()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GLCrowDataMan::GetInstance().SyncStringTable();
	AfxMessageBox("Sync Completion!");
}

void CCrowTree::OnBnClickedBtnCsvSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( GLCrowDataMan::GetInstance().SaveCsvFile( (CWnd*)this ) == S_OK )
		MessageBox( _T("CSV Save Success"), _T("Success"), MB_OK );
}

void CCrowTree::OnBnClickedBtnCsvLoad()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( GLCrowDataMan::GetInstance().LoadCsvFile( (CWnd*)this ) == S_OK )
	{
		if( m_pSheetTab )
			m_pSheetTab->UpdateSuitTreePage();
		OnBnClickedButtonReloadStrtbl();
		MessageBox( _T("CSV Load Success"), _T("Success"), MB_OK );
	}
}

void CCrowTree::SetReadMode( CMenu* pMenu )
{
//  읽기전용 모드일때만 실행한다. 
//  리소스 추가시 수정 요망 
#ifdef READTOOL_PARAM

	if ( pMenu ) 
	{
		pMenu->EnableMenuItem ( ID_MENUITEM_ADD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		pMenu->EnableMenuItem ( ID_MENUITEM_DEL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		pMenu->EnableMenuItem ( ID_MENUITEM_COPY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		pMenu->EnableMenuItem ( ID_MENUITEM_PASTE_NEW, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
		pMenu->EnableMenuItem ( ID_MENUITEM_PASTE_VALUE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED );
	}
	else 
	{
		SetWin_ShowWindow( this, IDC_BTN_CSV_LOAD, FALSE );
		SetWin_ShowWindow( this, IDC_BUTTON_UPDATE_SYNC, FALSE );
		SetWin_ShowWindow( this, IDC_BUTTON_RELOAD_STRTBL, FALSE );
		SetWin_ShowWindow( this, IDC_BUTTON_STRTBL_SYNC, FALSE );
	}

#endif
}

void CCrowTree::OnNMDblclkTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;

	HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();

	PCROWDATA pItemNode = NULL;
	if ( hItem )
	{
		pItemNode = (PCROWDATA)m_ctrlItemTree.GetItemData ( hItem );
		if( pItemNode ) 
		{
			SetSelectItem ( pItemNode );
			m_pSheetTab->ActiveCrowDataPage ( CROWTREEPAGE, pItemNode );
		}
	}	

}

// 데이터 통합 이후에는 지우자;
const bool GetChildElement( TIXMLOLD::TiXmlElement** outXmlElem, 
						   TIXMLOLD::TiXmlElement* rootElem,
						   const char* atrbName,
						   const char* atrbData)
{
	CString resTokenChild;
	CString strCompare;
	CString strChild, strElem, strValue, strAtrbData;

	int nPosChild = 0;
	int nPosFormat = 0;

	if ( !rootElem ) return false;

	TIXMLOLD::TiXmlElement* nodeElem = rootElem; 
	TIXMLOLD::TiXmlElement* nodeElemChild = rootElem->FirstChildElement(); 	

	while ( nodeElemChild )
	{
		TIXMLOLD::TiXmlAttribute* atrb = nodeElemChild->FirstAttribute();
		while ( atrb )
		{
			if ( strcmp( atrb->Name(),  atrbName ) == 0 && 
				strcmp( atrb->Value(), atrbData ) == 0)
			{
				(*outXmlElem) = nodeElemChild;
				return true;
			}

			strAtrbData = atrb->Value();
			atrb = atrb->Next();
		}
		if ( nodeElemChild == nodeElemChild->NextSiblingElement() )
			break;

		nodeElemChild = nodeElemChild->NextSiblingElement();
	}
	return false;
}

void CCrowTree::OnBnClickedBtnXmlConvert()
{
	std::string strFullPath = sc::getAppPath();
	strFullPath.append("\\CrowStrTable.xml");

	// 컨버팅;
	TIXMLOLD::TiXmlDocument XmlDoc;
	TIXMLOLD::TiXmlElement* pRootElem = NULL;
	if ( XmlDoc.LoadFile(strFullPath.c_str(), TIXMLOLD::TIXML_ENCODING_UTF8) )
	{
		pRootElem = XmlDoc.RootElement();
	}
	else
	{
		TIXMLOLD::TiXmlDeclaration*	XmlDecl = new TIXMLOLD::TiXmlDeclaration( "1.0", "utf-8", "");
		XmlDoc.LinkEndChild( XmlDecl );

		pRootElem = new TIXMLOLD::TiXmlElement("TEXT");
		XmlDoc.LinkEndChild(pRootElem);
	}

	int i, j, iLangFlag = language::ProvidelangToLangflag(RANPARAM::emProvideLangFlag);
	
	for ( i = 0; i < GLCrowDataMan::MAX_CROW_MID; ++i )
	{
		for ( j = 0; j < GLCrowDataMan::MAX_CROW_SID; ++j )
		{
			SCROWDATA* const pCrowData = GLCrowDataMan::GetInstance().GetCrowData(i, j);
			if ( pCrowData != NULL )
			{
				CString strNameKey;
				strNameKey.Format("CN_%03d_%03d", i, j);
				TIXMLOLD::TiXmlElement* pElem, *pChildElem;
				if ( !GetChildElement(&pElem, pRootElem, "Id", strNameKey.GetString()) )
				{
					pElem = new TIXMLOLD::TiXmlElement("SENTENSE");
					pElem->SetAttribute("Ver", 1);
					pElem->SetAttribute("Id", strNameKey.GetString());
					pRootElem->LinkEndChild(pElem);
				}
				if ( !GetChildElement(&pChildElem, pElem, "Lang", language::strProvideLanguage[RANPARAM::emProvideLangFlag]) )
				{
					pChildElem = new TIXMLOLD::TiXmlElement("VALUE");
					pChildElem->SetAttribute("Lang", language::strProvideLanguage[RANPARAM::emProvideLangFlag]);
					pElem->LinkEndChild(pChildElem);
				}
				else
				{
					pChildElem->RemoveChild(pChildElem->FirstChild());
				}

				char	szUTF8[1024] = {0};
				wchar_t	wsUTF8[512] = {0};
				if ( pCrowData->GetName() != NULL && strlen(pCrowData->GetName()) != 0)
				{
					_tcscpy(szUTF8, pCrowData->GetName());
				}
				else
				{
					_tcscpy(szUTF8, "skip");
				}

				MultiByteToWideChar(CD3DFontPar::nCodePage[iLangFlag],
					0,szUTF8, 
					strlen(szUTF8)
					,wsUTF8,sizeof(wsUTF8)/sizeof(wsUTF8[0]));			
				WideCharToMultiByte(CP_UTF8, 0, wsUTF8, -1, szUTF8,
					sizeof(szUTF8)/sizeof(szUTF8[0]), NULL,NULL );

				TIXMLOLD::TiXmlText* pXmlText = new TIXMLOLD::TiXmlText(szUTF8);
				pChildElem->LinkEndChild(pXmlText);
			}
		}
	}
	XmlDoc.SaveFile(strFullPath);
	std::string Msg(
		sc::string::format(
		"%1% 에 저장되었습니다.",
		strFullPath));
	MessageBox(Msg.c_str(), "Success", MB_OK);
}