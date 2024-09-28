// PropertyChaUserInven.cpp : 구현 파일입니다.
//

#include "stdafx.h"

#include "../../SigmaCore/Math/SeqUniqueGuid.h"

#include "../../MainFrm.h"
#include "../../GMCharEdit.h"
#include "./PropertyChaUserInven.h"
#include "../PropertySheetCharEdit.h"
#include "../../SubDialogs/DlgInvenElementEdit.h"
#include "../../SubDialogs/DlgInvenAdd.h"

#include "../../SigmaCore/Util/DateTime.h"

#include "../../RanLogicServer/DxServerInstance.h"
#include "../../RanLogicServer/Database/DbDefineLog.h"

#include "../../RanLogic/Item/GLItemCustom.h"
#include "../../RanLogic/Item/GLItemMan.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../Util/CGmCfg.h"

#include "../../Database/ADO/ADOManager.h"
#include "../../Database/ODBC/CGMOdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


// CPropertyChaUserInven 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPropertyChaUserInven, CPropertyPage)
CPropertyChaUserInven::CPropertyChaUserInven()
	: CPropertyPage(CPropertyChaUserInven::IDD)
	, m_llMoney(0)
    , m_nInven2Year(0)
    , m_nInven2Month(0)
    , m_nInven2Day(0)
    , m_nInven2Hour(0)
    , m_nInven2Min(0)
    , m_nInven3Year(0)
    , m_nInven3Month(0)
    , m_nInven3Day(0)
    , m_nInven3Hour(0)
    , m_nInven3Min(0)
    , m_nInven4Year(0)
    , m_nInven4Month(0)
    , m_nInven4Day(0)
    , m_nInven4Hour(0)
    , m_nInven4Min(0)
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();
}

CPropertyChaUserInven::~CPropertyChaUserInven()
{
}

void CPropertyChaUserInven::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_ListUserInven);
    DDX_Text(pDX, IDC_EDIT_USERINVEN_MONEY, m_llMoney);    
    DDX_Text(pDX, IDC_EDIT_INVEN2_DATE, m_nInven2Year);
    DDV_MinMaxInt(pDX, m_nInven2Year, 1970, 2049);
    DDX_Text(pDX, IDC_EDIT_INVEN2_DATE2, m_nInven2Month);
    DDV_MinMaxInt(pDX, m_nInven2Month, 1, 12);
    DDX_Text(pDX, IDC_EDIT_INVEN2_DATE3, m_nInven2Day);
    DDV_MinMaxInt(pDX, m_nInven2Day, 1, 31);
    DDX_Text(pDX, IDC_EDIT_INVEN2_DATE4, m_nInven2Hour);
    DDV_MinMaxInt(pDX, m_nInven2Hour, 0, 23);
    DDX_Text(pDX, IDC_EDIT_INVEN2_DATE5, m_nInven2Min);
    DDV_MinMaxInt(pDX, m_nInven2Min, 0, 59);
    DDX_Text(pDX, IDC_EDIT_INVEN2_DATE6, m_nInven3Year);
    DDV_MinMaxInt(pDX, m_nInven3Year, 1970, 2049);
    DDX_Text(pDX, IDC_EDIT_INVEN2_DATE7, m_nInven3Month);
    DDV_MinMaxInt(pDX, m_nInven3Month, 1, 12);
    DDX_Text(pDX, IDC_EDIT_INVEN2_DATE8, m_nInven3Day);
    DDV_MinMaxInt(pDX, m_nInven3Day, 1, 31);
    DDX_Text(pDX, IDC_EDIT_INVEN2_DATE9, m_nInven3Hour);
    DDV_MinMaxInt(pDX, m_nInven3Hour, 0, 23);
    DDX_Text(pDX, IDC_EDIT_INVEN2_DATE10, m_nInven3Min);
    DDV_MinMaxInt(pDX, m_nInven3Min, 0, 59);
    DDX_Text(pDX, IDC_EDIT_INVEN2_DATE11, m_nInven4Year);
    DDV_MinMaxInt(pDX, m_nInven4Year, 1970, 2049);
    DDX_Text(pDX, IDC_EDIT_INVEN2_DATE12, m_nInven4Month);
    DDV_MinMaxInt(pDX, m_nInven4Month, 1, 12);
    DDX_Text(pDX, IDC_EDIT_INVEN2_DATE13, m_nInven4Day);
    DDV_MinMaxInt(pDX, m_nInven4Day, 1, 31);
    DDX_Text(pDX, IDC_EDIT_INVEN2_DATE14, m_nInven4Hour);
    DDV_MinMaxInt(pDX, m_nInven4Hour, 0, 23);
    DDX_Text(pDX, IDC_EDIT_INVEN2_DATE15, m_nInven4Min);
	DDV_MinMaxInt(pDX, m_nInven4Min, 0, 59);
}


BEGIN_MESSAGE_MAP(CPropertyChaUserInven, CPropertyPage)	
	ON_BN_CLICKED(IDC_BTN_USER_INVEN_ADD, OnBnClickedBtnUserInvenAdd)
	ON_BN_CLICKED(IDC_BTN_USER_INVEN_EDIT, OnBnClickedBtnUserInvenEdit)
	ON_BN_CLICKED(IDC_BTN_USER_INVEN_DELETE, OnBnClickedBtnUserInvenDelete)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnNMDblclkList1)
	ON_BN_CLICKED(IDC_BTN_USER_INVEN_SAVE, OnBnClickedBtnUserInvenSave)
END_MESSAGE_MAP()


// CPropertyChaUserInven 메시지 처리기입니다.

BOOL CPropertyChaUserInven::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.    
	m_ListUserInven.SetExtendedStyle(LVS_EX_FULLROWSELECT); 
	
	CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();        
    InitListHead();
    SetData(pSheet->m_pChar);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

// 리스트 컨트롤 헤더 초기화
void CPropertyChaUserInven::InitListHead() 
{
	LVCOLUMN Col;
	CString strTemp;
	// Num, Name, Class, Tribe, Level,
	strTemp			= "Main";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;		
	Col.pszText		= strTemp.GetBuffer();
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListUserInven.InsertColumn(0, &Col);

	strTemp			= "Sub";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListUserInven.InsertColumn(1, &Col);

	strTemp			= "MakeNum";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListUserInven.InsertColumn(2, &Col);

	strTemp			= "Name";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 200;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListUserInven.InsertColumn(3, &Col);
}

void CPropertyChaUserInven::SetData(SCHARDATA2* pChar)
{
    // Erase all list items
	m_ListUserInven.DeleteAllItems();

	// 창고는 총 5 개로 구성되어 있다.
	// 현재는 하나만 사용. (0 번만 사용한다)
	// 추후에 늘어날 경우 0 에서 4 번까지 사용가능
	const GLInventory::CELL_MAP* pItemMap = pChar->m_cStorage[m_nUserInvenNum].GetItemList();
    GLInventory::CELL_MAP_CITER iter = pItemMap->begin();
    GLInventory::CELL_MAP_CITER iter_end = pItemMap->end();

    int nNum=0;
    m_vInvenItem.erase(m_vInvenItem.begin(), m_vInvenItem.end());
	m_vInvenKey.erase(m_vInvenKey.begin(), m_vInvenKey.end());

    for ( ; iter!=iter_end; ++iter )
    {
        const SINVENITEM &sInvenItem = *(*iter).second;
        SITEM *pItemData = GLogicData::GetInstance().GetItem ( sInvenItem.CustomItem().GetBoxWrappedItemID() );
        if ( !pItemData )   continue;

        int nCount;
		CString strTemp;
		nCount = m_ListUserInven.GetItemCount();

		LV_ITEM lvItem;
		::memset(&lvItem, 0, sizeof(LV_ITEM));

		lvItem.mask = LVIF_TEXT; // 속성 지정
		lvItem.iItem = nCount;
		lvItem.iSubItem = 0;
        strTemp.Format("%d", sInvenItem.sItemCustom.Mid());
		lvItem.pszText = strTemp.GetBuffer();		
		m_ListUserInven.InsertItem(&lvItem); // 새로운 열의 시작은 InsertItem을 사용한다.
				
        strTemp.Format("%d", sInvenItem.sItemCustom.Sid());
		m_ListUserInven.SetItemText(nNum, 1, strTemp);

		std::string strGuid = sc::string::uuidToString( ( UUID )sInvenItem.sItemCustom.guid );
		strTemp.Format("%s", strGuid.c_str());
		m_ListUserInven.SetItemText(nNum, 2, strTemp);

		m_ListUserInven.SetItemText(nNum, 3, pItemData->GetName());

        m_vInvenItem.push_back(sInvenItem);
		m_vInvenKey.push_back((*iter).first);

        nNum++;
    }

	m_llMoney = pChar->GetStorageMoney();
    
    CTime cTemp0(pChar->m_tSTORAGE[0]);
    m_nInven2Year  = cTemp0.GetYear();
    m_nInven2Month = cTemp0.GetMonth();
    m_nInven2Day   = cTemp0.GetDay();
    m_nInven2Hour  = cTemp0.GetHour();
    m_nInven2Min   = cTemp0.GetMinute();   

    CTime cTemp1(pChar->m_tSTORAGE[1]);
    m_nInven3Year  = cTemp1.GetYear();
    m_nInven3Month = cTemp1.GetMonth();
    m_nInven3Day   = cTemp1.GetDay();
    m_nInven3Hour  = cTemp1.GetHour();
    m_nInven3Min   = cTemp1.GetMinute();

    CTime cTemp2(pChar->m_tSTORAGE[2]);
    m_nInven4Year  = cTemp2.GetYear();
    m_nInven4Month = cTemp2.GetMonth();
    m_nInven4Day   = cTemp2.GetDay();
    m_nInven4Hour  = cTemp2.GetHour();
    m_nInven4Min   = cTemp2.GetMinute();

	UpdateData(FALSE);
}

///////////////////////////////////////////////////////////////////////////////
// Add Item To User Inven
void CPropertyChaUserInven::OnBnClickedBtnUserInvenAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}	

	CDlgInvenAdd dlgAdd;	
	CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();
	GLCHARLOGIC* pChar = pSheet->m_pChar;

	if (dlgAdd.DoModal() == IDOK)
    {
        if (dlgAdd.m_nSelected != -1)
        {
            for (int i=0; i<(int) dlgAdd.m_vSelected.size(); i++)
            {
				CTime cTime = CTime::GetCurrentTime();

				SITEMCUSTOM sItem(dlgAdd.m_vItem[dlgAdd.m_vSelected[i]].sBasicOp.sNativeID);
				sc::SeqUniqueGUID::Instance()->getGUID( sItem.guid );
				sItem.GenerateBasicStat( false );
				sItem.GenerateLinkSkill();
				sItem.GenerateAddOption();
				sItem.tBORNTIME = cTime.GetTime();
				sItem.SetDbState( db::DB_INSERT );

				// 아이템 에디트에서 입력한 개조 등급 적용
				sItem.cDAMAGE = (BYTE)dlgAdd.m_vItem[dlgAdd.m_vSelected[i]].sBasicOp.wGradeAttack;
				sItem.cDEFENSE = (BYTE)dlgAdd.m_vItem[dlgAdd.m_vSelected[i]].sBasicOp.wGradeDefense;

// GMCharEdit 에 대한 거래 제한 장치 국가별 요구 사항이 달라서 다르게 코딩한다.
#ifdef TAIWAN // 대만
				// Item can trade
				sItem.SetGM_GEN2(pFrame->m_nUserNum);
#elif THAILAND // 태국
				// Item can't trade
				sItem.SetGM_GEN(pFrame->m_nUserNum);
#elif KOREA // 한국
				if (MessageBox(_T("거래를 불가능하게 하시겠습니까?"), _T("Confirm"), MB_ICONQUESTION | MB_YESNO) == IDYES)
				{
					// Item can't trade
					sItem.SetGM_GEN(pFrame->m_nUserNum);				
				}
				else
				{
					// Item can trade
					sItem.SetGM_GEN2(pFrame->m_nUserNum);					
				}
#else
				if (MessageBox(_T("Do you want item can not trade?"), _T("Confirm"), MB_ICONQUESTION | MB_YESNO) == IDYES)
				{
					// Item can't trade
					sItem.SetGM_GEN(pFrame->m_nUserNum);					
				}
				else
				{
					// Item can trade
					sItem.SetGM_GEN2(pFrame->m_nUserNum);					
				}
#endif // GMCharEdit 에 대한 거래 제한 장치 국가별 요구 사항이 달라서 다르게 코딩한다.

                if (pChar->m_cStorage[m_nUserInvenNum].InsertItem(sItem) == TRUE)
			    {
                    ///////////////////////////////////////////////////////////////////////////
					// GM Log
					CString strUserIP = m_pDBMan->GetLocalIP();
					int nGmNum = pFrame->m_nUserNum;
					std::strstream strGmCmd;
					strGmCmd << "User(" << pChar->GetUserID() << ") Character(" << pChar->m_CharDbNum << ") ";
					strGmCmd << " Item(" << sItem.Mid() << "," << sItem.Sid() << ") Add";
					strGmCmd << '\0';
					m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
					strGmCmd.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
					///////////////////////////////////////////////////////////////////////////
			    }
			    else
			    {
				    MessageBox(_T("Add item failed!"));
			    }
			    SetData(pSheet->m_pChar);

				m_vecWriteAddItemLogs.push_back( sItem );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// User Inven Item Edit
void CPropertyChaUserInven::OnBnClickedBtnUserInvenEdit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

	CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();
	GLCHARLOGIC* pChar = pSheet->m_pChar;

    int nSelected = m_ListUserInven.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		
	if (nSelected != -1) // 선택된 항목이 있음
	{		
		// Used (사용횟수, ex> 화살갯수) 0-100
		CDlgInvenElementEdit dlgEdit;
		dlgEdit.m_sItem = m_vInvenItem[nSelected];
		
		if (dlgEdit.DoModal() == IDOK)
		{
			GLInventory::CELL_KEY sKey = m_vInvenKey[nSelected];

			const GLInventory::CELL_MAP* pItemMap = pChar->m_cStorage[m_nUserInvenNum].GetItemList();
			GLInventory::CELL_MAP_CITER iter = pItemMap->find(sKey);
			
			SINVENITEM &sInvenItem = *(*iter).second;

			///////////////////////////////////////////////////////////////////////////
			// GM Log
			CString strUserIP = m_pDBMan->GetLocalIP();
			int nGmNum = pFrame->m_nUserNum;
			std::strstream strGmCmd;
			strGmCmd << "User(" << pChar->GetUserID() << ") Character(" << pChar->m_CharDbNum << ") ";
			strGmCmd << " Item(" << sInvenItem.sItemCustom.Mid() << "," << sInvenItem.sItemCustom.Sid() << ") Edit Old";
			strGmCmd << "(" << (int) sInvenItem.sItemCustom.cDAMAGE;
            strGmCmd << "," << (int) sInvenItem.sItemCustom.cDEFENSE;
			strGmCmd << "," << (int) sInvenItem.sItemCustom.cRESIST_ELEC;
			strGmCmd << "," << (int) sInvenItem.sItemCustom.cRESIST_FIRE;
			strGmCmd << "," << (int) sInvenItem.sItemCustom.cRESIST_ICE;
			strGmCmd << "," << (int) sInvenItem.sItemCustom.cRESIST_POISON;
			strGmCmd << "," << (int) sInvenItem.sItemCustom.cRESIST_SPIRIT;
			strGmCmd << "," << (int) sInvenItem.sItemCustom.wTurnNum;
			strGmCmd <<	")";
			strGmCmd << '\0';
			m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
			strGmCmd.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
			///////////////////////////////////////////////////////////////////////////

			sInvenItem.sItemCustom = dlgEdit.m_sItem.sItemCustom;

			sInvenItem.sItemCustom.cDAMAGE        = dlgEdit.m_nDamage;
			sInvenItem.sItemCustom.cDEFENSE       = dlgEdit.m_nDefense;

			sInvenItem.sItemCustom.cRESIST_ELEC   = dlgEdit.m_nElectric;
			sInvenItem.sItemCustom.cRESIST_FIRE   = dlgEdit.m_nFire;
			sInvenItem.sItemCustom.cRESIST_ICE    = dlgEdit.m_nIce;
			sInvenItem.sItemCustom.cRESIST_POISON = dlgEdit.m_nPoison;
			sInvenItem.sItemCustom.cRESIST_SPIRIT = dlgEdit.m_nSpirit;

			sInvenItem.sItemCustom.wTurnNum = (WORD) dlgEdit.m_wTurnNum;

			SetData(pSheet->m_pChar);

			///////////////////////////////////////////////////////////////////////////
			// GM Log			
			std::strstream strGmCmd2;
			strGmCmd2 << "User(" << pChar->GetUserID() << ") Character(" << pChar->m_CharDbNum << ") ";
			strGmCmd2 << " Item(" << sInvenItem.sItemCustom.Mid() << "," << sInvenItem.sItemCustom.Sid() << ") Edit New";
			strGmCmd2 << "(" << (int) sInvenItem.sItemCustom.cDAMAGE;
            strGmCmd2 << "," << (int) sInvenItem.sItemCustom.cDEFENSE;
			strGmCmd2 << "," << (int) sInvenItem.sItemCustom.cRESIST_ELEC;
			strGmCmd2 << "," << (int) sInvenItem.sItemCustom.cRESIST_FIRE;
			strGmCmd2 << "," << (int) sInvenItem.sItemCustom.cRESIST_ICE;
			strGmCmd2 << "," << (int) sInvenItem.sItemCustom.cRESIST_POISON;
			strGmCmd2 << "," << (int) sInvenItem.sItemCustom.cRESIST_SPIRIT;
			strGmCmd2 << "," << (int) sInvenItem.sItemCustom.wTurnNum;
			strGmCmd2 <<	")";
			strGmCmd2 << '\0';
			m_pDBMan->WriteGMLog(nGmNum, strGmCmd2, strUserIP);
			strGmCmd2.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
			///////////////////////////////////////////////////////////////////////////

			m_vecWriteAddRamdomoptionLogs.push_back( sInvenItem.sItemCustom );

		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Delete Item From User Inventory
void CPropertyChaUserInven::OnBnClickedBtnUserInvenDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

	CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();
	GLCHARLOGIC* pChar = pSheet->m_pChar;
    
	int nSelected = m_ListUserInven.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		
	if (nSelected != -1) // 선택된 항목이 있음
	{		
		if (MessageBox(_T("Really delete?"), _T("DELETE"), MB_YESNO) == IDYES)
		{
			SINVENITEM sItem = m_vInvenItem[nSelected];
			m_pDBMan->ItemDelete( sItem.GetGuid(), sItem.GetDbState(), 429087 );
			if (pChar->m_cStorage[m_nUserInvenNum].DeleteItem(sItem.wPosX, sItem.wPosY) == TRUE)
			{	
				SetData(pSheet->m_pChar);				

				///////////////////////////////////////////////////////////////////////////
				// GM Log
				CString strUserIP = m_pDBMan->GetLocalIP();
				int nGmNum = pFrame->m_nUserNum;
				std::strstream strGmCmd;
				strGmCmd << "User(" << pChar->GetUserID() << ") Character(" << pChar->m_CharDbNum << ") ";
				strGmCmd << " Item(" << sItem.sItemCustom.Mid() << "," << sItem.sItemCustom.Sid() << ") Delete";
				strGmCmd << '\0';
				m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
				strGmCmd.freeze( false );												// Note : std::strstream의 freeze. 안 하면 Leak 발생.
				///////////////////////////////////////////////////////////////////////////
			}
			else
			{
				MessageBox(_T("Delete Failed"));
			}		
		}
	}
}

void CPropertyChaUserInven::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnBnClickedBtnUserInvenEdit();

	*pResult = 0;
}

// save
void CPropertyChaUserInven::OnBnClickedBtnUserInvenSave()
{
	

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

	UpdateData(TRUE);
	CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();
	GLCHARLOGIC* pChar = pSheet->m_pChar;

	pChar->SetStorageMoney( m_llMoney );

    CTime cTemp0(m_nInven2Year, m_nInven2Month, m_nInven2Day, m_nInven2Hour, m_nInven2Min, 0);
    pChar->m_tSTORAGE[0] = cTemp0.GetTime();

    CTime cTemp1(m_nInven3Year, m_nInven3Month, m_nInven3Day, m_nInven3Hour, m_nInven3Min, 0);
    pChar->m_tSTORAGE[1] = cTemp1.GetTime();

    CTime cTemp2(m_nInven4Year, m_nInven4Month, m_nInven4Day, m_nInven4Hour, m_nInven4Min, 0);
    pChar->m_tSTORAGE[2] = cTemp2.GetTime();    

	m_pDBMan->SaveCharacterUserInven(pChar);
	SetData(pChar);

	///////////////////////////////////////////////////////////////////////////
	// GM Log	
	CString strUserIP = m_pDBMan->GetLocalIP();
	int nGmNum = pFrame->m_nUserNum;
	std::strstream strGmCmd;
	strGmCmd << "User(" << pChar->GetUserID() << ") Character(" << pChar->m_CharDbNum << ") ";
	strGmCmd << "User Inven Save";
	strGmCmd << '\0';
	m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
	strGmCmd.freeze( false );										// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	///////////////////////////////////////////////////////////////////////////


	for ( unsigned int i=0 ; i<m_vecWriteAddItemLogs.size() ; i++ )
	{
		SITEMCUSTOM& sItemCustom = m_vecWriteAddItemLogs[i];
		SNATIVEID sOrigNID(0, 0);
		if ( sItemCustom.IsBoxWrapped() )
		{
			sOrigNID = sItemCustom.GetBoxWrappedItemID();
		}

		LOG_ITEM_EXCHANGE logItemExchange(
			sItemCustom.Mid(),
			sItemCustom.Sid(),
			0,
			0,
			0,
			sItemCustom.cGenType,
			sItemCustom.guid,

			gamelog::ID_CHAR,
			0,
			gamelog::ID_CHAR,
			pChar->m_CharDbNum,
			ITEM_ROUTE_CHAR,
			sItemCustom.wTurnNum,

			sItemCustom.nidDISGUISE.Mid(),
			sItemCustom.nidDISGUISE.Sid(),

			sItemCustom.cDAMAGE,
			sItemCustom.cDEFENSE,

			sItemCustom.cRESIST_FIRE,
			sItemCustom.cRESIST_ICE,
			sItemCustom.cRESIST_ELEC,
			sItemCustom.cRESIST_POISON,
			sItemCustom.cRESIST_SPIRIT,


			0,
			0,
			0,
			0,
			sOrigNID.Mid(),
			sOrigNID.Sid(),
			LOG_ITEM_CHAR_INVEN_CURRENT,
			sc::time::GetCurrentTime() );
		m_pDBMan->WriteLogItemExchange(logItemExchange);
	}

	for ( unsigned int i=0 ; i<m_vecWriteAddRamdomoptionLogs.size() ; i++ )
	{
		SITEMCUSTOM& sItemCustom = m_vecWriteAddRamdomoptionLogs[i];

		// 랜덤옵션 로그
		LOG_RANDOM_OPTION LogRandomOption(
			sItemCustom.Mid(),
			sItemCustom.Sid(),
			0,
			0,
			0,
			sItemCustom.cGenType,
			sItemCustom.guid,
			sItemCustom.GETOptTYPE1(),
			sItemCustom.GETOptTYPE2(),
			sItemCustom.GETOptTYPE3(),
			sItemCustom.GETOptTYPE4(),
			sItemCustom.GETOptTYPE5(),
			sItemCustom.randomOption.getValue(0),
			sItemCustom.randomOption.getValue(1),
			sItemCustom.randomOption.getValue(2),
			sItemCustom.randomOption.getValue(3),
			sItemCustom.randomOption.getValue(4),
			static_cast<BYTE>(sItemCustom.getOptCount(0)),
			static_cast<BYTE>(sItemCustom.getOptCount(1)),
			static_cast<BYTE>(sItemCustom.getOptCount(2)),
			static_cast<BYTE>(sItemCustom.getOptCount(3)), 
			static_cast<BYTE>(sItemCustom.getOptCount(4)), 
			sc::time::GetCurrentTime(),
			0);
		m_pDBMan->WriteLogRandomItem(LogRandomOption);
	}

	m_vecWriteAddItemLogs.clear();
	m_vecWriteAddRamdomoptionLogs.clear();
}

void CPropertyChaUserInven::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	OnBnClickedBtnUserInvenSave();

	CPropertyPage::OnOK();
}
