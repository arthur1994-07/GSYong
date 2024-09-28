// SkillTreePage.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../LevelEdit.h"
#include "./SkillTreePage.h"
#include "./LevelSheetWithTab.h"

#include "../../RanLogic/Skill/GLSkill.h"

// CSkillTreePage 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSkillTreePage, CPropertyPage)
CSkillTreePage::CSkillTreePage()
	: CPropertyPage(CSkillTreePage::IDD)
{
	m_CalledPage = -1;
	m_bDlgInit = FALSE;	
	m_bNativeIDInit = FALSE;
}

CSkillTreePage::~CSkillTreePage()
{
}

void CSkillTreePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ITEM, m_ctrlItemTree);
}


BEGIN_MESSAGE_MAP(CSkillTreePage, CPropertyPage)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_OK, OnBnClickedButtonOk)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// CSkillTreePage 메시지 처리기입니다.

BOOL CSkillTreePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_bDlgInit = TRUE;
	UpdateTreeItem ();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL CSkillTreePage::UpdateTreeItem ()
{
	if ( !m_bDlgInit )
	{
		return FALSE;
	}

	m_ctrlItemTree.DeleteAllItems ();

    HTREEITEM hSelectItem = NULL;
	for ( WORD MID = 0; MID < EMSKILLCLASS_NSIZE; MID++ )
	{
		HTREEITEM hMainItem = NULL;
		HTREEITEM hSubItem = NULL;

		for ( WORD SID = 0; SID < GLSkillMan::MAX_CLASSSKILL; SID++ )
		{
			SNATIVEID sCUR_NID(MID,SID);
			PGLSKILL pItem = GLSkillMan::GetInstance().GetData ( sCUR_NID );

			if ( pItem )
			{
				if ( !hMainItem )
				{
					hMainItem = m_ctrlItemTree.InsertItem ( pItem->GetName() );
					m_ctrlItemTree.SetItemData ( hMainItem, (DWORD_PTR)pItem );					

					if ( m_bNativeIDInit && m_NativeID==sCUR_NID )
					{
						hSelectItem = hMainItem;
					}
				}
				else
				{
					hSubItem = m_ctrlItemTree.InsertItem ( pItem->GetName(), hMainItem );
					m_ctrlItemTree.SetItemData ( hSubItem, (DWORD_PTR)pItem );

					if ( m_bNativeIDInit && m_NativeID==sCUR_NID )
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

void CSkillTreePage::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyPage::OnShowWindow(bShow, nStatus);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if ( bShow )
	{
		UpdateTreeItem ();
	}
}

void CSkillTreePage::CleanAllItem ()
{
	if ( !m_bDlgInit )
	{
		return ;
	}

	m_ctrlItemTree.DeleteAllItems ();
}

void CSkillTreePage::OnBnClickedButtonOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SNATIVEID	NativeID;

	HTREEITEM hTreeItem = m_ctrlItemTree.GetSelectedItem ();
	GLSKILL* pItem = (GLSKILL*)m_ctrlItemTree.GetItemData ( hTreeItem );	

	if ( m_CalledPage == REQUIREINDEX )
	{
		SNATIVEID NativeID = pItem->m_sBASIC.sNATIVEID;
		m_pSheetTab->m_RequirePage.SetReqSkill ( NativeID );
	}

	m_pSheetTab->SetActivePage ( m_CalledPage );
	m_bNativeIDInit = FALSE;
}

void CSkillTreePage::OnBnClickedButtonCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pSheetTab->SetActivePage ( m_CalledPage );
	m_bNativeIDInit = FALSE;
}

void CSkillTreePage::SetPropertyData ( int CalledPage, SNATIVEID NativeID )
{
	m_CalledPage = CalledPage;
	m_NativeID = NativeID;

	m_bNativeIDInit = TRUE;
}
