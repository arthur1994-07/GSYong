// MapsListPage.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../../SigmaCore/File/StringFile.h"
#include "../../SigmaCore/File/SerialFile.h"

#include "../../EngineLib/G-Logic/GLogic.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Land/GLMapList.h"

#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "../SpecialTool.h"
#include "../SheetWithTab.h"
#include "./MapsListPage.h"
#include "./MapsListData.h"

// CMapsListPage 대화 상자입니다.
IMPLEMENT_DYNAMIC(CMapsListPage, CPropertyPage)
CMapsListPage::CMapsListPage( LOGFONT logfont )
	: CPropertyPage(CMapsListPage::IDD)
	, m_sFindID( NATIVEID_NULL() )
	, m_bEdit(false)
	, m_nResultSortColumn( 0 )
	, m_bResultSortDirect( false )
	, m_bUpdate( false )
{
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CMapsListPage::~CMapsListPage()
{
	SAFE_DELETE ( m_pFont );
}

void CMapsListPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MAPSLIST, m_ListMapsList);
}


BEGIN_MESSAGE_MAP(CMapsListPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_MAPSLIST, OnNMDblclkListMapslist)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_FIND_ID, OnBnClickedButtonFindId)
	ON_BN_CLICKED(IDC_BUTTON_FIND_MAPNAME, OnBnClickedButtonFindMapname)
	ON_NOTIFY(HDN_ITEMCLICK, 0, OnHdnItemclickListMapslist)
    ON_BN_CLICKED(IDC_BUTTON_CONVERT_TO_LUA, &CMapsListPage::OnBnClickedButtonConvertToLua)
END_MESSAGE_MAP()

BOOL CMapsListPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	RECT rectCtrl;
	LONG lnWidth;
	LV_COLUMN lvColumn;

	m_ListMapsList.SetExtendedStyle ( m_ListMapsList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_ListMapsList.GetClientRect ( &rectCtrl );
	lnWidth = rectCtrl.right - rectCtrl.left;

	const int nColumnCount = 34;	
	char* szColumnName1[nColumnCount] =
	{ 
		"Mid,Sid", "MapName", "Used", "FileName", "FieldServer",
		"Bgm", "LoadingImg", "RateExpDrop", "RateItemDrop", "Piece",
		"CommissionFree", "PKEnable", "FreePK", "ItemDrop", "FriendCard", 
		"Resurrection Forbid", "PatActivity", "ExpDrop", "VehicleActivity", "ClubBattleZone", 
		"InstantMap", "QBoxDrop", "LunchBox Forbid", "CPRest", "PKMap", 
		"UIMapSelect", "UIMapInfo", "PartyMap", "ExpeditionMap", "PrivateMarketOpenalbe", 
		"Sparring", "Cull By Object Map", "Rebirth Block", "EnableSwitchMob"
	};

	int nColumnWidthPercent1[nColumnCount] =
	{
		4, 10, 4, 12, 4,
		8, 8, 3, 3, 3, 
		3, 3, 3, 3, 3, 
		3, 3, 3, 3, 3, 
		3, 3, 3, 3, 3, 
		3, 3, 3, 3, 3,
		3, 3, 3, 3 };

	for ( int i = 0; i < nColumnCount; i++ )
	{		
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
		lvColumn.fmt = LVCFMT_CENTER;
		lvColumn.pszText = szColumnName1[i];
		lvColumn.iSubItem = i;
		lvColumn.cx = ( lnWidth*nColumnWidthPercent1[i] ) / 100;
		m_ListMapsList.InsertColumn (i, &lvColumn );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CMapsListPage::Init()
{
	m_sMapsList.CleanUpMapList();
    m_sMapsList.LoadMapList(TRUE); // "mapslist.mst"
	UpdateItems();

	m_bEdit = false;
}

void CMapsListPage::UpdateItems ()
{
	m_ListMapsList.DeleteAllItems();

	GLMapList::FIELDMAP_ITER iter = m_sMapsList.GetMapList().begin();
	GLMapList::FIELDMAP_ITER iter_end = m_sMapsList.GetMapList().end();

	CString strTemp;
	for ( ; iter != iter_end; ++iter )
	{		
		INT nCount = 0;
		const SMAPNODE* pMapNode = &(*iter).second;
		if ( !pMapNode ) continue;

		strTemp.Format( "%03d/%03d", pMapNode->m_NativeID.wMainID, pMapNode->m_NativeID.wSubID );
		int nIndex = m_ListMapsList.InsertItem( nCount++, strTemp );
		if ( nIndex == -1 )
			continue;

        m_ListMapsList.SetItemData( nIndex, pMapNode->m_NativeID.dwID );

		m_ListMapsList.SetItemText( nIndex, nCount++, pMapNode->m_MapID.c_str() );
		
		strTemp.Format( "%d", ( INT ) pMapNode->m_bUse );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		m_ListMapsList.SetItemText( nIndex, nCount++, pMapNode->m_LevelFile.c_str() );

		strTemp.Format( "%d", pMapNode->m_FieldServerID );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		m_ListMapsList.SetItemText( nIndex, nCount++, pMapNode->m_BgmName.c_str() );

		m_ListMapsList.SetItemText( nIndex, nCount++, pMapNode->m_LoadingImageName.c_str() );

		strTemp.Format( "%f", pMapNode->m_fRateExpDrop );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%f", pMapNode->m_fRateItemDrop );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bPeaceZone );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) !pMapNode->m_bCommission );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bPKZone );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bFreePK );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bItemDrop );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bFriendMove );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bRestart );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bPetActivity );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bDecExp );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bVehicleActivity );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bClubBattleZone );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_emInstanceType );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bQBoxDrop );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bLunchBoxForbid );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bCPReset );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bPKMap );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bUIMapSelect );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bUIMapInfo );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bPartyMap );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

        strTemp.Format( "%d", ( INT ) pMapNode->m_bExpeditionMap );
        m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bPrivateMarketOpenable );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

        strTemp.Format( "%d", ( INT ) pMapNode->m_bSparring );
        m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bCullByObjectMap );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", (INT) pMapNode->m_bRebirthBlock  );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", (INT) pMapNode->m_bDropBlockItem );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", (INT) pMapNode->m_bDisableSwitchMob );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bChangeSchool );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bMacro );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bPrison );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );

		strTemp.Format( "%d", ( INT ) pMapNode->m_bChangeCharSlot );
		m_ListMapsList.SetItemText( nIndex, nCount++, strTemp );
	}
	m_bUpdate = true;
	LRESULT Result = 0;
	OnHdnItemclickListMapslist( &m_sNMHDR, &Result );
	m_bUpdate = false;
}

bool CMapsListPage::NewFile()
{
	m_sMapsList.CleanUpMapList();
	UpdateItems();

	return true;
}

bool CMapsListPage::LoadFile()
{
	CString szFilter = "Maps List Setting File (*.lua;*.mst)|*.lua;*.mst|";

	HRESULT hr;
	hr = m_sMapsList.LoadMapList ( TRUE );

	if ( FAILED ( hr ) )
	{
		MessageBox ( "Failure to Load File" );
		return false;
	}

	UpdateItems();
	m_bEdit = false;
	MessageBox("Mapslist Load Complit");
	return true;
}

bool CMapsListPage::LoadFileOld()
{
	MessageBox("LoadFileOld Not Use");
// 	CString szFilter = "Item Mix Setting File (*.ini)|*.ini|";
// 
// 	//	Note : 파일 오픈 다이알로그를 만듬.
// 	CFileDialog dlg(TRUE,".ini",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
// 		(CItemMixPage*)this);
// 
// 	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
// 	if ( dlg.DoModal() == IDOK )
// 	{
// 		HRESULT hr;
//         hr = m_sMapsList.LoadMapList( dlg.GetFileName().GetString(), TRUE );
// 
// 		if ( FAILED ( hr ) )
// 		{
// 			MessageBox ( "Failure to Load File OLD" );
// 			return false;
// 		}
// 
// 		UpdateItems();
// 		m_bEdit = false;
// 	}

	return true;
}

bool CMapsListPage::SaveFile()
{
	CString szFilter = "Maps List File (*.lua;*.mst)|*.lua;*.mst|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".mst|.lua",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemMixPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		// hr = m_sMapsList.SaveMapListLua( dlg.GetFileName().GetString() );
		// hr = m_sMapsList.SaveMapList( dlg.GetFileName().GetString() );
		hr = m_sMapsList.SAVE( dlg.GetFileName().GetString() );
		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Save File" );
			return false;
		}

		m_bEdit = false;

		return true;
	}

	return false;
}

bool CMapsListPage::SaveCsv()
{
	CString szFilter = "Maps List Csv File (*.csv)|*.csv|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".csv",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAutoLevelPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		std::string strFileName = dlg.GetPathName();

		std::fstream streamFILE;
		streamFILE.open ( strFileName.c_str(), std::ios_base::out );
		bool bOK = m_sMapsList.SaveCsv( streamFILE );
		streamFILE.close();

		if ( !bOK )
		{
			MessageBox ( "CMapsListPage::SaveCsv, Error GLItemMixMan", "", MB_OK );
			return false;
		}

		return true;
	}

	return true;	
}

bool CMapsListPage::LoadCsv()
{
	CString szFilter = "Maps List Csv File (*.csv)|*.csv|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".csv",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAutoLevelPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		std::string strFileName = dlg.GetPathName();
		bool bOK = m_sMapsList.LoadCsv( strFileName );
		if ( !bOK )
		{
			MessageBox ( "CMapsListPage::LoadCsv, Error GLItemMixMan", "", MB_OK );
			return false;
		}

		UpdateItems();

		return true;
	}

	return true;	
}

void CMapsListPage::OnBnClickedButtonAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMapsListData dlg;

	dlg.m_sMapNode.m_BgmName = "null";
	dlg.m_sMapNode.m_LevelFile = "null";
	dlg.m_sMapNode.m_LoadingImageName = "null";
	dlg.m_sMapNode.m_MapID = "null";

	if ( dlg.DoModal() == IDOK )
	{
		std::pair< GLMapList::FIELDMAP_ITER, bool > pr;
		pr = m_sMapsList.GetMapList().insert( std::make_pair( dlg.m_sMapNode.m_NativeID.dwID, dlg.m_sMapNode ) );
		if( pr.second != true )   
		{
			MessageBox( "Mid,Sid Exist" );
			return;
		}
		UpdateItems();

		int nSize = m_ListMapsList.GetItemCount();
		m_ListMapsList.SetItemState( nSize-1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 

		CSize sz(0,9999);
		m_ListMapsList.Scroll(sz);

		m_bEdit = true;
	}
}

void CMapsListPage::OnBnClickedButtonEdit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMapsListData dlg;

	int nSelect = m_ListMapsList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if( nSelect == -1 ) return;

	DWORD dwKey = (DWORD)m_ListMapsList.GetItemData( nSelect );

	GLMapList::FIELDMAP_ITER iter = m_sMapsList.GetMapList().find( dwKey );
	GLMapList::FIELDMAP_ITER iter_end = m_sMapsList.GetMapList().end();

	if ( iter == iter_end ) return;
	dlg.m_sMapNode = iter->second;	

	if ( dlg.DoModal() == IDOK )
	{
		DWORD dwComp = dlg.m_sMapNode.m_NativeID.dwID;
		if( dwKey != dwComp )
		{
			GLMapList::FIELDMAP_ITER iter_comp = m_sMapsList.GetMapList().find( dwComp );

			if( iter_comp != iter_end)
			{
                MessageBox( "Mid,Sid Exist" );
				return;
			}
			else
			{
				m_sMapsList.GetMapList().erase( iter );
				m_sMapsList.GetMapList().insert( std::make_pair( dlg.m_sMapNode.m_NativeID.dwID, dlg.m_sMapNode ) );
			}
		}
		else
		{
			iter->second = dlg.m_sMapNode;
		}
		int nPos = m_ListMapsList.GetScrollPos( SB_VERT );
		UpdateItems();

		m_ListMapsList.SetItemState( nSelect, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 

		CRect ref;
		m_ListMapsList.GetItemRect(0,ref,LVIR_BOUNDS);   // 한칸의 높이를 측정

		CSize sz(0,nPos*ref.Height());
		m_ListMapsList.Scroll(sz);

		m_bEdit = true;
	}
}

void CMapsListPage::OnBnClickedButtonDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSelect = m_ListMapsList.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if( nSelect == -1 ) return;

	if ( IDNO == MessageBox ( "Delete?", "Caution", MB_YESNO ) )	return;

	DWORD dwKey = (DWORD)m_ListMapsList.GetItemData( nSelect );
	DWORD dwSize = (DWORD)m_ListMapsList.GetItemCount();

	GLMapList::FIELDMAP_ITER iter = m_sMapsList.GetMapList().find( dwKey );
	GLMapList::FIELDMAP_ITER iter_end = m_sMapsList.GetMapList().end();

	if ( iter == iter_end ) return;

	m_sMapsList.GetMapList().erase( iter );

	UpdateItems();

	m_bEdit = true;	
}

void CMapsListPage::OnNMDblclkListMapslist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	NMITEMACTIVATE* pNMIA = ( NMITEMACTIVATE* ) pNMHDR;

	if ( pNMIA->iItem != -1 )
	{
		OnBnClickedButtonEdit();
	}
	*pResult = 0;
}


void CMapsListPage::OnBnClickedButtonFindId()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_sFindID.wMainID = GetWin_Num_int( this, IDC_EDIT_FINDMID );
	m_sFindID.wSubID = GetWin_Num_int( this, IDC_EDIT_FINDSID );

	int nSize = m_ListMapsList.GetItemCount();
	SNATIVEID sNativeID;
	for( INT i = 0; i < nSize; i++ )
	{
		sNativeID.dwID = ( DWORD )m_ListMapsList.GetItemData( i );
		if( sNativeID.dwID == m_sFindID.dwID )
		{
			int nPage = m_ListMapsList.GetCountPerPage();

			m_ListMapsList.SetItemState( i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 

			//	스크롤바 설정
			{
				CRect ref;
				m_ListMapsList.GetItemRect(0,ref,LVIR_BOUNDS);   // 한칸의 높이를 측정

				int nSize = 0;
				int nIndex = i - ( nPage /2 );
				int nPos = m_ListMapsList.GetScrollPos( SB_VERT );

				if ( nIndex < 0 ) nIndex = 0;

				nIndex = nIndex - nPos;

				nSize = nIndex * ref.Height();		

				CSize sz( 0, nSize );
				m_ListMapsList.Scroll(sz);
			}
			return;
		}
	}

	MessageBox( "Not Find" );
}

void CMapsListPage::OnBnClickedButtonFindMapname()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_strFindMapName = GetWin_Text( this, IDC_EDIT_FIND_MAPNAME );

	int nSize = m_ListMapsList.GetItemCount();

	std::string strTemp;
	for( INT i = 0; i < nSize; i++ )
	{
		strTemp = m_ListMapsList.GetItemText( i, 1 );
		if( m_strFindMapName == strTemp )
		{
			int nPage = m_ListMapsList.GetCountPerPage();

			m_ListMapsList.SetItemState( i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 

			//	스크롤바 설정
			{
				CRect ref;
				m_ListMapsList.GetItemRect(0,ref,LVIR_BOUNDS);   // 한칸의 높이를 측정

				int nSize = 0;
				int nIndex = i - ( nPage /2 );
				int nPos = m_ListMapsList.GetScrollPos( SB_VERT );

				if ( nIndex < 0 ) nIndex = 0;

				nIndex = nIndex - nPos;

				nSize = nIndex * ref.Height();		

				CSize sz( 0, nSize );
				m_ListMapsList.Scroll(sz);
			}
			return;
		}
	}
	MessageBox( "Not Find" );
}

INT CALLBACK CMapsListPage::CompareFunc(LPARAM LParam1, LPARAM lParam2, LPARAM lParamSort )
{
	CListCtrl* pListCtrl = ( ( SORTPARAM* ) lParamSort)->pList;
	INT nColumn = ( ( SORTPARAM* ) lParamSort)->sortColumn;
	bool bDirect = ( ( SORTPARAM* ) lParamSort)->sortDirect;

	LVFINDINFO info1, info2;
	info1.flags = LVFI_PARAM;
	info1.lParam = LParam1;

	info2.flags = LVFI_PARAM;
	info2.lParam = lParam2;

	int nRow1 = pListCtrl->FindItem( &info1, -1 );
	int nRow2 = pListCtrl->FindItem( &info2, -1 );

	CString strItem1 = pListCtrl->GetItemText( nRow1, nColumn );
	CString strItem2 = pListCtrl->GetItemText( nRow2, nColumn );

	return bDirect ? strcmp( strItem1, strItem2 ) : - strcmp( strItem1, strItem2 );
}


void CMapsListPage::OnHdnItemclickListMapslist(NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_sNMHDR.code = pNMHDR->code;
	m_sNMHDR.hwndFrom = pNMHDR->hwndFrom;
	m_sNMHDR.idFrom = pNMHDR->idFrom;

	SORTPARAM sParam;
    if( !m_bUpdate )
	{
		if( phdr->hdr.hwndFrom == m_ListMapsList.GetDlgItem( 0 )->GetSafeHwnd() )
		{
			sParam.pList = &m_ListMapsList;
			if( m_nResultSortColumn == phdr->iItem )
			{
				m_bResultSortDirect = !m_bResultSortDirect;
			}
			else
			{
				m_nResultSortColumn = phdr->iItem;
				m_bResultSortDirect = true;
			}
			sParam.sortColumn = m_nResultSortColumn;
			sParam.sortDirect = m_bResultSortDirect;
		}
		else
		{
			return;
		}
	}
	else
	{
		//if( phdr->hdr.hwndFrom == m_ListMapsList.GetDlgItem( 0 )->GetSafeHwnd() )
		{
			sParam.pList = &m_ListMapsList;
			sParam.sortColumn = m_nResultSortColumn;
			sParam.sortDirect = m_bResultSortDirect;
		}
	}

	sParam.pList->SortItems( CompareFunc, ( LPARAM ) &sParam );
	*pResult = 0;
}

void CMapsListPage::OnBnClickedButtonConvertToLua()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    // m_sMapsList.SaveMapListLua();
	m_sMapsList.SaveLua();
}
