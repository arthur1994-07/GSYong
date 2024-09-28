// QuestTree.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "./TreeSpecialActivity.h"
#include "./SheetWithTab.h"
#include "./DlgSpecialActivitySet.h"

#include "../../RanLogic/Activity/ActivityVerifier.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../enginelib/GUInterface/GameTextControl.h"

#include <boost/tokenizer.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CQuestTree 대화 상자입니다.

IMPLEMENT_DYNAMIC(TreeSpecialActivity, CPropertyPage)
TreeSpecialActivity::TreeSpecialActivity( LOGFONT logfont )
	: CPropertyPage(TreeSpecialActivity::IDD),
	m_pFont ( NULL )
{
	m_bDlgInit = FALSE;

    m_ItemNodeID = false;
	m_DummyItemNodeID = false;
	m_bDummyHasItem = FALSE;

	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

TreeSpecialActivity::~TreeSpecialActivity()
{
    SAFE_DELETE( m_pFont );
}

void TreeSpecialActivity::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE_ITEM, m_ctrlSpcialTree);
    DDX_Control(pDX, IDC_EDIT_CROWSALE_FILENAME, m_ebSalesFilename);
}


BEGIN_MESSAGE_MAP(TreeSpecialActivity, CPropertyPage)
    ON_NOTIFY(NM_DBLCLK, IDC_TREE_ITEM, &TreeSpecialActivity::OnNMDblclkTreeItem)
    ON_NOTIFY(NM_RCLICK, IDC_TREE_ITEM, &TreeSpecialActivity::OnNMRclickTreeItem)
    ON_EN_KILLFOCUS(IDC_EDIT_CROWSALE_FILENAME, &TreeSpecialActivity::OnNMKillFocusCrowsalesEditbox)
END_MESSAGE_MAP()

// CQuestTree 메시지 처리기입니다.

BOOL TreeSpecialActivity::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// PropertyPage 폰트 강제 변경
	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_bDlgInit = TRUE;

    UpdateTree();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


HRESULT TreeSpecialActivity::UpdateTree()
{
	if ( !m_bDlgInit )
	{
		return FALSE;
	}

	CleanAllItem ();

    HTREEITEM hMainItem[ACTIVITY_CATEGORY_SIZE];
    HTREEITEM hSubItem = NULL;

    for ( int i = 0; i < ACTIVITY_CATEGORY_SIZE; ++i ) 
    {
        SNATIVEID sID(false);
        sID.wMainID = i;
 
		if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
			hMainItem[i] = m_ctrlSpcialTree.InsertItem(CGameTextMan::GetInstance().GetCommentText("ACTIVITY_CATEGORY",i).GetString());
		else
			hMainItem[i] = m_ctrlSpcialTree.InsertItem ( COMMENT::ACTIVITY_CATEGORY[i].c_str() );

		m_ctrlSpcialTree.SetItemData ( hMainItem[i],	(DWORD_PTR)sID.dwID );
    }

    MMACTIVITIES& mmActivityData = ActivityBaseData::GetInstance()->GetData();

    ITER_MMACTIVITIES pos = mmActivityData.begin();
    ITER_MMACTIVITIES pos_end = mmActivityData.end();

    VEC_ACTIVITIES vecActivity;

    for ( ; pos != pos_end; ++pos )
    {
        Activity sActivity = pos->second;
        
        vecActivity.push_back( sActivity );
    }

    ITER_VEC_ACTIVITIES pos_vec = vecActivity.begin();
    ITER_VEC_ACTIVITIES pos_vec_end = vecActivity.end();

    for ( ; pos_vec != pos_vec_end; ++pos_vec )
    {
        Activity& sActivity = *pos_vec;

        if ( sActivity.m_nClass != m_ActivityClass ) continue;

        ENUM_ACTIVITY_CATEGORY emType = sActivity.GetType();

        std::string strActivityName = GetFormatName ( &sActivity );
		hSubItem = m_ctrlSpcialTree.InsertItem ( strActivityName.c_str(), hMainItem[emType] );
		m_ctrlSpcialTree.SetItemData ( hSubItem, (DWORD_PTR)sActivity.m_id.dwID ); 
    }  

    // 특별활동 판매 아이템 리스트
    UpdateSalesList();

	return TRUE;
}


void TreeSpecialActivity::CleanAllItem ()
{
	if ( !m_bDlgInit )
	{
		return;
	}

	m_ctrlSpcialTree.DeleteAllItems();	
    m_ebSalesFilename.SetWindowText ("");
}

void TreeSpecialActivity::InitData ( ENUM_ACTIVITY_CLASS _ActivityClass )
{
    m_ActivityClass = _ActivityClass;
}

std::string TreeSpecialActivity::GetFormatName ( Activity* pActivity )
{
    std::string strItem;

    if ( !pActivity )
    {
        strItem = "None Itme";
        return strItem;
    }

    strItem += sc::string::format( "<%1$04d/%2$04d>", pActivity->m_id.wMainID, pActivity->m_id.wSubID );
    strItem += pActivity->GetTitle();

    return strItem;
}

void TreeSpecialActivity::UpdateName ()
{
	HTREEITEM hItem = m_ctrlSpcialTree.GetSelectedItem ();
	if ( !hItem )	return;
	
	Activity* pActivity = ActivityBaseData::GetInstance()->GetActivity ( m_ItemNodeID );
	if ( !pActivity )	return;

	m_ctrlSpcialTree.SetItemText ( hItem, GetFormatName ( pActivity ).c_str() );
}

void TreeSpecialActivity::UpdateSalesList ()
{
    std::string strFilenames("");

    MMACTIVITYSALESITEM& mmActivitySaleItem = ActivityBaseData::GetInstance()->GetSalesItemData ();

    RANGE_MMACTIVITYSALESITEM Range = mmActivitySaleItem.equal_range( m_ActivityClass );
    for ( ITER_MMACTIVITYSALESITEM iter = Range.first; iter != Range.second; ++iter )
    {
        SACTIVITYSALESITEM& sSalesItem = iter->second;
        strFilenames += sSalesItem.strFILENAME;

        ITER_MMACTIVITYSALESITEM NextIter = iter;
        if ( ++NextIter != Range.second )
        {
            strFilenames += "; ";
        }
    }

    m_ebSalesFilename.SetWindowText ( strFilenames.c_str() );
}

void TreeSpecialActivity::AddItem()
{
    DlgSpecialActivitySet dlg;

    dlg.SetModify(false);

    if ( dlg.DoModal() == IDOK )
    {
        // ID 부여
        Activity cActivity = dlg.m_Activity;       
        cActivity.m_nClass = m_ActivityClass;

        ActivityBaseData::GetInstance()->InsertActivity( cActivity );

        UpdateTree();
    }	
}

void TreeSpecialActivity::ModifyItem()
{

    Activity* pActivity = ActivityBaseData::GetInstance()->GetActivity( m_ItemNodeID );
    if ( !pActivity ) return;

    DlgSpecialActivitySet dlg;

    dlg.SetModify(true);
    dlg.m_Activity = *pActivity;

    if ( dlg.DoModal() == IDOK )
    {

        // ID 부여
        Activity cActivity = dlg.m_Activity;       

        Activity* pActivity = ActivityBaseData::GetInstance()->GetActivity( cActivity.m_id );
        if ( !pActivity ) 
        {
            AfxMessageBox( "ModifyItem ID Error" );
            return; 
        }

        *pActivity = cActivity;
        
        HTREEITEM hItem = m_ctrlSpcialTree.GetSelectedItem ();
        if ( hItem ) 
        {
            std::string strActivityName = GetFormatName ( pActivity );
            m_ctrlSpcialTree.SetItemData( hItem, (DWORD_PTR) pActivity->m_id.dwID );
            m_ctrlSpcialTree.SetItemText( hItem, strActivityName.c_str() );
        }

    }	
}

void TreeSpecialActivity::DelItem ()
{
	//	Note : 스킬 제거.
	ActivityBaseData::GetInstance()->DeleteActivity( m_ItemNodeID );

	HTREEITEM htreeitem = m_ctrlSpcialTree.GetSelectedItem ();
	if ( htreeitem != NULL )	m_ctrlSpcialTree.DeleteItem ( htreeitem );
}

void TreeSpecialActivity::CopyItem ()
{
	HTREEITEM hMainItem = m_ctrlSpcialTree.GetSelectedItem ();    
	if ( hMainItem )
	{
		m_DummyItemNodeID = m_ItemNodeID;
		m_bDummyHasItem = TRUE;
		return;
	}
}

void TreeSpecialActivity::PasteItemNew ()
{
	if ( !m_bDummyHasItem )
	{
		return;
	}	


    Activity* pActivity = ActivityBaseData::GetInstance()->GetActivity ( m_DummyItemNodeID );
    if ( !pActivity ) return;

    DlgSpecialActivitySet dlg;
	
    dlg.SetModify(false);
    dlg.m_Activity = *pActivity;
    dlg.m_Activity.m_id.wSubID = USHRT_MAX;

    if ( dlg.DoModal() == IDOK )
    {
        // ID 부여
        Activity cActivity = dlg.m_Activity;       
        cActivity.m_nClass = m_ActivityClass;

        SNATIVEID sID;
        sID.wMainID = cActivity.m_id.wMainID;

        bool m_bFindID = false;

        //  유휴 아이디 찾기
        for ( WORD i = 0; i < ActivityBaseData::MAX_SID; ++i )
        {
            sID.wSubID = i;
            Activity* pActivity = ActivityBaseData::GetInstance()->GetActivity( sID );
            if ( !pActivity )
            {
                m_bFindID = true;
                cActivity.m_id = sID;
                ActivityBaseData::GetInstance()->InsertActivity( cActivity );
                break;
            }
        }      

        if ( !m_bFindID ) 
        {
            AfxMessageBox( "No Find ID" );
        }           
        
        UpdateTree();
    }	
}

BOOL TreeSpecialActivity::OnSetActive()
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    UpdateSalesList();

    return CPropertyPage::OnSetActive();
}

void TreeSpecialActivity::OnNMDblclkTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    *pResult = 0;


	HTREEITEM hItem = m_ctrlSpcialTree.GetSelectedItem ();

	m_ItemNodeID = false;
	if ( hItem )
	{
		m_ItemNodeID = (DWORD)m_ctrlSpcialTree.GetItemData ( hItem );
        if( m_ItemNodeID.wSubID != USHRT_MAX  ) 
		{
            ModifyItem();
		}
	}
}

void TreeSpecialActivity::OnNMRclickTreeItem(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    *pResult = 0;

	switch ( GetSelectMenu() )
	{
	case ID_ACTIVITY_MODIFY:
		{
            if ( m_ItemNodeID.wSubID == USHRT_MAX )
			{
				return;
			}

			ModifyItem();
		}
		break;
	case ID_ACTIVITY_ADDITEM:
		{
			AddItem ();
		}
		break;
	case ID_ACTIVITY_DELITEM:
		{
			if ( m_ItemNodeID.wSubID == USHRT_MAX )
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
	case ID_ACTIVITY_COPYITEM:
		{
            if ( m_ItemNodeID.wSubID == USHRT_MAX )
			{
				return;
			}

			CopyItem ();
		}
		break;
	case ID_ACTIVITY_PASTE:
		{
			PasteItemNew ();
		}
		break;
	};	
}

void TreeSpecialActivity::OnNMKillFocusCrowsalesEditbox()
{
    CString scSalesItem;
    m_ebSalesFilename.GetWindowText(scSalesItem);
    std::string strSaleItem(scSalesItem.GetString());

    typedef boost::char_separator<char> CHARSEP;
    boost::tokenizer<CHARSEP> token(strSaleItem, CHARSEP(",; "));

    // 특별활동 판매 아이템 리스트
    MMACTIVITYSALESITEM& mmActivitySaleItem = ActivityBaseData::GetInstance()->GetSalesItemData ();
    mmActivitySaleItem.erase( m_ActivityClass );

    for ( boost::tokenizer<CHARSEP>::iterator iter = token.begin(); iter != token.end(); ++iter )
    {
        const std::string& strToken = (*iter);
        
        if ( strToken.empty() )
        {
            continue;
        }

        ITER_MMACTIVITYSALESITEM InsertIter = mmActivitySaleItem.insert( std::make_pair(m_ActivityClass, SACTIVITYSALESITEM()) );
        SACTIVITYSALESITEM&      sSalesItem = InsertIter->second;

        sSalesItem.strFILENAME = strToken;
    }
}

int TreeSpecialActivity::GetSelectMenu ()
{
	POINT OriginPoint, ClientPoint;

	//	<--	우클릭으로 포커스 옮기기
	GetCursorPos ( &OriginPoint );
	ClientPoint = OriginPoint;

	m_ctrlSpcialTree.ScreenToClient(&ClientPoint);

	UINT uFlag = 0;
	HTREEITEM hItem = m_ctrlSpcialTree.HitTest ( ClientPoint, &uFlag );
	m_ctrlSpcialTree.SelectItem ( hItem );

	if ( hItem && uFlag & TVHT_ONITEMLABEL )
	{		
		m_ItemNodeID = (DWORD)m_ctrlSpcialTree.GetItemData ( hItem );
	}
	else
	{
		m_ItemNodeID = NATIVEID_NULL();
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
		VERIFY ( Menu.LoadMenu ( IDR_ACTIVITY_POPUP_MENU ) );

		CMenu* pPopup = Menu.GetSubMenu ( pMenuID );
		ASSERT ( pPopup != NULL );	

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