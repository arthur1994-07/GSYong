#include "stdafx.h"
#include "../../GMCharEdit.h"
#include "../../MainFrm.h"
#include "./PropertyChaClubInven.h"
#include "../PropertySheetCharEdit.h"
#include "../../SubDialogs/DlgInvenElementEdit.h"
#include "../../SubDialogs/DlgInvenAdd.h"

#include "../../RanLogic/Item/GLItemCustom.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogicServer/DxServerInstance.h"
#include "../../RanLogic/Item/GLItem.h"
#include "../../RanLogic/Item/GLItemMan.h"

#include "../../Util/CGmCfg.h"

#include "../../Database/ADO/ADOManager.h"
#include "../../Database/ODBC/CGMOdbcManager.h"

#include "../../DataManager/CDataManagerClubInven.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC( CPropertyChaClubInven, CPropertyPage )
CPropertyChaClubInven::CPropertyChaClubInven()
: CPropertyPage( CPropertyChaClubInven::IDD )
, m_llMoney( 0 )
{
	m_pDBMan = CGmCfg::GetInstance()->GetDBMan();
}

CPropertyChaClubInven::~CPropertyChaClubInven()
{
}

void CPropertyChaClubInven::DoDataExchange( CDataExchange* pDX )
{
	CPropertyPage::DoDataExchange( pDX );
	DDX_Control( pDX, IDC_LIST1, m_ListItem );
	DDX_Control( pDX, IDC_EDIT_CLUB_INVEN_MONEY, m_moneyEditBox );
	DDX_Text( pDX, IDC_EDIT_CLUB_INVEN_MONEY, m_llMoney );
}

BEGIN_MESSAGE_MAP( CPropertyChaClubInven, CPropertyPage )
	ON_BN_CLICKED(IDC_BTN_INVEN_DELETE, OnBnClickedBtnInvenDelete)
	ON_BN_CLICKED(IDC_BTN_INVEN_SAVE, OnBnClickedBtnInvenSave)
END_MESSAGE_MAP()

BOOL CPropertyChaClubInven::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_ListItem.SetExtendedStyle( LVS_EX_FULLROWSELECT ); 

	InitListHead();
	SetData();

	MessageBox( _T("Please shut down the server!") );

	return TRUE;
}


// 리스트 컨트롤 헤더 초기화;
void CPropertyChaClubInven::InitListHead() 
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
	m_ListItem.InsertColumn( 0, &Col );

	strTemp			= "Sub";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 50;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListItem.InsertColumn( 1, &Col );

	strTemp			= "MakeNum";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 200;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListItem.InsertColumn( 2, &Col );

	strTemp			= "Name";
	Col.mask		= LVCF_TEXT | LVCF_WIDTH;
	Col.cx			= 200;
	Col.fmt			= LVCFMT_CENTER;	
	Col.pszText		= strTemp.GetBuffer();	
	Col.iSubItem	= 0;
	Col.iImage		= 0;
	m_ListItem.InsertColumn( 3, &Col );    
}

void CPropertyChaClubInven::SetData()
{
	if( !dm::CDataManagerClubInven::Instance()->GetUseStorageState() )
	{
		m_moneyEditBox.SetWindowText( "0" );
		m_llMoney = 0;
		m_ListItem.DeleteAllItems();

		m_vInvenItem.erase(m_vInvenItem.begin(), m_vInvenItem.end());
		m_vInvenKey.erase(m_vInvenKey.begin(), m_vInvenKey.end());

		MessageBox(_T("You are not a club master"));

		return;
	}

	//m_moneyEditBox.SetWindowText(
	//	sc::string::format( "%1%",
	//	dm::CDataManagerClubInven::Instance()->GetClubInvenMoney() ).c_str() );
	m_llMoney = dm::CDataManagerClubInven::Instance()->GetClubInvenMoney();

	// Erase all list items
	m_ListItem.DeleteAllItems();

	m_vInvenItem.erase(m_vInvenItem.begin(), m_vInvenItem.end());
	m_vInvenKey.erase(m_vInvenKey.begin(), m_vInvenKey.end());

	int nNum = 0;
	for( unsigned int i=0; i<MAX_CLUBSTORAGE; ++i )
	{
		GLInventoryStorage* pInvenStorage =
			dm::CDataManagerClubInven::Instance()->GetStorage( i );
		if( !pInvenStorage )
			continue;

		const GLInventory::CELL_MAP* pItemMap = pInvenStorage->GetItemList();

		GLInventory::CELL_MAP_CITER iter = pItemMap->begin();
		GLInventory::CELL_MAP_CITER iter_end = pItemMap->end();
		for( ; iter!=iter_end; ++iter )
		{
			const SINVENITEM &sInvenItem = *(*iter).second;
			SITEM *pItemData = GLogicData::GetInstance().GetItem(
				sInvenItem.sItemCustom.GetNativeID() );
			if( !pItemData )
				continue;

			int nCount;
			CString strTemp;
			nCount = m_ListItem.GetItemCount();

			LV_ITEM lvItem;
			::memset( &lvItem, 0, sizeof(LV_ITEM) );

			// 속성 지정;
			lvItem.mask = LVIF_TEXT;

			lvItem.iItem = nCount;
			lvItem.iSubItem = 0;
			strTemp.Format( "%d", sInvenItem.sItemCustom.Mid() );
			lvItem.pszText = strTemp.GetBuffer();

			// 새로운 열의 시작은 InsertItem을 사용한다;
			m_ListItem.InsertItem( &lvItem );

			strTemp.Format( "%d", sInvenItem.sItemCustom.Sid() );
			m_ListItem.SetItemText( nNum, 1, strTemp );

			std::string strGuid = sc::string::uuidToString( ( UUID )sInvenItem.sItemCustom.guid );
			strTemp.Format( "%s", strGuid.c_str() );
			m_ListItem.SetItemText( nNum, 2, strTemp );

			m_ListItem.SetItemText( nNum, 3, pItemData->GetName() );		

			SCLUBINVENITEM sClubInvenItem;
			sClubInvenItem.sItem = sInvenItem;
			sClubInvenItem.nChannel = i;
			m_vInvenItem.push_back( sClubInvenItem );
			m_vInvenKey.push_back( (*iter).first );

			nNum++;
		}
	}	

	UpdateData(FALSE);
}

///////////////////////////////////////////////////////////////////////////////
// Delete Item
void CPropertyChaClubInven::OnBnClickedBtnInvenDelete()
{
	CMainFrame* pFrame = static_cast<CMainFrame*>( AfxGetMainWnd() );
	if( pFrame->m_nUserLevel < USER_GM1 )
	{
		MessageBox( _T("Permission Denied") );
		return;
	}

	int nSelected = m_ListItem.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED );

	// 선택된 항목이 있음;
	if( nSelected != -1 )
	{
		if( MessageBox( _T("Really delete?"), _T("DELETE"), MB_YESNO ) == IDYES )
		{
			
			SCLUBINVENITEM sClubItem = m_vInvenItem[ nSelected ];
			if( dm::CDataManagerClubInven::Instance()->DeleteItem(
				sClubItem.nChannel,
				sClubItem.sItem.wPosX,
				sClubItem.sItem.wPosY ) )
			{
				SetData();

				///////////////////////////////////////////////////////////////////////////
				// GM Log
				CString strUserIP = m_pDBMan->GetLocalIP();
				int nGmNum = pFrame->m_nUserNum;
				std::strstream strGmCmd;
				strGmCmd << "Club Inventory(" << dm::CDataManagerClubInven::Instance()->GetChaNum() << ") ";
				strGmCmd << " Item(" << sClubItem.sItem.sItemCustom.Mid() << "," << sClubItem.sItem.sItemCustom.Sid() << ") Delete";
				strGmCmd << '\0';
				m_pDBMan->WriteGMLog( nGmNum, strGmCmd, strUserIP );
				strGmCmd.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
				///////////////////////////////////////////////////////////////////////////

				printf( "Item이 삭제되었습니다. (%d, %d, %d)",
					sClubItem.nChannel,
					sClubItem.sItem.wPosX,
					sClubItem.sItem.wPosY );
			}
			else
			{
				printf( "Delete Failed" );
			}	
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Save Inventory
void CPropertyChaClubInven::OnBnClickedBtnInvenSave()
{
	if( !dm::CDataManagerClubInven::Instance()->GetUseStorageState() )
	{
		MessageBox(_T("You are not a club master"));

		return;
	}

	CMainFrame* pFrame = (CMainFrame*) AfxGetMainWnd();
	if (pFrame->m_nUserLevel < USER_GM1)
	{
		MessageBox(_T("Permission Denied"));
		return;
	}

	UpdateData(TRUE);

	dm::CDataManagerClubInven::Instance()->SetClubInvenMoney(
		m_llMoney );

	dm::CDataManagerClubInven::Instance()->SaveClubInvenItemToDB();
	SetData();

	///////////////////////////////////////////////////////////////////////////
	// GM Log
	CString strUserIP = m_pDBMan->GetLocalIP();
	int nGmNum = pFrame->m_nUserNum;
	std::strstream strGmCmd;
	strGmCmd << "Club Master DbNum(" <<
		dm::CDataManagerClubInven::Instance()->GetChaNum() << ") Club Inventory Save";
	strGmCmd << '\0';
	m_pDBMan->WriteGMLog( nGmNum, strGmCmd, strUserIP );
	strGmCmd.freeze( false );									// Note : std::strstream의 freeze. 안 하면 Leak 발생.
	///////////////////////////////////////////////////////////////////////////

}

void CPropertyChaClubInven::OnOK()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	OnBnClickedBtnInvenSave();

	CPropertyPage::OnOK();
}