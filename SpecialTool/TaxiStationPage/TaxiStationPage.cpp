// TaxiStationPage.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"

#include "../../EngineLib/G-Logic/GLogic.h"

#include "../../RanLogic/Transport/GLTaxiStation.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../MfcExLib/ExLibs/EtcFunction.h"

#include "../../RanLogic/GLUseFeatures.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "./TaxiData.h"
#include "./TaxiStationPage.h"
#include "../SpecialTool.h"
#include "../SheetWithTab.h"

// CTaxiStationPage ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CTaxiStationPage, CPropertyPage)
CTaxiStationPage::CTaxiStationPage( LOGFONT logfont )
	: CPropertyPage(CTaxiStationPage::IDD)
	, m_pFont(NULL)
	, m_bEdit(false)
	, m_nFindIndexMap(-1)
	, m_nFindIndexNpc(-1)
	, m_sFindID(false)
{
	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CTaxiStationPage::~CTaxiStationPage()
{
	SAFE_DELETE ( m_pFont );
}

void CTaxiStationPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TAXISTATION, m_ListTaxi);
}


BEGIN_MESSAGE_MAP(CTaxiStationPage, CPropertyPage)
	ON_BN_CLICKED(IDC_BUTTON_UP, OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, OnBnClickedButtonDown)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_FIND, OnBnClickedButtonFind)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, OnBnClickedButtonNext)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_TAXISTATION, OnNMDblclkListTaxistation)
END_MESSAGE_MAP()


// CTaxiStationPage �޽��� ó�����Դϴ�.

BOOL CTaxiStationPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_pSheetTab->ChangeDialogFont( this, m_pFont, CDF_TOPLEFT );

	RECT rectCtrl;
	LONG lnWidth;
	LV_COLUMN lvColumn;

	
	m_ListTaxi.SetExtendedStyle ( m_ListTaxi.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	m_ListTaxi.GetClientRect ( &rectCtrl );
	lnWidth = rectCtrl.right - rectCtrl.left;

	const int nColumnCount = 8;	
	char* szColumnName1[nColumnCount] = { "ID", "Use", "MAP", "X/Y", "NPC", "Charge", "Map Name", "Npc Name" };
	
	int nColumnWidthPercent1[nColumnCount] = { 6, 6, 10, 10, 10, 10, 23, 23 };

	for ( int i = 0; i < nColumnCount; i++ )
	{		
		lvColumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH ;
		lvColumn.fmt = LVCFMT_CENTER;
		lvColumn.pszText = szColumnName1[i];
		lvColumn.iSubItem = i;
		lvColumn.cx = ( lnWidth*nColumnWidthPercent1[i] ) / 100;
		m_ListTaxi.InsertColumn (i, &lvColumn );
	}
	

	UpdateItems();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CTaxiStationPage::Init()
{
	
	GLTaxiStation::GetInstance().CleanUp();
    GLTaxiStation::GetInstance().LoadFile(GLogicData::GetInstance().GetTaxiStationFileName());
	UpdateItems();

	m_bEdit = false;
}

void CTaxiStationPage::UpdateItems ()
{

	m_ListTaxi.DeleteAllItems();

	
	SetWin_Num_int( this, IDC_EDIT_BASIC_CHARGE, GLTaxiStation::GetInstance().m_dwBasicCharge );

	
	CString strTemp;

	TAXI_VECTOR& vecTaxiMap = GLTaxiStation::GetInstance().m_vecTaxiMap;

	for ( int i = 0; i < vecTaxiMap.size(); ++i )
	{
		const STAXI_MAP& sTaxiMap = vecTaxiMap[i];

		for ( int j =0; j < sTaxiMap.vecTaxiStation.size(); ++j )
		{
			const STAXI_STATION& sTaxiStation = sTaxiMap.vecTaxiStation[j];
			
			strTemp.Format( "%03d", sTaxiStation.dwLINKID );
			int nIndex = m_ListTaxi.InsertItem( 0, strTemp );
			if ( nIndex == -1 ) continue;

			m_ListTaxi.SetItemData( nIndex, (DWORD_PTR)&sTaxiMap.vecTaxiStation[j] );

			if ( sTaxiStation.bUsed  )	strTemp.Format( "used" );
			else strTemp.Format( "unused" );
			
			m_ListTaxi.SetItemText( nIndex, 1, strTemp );

            SNATIVEID sMapID = sTaxiStation.dwMAPID;
			strTemp.Format( "%hd/%hd", sMapID.wMainID, sMapID.wSubID );
			m_ListTaxi.SetItemText( nIndex, 2, strTemp );

			strTemp.Format( "%hd/%hd", sTaxiStation.wPosX, sTaxiStation.wPosY );
			m_ListTaxi.SetItemText( nIndex, 3, strTemp );

			SNATIVEID sNpcID = sTaxiStation.dwNPCID;
			strTemp.Format( "%hd/%hd", sNpcID.wMainID, sNpcID.wSubID );
			m_ListTaxi.SetItemText( nIndex, 4, strTemp );

			strTemp.Format( "%d", sTaxiStation.dwMapCharge );
			m_ListTaxi.SetItemText( nIndex, 5, strTemp );

			if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() == TRUE )
			{
				const SNATIVEID sMapNativeID = sTaxiStation.dwMAPID;
				const SNATIVEID sNPCNativeID = sTaxiStation.dwNPCID;

				CString strFind;
				strFind.Format("CN_%03d_%03d", sNPCNativeID.wMainID, sNPCNativeID.wSubID);
				m_ListTaxi.SetItemText( nIndex, 6, CGameTextMan::GetInstance().GetCrowText(strFind.GetString()).GetString());
				strFind.Format("MAP_%d_%d", sMapNativeID.wMainID, sMapNativeID.wSubID);
				m_ListTaxi.SetItemText( nIndex, 7, CGameTextMan::GetInstance().GetGameInText(strFind.GetString()).GetString());
			}
			else
			{
				m_ListTaxi.SetItemText( nIndex, 6, sTaxiStation.strMAP.c_str() );
				m_ListTaxi.SetItemText( nIndex, 7, sTaxiStation.strSTATION.c_str() );
			}
		}
	}
}

void CTaxiStationPage::InverseUpdateItems()
{
	GLTaxiStation::GetInstance().m_dwBasicCharge = GetWin_Num_int( this, IDC_EDIT_BASIC_CHARGE );
	return;
}

bool CTaxiStationPage::NewFile()
{
	GLTaxiStation::GetInstance().CleanUp();
	UpdateItems();
	
	return true;
}

bool CTaxiStationPage::LoadFile()
{
	CString szFilter = "Taxi Station Setting File (*.tsf)|*.tsf|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".tsf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CTaxiStationPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		hr = GLTaxiStation::GetInstance().LoadFile(dlg.GetFileName().GetString());
		
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

bool CTaxiStationPage::LoadFileOld()
{
	CString szFilter = "Taxi Station Setting File (*.ini)|*.ini|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".ini",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CTaxiStationPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		hr = GLTaxiStation::GetInstance().LoadFile_OLD ( dlg.GetFileName().GetString(), true );
		
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

bool CTaxiStationPage::SaveFile()
{
	CString szFilter = "Taxi Station Setting File (*.tsf)|*.tsf|";
	
	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(FALSE,".tsf",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CTaxiStationPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		HRESULT hr;
		hr = GLTaxiStation::GetInstance().SaveFile ( dlg.GetFileName().GetString() );
		if ( FAILED ( hr ) )
		{
			MessageBox ( "Failure to Save File" );
			return false;
		}

		m_bEdit = false;

		return true;
	}

	return true;
}

bool CTaxiStationPage::SaveCsv()
{
	CString szFilter = "Taxi Station Csv File (*.csv)|*.csv|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(FALSE,".csv",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAutoLevelPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		std::string strFileName = dlg.GetFileName();

		std::fstream streamFILE;
		streamFILE.open ( strFileName.c_str(), std::ios_base::out );
		bool bOK = GLTaxiStation::GetInstance().SaveCsv( streamFILE );
		streamFILE.close();

		if ( !bOK )
		{
			MessageBox ( "CTaxiStationPage::SaveCsv, Error GLTaxiStation", "", MB_OK );
			return false;
		}

		return true;
	}

	return true;	
}

bool CTaxiStationPage::LoadCsv()
{
	CString szFilter = "Taxi Station Csv File (*.csv)|*.csv|";

	//	Note : ���� ���� ���̾˷α׸� ����.
	CFileDialog dlg(TRUE,".csv",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, szFilter,
		(CAutoLevelPage*)this);

	dlg.m_ofn.lpstrInitialDir = GLOGIC::GetPath ();
	if ( dlg.DoModal() == IDOK )
	{
		std::string strFileName = dlg.GetFileName();
		bool bOK = GLTaxiStation::GetInstance().LoadCsv( strFileName );

		if ( !bOK )
		{
			MessageBox ( "CTaxiStationPage::LoadCsv, Error GLTaxiStation", "", MB_OK );
			return false;
		}

		UpdateItems();

		return true;
	}

	return true;	
}

void CTaxiStationPage::OnBnClickedButtonUp()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nSelect = m_ListTaxi.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if( nSelect <= 0 ) return;

	STAXI_STATION* pTaxiStation = (STAXI_STATION*)m_ListTaxi.GetItemData( nSelect );
	if ( !pTaxiStation ) return;

	STAXI_STATION* pTaxiStationUP = (STAXI_STATION*)m_ListTaxi.GetItemData( nSelect-1 );
	if ( !pTaxiStationUP ) return;

	pTaxiStation->dwLINKID--;
	pTaxiStationUP->dwLINKID++;

	int nPos = m_ListTaxi.GetScrollPos( SB_VERT );
	
	int nTop = m_ListTaxi.GetTopIndex();
	int nPage = m_ListTaxi.GetCountPerPage();

	UpdateItems();

	m_ListTaxi.SetItemState( --nSelect, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 

	//	��ũ�ѹ� ����
	{
		CRect ref;
		m_ListTaxi.GetItemRect(0,ref,LVIR_BOUNDS);   // ��ĭ�� ���̸� ����
		
		int nSize = 0;
		
		if ( nSelect < nTop+1 ) nSize = (nSelect-1) * ref.Height();
		else nSize = nPos * ref.Height();

		CSize sz( 0, nSize );
		m_ListTaxi.Scroll(sz);
	}

	m_bEdit = true;
}

void CTaxiStationPage::OnBnClickedButtonDown()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nSelect = m_ListTaxi.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	int nSize = m_ListTaxi.GetItemCount();
	if( nSelect < 0 || nSelect >= nSize-1 ) return;

	STAXI_STATION* pTaxiStation = (STAXI_STATION*)m_ListTaxi.GetItemData( nSelect );
	if ( !pTaxiStation ) return;

	STAXI_STATION* pTaxiStationDW = (STAXI_STATION*)m_ListTaxi.GetItemData( nSelect+1 );
	if ( !pTaxiStationDW ) return;

	pTaxiStation->dwLINKID++;
	pTaxiStationDW->dwLINKID--;

	int nPos = m_ListTaxi.GetScrollPos( SB_VERT );

	int nTop = m_ListTaxi.GetTopIndex();
	int nPage = m_ListTaxi.GetCountPerPage();

	UpdateItems();

	m_ListTaxi.SetItemState( ++nSelect, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 
	
	
	//	��ũ�ѹ� ����
	{
		CRect ref;
		m_ListTaxi.GetItemRect(0,ref,LVIR_BOUNDS);   // ��ĭ�� ���̸� ����

		int nSize = 0;

		if ( nSelect+1 >= ( nTop + nPage ) ) nSize = ( nSelect-nPage+2 ) * ref.Height();
		else nSize = nPos * ref.Height();		

		CSize sz( 0, nSize );
		m_ListTaxi.Scroll(sz);
	}

	m_bEdit = true;
}

void CTaxiStationPage::OnBnClickedButtonAdd()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CTaxiData dlg;
	dlg.m_sTaxiStation.dwLINKID = m_ListTaxi.GetItemCount();
	dlg.m_sTaxiStation.bUsed = true;

	if ( dlg.DoModal() == IDOK )
	{
		GLTaxiStation::GetInstance().insert( dlg.m_sTaxiStation );
		UpdateItems();

		int nSize = m_ListTaxi.GetItemCount();
		m_ListTaxi.SetItemState( nSize-1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 

		CSize sz(0,9999);
		m_ListTaxi.Scroll(sz);

		m_bEdit = true;
	}

	return;
}

void CTaxiStationPage::OnBnClickedButtonEdit()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CTaxiData dlg;

	int nSelect = m_ListTaxi.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if( nSelect == -1 ) return;

	STAXI_STATION* pTaxiStation = (STAXI_STATION*)m_ListTaxi.GetItemData( nSelect );
	if ( !pTaxiStation ) return;

	dlg.m_sTaxiStation = *pTaxiStation;

	if ( dlg.DoModal() == IDOK )
	{
		int nPos = m_ListTaxi.GetScrollPos( SB_VERT );
		
		*pTaxiStation = dlg.m_sTaxiStation;
		UpdateItems();

		m_ListTaxi.SetItemState( nSelect, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 

		CRect ref;
		m_ListTaxi.GetItemRect(0,ref,LVIR_BOUNDS);   // ��ĭ�� ���̸� ����
		
		CSize sz(0,nPos*ref.Height());
		m_ListTaxi.Scroll(sz);

		m_bEdit = true;
	}
}

void CTaxiStationPage::OnBnClickedButtonDel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	int nSelect = m_ListTaxi.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if( nSelect == -1 ) return;

	if ( IDNO == MessageBox ( "Delete?", "Caution", MB_YESNO ) )	return;

	STAXI_STATION* pTaxiStation = (STAXI_STATION*)m_ListTaxi.GetItemData( nSelect );
	if ( !pTaxiStation ) return;

	DWORD dwKey = pTaxiStation->dwLINKID;
	DWORD dwSize = (DWORD)m_ListTaxi.GetItemCount();	


	TAXI_VECTOR& vecTaxiMap = GLTaxiStation::GetInstance().m_vecTaxiMap;

	for ( DWORD i=0; i< vecTaxiMap.size(); ++i )
	{
		STAXI_MAP &sTaxiMap = vecTaxiMap[i];
		if ( sTaxiMap.dwMAPID == pTaxiStation->dwMAPID )
		{
			
			VEC_STATION_ITER pos = sTaxiMap.vecTaxiStation.begin();
			VEC_STATION_ITER end = sTaxiMap.vecTaxiStation.end();			
			
			for ( ; pos != end; ++pos )
			{
				const STAXI_STATION& sTaxiStation = *pos;
				if ( sTaxiStation.dwLINKID == pTaxiStation->dwLINKID )
				{
					sTaxiMap.vecTaxiStation.erase( pos );

					i = vecTaxiMap.size();
					break;
				}
			}
		}
	}

	for ( DWORD i=0; i< vecTaxiMap.size(); ++i )
	{
		STAXI_MAP &sTaxiMap = vecTaxiMap[i];
				
		for ( DWORD j=0; j<sTaxiMap.vecTaxiStation.size(); ++j )
		{
			STAXI_STATION& sTaxiStation = sTaxiMap.vecTaxiStation[j];

			if ( sTaxiStation.dwLINKID > dwKey )
			{
				sTaxiStation.dwLINKID--;
			}
		}
	}

	int nPos = m_ListTaxi.GetScrollPos( SB_VERT );

	UpdateItems();

	CRect ref;
	m_ListTaxi.GetItemRect(0,ref,LVIR_BOUNDS);   // ��ĭ�� ���̸� ����
	
	CSize sz(0,nPos*ref.Height());
	m_ListTaxi.Scroll(sz);

	m_bEdit = true;	
}


void CTaxiStationPage::FindItem()
{
	TAXI_VECTOR& vecTaxiMap = GLTaxiStation::GetInstance().m_vecTaxiMap;

	if ( m_nFindIndexMap >= vecTaxiMap.size() )
	{
		MessageBox( "Not Find" );
		return;
	}

	for ( ; m_nFindIndexMap < vecTaxiMap.size(); ++m_nFindIndexMap )
	{
		for ( ; m_nFindIndexNpc < vecTaxiMap[m_nFindIndexMap].vecTaxiStation.size(); ++m_nFindIndexNpc )
		{
			if ( vecTaxiMap[m_nFindIndexMap].vecTaxiStation[m_nFindIndexNpc].dwNPCID == m_sFindID.dwID )
			{
				int nPage = m_ListTaxi.GetCountPerPage();
				DWORD dwKey = vecTaxiMap[m_nFindIndexMap].vecTaxiStation[m_nFindIndexNpc].dwLINKID;
				
				m_ListTaxi.SetItemState( dwKey, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED ); 
	
				CRect ref;
				m_ListTaxi.GetItemRect(0,ref,LVIR_BOUNDS);   // ��ĭ�� ���̸� ����

				int nSize = 0;
				int nIndex = dwKey - ( nPage /2 );
				int nPos = m_ListTaxi.GetScrollPos( SB_VERT );

				if ( nIndex < 0 ) nIndex = 0;

				nIndex = nIndex - nPos;

				nSize = nIndex * ref.Height();		

				CSize sz( 0, nSize );
				m_ListTaxi.Scroll(sz);

				return;
			}
		}

		m_nFindIndexNpc = 0;
	}

	//	find ����
	MessageBox( "Not Find" );
	return;

}

void CTaxiStationPage::OnBnClickedButtonFind()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_nFindIndexMap = 0;
	m_nFindIndexNpc = 0;

	m_sFindID.wMainID = GetWin_Num_int( this, IDC_EDIT_FIND_MID );
	m_sFindID.wSubID = GetWin_Num_int( this, IDC_EDIT_FIND_SID );

	FindItem();
}

void CTaxiStationPage::OnBnClickedButtonNext()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_nFindIndexNpc++;

	FindItem();
}

void CTaxiStationPage::OnNMDblclkListTaxistation(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	NMITEMACTIVATE* pNMIA = ( NMITEMACTIVATE* ) pNMHDR;

	if ( pNMIA->iItem != -1 )
	{
		OnBnClickedButtonEdit();
	}

	*pResult = 0;
}
