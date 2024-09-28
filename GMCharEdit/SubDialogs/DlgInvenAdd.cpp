// DlgInvenAdd.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "../GMCharEdit.h"
#include "DlgInvenAdd.h"

#include "../DoCharEdit/PropertySheetCharEdit.h"
#include "DlgInvenElementEdit.h"

#include "../../SigmaCore/Util/DateTime.h"

#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include ".\dlginvenadd.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CDlgInvenAdd 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgInvenAdd, CDialog)
CDlgInvenAdd::CDlgInvenAdd(int _Type /* = INVEN_ADD */, CWnd* pParent /* = NULL */, std::vector<ExchangeItemHistory>* InExChangeHistory /* = NULL */)
	: CDialog(CDlgInvenAdd::IDD, pParent)
    , Type(_Type)
{
	m_nSelected = -1;

    vecExchangeItemHistory.clear();
    if ( InExChangeHistory )
        vecExchangeItemHistory = *InExChangeHistory;
}

CDlgInvenAdd::~CDlgInvenAdd()
{
}

void CDlgInvenAdd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SKILL_ADD, m_ListInvenAdd);
}


BEGIN_MESSAGE_MAP(CDlgInvenAdd, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SKILL_ADD, OnNMDblclkListSkillAdd)
END_MESSAGE_MAP()


// CDlgInvenAdd 메시지 처리기입니다.

BOOL CDlgInvenAdd::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_ListInvenAdd.SetExtendedStyle ( LVS_EX_FULLROWSELECT ); 
    InitListHead();
    SetData();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgInvenAdd::InitListHead() 
{
    if ( Type == INVEN_ADD )
    {
        LVCOLUMN Col;
        CString strTemp;
        // Num, Name, Class, Tribe, Level,
        strTemp			= "Main";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 100;
        Col.fmt			= LVCFMT_CENTER;
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(0, &Col);

        strTemp			= "Sub";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 100;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(1, &Col);

        strTemp			= "Name";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 200;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(2, &Col);
    }
    else if ( Type == GEN_TYPE )
    {
        LVCOLUMN Col;
        CString strTemp;

        strTemp			= "Gen Type";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 100;
        Col.fmt			= LVCFMT_CENTER;
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(0, &Col);

        strTemp			= "Description";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 300;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(1, &Col);
    }
    else if ( Type == EXCHANGE_FLAG )
    {
        LVCOLUMN Col;
        CString strTemp;

        strTemp			= "ExchangeFlag";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 100;
        Col.fmt			= LVCFMT_CENTER;
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(0, &Col);

        strTemp			= "Description";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 300;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(1, &Col);
    }
    else if ( Type == EXCHANGE_ITEM_HISTORY )
    {
        LVCOLUMN Col;
        CString strTemp;

        strTemp			= "ExchangeNum";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 80;
        Col.fmt			= LVCFMT_CENTER;
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(0, &Col);

        strTemp			= "NIDMain";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 80;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(1, &Col);

        strTemp			= "NIDSub";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 80;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(2, &Col);

        strTemp			= "SGNum";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 80;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(3, &Col);

        strTemp			= "SvrNum";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 80;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(4, &Col);

        strTemp			= "FldNum";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 80;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(5, &Col);

        strTemp			= "MakeType";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 80;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(6, &Col);

        strTemp			= "MakeNum";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 80;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(7, &Col);

        strTemp			= "ItemAmount";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 80;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(8, &Col);

        strTemp			= "ItemFromFlag";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 80;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(9, &Col);

        strTemp			= "ItemFrom";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 80;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(10, &Col);

        strTemp			= "ItemToFlag";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 80;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(11, &Col);

        strTemp			= "ItemTo";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 80;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(12, &Col);

        strTemp			= "ExchangeFlag";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 80;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(13, &Col);

        strTemp			= "ExchangeDate";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 80;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(14, &Col);

        strTemp			= "TradePrice";
        Col.mask		= LVCF_TEXT | LVCF_WIDTH;
        Col.cx			= 80;
        Col.fmt			= LVCFMT_CENTER;	
        Col.pszText		= strTemp.GetBuffer();
        Col.iSubItem	= 0;
        Col.iImage		= 0;
        m_ListInvenAdd.InsertColumn(15, &Col);
    }
}

void CDlgInvenAdd::SetData()
{
    if ( Type == INVEN_ADD )
    {
        CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();

        // Erase all list items
        m_ListInvenAdd.DeleteAllItems();

        m_vItem.erase(m_vItem.begin(), m_vItem.end());

        int nNum=0;

        for ( WORD i = 0; i < GLItemMan::MAX_MID; ++i )
        {
            for ( WORD j = 0; j < GLItemMan::MAX_SID; ++j )
            {
                SITEM *pItemData = GLogicData::GetInstance().GetItem ( i, j );
                if ( !pItemData )	continue;

                int nCount;
                CString strTemp;
                nCount = m_ListInvenAdd.GetItemCount();

                LV_ITEM lvItem;
                ::memset(&lvItem, 0, sizeof(LV_ITEM));

                lvItem.mask = LVIF_TEXT; // 속성 지정
                lvItem.iItem = nCount;
                lvItem.iSubItem = 0;
                strTemp.Format("%d", pItemData->sBasicOp.sNativeID.wMainID);
                lvItem.pszText = strTemp.GetBuffer();		
                m_ListInvenAdd.InsertItem(&lvItem); // 새로운 열의 시작은 InsertItem을 사용한다.

                strTemp.Format("%d", pItemData->sBasicOp.sNativeID.wSubID);
                m_ListInvenAdd.SetItemText(nNum, 1, strTemp);
                m_ListInvenAdd.SetItemText(nNum, 2, pItemData->GetName() );

                SITEM sItem;
                sItem = *pItemData;

                m_vItem.push_back(sItem);

                nNum++;
            }
        }
    }
    else if ( Type == GEN_TYPE )
    {
        SetGenTypeData();
    }
    else if ( Type == EXCHANGE_FLAG )
    {
        SetExchangeFlagData();
    }
    else if ( Type = EXCHANGE_ITEM_HISTORY )
    {

        SetExchangeItemHistoryData(vecExchangeItemHistory);
    }
}

void CDlgInvenAdd::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    UINT uSelectedCount = m_ListInvenAdd.GetSelectedCount();
    int  nItem = -1;

    // Check selected item
    if (uSelectedCount > 0)
    {
        for (UINT i=0;i < uSelectedCount;i++)
        {
	        nItem = m_ListInvenAdd.GetNextItem(nItem, LVNI_ALL | LVNI_SELECTED);
        		
	        if (nItem != -1) // 선택된 항목이 있음
	        {	
                m_vSelected.push_back(nItem);
                // m_nSelected = 1;
				m_nSelected = nItem;
            }
            else
            {
                nItem = -1;
            }
        }
    }
	OnOK();
}

void CDlgInvenAdd::OnNMDblclkListSkillAdd(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedOk();

	*pResult = 0;
}

void CDlgInvenAdd::SetGenTypeData()
{
    // Erase all list items
    m_ListInvenAdd.DeleteAllItems();

    m_vItem.erase(m_vItem.begin(), m_vItem.end());

    int nNum=0;

    int nCount;
    CString strTemp;

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", EMGEN_DEFAULT);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("Generated by default");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", EMGEN_INIT);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("Generated by initialization");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", EMGEN_SHOP);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("Generated by shop");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", EMGEN_NPC);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("Generated by NPC");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", EMGEN_QUEST);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("Generated by quest");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", EMGEN_MOB);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("Generated by monster");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", EMGEN_GMEDIT);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("Generated by GMEdit");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", EMGEN_OLD);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("Generated by old");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", EMGEN_BILLING);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("Generated by billing");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", EMGEN_GMEDIT2);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("Generated by GMEdit2");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", EMGEN_ITEMMIX);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("Generated by itemmix");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", EMGEN_GATHERING);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("Generated by gathering");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", EMGEN_SYSTEM);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("Generated by system");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", EMGEN_POINTSHOP);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("Generated by pointshop");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", EMGEN_CHEAT);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("Generated by cheating");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", EMGEN_REBUY);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("Generated by rebyuing");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }
}

void CDlgInvenAdd::SetExchangeFlagData()
{
    // Erase all list items
    m_ListInvenAdd.DeleteAllItems();

    m_vItem.erase(m_vItem.begin(), m_vItem.end());

    int nNum=0;

    int nCount;
    CString strTemp;

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", ITEM_ROUTE_CHAR);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("ITEM_ROUTE_CHAR");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", ITEM_ROUTE_GROUND);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("ITEM_ROUTE_GROUND");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", ITEM_ROUTE_SHOP);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("ITEM_ROUTE_SHOP");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", ITEM_ROUTE_USER_INVEN);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("ITEM_ROUTE_USER_INVEN");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", ITEM_ROUTE_DELETE);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("ITEM_ROUTE_DELETE");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", ITEM_ROUTE_CLUB);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("ITEM_ROUTE_CLUB");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", MONEY_ROUTE_CLUB_INCOME);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("MONEY_ROUTE_CLUB_INCOME");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", MONEY_ROUTE_NPC_REMAKE);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("MONEY_ROUTE_NPC_REMAKE");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", ITEM_ROUTE_PET_CARD);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("ITEM_ROUTE_PET_CARD");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", ITEM_ROUTE_ODD_EVEN);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("ITEM_ROUTE_ODD_EVEN");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", ITEM_ROUTE_VEHICLE);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("ITEM_ROUTE_VEHICLE");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", ITEM_ROUTE_VNINVEN);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("ITEM_ROUTE_VNINVEN");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", ITEM_ROUTE_GARBAGE);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("ITEM_ROUTE_GARBAGE");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", ITEM_ROUTE_NPCCOME);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("ITEM_ROUTE_NPCCOME");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", ITEM_ROUTE_ITEM_MIX);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("ITEM_ROUTE_ITEM_MIX");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", ITEM_ROUTE_GATHERING);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("ITEM_ROUTE_GATHERING");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", ITEM_ROUTE_SYSTEM);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("ITEM_ROUTE_SYSTEM");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", ITEM_ROUTE_PK_DROP);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("ITEM_ROUTE_PK_DROP");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("MONEY_ROUTE_CHAR_INVEN_TOTAL_MONEY");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", MONEY_ROUTE_USER_INVEN_TOTAL_MONEY);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("MONEY_ROUTE_USER_INVEN_TOTAL_MONEY");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", MONEY_ROUTE_CLUB_TOTAL_MONEY);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("MONEY_ROUTE_CLUB_TOTAL_MONEY");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }

    {
        nCount = m_ListInvenAdd.GetItemCount();
        LV_ITEM lvItem;
        ::memset(&lvItem, 0, sizeof(LV_ITEM));

        lvItem.mask = LVIF_TEXT; // 속성 지정
        lvItem.iItem = nCount;
        lvItem.iSubItem = 0;
        strTemp.Format("%d", MONEY_RECOVERY_CONSUME);
        lvItem.pszText = strTemp.GetBuffer();		
        m_ListInvenAdd.InsertItem(&lvItem);

        strTemp = CString("MONEY_RECOVERY_CONSUME");
        m_ListInvenAdd.SetItemText(nNum, 1, strTemp);

        nNum++;
    }
}

void CDlgInvenAdd::SetExchangeItemHistoryData( const std::vector<ExchangeItemHistory>& InExChangeHistory )
{
    // Erase all list items
    m_ListInvenAdd.DeleteAllItems();

    m_vItem.erase(m_vItem.begin(), m_vItem.end());

    int nNum=0;

    CString strTemp;

    std::string strFormatTemp;
    for ( size_t i = 0; i < InExChangeHistory.size(); i++ )
    {
        std::string strTime = sc::time::DateTimeFormat(InExChangeHistory[i].ExchangeDate);

        strFormatTemp = sc::string::format("%1%", InExChangeHistory[i].ExchangeNum);
        strTemp.Format("%s", strFormatTemp.c_str());
        int nIndex = m_ListInvenAdd.InsertItem( nNum, strTemp );

        strTemp.Format("%d", InExChangeHistory[i].NIDMain);
        m_ListInvenAdd.SetItemText( nIndex, 1, strTemp );

        strTemp.Format("%d", InExChangeHistory[i].NIDSub);
        m_ListInvenAdd.SetItemText( nIndex, 2, strTemp );

        strTemp.Format("%d", InExChangeHistory[i].SGNum);
        m_ListInvenAdd.SetItemText( nIndex, 3, strTemp );

        strTemp.Format("%d", InExChangeHistory[i].SvrNum);
        m_ListInvenAdd.SetItemText( nIndex, 4, strTemp );

        strTemp.Format("%d", InExChangeHistory[i].FldNum);
        m_ListInvenAdd.SetItemText( nIndex, 5, strTemp );

        strTemp.Format("%d", InExChangeHistory[i].MakeType);
        m_ListInvenAdd.SetItemText( nIndex, 6, strTemp );

        strFormatTemp = sc::string::format("%1%", InExChangeHistory[i].MakeNum);
        strTemp.Format("%s", strFormatTemp.c_str());
        m_ListInvenAdd.SetItemText( nIndex, 7, strTemp );

        strTemp.Format("%d", InExChangeHistory[i].ItemAmount);
        m_ListInvenAdd.SetItemText( nIndex, 8, strTemp );

        strTemp.Format("%d", InExChangeHistory[i].ItemFromFlag);
        m_ListInvenAdd.SetItemText( nIndex, 9, strTemp );

        strTemp.Format("%d", InExChangeHistory[i].ItemFrom);
        m_ListInvenAdd.SetItemText( nIndex, 10, strTemp );

        strTemp.Format("%d", InExChangeHistory[i].ItemToFlag);
        m_ListInvenAdd.SetItemText( nIndex, 11, strTemp );

        strTemp.Format("%d", InExChangeHistory[i].ItemTo);
        m_ListInvenAdd.SetItemText( nIndex, 12, strTemp );

        strTemp.Format("%d", InExChangeHistory[i].ExchangeFlag);
        m_ListInvenAdd.SetItemText( nIndex, 13, strTemp );

        strTemp.Format("%s", strTime.c_str());
        m_ListInvenAdd.SetItemText( nIndex, 14, strTemp );

        strFormatTemp = sc::string::format("%1%", InExChangeHistory[i].TradePrice);
        strTemp.Format("%s", strFormatTemp.c_str());
        m_ListInvenAdd.SetItemText( nIndex, 15, strTemp );

        nNum++;
    }
}
