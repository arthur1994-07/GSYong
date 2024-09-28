// QuestTree.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../../RanLogic/GLogicData.h"
#include "../QuestEdit.h"
#include "../Dialogue/InputIDDlg.h"
#include "../QuestEditDlg.h"
#include "./SheetWithTab.h"
#include "./QuestTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CQuestTree ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CQuestTree, CPropertyPage)
CQuestTree::CQuestTree(LOGFONT logfont)
	: CPropertyPage(CQuestTree::IDD)
    , m_pFont(NULL)
    , m_SchoolRadioButton(3)
{
	m_bDlgInit = FALSE;
	m_pDummyQuestNode = new QUESTNODE;
	m_pDummyQuestNode->pQUEST = new GLQUEST;
	m_bDummyHasQuest = FALSE;

	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CQuestTree::~CQuestTree()
{
	SAFE_DELETE(m_pDummyQuestNode->pQUEST);
	SAFE_DELETE(m_pDummyQuestNode);	
	SAFE_DELETE(m_pFont);
}

void CQuestTree::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_ITEM, m_ctrlQuestTree);
    DDX_Radio(pDX, IDC_RADIO_QUEST_SCHOOL, (int&) m_SchoolRadioButton);
}


BEGIN_MESSAGE_MAP(CQuestTree, CPropertyPage)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_ITEM, OnNMRclickTreeItem)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_ITEM, OnNMDblclkTreeItem)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_RADIO_QUEST_SCHOOL, &CQuestTree::OnBnClickedRadioQuestSchool)
    ON_BN_CLICKED(IDC_RADIO2, &CQuestTree::OnBnClickedRadio2)
    ON_BN_CLICKED(IDC_RADIO3, &CQuestTree::OnBnClickedRadio3)
    ON_BN_CLICKED(IDC_RADIO4, &CQuestTree::OnBnClickedRadio4)
    ON_BN_CLICKED(IDC_CHECK_CLASS_FIGHT, &CQuestTree::OnBnClickedCheckClassFight)
    ON_BN_CLICKED(IDC_CHECK_CLASS_ARCHER, &CQuestTree::OnBnClickedCheckClassArcher)
    ON_BN_CLICKED(IDC_CHECK_CLASS_FENCING, &CQuestTree::OnBnClickedCheckClassFencing)
    ON_BN_CLICKED(IDC_CHECK_CLASS_SPIRIT, &CQuestTree::OnBnClickedCheckClassSpirit)
    ON_BN_CLICKED(IDC_CHECK_CLASS_EXTREAM, &CQuestTree::OnBnClickedCheckClassExtream)
    ON_BN_CLICKED(IDC_CHECK_CLASS_SCIENTIST, &CQuestTree::OnBnClickedCheckClassScientist)
    ON_BN_CLICKED(IDC_CHECK_CLASS_ASSASSIN, &CQuestTree::OnBnClickedCheckClassAssassin)
    ON_BN_CLICKED(IDC_CHECK_CLASS_TRICKER, &CQuestTree::OnBnClickedCheckClassEtc)
	ON_BN_CLICKED(IDC_CHECK_CLASS_ACTOR, &CQuestTree::OnBnClickedCheckClassActor)
END_MESSAGE_MAP()


// CQuestTree �޽��� ó�����Դϴ�.

BOOL CQuestTree::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// PropertyPage ��Ʈ ���� ����
	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_bDlgInit = TRUE;
	
    m_SchoolRadioButton = 3;
    CheckDlgButton(IDC_CHECK_CLASS_FIGHT, TRUE);
    CheckDlgButton(IDC_CHECK_CLASS_ARCHER, TRUE);
    CheckDlgButton(IDC_CHECK_CLASS_FENCING, TRUE);
    CheckDlgButton(IDC_CHECK_CLASS_SPIRIT, TRUE);
    CheckDlgButton(IDC_CHECK_CLASS_EXTREAM, TRUE);
    CheckDlgButton(IDC_CHECK_CLASS_SCIENTIST, TRUE);
    CheckDlgButton(IDC_CHECK_CLASS_ASSASSIN, TRUE);
    CheckDlgButton(IDC_CHECK_CLASS_TRICKER, TRUE);
	CheckDlgButton(IDC_CHECK_CLASS_ACTOR, TRUE);

    UpdateData(FALSE);

    UpdateTree();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CQuestTree::OnNMRclickTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
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

void CQuestTree::RunSelectMenu ( int nSelect )
{
	HTREEITEM hItem = m_ctrlQuestTree.GetSelectedItem();

    CQuestEditDlg* pDlg = (CQuestEditDlg*) m_pSheetTab->m_pWndParent;
    
	PQUESTNODE pQuestNode = NULL;
	if ( hItem )
	{
		pQuestNode = (PQUESTNODE)m_ctrlQuestTree.GetItemData ( hItem );
	}

	switch ( nSelect )
	{
	case ID_MENUITEM_QUEST_MODIFY:
		{
			if (GetSelectItem())
			{				
				m_pSheetTab->ActiveQuestPage(QUESTTREE_PAGE, pQuestNode);
                if (pDlg)
                    pDlg->SetNpcAction(pQuestNode->pQUEST);
			}			
		}
		break;
	case ID_MENUITEM_QUESTSTEP_MODIFY:
		{
			if (GetSelectItem())
			{
				m_pSheetTab->ActiveQuestStepPage(QUESTTREE_PAGE, pQuestNode);
                if (pDlg)
                    pDlg->SetNpcAction(pQuestNode->pQUEST);
			}			
		}
		break;
	case ID_MENUITEM_ADD:
		{
			AddQuest ();
		}
		break;
	case ID_MENUITEM_DEL:
		{
			if ( GetSelectItem () )
			{
				if ( MessageBox ( "Do you want to delete?", "Question", MB_YESNO ) == IDYES )
				{
					DelQuest();
				}
			}
		}
		break;
	case ID_MENUITEM_COPY:
		{
			CopyQuest ();
		}
		break;
	case ID_MENUITEM_PASTE_NEW:
		{
			PasteQuestNew ();
		}
		break;
	case ID_MENUITEM_PASTE_VALUE:
		{
			PasteQuestValue ();
		}
		break;
	};
}

int	 CQuestTree::GetSelectMenu ( POINT pt )
{
	//	<--	���� �۵� �޴� ����
	int pMenuID = 0;
	CMenu Menu;

	int SelectionMade = -1;
	//switch ( m_SelectMenu )
	//{
	//case 0:
	//	{
		VERIFY ( Menu.LoadMenu ( IDR_MENU_QUEST_POPUP ) );

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

BOOL CQuestTree::UpdateTree()
{	
	if (!m_bDlgInit)
	{
		return FALSE;
	}

    UpdateData(TRUE);
	CleanAllQuest();
	GLQuestMan::MAPQUEST& m_mapQuestMap = GLQuestMan::GetInstance().GetMap();

	GLQuestMan::MAPQUEST_ITER iter_pos;
	DWORD dwQuestID = 0;
	PQUESTNODE pQuestNode = NULL;
	CString strItem;

	for (iter_pos = m_mapQuestMap.begin(); iter_pos != m_mapQuestMap.end(); ++iter_pos)
	{
		HTREEITEM hMainItem= NULL;
		
		dwQuestID = (DWORD) iter_pos->first;	
		pQuestNode = (PQUESTNODE) &iter_pos->second;
        if (pQuestNode) 
		{
            GLQUEST* pQuest = pQuestNode->pQUEST;
            
            // School ����
            if (m_SchoolRadioButton != 3)
            {
                if (pQuest->m_sSTARTOPT.wSCHOOL != MAX_SCHOOL && m_SchoolRadioButton != pQuest->m_sSTARTOPT.wSCHOOL)
                    continue;
            }

            // Class ����
            if (!ClassFilter(pQuest))
                continue;            
            
			strItem = GetFormatName(pQuestNode);
			hMainItem = m_ctrlQuestTree.InsertItem(strItem, m_TreeRoot);
			m_ctrlQuestTree.SetItemData(hMainItem, (DWORD_PTR) pQuestNode);
		}
	}

	return TRUE;
}

bool CQuestTree::ClassFilter(GLQUEST* pQuest)
{
    DWORD ChaClass = pQuest->m_sSTARTOPT.dwCLASS;

    if (IsDlgButtonChecked(IDC_CHECK_CLASS_FIGHT))
    {
        if (ChaClass & GLCC_FIGHTER)
            return true;
    }

    if (IsDlgButtonChecked(IDC_CHECK_CLASS_ARCHER))
    {
        if (ChaClass & GLCC_ARCHER)
            return true;
    }

    if (IsDlgButtonChecked(IDC_CHECK_CLASS_FENCING))
    {
        if (ChaClass & GLCC_ARMS)
            return true;
    }

    if (IsDlgButtonChecked(IDC_CHECK_CLASS_SPIRIT))
    {
        if (ChaClass & GLCC_SPIRIT)
            return true;
    }

    if (IsDlgButtonChecked(IDC_CHECK_CLASS_EXTREAM))
    {
        if (ChaClass & GLCC_EXTREME)
            return true;
    }

    if (IsDlgButtonChecked(IDC_CHECK_CLASS_SCIENTIST))
    {
        if (ChaClass & GLCC_SCIENTIST)
            return true;
    }

    if (IsDlgButtonChecked(IDC_CHECK_CLASS_ASSASSIN))
    {
        if (ChaClass & GLCC_ASSASSIN)
            return true;
    }

    if (IsDlgButtonChecked(IDC_CHECK_CLASS_TRICKER))
    {
        if (ChaClass & GLCC_TRICKER)
            return true;
    }

	if (IsDlgButtonChecked(IDC_CHECK_CLASS_ACTOR))
	{
		if (ChaClass & GLCC_ACTOR)
			return true;
	}

    return false;
}

void CQuestTree::CleanAllQuest ()
{
	if ( !m_bDlgInit )
	{
		return ;
	}

	m_ctrlQuestTree.DeleteAllItems();
	m_TreeRoot = m_ctrlQuestTree.InsertItem("Root Node");
}

BOOL CQuestTree::CopyQuest()
{
	HTREEITEM hMainItem = m_ctrlQuestTree.GetSelectedItem ();
	if ( hMainItem && hMainItem != m_TreeRoot )
	{
		PQUESTNODE pQuestNode = (PQUESTNODE)m_ctrlQuestTree.GetItemData ( hMainItem );
		*(m_pDummyQuestNode->pQUEST) = *(pQuestNode->pQUEST);
		m_pDummyQuestNode->strFILE = pQuestNode->strFILE;
		m_bDummyHasQuest = TRUE;
		return TRUE;
	}

	return FALSE;
}

void CQuestTree::SetSelectItem ( PQUESTNODE pQuestNode )
{
	m_pQuestNode = pQuestNode;
}

PQUESTNODE CQuestTree::GetSelectItem ()
{
	return m_pQuestNode;
}


BOOL CQuestTree::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	switch ( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			if ( pMsg->wParam == VK_RETURN )
			{
				HTREEITEM hItem = m_ctrlQuestTree.GetSelectedItem ();
				if ( !hItem ) return TRUE;

				PQUESTNODE pQuestNode = (PQUESTNODE)m_ctrlQuestTree.GetItemData ( hItem );
				if ( !pQuestNode )
				{
					return TRUE;
				}

				m_pSheetTab->ActiveQuestPage ( QUEST_PAGE, pQuestNode );
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
					HTREEITEM hItem = m_ctrlQuestTree.GetSelectedItem ();
					PQUESTNODE pQuestNode = (PQUESTNODE)m_ctrlQuestTree.GetItemData ( hItem );
					SetSelectItem ( pQuestNode );

					RunSelectMenu ( ID_MENUITEM_PASTE_NEW );
				}
			}

			if ( ::GetKeyState ( VK_CONTROL ) < 0 )
			{
				if ( pMsg->wParam == 'B' )
				{
					HTREEITEM hItem = m_ctrlQuestTree.GetSelectedItem ();
					PQUESTNODE pQuestNode = (PQUESTNODE)m_ctrlQuestTree.GetItemData ( hItem );
					SetSelectItem ( pQuestNode );

					RunSelectMenu ( ID_MENUITEM_PASTE_VALUE );
				}
			}

		}
		break;
	}

	return CPropertyPage::PreTranslateMessage(pMsg);
}

void CQuestTree::SetReadMode( CMenu* pMenu )
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



void CQuestTree::OnNMDblclkTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	*pResult = 0;

	HTREEITEM hItem = m_ctrlQuestTree.GetSelectedItem ();

	PQUESTNODE pQuestNode = NULL;
	if ( hItem )
	{
		pQuestNode = (PQUESTNODE) m_ctrlQuestTree.GetItemData(hItem);
		if (pQuestNode) 
		{
            CQuestEditDlg* pDlg = (CQuestEditDlg*) m_pSheetTab->m_pWndParent;
            if (pDlg)
                pDlg->SetNpcAction(pQuestNode->pQUEST);

			SetSelectItem(pQuestNode);
			m_pSheetTab->ActiveQuestPage(QUESTTREE_PAGE, pQuestNode);
		}
	}	
}

void CQuestTree::OnSize(UINT nType, int cx, int cy)
{
    CPropertyPage::OnSize(nType, cx, cy);

    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
    CRect Pos;
    GetWindowRect(Pos);
    ScreenToClient(&Pos);
    SetWindowPos(NULL, Pos.top, Pos.left, cx, Pos.bottom-Pos.top, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void CQuestTree::OnBnClickedRadioQuestSchool()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    UpdateTree();
}

void CQuestTree::OnBnClickedRadio2()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    UpdateTree();
}

void CQuestTree::OnBnClickedRadio3()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    UpdateTree();
}

void CQuestTree::OnBnClickedRadio4()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    UpdateTree();
}

void CQuestTree::OnBnClickedCheckClassFight()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    UpdateTree();
}

void CQuestTree::OnBnClickedCheckClassArcher()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    UpdateTree();
}

void CQuestTree::OnBnClickedCheckClassFencing()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    UpdateTree();
}

void CQuestTree::OnBnClickedCheckClassSpirit()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    UpdateTree();
}

void CQuestTree::OnBnClickedCheckClassExtream()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    UpdateTree();
}

void CQuestTree::OnBnClickedCheckClassScientist()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    UpdateTree();
}

void CQuestTree::OnBnClickedCheckClassAssassin()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    UpdateTree();
}

void CQuestTree::OnBnClickedCheckClassEtc()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    UpdateTree();
}

void CQuestTree::OnBnClickedCheckClassActor()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateTree();
}
