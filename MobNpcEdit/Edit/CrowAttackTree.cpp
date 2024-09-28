#include "stdafx.h"

#include "../MobNpcEdit.h"
#include "./SheetWithTab.h"
#include "../LogDialog.h"
#include "./CrowAttackTree.h"

#include "../../RanLogic/GLStringTable.h"
#include "../../RanLogic/Crow/GLCrowDataMan.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CCrowAttackTree, CPropertyPage)
CCrowAttackTree::CCrowAttackTree( LOGFONT logfont )
: CPropertyPage( CCrowAttackTree::IDD )
, m_pCrowData( NULL )
, m_pSheetTab( NULL )
, m_sSelectPosition( SNATIVEID( false ) )
{
	m_bDlgInit = FALSE;

	m_pDummyCrowData = new SCROWDATA;
}

CCrowAttackTree::~CCrowAttackTree()
{
	SAFE_DELETE ( m_pDummyCrowData );
}

void CCrowAttackTree::DoDataExchange( CDataExchange* pDX )
{
	CPropertyPage::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_CROWATTACK_TREE, m_ctrlItemTree );
}


BEGIN_MESSAGE_MAP( CCrowAttackTree, CPropertyPage )
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_PREV, OnBnClickedButtonPrev)
	ON_NOTIFY(NM_DBLCLK, IDC_CROWATTACK_TREE, OnNMDblclkTreeItem)
END_MESSAGE_MAP()

BOOL CCrowAttackTree::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// 초기화;
	m_bDlgInit = TRUE;

	InitDefaultCtrls ();
	UpdateItems ();

	SetReadMode ( NULL );

	return TRUE;
}

void CCrowAttackTree::InitAllItem ()
{
	m_pCrowData = NULL;
}

BOOL CCrowAttackTree::SetCrowData ( SCROWDATA* pCrowData )
{
	if ( m_pCrowData )
	{
		return FALSE;
	}

	m_pCrowData = pCrowData;

	// 더미로 가져가기;
	m_pDummyCrowData->Assign( *m_pCrowData );

	if ( m_bDlgInit )
	{
		InitDefaultCtrls ();
		UpdateItems ();		
	}

	return TRUE;
}

void CCrowAttackTree::InitDefaultCtrls ()
{
	// UI Ctrl들의 초기화;
}

void CCrowAttackTree::UpdateItems ()
{
	if ( !m_bDlgInit )
	{
		return;
	}

	CleanAllItem();

	m_ctrlItemTree.SetItemData ( m_TreeRoot, (DWORD_PTR)SNATIVEID::ID_NULL );

	// Basic Crow Attack Slot;
	HTREEITEM hBasic = NULL;
	hBasic = m_ctrlItemTree.InsertItem ( "Basic Skill Slot", m_TreeRoot );
	m_ctrlItemTree.SetItemData ( hBasic, (DWORD_PTR)SNATIVEID::ID_NULL );

	for ( unsigned int i=0; i<m_pDummyCrowData->m_vecCrowAttackPage.size(); ++i )
	{
		HTREEITEM hSub = NULL;
		SNATIVEID sPosition( 0, i );
		//CROWATTACK_VEC* pTempCrowAttack = &m_pCrowData->m_vecCrowAttackPage.at( i );
		hSub = m_ctrlItemTree.InsertItem ( sc::string::format( "Basic Slot %1%", i+1 ).c_str(), hBasic );
		m_ctrlItemTree.SetItemData ( hSub, (DWORD_PTR)sPosition.dwID );
	}

	// Event Crow Attack Slot;
	HTREEITEM hEvent = NULL;
	hEvent = m_ctrlItemTree.InsertItem ( "Event Skill Slot", m_TreeRoot );
	m_ctrlItemTree.SetItemData ( hEvent, (DWORD_PTR)SNATIVEID::ID_NULL );

	for ( unsigned int i=0; i<m_pDummyCrowData->m_vecCrowAttackEventPage.size(); ++i )
	{
		HTREEITEM hSub = NULL;
		SNATIVEID sPosition( 1, i );
		//CROWATTACK_VEC* pTempCrowAttack = &m_pCrowData->m_vecCrowAttackEventPage.at( i );
		hSub = m_ctrlItemTree.InsertItem ( sc::string::format( "Event Slot %1%", i+1 ).c_str(), hEvent );
		m_ctrlItemTree.SetItemData ( hSub, (DWORD_PTR)sPosition.dwID );
	}
}

// Item들을 초기화한다;
void CCrowAttackTree::CleanAllItem ()
{
	if ( !m_bDlgInit )
	{
		return ;
	}

	m_ctrlItemTree.DeleteAllItems ();
	m_TreeRoot = m_ctrlItemTree.InsertItem ( "Root Node" );
}

void CCrowAttackTree::SetSelectItem ( const SNATIVEID& sSelectPosition )
{
	m_sSelectPosition = sSelectPosition;
}

const SNATIVEID& CCrowAttackTree::GetSelectItem ()
{
	return m_sSelectPosition;
}

// 읽기 전용 모드;
void CCrowAttackTree::SetReadMode( CMenu* pMenu )
{
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

void CCrowAttackTree::OnBnClickedButtonCancel()
{
	m_pSheetTab->ActiveCrowDataPage ( m_CallPage, m_pCrowData );

	m_pCrowData = NULL;
}

void CCrowAttackTree::OnBnClickedButtonOk()
{
	GLCrowDataMan::GetInstance().InsertCrowData ( m_pDummyCrowData->sNativeID, m_pDummyCrowData );

	m_pSheetTab->ActiveCrowDataPage ( m_CallPage, m_pCrowData );

	m_pCrowData = NULL;
}

void CCrowAttackTree::OnBnClickedButtonPrev()
{
	GLCrowDataMan::GetInstance().InsertCrowData ( m_pDummyCrowData->sNativeID, m_pDummyCrowData );

	m_pSheetTab->ActiveCrowDataPage ( m_CallPage, m_pCrowData );

	m_pCrowData = NULL;
}

// Tree Item 더블클릭;
void CCrowAttackTree::OnNMDblclkTreeItem ( NMHDR *pNMHDR, LRESULT *pResult )
{
	*pResult = 0;

	HTREEITEM hItem = m_ctrlItemTree.GetSelectedItem ();

	//CROWATTACK_VEC* pItemNode = NULL;
	if ( hItem )
	{
		//pItemNode = (CROWATTACK_VEC*)m_ctrlItemTree.GetItemData( hItem );
		DWORD dwID = (DWORD)m_ctrlItemTree.GetItemData( hItem );
		if ( SNATIVEID::ID_NULL != dwID ) 
		{
			SetSelectItem ( dwID );
			m_pSheetTab->ActiveCrowAttackPage ( m_CallPage, m_pCrowData, dwID );

			m_pCrowData = NULL;
		}
	}
}