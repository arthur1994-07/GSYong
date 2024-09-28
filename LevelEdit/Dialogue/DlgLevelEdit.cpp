// DlgLevelEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../LevelEdit.h"
#include "../Edit/LevelSheetWithTab.h"

#include "./DlgLevelEdit.h"


// CDlgLevelEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgLevelEdit, CDialog)
CDlgLevelEdit::CDlgLevelEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgLevelEdit::IDD, pParent),
	m_pSheetTab(NULL),
	m_bINIT(NULL)
{
}

CDlgLevelEdit::~CDlgLevelEdit()
{
}

void CDlgLevelEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PAGE, m_comboPage);
}


BEGIN_MESSAGE_MAP(CDlgLevelEdit, CDialog)
ON_CBN_SELCHANGE(IDC_COMBO_PAGE, OnCbnSelchangeComboPage)
END_MESSAGE_MAP()


// CDlgLevelEdit 메시지 처리기입니다.

BOOL CDlgLevelEdit::OnInitDialog()
{
	CDialog::OnInitDialog();


	CRect SheetRect;
	CWnd *pWnd;

	//	Note : Sheet 1 를 만든다.
	//
	pWnd = GetDlgItem ( IDC_STATIC_SHEETTAP );
	pWnd->GetWindowRect ( SheetRect );
	ScreenToClient ( &SheetRect );

	m_pSheetTab = new CsheetWithTab ( SheetRect, this );
	m_pSheetTab->Create( this, WS_CHILD|WS_VISIBLE|WS_TABSTOP, 0 );
	m_pSheetTab->SetActivePage ( MAINPAGEINDEX );

	int nIndex;
	nIndex = m_comboPage.AddString ( g_strEditSheet[MAINPAGEINDEX] );
	m_comboPage.SetItemData ( nIndex, MAINPAGEINDEX );

	//	Note : Mob Setting 를 만든다.
	//	순서 유지 ~~
	nIndex = m_comboPage.AddString ( g_strEditSheet[MOBSETPAGEINDEX] );
	m_comboPage.SetItemData ( nIndex, MOBSETPAGEINDEX );

	nIndex = m_comboPage.AddString ( g_strEditSheet[MATERIALSETPAGEINDEX] );
	m_comboPage.SetItemData ( nIndex, MOBSETPAGEINDEX );

	nIndex = m_comboPage.AddString ( g_strEditSheet[LANDMARKINDEX] );
	m_comboPage.SetItemData ( nIndex, LANDMARKINDEX );

	nIndex = m_comboPage.AddString ( g_strEditSheet[LANDGATEINDEX] );
	m_comboPage.SetItemData ( nIndex, LANDGATEINDEX );

	nIndex = m_comboPage.AddString ( g_strEditSheet[SINGLEFFECTINDEX] );
	m_comboPage.SetItemData ( nIndex, SINGLEFFECTINDEX );

	nIndex = m_comboPage.AddString ( g_strEditSheet[SINGLEFFECTLAYERINDEX] );
	m_comboPage.SetItemData ( nIndex, SINGLEFFECTLAYERINDEX );

	nIndex = m_comboPage.AddString ( g_strEditSheet[REQUIREINDEX] );
	m_comboPage.SetItemData ( nIndex, REQUIREINDEX );

	nIndex = m_comboPage.AddString ( g_strEditSheet[ETCFUNCINDEX] );
	m_comboPage.SetItemData ( nIndex, ETCFUNCINDEX );

	nIndex = m_comboPage.AddString ( g_strEditSheet[COLLISIONINDEX] );
	m_comboPage.SetItemData ( nIndex, COLLISIONINDEX );

	nIndex = m_comboPage.AddString ( g_strEditSheet[COLLISIONLAYERINDEX] );
	m_comboPage.SetItemData ( nIndex, COLLISIONLAYERINDEX );


	m_comboPage.SetCurSel ( MAINPAGEINDEX );

	m_bINIT = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgLevelEdit::PostNcDestroy()
{
	SAFE_DELETE(m_pSheetTab);

	CDialog::PostNcDestroy();
}

void CDlgLevelEdit::OnCbnSelchangeComboPage()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DWORD nIndex = m_comboPage.GetCurSel ();
	if ( nIndex == -1 )		return;

	CString strIndex;
	m_comboPage.GetLBText( nIndex, strIndex );
	


	DWORD_PTR dwData = m_comboPage.GetItemData ( nIndex );
	m_pSheetTab->SetActivePage ( (DWORD)dwData );
	
	if ( strIndex == g_strEditSheet[MOBSETPAGEINDEX] )
	{		
		CMobSetPage::m_bMaterial = FALSE;
		m_pSheetTab->m_MobSetPage.ActiveMobSetEdit();
	}
	else if ( strIndex == g_strEditSheet[MATERIALSETPAGEINDEX]  )
	{
		CMobSetPage::m_bMaterial = TRUE;
		m_pSheetTab->m_MobSetPage.ActiveMobSetEdit();
	}
	else if ( strIndex == g_strEditSheet[LANDGATEINDEX] )
	{
		m_pSheetTab->m_LandGatePage.ActiveLandGateEdit();
	}
	else if ( strIndex == g_strEditSheet[SINGLEFFECTINDEX] )
	{
		m_pSheetTab->m_SingleEffect.ActiveSingleEffectIndex();
	}
	else if ( strIndex == g_strEditSheet[SINGLEFFECTLAYERINDEX] )
	{
		m_pSheetTab->m_SingleEffectLayer.ActiveSingleEffectLayerIndex();
	}
	else if ( strIndex == g_strEditSheet[COLLISIONINDEX] )
	{
		m_pSheetTab->m_Collision.ActiveCollisionIndex();
	}
	else if ( strIndex == g_strEditSheet[COLLISIONLAYERINDEX] )
	{
		m_pSheetTab->m_CollisionLayer.ActiveCollisionLayerIndex();
	}
}

void CDlgLevelEdit::UpdatePage ( BOOL bMainPage )
{
	if ( m_bINIT )
	{
		m_pSheetTab->UpdatePage ();
		if ( bMainPage )	m_pSheetTab->SetActivePage(MAINPAGEINDEX);

		m_comboPage.SetCurSel ( MAINPAGEINDEX );
	}
}

void CDlgLevelEdit::UpdateKeyBoardMouse()
{
	if ( !m_pSheetTab )
		return;

	m_pSheetTab->UpdateKeyBoardMouse();
}
