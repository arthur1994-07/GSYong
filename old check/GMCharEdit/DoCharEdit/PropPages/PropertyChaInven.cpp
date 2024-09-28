// PropertyChaInven.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../../GMCharEdit.h"
#include "../../MainFrm.h"
#include "./PropertyChaInven.h"
#include "../PropertySheetCharEdit.h"
#include "../../SubDialogs/DlgInvenElementEdit.h"
#include "../../SubDialogs/DlgInvenAdd.h"

#include "../../SigmaCore/Util/DateTime.h"
#include "../../SigmaCore/Math/SeqUniqueGuid.h"

#include "../../RanLogic/Item/GLItemCustom.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Item/GLItem.h"
#include "../../RanLogic/Item/GLItemMan.h"

#include "../../RanLogicServer/Database/DbDefineLog.h"
#include "../../RanLogicServer/DxServerInstance.h"

#include "../../Util/CGmCfg.h"

#include "../../Database/ADO/ADOManager.h"
#include "../../Database/ODBC/CGMOdbcManager.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// CPropertyChaInven ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CPropertyChaInven, CPropertyPage)
CPropertyChaInven::CPropertyChaInven()
	: CPropertyPage(CPropertyChaInven::IDD)
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();//CADOManager::GetInstance();
}

CPropertyChaInven::~CPropertyChaInven()
{
}

void CPropertyChaInven::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_ListItem);
}


BEGIN_MESSAGE_MAP(CPropertyChaInven, CPropertyPage)
    ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnNMDblclkList1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BTN_INVEN_EDIT, OnBnClickedBtnInvenEdit)
	ON_BN_CLICKED(IDC_BTN_INVEN_ADD, OnBnClickedBtnInvenAdd)
	ON_BN_CLICKED(IDC_BTN_INVEN_DELETE, OnBnClickedBtnInvenDelete)
	ON_BN_CLICKED(IDC_BTN_INVEN_SAVE, OnBnClickedBtnInvenSave)
END_MESSAGE_MAP()


// CPropertyChaInven �޽��� ó�����Դϴ�.

BOOL CPropertyChaInven::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
    m_ListItem.SetExtendedStyle(LVS_EX_FULLROWSELECT); 

    CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();        
    InitListHead();
    SetData(pSheet->m_pChar);

    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


// ����Ʈ ��Ʈ�� ��� �ʱ�ȭ
void CPropertyChaInven::InitListHead() 
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
	m_ListItem.InsertColumn(0, &Col);

	strTemp			= "Sub";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListItem.InsertColumn(1, &Col);

	strTemp			= "MakeNum";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListItem.InsertColumn(2, &Col);

	strTemp			= "Name";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 200;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListItem.InsertColumn(3, &Col);    
}

void CPropertyChaInven::SetData(SCHARDATA2* pChar)
{
    // Erase all list items
	m_ListItem.DeleteAllItems();

    //m_vSkill.erase(m_vSkill.begin(), m_vSkill.end());

    EMCHARINDEX emCharIndex = CharClassToIndex ( pChar->m_emClass );

    const GLInventory::CELL_MAP* pItemMap = pChar->m_cInventory.GetItemList();
    GLInventory::CELL_MAP_CITER iter = pItemMap->begin();
    GLInventory::CELL_MAP_CITER iter_end = pItemMap->end();

    int nNum=0;
    m_vInvenItem.erase(m_vInvenItem.begin(), m_vInvenItem.end());
	m_vInvenKey.erase(m_vInvenKey.begin(), m_vInvenKey.end());

    for ( ; iter!=iter_end; ++iter )
    {
        const SINVENITEM &sInvenItem = *(*iter).second;
        SITEM *pItemData = GLogicData::GetInstance().GetItem ( sInvenItem.sItemCustom.GetNativeID() );
        if ( !pItemData )   continue;

        int nCount;
		CString strTemp;
		nCount = m_ListItem.GetItemCount();

		LV_ITEM lvItem;
		::memset(&lvItem, 0, sizeof(LV_ITEM));

		lvItem.mask = LVIF_TEXT; // �Ӽ� ����
		lvItem.iItem = nCount;
		lvItem.iSubItem = 0;
        strTemp.Format("%d", sInvenItem.sItemCustom.Mid());
		lvItem.pszText = strTemp.GetBuffer();		
		m_ListItem.InsertItem(&lvItem); // ���ο� ���� ������ InsertItem�� ����Ѵ�.
				
        strTemp.Format("%d", sInvenItem.sItemCustom.Sid());
		m_ListItem.SetItemText(nNum, 1, strTemp);

		std::string strGuid = sc::string::uuidToString( ( UUID )sInvenItem.sItemCustom.guid );
		
		strTemp.Format("%s", strGuid.c_str());
		m_ListItem.SetItemText(nNum, 2, strTemp);

		m_ListItem.SetItemText(nNum, 3, pItemData->GetName());		

        
//      strTemp.Format("%d",);
//		m_ListItem.SetItemText(nNum, 3, strTemp);
        
        m_vInvenItem.push_back(sInvenItem);
		m_vInvenKey.push_back((*iter).first);

        nNum++;
    }
}

void CPropertyChaInven::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnBnClickedBtnInvenEdit();
    *pResult = 0;
}

void CPropertyChaInven::OnBnClickedButton2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	
}

///////////////////////////////////////////////////////////////////////////////
// Edit Item
void CPropertyChaInven::OnBnClickedBtnInvenEdit()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

	CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();
	GLCHARLOGIC* pChar = pSheet->m_pChar;

    int nSelected = m_ListItem.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		
	if (nSelected != -1) // ���õ� �׸��� ����
	{
		// Rate (-100)-(+100)
		// Resist (-100)-(+100)
		// Defence, Damage (-100)-(+100)
		// ReModel (����Ƚ��) 0-9
		// Used (���Ƚ��, ex> ȭ�찹��) 0-100
		CDlgInvenElementEdit dlgEdit;
		dlgEdit.m_sItem = m_vInvenItem[nSelected];
		
		if (dlgEdit.DoModal() == IDOK)
		{
			GLInventory::CELL_KEY sKey = m_vInvenKey[nSelected];

			const GLInventory::CELL_MAP* pItemMap = pChar->m_cInventory.GetItemList();
			GLInventory::CELL_MAP_CITER iter = pItemMap->find(sKey);
			SINVENITEM &sInvenItem = *(*iter).second;

			///////////////////////////////////////////////////////////////////////////
			// GM Log
			CString strUserIP = m_pDBMan->GetLocalIP();
			int nGmNum = pFrame->m_nUserNum;
			std::strstream strGmCmd;
			strGmCmd << "Character(" << pChar->m_CharDbNum << ") ";
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
			strGmCmd.freeze( false );									// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
			///////////////////////////////////////////////////////////////////////////

			sInvenItem.sItemCustom = dlgEdit.m_sItem.sItemCustom;

			sInvenItem.sItemCustom.cDAMAGE = dlgEdit.m_nDamage;
			sInvenItem.sItemCustom.cDEFENSE = dlgEdit.m_nDefense;

			sInvenItem.sItemCustom.cRESIST_ELEC = dlgEdit.m_nElectric;
			sInvenItem.sItemCustom.cRESIST_FIRE = dlgEdit.m_nFire;
			sInvenItem.sItemCustom.cRESIST_ICE = dlgEdit.m_nIce;
			sInvenItem.sItemCustom.cRESIST_POISON = dlgEdit.m_nPoison;
			sInvenItem.sItemCustom.cRESIST_SPIRIT = dlgEdit.m_nSpirit;

			sInvenItem.sItemCustom.wTurnNum = (WORD) dlgEdit.m_wTurnNum;

			SetData(pSheet->m_pChar);

			///////////////////////////////////////////////////////////////////////////
			// GM Log			
			std::strstream strGmCmd2;
			strGmCmd2 << "Character(" << pChar->m_CharDbNum << ") ";
			strGmCmd2 << " Item(" << sInvenItem.sItemCustom.Mid() << "," << sInvenItem.sItemCustom.Sid()<< ") Edit New";
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
			strGmCmd2.freeze( false );									// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
			///////////////////////////////////////////////////////////////////////////

			m_vecWriteAddRamdomoptionLogs.push_back( sInvenItem.sItemCustom );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Add Item to Inventory
void CPropertyChaInven::OnBnClickedBtnInvenAdd()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
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
				sItem.SetDbState( db::DB_INSERT );
				sItem.tBORNTIME = cTime.GetTime();

				// ������ ����Ʈ���� �Է��� ���� ��� ����
				sItem.cDAMAGE = (BYTE)dlgAdd.m_vItem[dlgAdd.m_vSelected[i]].sBasicOp.wGradeAttack;
				sItem.cDEFENSE = (BYTE)dlgAdd.m_vItem[dlgAdd.m_vSelected[i]].sBasicOp.wGradeDefense;

// GMCharEdit �� ���� �ŷ� ���� ��ġ ������ �䱸 ������ �޶� �ٸ��� �ڵ��Ѵ�.
#ifdef THAIWAN // �븸
				// Item can trade
				sItem.SetGM_GEN2(pFrame->m_nUserNum);
#elif THAILAND // �±�
				// Item can't trade
				sItem.SetGM_GEN(pFrame->m_nUserNum);
#elif KOREA // �ѱ�
				if (MessageBox(_T("�ŷ��� �Ұ����ϰ� �Ͻðڽ��ϱ�?"), _T("Confirm"), MB_ICONQUESTION | MB_YESNO) == IDYES)
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
// 				if (MessageBox(_T("Do you want item can not trade?"), _T("Confirm"), MB_ICONQUESTION | MB_YESNO) == IDYES)
// 				{
// 					// Item can't trade
// 					sItem.SetGM_GEN(pFrame->m_nUserNum);					
// 				}
// 				else
// 				{
// 					// Item can trade
// 					sItem.SetGM_GEN2(pFrame->m_nUserNum);					
// 				}
				// OK == Item can't trade
				sc::MGUID guid;
				sc::SeqUniqueGUID::Instance()->getGUID( guid );
				sItem.GenerateBasicStat( false );
				sItem.GenerateLinkSkill();
				sItem.GenerateAddOption();
				sItem.SetGM_GEN_RANDOMGENNUM( ( MessageBox(_T("Do you want item can not trade?"), _T("Confirm"), MB_ICONQUESTION | MB_YESNO) != IDYES ), guid );
#endif // GMCharEdit �� ���� �ŷ� ���� ��ġ ������ �䱸 ������ �޶� �ٸ��� �ڵ��Ѵ�.
    			                        
                if (pSheet->m_pChar->m_cInventory.InsertItem(sItem) == TRUE)
			    {
					///////////////////////////////////////////////////////////////////////////
					// GM Log
					CString strUserIP = m_pDBMan->GetLocalIP();
					int nGmNum = pFrame->m_nUserNum;
					std::strstream strGmCmd;
					strGmCmd << "Character(" << pChar->m_CharDbNum << ") ";
					strGmCmd << " Item(" << sItem.Mid() << "," << sItem.Sid() << ") Add";
					strGmCmd << '\0';
					m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
					strGmCmd.freeze( false );									// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
					///////////////////////////////////////////////////////////////////////////
			    }
			    else
			    {
				    MessageBox("Add item failed!");
			    }
			    SetData(pSheet->m_pChar);

				m_vecWriteAddItemLogs.push_back( sItem );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Delete Item
void CPropertyChaInven::OnBnClickedBtnInvenDelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

	CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();
	GLCHARLOGIC* pChar = pSheet->m_pChar;
    
	int nSelected = m_ListItem.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
		
	if (nSelected != -1) // ���õ� �׸��� ����
	{
		if (MessageBox(_T("Really delete?"), _T("DELETE"), MB_YESNO) == IDYES)
		{
			SINVENITEM sItem = m_vInvenItem[nSelected];

			SINVENITEM* pInvenItem = pChar->m_cInventory.GetItem( m_vInvenItem[nSelected].wPosX, m_vInvenItem[nSelected].wPosY );
			if ( pInvenItem )
			{
				m_pDBMan->ItemDelete( pInvenItem->GetGuid(), pInvenItem->GetDbState(), 2989 );
				if (pChar->m_cInventory.DeleteItem(m_vInvenItem[nSelected].wPosX, m_vInvenItem[nSelected].wPosY) == TRUE)
				{
					SetData(pSheet->m_pChar);

					///////////////////////////////////////////////////////////////////////////
					// GM Log
					CString strUserIP = m_pDBMan->GetLocalIP();
					int nGmNum = pFrame->m_nUserNum;
					std::strstream strGmCmd;
					strGmCmd << "Character(" << pChar->m_CharDbNum << ") ";
					strGmCmd << " Item(" << sItem.sItemCustom.Mid() << "," << sItem.sItemCustom.Sid() << ") Delete";
					strGmCmd << '\0';
					m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
					strGmCmd.freeze( false );									// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
					///////////////////////////////////////////////////////////////////////////
				}
				else
				{
					MessageBox("Delete Failed");
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Save Inventory
void CPropertyChaInven::OnBnClickedBtnInvenSave()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}	

	CPropertySheetCharEdit* pSheet = (CPropertySheetCharEdit *) GetParent();
	GLCHARLOGIC* pChar = pSheet->m_pChar;
	m_pDBMan->SaveCharacterInven(pChar);
	SetData(pChar);

	///////////////////////////////////////////////////////////////////////////
	// GM Log
	CString strUserIP = m_pDBMan->GetLocalIP();
	int nGmNum = pFrame->m_nUserNum;
	std::strstream strGmCmd;
	strGmCmd << "Character(" << pChar->m_CharDbNum << ") Inventory Save";
	strGmCmd << '\0';
	m_pDBMan->WriteGMLog(nGmNum, strGmCmd, strUserIP);
	strGmCmd.freeze( false );									// Note : std::strstream�� freeze. �� �ϸ� Leak �߻�.
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

		if( sItemCustom.IsSetRandOpt() == FALSE )
			continue;

		// �����ɼ� �α�
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

void CPropertyChaInven::OnOK()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	OnBnClickedBtnInvenSave();

	CPropertyPage::OnOK();
}
