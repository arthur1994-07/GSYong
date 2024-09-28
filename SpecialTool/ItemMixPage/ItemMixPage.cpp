#include "stdafx.h"

#include "../../EngineLib/G-Logic/GLogic.h"
#include "../../RanLogic/GLogicDataMan.h"
//#include "../../RanLogic/Item/GLItemMixMan.h"
#include "../../RanLogic/Product/GLProductRecipeMan.h"
#include "../../RanLogic/Item/GLItemMan.h"

#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "../SpecialTool.h"
#include "../SheetWithTab.h"
#include "./ItemMixPage.h"
#include "./ItemMixData.h"

IMPLEMENT_DYNAMIC(CItemMixPage, CPropertyPage)

BEGIN_MESSAGE_MAP(CItemMixPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnBnClickedButtonDown)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ITEMMIX, OnNMDblclkListItemmix)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnBnClickedButtonFind)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnBnClickedButtonNext)
END_MESSAGE_MAP()

CItemMixPage::CItemMixPage( LOGFONT logfont )
	: CPropertyPage(CItemMixPage::IDD)
	, m_pFont(NULL)
	, m_nFindIndex(-1)
	, m_sFindID(false)
	, m_bEdit(false)
{
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CItemMixPage::~CItemMixPage()
{
	SAFE_DELETE( m_pFont );
}

void CItemMixPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ITEMMIX, m_ListItemMix);
}

BOOL CItemMixPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	RECT rectCtrl;
	LONG lnWidth;
	LV_COLUMN lvColumn;

	m_ListItemMix.SetExtendedStyle( m_ListItemMix.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_ListItemMix.GetClientRect( &rectCtrl );
	lnWidth = rectCtrl.right - rectCtrl.left;
	lnWidth -= ::GetSystemMetrics( SM_CXVSCROLL );

	const int nColumnCount = 25;
	char* szColumnName1[nColumnCount] = {
		"ID",
		"Rate",
		"Price",
		"Delay",
		"NeedLearn",
		"RandomOpt",
		"Attack",
		"Defense",
		"Result", "Num",
		"Material_01", "Num",
		"Material_02", "Num",
		"Material_03", "Num",
		"Material_04", "Num",
		"Material_05", "Num",
		"ReqLevel",
		"Type",
		"WS",
		"WSP",
		"Per(%)"
	};

	int nColumnWidthPercent1[nColumnCount] = {
		3, // ID
		4, // Rate
		5, // Price
		4, // Delay
		5, // NeedLearn
		5, // RandomOpt
		4, // Attack
		4, // Defense
		8, 3,
		8, 3,
		8, 3,
		8, 3,
		8, 3,
		8, 3,
		5,	//Level
		5,	//Type
		5,	//WS
		5,	//WSP
		5	//per
	};

	for( int i = 0; i < nColumnCount; i++ )
	{		
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
		lvColumn.fmt = LVCFMT_CENTER;
		lvColumn.pszText = szColumnName1[i];
		lvColumn.iSubItem = i;
		lvColumn.cx = ( lnWidth * nColumnWidthPercent1[ i ] ) / 100;
		m_ListItemMix.InsertColumn( i, &lvColumn );
	}

	UpdateItems();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CItemMixPage::Init()
{
	GLProductRecipeMan::GetInstance().CleanUp();
    GLProductRecipeMan::GetInstance().LoadFile( GLogicData::GetInstance().GetItemMixFileName(), true );
	UpdateItems();
	m_bEdit = false;
}

void CItemMixPage::UpdateItems ()
{
	m_ListItemMix.DeleteAllItems();
	
	PRODUCT_RECIPE_KEYMAP_ITER pos = GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.begin();
	PRODUCT_RECIPE_KEYMAP_ITER end = GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.end();
	DWORD dwMapSize = GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.size();

	CString temp;

	temp.Format("%d", dwMapSize);
	int len = temp.GetLength();
	temp.Format("%%0%dd", len);
	
	CString strTemp;
	CString strItemName;

	for ( ; pos != end; ++pos )
	{
		const SProductRecipe* pProductRecipe = &(*pos).second;
		if ( !pProductRecipe )
			continue;

		strTemp.Format( temp, pProductRecipe->dwKey );
		int nIndex = m_ListItemMix.InsertItem( 0, strTemp );
		if ( nIndex == -1 )
			continue;

		m_ListItemMix.SetItemData( nIndex, pProductRecipe->dwKey );

		strTemp.Format( "%d", pProductRecipe->dwRate );
		m_ListItemMix.SetItemText( nIndex, 1, strTemp );

		strTemp.Format( "%d", pProductRecipe->dwPrice );
		m_ListItemMix.SetItemText( nIndex, 2, strTemp );

		strTemp.Format( "%d", INT( pProductRecipe->fDelaySec ) );
		m_ListItemMix.SetItemText( nIndex, 3, strTemp );

		strTemp.Format( "%s", pProductRecipe->bNeedLearn ? "true" : "false" );
		m_ListItemMix.SetItemText( nIndex, 4, strTemp );

		strTemp.Format( "%s", pProductRecipe->bRandomOption ? "true" : "false" );
		m_ListItemMix.SetItemText( nIndex, 5, strTemp );

		strTemp.Format( "%d", pProductRecipe->nGradeAttack );
		m_ListItemMix.SetItemText( nIndex, 6, strTemp );

		strTemp.Format( "%d", pProductRecipe->nGradeDefense );
		m_ListItemMix.SetItemText( nIndex, 7, strTemp );

		SNATIVEID sNativeID = pProductRecipe->sResultItem.sNID;
		SITEM* pItem = GLogicData::GetInstance().GetItem(sNativeID);
		if ( pItem )
		{
			strItemName = pItem->GetName();

			strTemp.Format( "%s(%hd/%hd)", strItemName, sNativeID.wMainID, sNativeID.wSubID );
			m_ListItemMix.SetItemText( nIndex, 8, strTemp );
			
			strTemp.Format( "%d", pProductRecipe->sResultItem.nNum );
			m_ListItemMix.SetItemText( nIndex, 9, strTemp );
		}

		int j = 0;
		for ( ; j<ITEMMIX_ITEMNUM; j++ )
		{
			SNATIVEID sNativeID = pProductRecipe->sMeterialItem[j].sNID;
			SITEM* pItem = GLogicData::GetInstance().GetItem(sNativeID);
			if ( pItem )
			{
				strItemName = pItem->GetName();

				strTemp.Format( "%s(%hd/%hd)", strItemName, sNativeID.wMainID, sNativeID.wSubID );
				m_ListItemMix.SetItemText( nIndex, (j*2)+10, strTemp );
			
				strTemp.Format( "%d", pProductRecipe->sMeterialItem[j].nNum );
				m_ListItemMix.SetItemText( nIndex, (j*2)+11, strTemp );
			}
		}

		strTemp.Format( "%d", pProductRecipe->wLevel );
		m_ListItemMix.SetItemText( nIndex, 20, strTemp );

		strTemp.Format( "%d", pProductRecipe->eProductType );
		m_ListItemMix.SetItemText( nIndex, 21, strTemp );

		strTemp.Format( "%d", pProductRecipe->wWs );
		m_ListItemMix.SetItemText( nIndex, 22, strTemp );

		strTemp.Format( "%d", pProductRecipe->wWsp );
		m_ListItemMix.SetItemText( nIndex, 23, strTemp );

		strTemp.Format( "%d", pProductRecipe->wWspPer );
		m_ListItemMix.SetItemText( nIndex, 24, strTemp );
		
	}
}

void CItemMixPage::OnBnClickedButtonAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CItemMixData dlg;
    //조합법키는 현재 조합법 키 갯수 +1;
	dlg.m_sProductRecipe.dwKey = m_ListItemMix.GetItemCount() + 1;

	if ( dlg.DoModal() == IDOK )
	{
		GLProductRecipeMan::GetInstance().InsertRecipe( dlg.m_sProductRecipe );
		UpdateItems();

		int nSize = m_ListItemMix.GetItemCount();
		m_ListItemMix.SetItemState( nSize-1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 

		CSize sz(0,9999);
		m_ListItemMix.Scroll(sz);

		m_bEdit = true;
	}

	return;
}

void CItemMixPage::OnBnClickedButtonEdit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CItemMixData dlg;

	int nSelect = m_ListItemMix.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if( nSelect == -1 ) return;

	DWORD dwKey = (DWORD)m_ListItemMix.GetItemData( nSelect );

	PRODUCT_RECIPE_KEYMAP_ITER pos = GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.find( dwKey );
	PRODUCT_RECIPE_KEYMAP_ITER end = GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.end();
	if ( pos == end )
		return;

	dlg.m_sProductRecipe = pos->second;	

	if ( dlg.DoModal() == IDOK )
	{
		int nPos = m_ListItemMix.GetScrollPos( SB_VERT );
		
		pos->second = dlg.m_sProductRecipe;
		UpdateItems();

		m_ListItemMix.SetItemState( nSelect, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 

		CRect ref;
		m_ListItemMix.GetItemRect(0,ref,LVIR_BOUNDS);   // 한칸의 높이를 측정
		
		CSize sz(0,nPos*ref.Height());
		m_ListItemMix.Scroll(sz);

		m_bEdit = true;
	}
}

void CItemMixPage::OnBnClickedButtonDel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSelect = m_ListItemMix.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if( nSelect == -1 ) return;

	if ( IDNO == MessageBox ( "Delete?", "Caution", MB_YESNO ) )
		return;

	DWORD dwKey = (DWORD)m_ListItemMix.GetItemData( nSelect );
	DWORD dwSize = (DWORD)m_ListItemMix.GetItemCount();

	PRODUCT_RECIPE_KEYMAP_ITER pos = GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.find( dwKey );
	PRODUCT_RECIPE_KEYMAP_ITER end = GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.end();
	if ( pos == end )
		return;

	GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.erase( pos );

	for ( DWORD i = dwKey+1; i < dwSize; ++i )
	{
		pos = GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.find( i );
		end = GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.end();

		if ( pos == end ) 
		{
			MessageBox( "Critical Error Delete Item Error", "", MB_OK );
			return;
		}

		SProductRecipe sProductRecipe = pos->second;
		sProductRecipe.dwKey--;

		GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.erase( pos );
		GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.insert( std::make_pair( sProductRecipe.dwKey, sProductRecipe ) );
	}

	int nPos = m_ListItemMix.GetScrollPos( SB_VERT );

	UpdateItems();

	CRect ref;
	m_ListItemMix.GetItemRect(0,ref,LVIR_BOUNDS);   // 한칸의 높이를 측정
	
	CSize sz(0,nPos*ref.Height());
	m_ListItemMix.Scroll(sz);

	m_bEdit = true;
}

void CItemMixPage::OnBnClickedButtonUp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSelect = m_ListItemMix.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if( nSelect <= 0 ) return;

	DWORD dwKey = (DWORD)m_ListItemMix.GetItemData( nSelect );

	PRODUCT_RECIPE_KEYMAP_ITER pos = GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.find( dwKey );
	PRODUCT_RECIPE_KEYMAP_ITER end = GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.end();
	if ( pos == end )
		return;

	SProductRecipe sProductRecipe = pos->second;
	sProductRecipe.dwKey--;

	GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.erase( pos );

	pos = GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.find( dwKey-1 );
	if ( pos == end ) 
	{
		MessageBox( "Critical Error", "", MB_OK );
	}

	SProductRecipe sProductRecipe2 = pos->second;
	sProductRecipe2.dwKey++;

	GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.erase( pos );

	GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.insert( std::make_pair( sProductRecipe.dwKey, sProductRecipe ) );
	GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.insert( std::make_pair( sProductRecipe2.dwKey, sProductRecipe2 ) );

	int nPos = m_ListItemMix.GetScrollPos( SB_VERT );
	
	int nTop = m_ListItemMix.GetTopIndex();
	int nPage = m_ListItemMix.GetCountPerPage();

	UpdateItems();

	m_ListItemMix.SetItemState( --nSelect, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 

	//	스크롤바 설정
	{
		CRect ref;
		m_ListItemMix.GetItemRect(0,ref,LVIR_BOUNDS);   // 한칸의 높이를 측정
		
		int nSize = 0;
		
		if ( nSelect < nTop+1 ) nSize = (nSelect-1) * ref.Height();
		else nSize = nPos * ref.Height();

		CSize sz( 0, nSize );
		m_ListItemMix.Scroll(sz);
	}

	m_bEdit = true;
}

void CItemMixPage::OnBnClickedButtonDown()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSelect = m_ListItemMix.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	int nSize = m_ListItemMix.GetItemCount();
	if( nSelect < 0 || nSelect >= nSize-1 )
		return;

	DWORD dwKey = (DWORD)m_ListItemMix.GetItemData( nSelect );

	PRODUCT_RECIPE_KEYMAP_ITER pos = GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.find( dwKey );
	PRODUCT_RECIPE_KEYMAP_ITER end = GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.end();
	if ( pos == end )
		return;

	SProductRecipe sProductRecipe = pos->second;
	sProductRecipe.dwKey++;

	GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.erase( pos );

	pos = GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.find( dwKey+1 );
	if ( pos == end ) 
	{
		MessageBox( "Critical Error", "", MB_OK );
		return;
	}

	SProductRecipe sProductRecipe2 = pos->second;
	sProductRecipe2.dwKey--;

	GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.erase( pos );

	GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.insert( std::make_pair( sProductRecipe.dwKey, sProductRecipe ) );
	GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.insert( std::make_pair( sProductRecipe2.dwKey, sProductRecipe2 ) );

	int nPos = m_ListItemMix.GetScrollPos( SB_VERT );

	int nTop = m_ListItemMix.GetTopIndex();
	int nPage = m_ListItemMix.GetCountPerPage();

	UpdateItems();

	m_ListItemMix.SetItemState( ++nSelect, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 
	
	
	//	스크롤바 설정
	{
		CRect ref;
		m_ListItemMix.GetItemRect(0,ref,LVIR_BOUNDS);   // 한칸의 높이를 측정

		int nSize = 0;

		if ( nSelect+1 >= ( nTop + nPage ) ) nSize = ( nSelect-nPage+2 ) * ref.Height();
		else nSize = nPos * ref.Height();		

		CSize sz( 0, nSize );
		m_ListItemMix.Scroll(sz);
	}

	m_bEdit = true;
}

void CItemMixPage::OnNMDblclkListItemmix(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	NMITEMACTIVATE* pNMIA = ( NMITEMACTIVATE* ) pNMHDR;
	if ( pNMIA->iItem != -1 )
		OnBnClickedButtonEdit();

	*pResult = 0;
}

bool CItemMixPage::NewFile()
{
	GLProductRecipeMan::GetInstance().CleanUp();
	UpdateItems();

	return true;
}

bool CItemMixPage::LoadFile()
{
	CString szFilter = "Item Mix Setting File (*.ims)|*.ims|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".ims",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemMixPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
        std::string strFileName = dlg.GetPathName();
		HRESULT hr;
		hr = GLProductRecipeMan::GetInstance().LoadFile( strFileName.c_str(), dlg.GetFileName().GetString() , TRUE);
		
		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Load File" );
			return false;
		}

		UpdateItems();

		m_bEdit = false;
	}

	return true;
}

bool CItemMixPage::LoadFileOld()
{
	CString szFilter = "Item Mix Setting File (*.ini)|*.ini|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".ini",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemMixPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
        std::string strFileName = dlg.GetPathName();
        HRESULT hr;
        hr = GLProductRecipeMan::GetInstance().LoadFile_OLD( strFileName.c_str(), dlg.GetFileName().GetString() , TRUE, true);
		
		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Load File OLD" );
			return false;
		}

		UpdateItems();

		m_bEdit = false;
	}

	return true;
}

bool CItemMixPage::SaveFile()
{
	CString szFilter = "Item Setting File (*.ims)|*.ims|";
	
	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".ims",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CItemMixPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
        std::string strFileName = dlg.GetPathName();
		HRESULT hr;
		hr = GLProductRecipeMan::GetInstance().SaveFile ( strFileName.c_str() );
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

bool CItemMixPage::SaveCsv()
{
	CString szFilter = "ItemMix Csv File (*.csv)|*.csv|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(FALSE,".csv",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAutoLevelPage*)this);
    
    dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
    if ( dlg.DoModal() == IDOK )
    {
        std::string strFileName = dlg.GetPathName();
        std::fstream streamFILE;
        streamFILE.open ( strFileName.c_str(), std::ios_base::out );
		bool bOK = GLProductRecipeMan::GetInstance().SaveCsv( streamFILE );
		streamFILE.close();

		if ( !bOK )
		{
			MessageBox ( "GLItemMixMan::SaveCsv, Error GLItemMixMan", "", MB_OK );
			return false;
		}

		return true;
	}

	return true;	
}

bool CItemMixPage::LoadCsv()
{
	CString szFilter = "ItemMix Csv File (*.csv)|*.csv|";

	//	Note : 파일 오픈 다이알로그를 만듬.
	CFileDialog dlg(TRUE,".csv",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAutoLevelPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		std::string strFileName = dlg.GetPathName();
		bool bOK = GLProductRecipeMan::GetInstance().LoadCsv( strFileName );

		if ( !bOK )
		{
			MessageBox ( "GLItemMixMan::LoadCsv, Error GLItemMixMan", "", MB_OK );
			return false;
		}

		UpdateItems();

		return true;
	}

	return true;	
}

void CItemMixPage::FindItem()
{
	int nSize = m_ListItemMix.GetItemCount();

	if ( m_nFindIndex >= nSize ) 
	{
		MessageBox( "Not Find" );
		return;
	}

	PRODUCT_RECIPE_KEYMAP_ITER end = GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.end();

	for ( ; m_nFindIndex < nSize; ++m_nFindIndex )
	{
		PRODUCT_RECIPE_KEYMAP_ITER pos = GLProductRecipeMan::GetInstance().m_mapKeyProductRecipe.find( m_nFindIndex );
		if ( pos == end ) continue;

		const SProductRecipe* pProductRecipe = &pos->second;

		for ( int i = 0; i < ITEMMIX_ITEMNUM; ++i )
		{
			if( pProductRecipe->sMeterialItem[i].sNID == m_sFindID )
			{
				int nPage = m_ListItemMix.GetCountPerPage();
				
				m_ListItemMix.SetItemState( m_nFindIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 
	
				//	스크롤바 설정
				{
					CRect ref;
					m_ListItemMix.GetItemRect(0,ref,LVIR_BOUNDS);   // 한칸의 높이를 측정

					int nSize = 0;
					int nIndex = m_nFindIndex - ( nPage /2 );
					int nPos = m_ListItemMix.GetScrollPos( SB_VERT );

					if ( nIndex < 0 ) nIndex = 0;

					nIndex = nIndex - nPos;

					nSize = nIndex * ref.Height();		

					CSize sz( 0, nSize );
					m_ListItemMix.Scroll(sz);
				}

				return;
			}
		}

		if ( pProductRecipe->sResultItem.sNID == m_sFindID )
		{

			int nPage = m_ListItemMix.GetCountPerPage();
				
			m_ListItemMix.SetItemState( m_nFindIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 

			CRect ref;
			m_ListItemMix.GetItemRect(0,ref,LVIR_BOUNDS);   // 한칸의 높이를 측정

			int nSize = 0;
			int nIndex = m_nFindIndex - ( nPage /2 );
			int nPos = m_ListItemMix.GetScrollPos( SB_VERT );

			if ( nIndex < 0 ) nIndex = 0;
			
			nIndex = nIndex - nPos;
			nSize = nIndex * ref.Height();		

			CSize sz( 0, nSize );
			m_ListItemMix.Scroll(sz);
			return;
		}
	}

	MessageBox( "Not Find" );
}

void CItemMixPage::OnBnClickedButtonFind()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nFindIndex = 0;

	m_sFindID.wMainID = GetWin_Num_int( this, IDC_EDIT_FIND_MID );
	m_sFindID.wSubID = GetWin_Num_int( this, IDC_EDIT_FIND_SID );

	FindItem();
}

void CItemMixPage::OnBnClickedButtonNext()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nFindIndex++;

	FindItem();
}