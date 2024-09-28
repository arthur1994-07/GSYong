#include "stdafx.h"

#include "resource.h"
#include "SpecialTool.h"
#include "SheetWithTab.h"
#include "SpecialToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

std::string g_strEditSheet[PAGE_NUM] =
{
	"Item_Mix_Page",
	"Maps_List_Page",
	"Taxi_Station_Page",
	"Auto_Level_Page"
};

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

IMPLEMENT_DYNAMIC(CsheetWithTab, CPropertySheet)

CsheetWithTab::CsheetWithTab(CRect& rect, LOGFONT logfont, CWnd* pParentWnd)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pParentWnd),
	 m_bNoTabs(FALSE),
	 m_pWndParent(NULL),
	 m_iDistanceFromTop(0),
	 m_ItemMixPage ( logfont ),
	 m_MapsListPage ( logfont ),
	 m_TaxiStationPage ( logfont ),
	 m_AutoLevelPage ( logfont ),
	 m_pFont ( NULL )
{
	m_pWndParent = pParentWnd;
	m_Rect = rect;

	m_bNoTabs = TRUE;
	m_bStacked = FALSE;

	m_ItemMixPage.SetSheetTab ( this );		//0
	AddPage ( &m_ItemMixPage );

	m_MapsListPage.SetSheetTab( this );		//1
	AddPage ( &m_MapsListPage );

	m_TaxiStationPage.SetSheetTab ( this );	//2
	AddPage ( &m_TaxiStationPage );	

	m_AutoLevelPage.SetSheetTab ( this );	//3
	AddPage( &m_AutoLevelPage );

	m_pFont = new CFont();
	m_pFont->CreateFontIndirect(&logfont);
}

CsheetWithTab::~CsheetWithTab()
{
	SAFE_DELETE( m_pFont );
}

static int module_piButtons[] = { IDOK, IDCANCEL, ID_APPLY_NOW, IDHELP };

BOOL CsheetWithTab::OnInitDialog() 
{
	HWND hTabWnd;
	CRect rectTabCtrl;

	// PropertyPage 폰트 강제 변경
	ChangeDialogFont( this, m_pFont, CDF_NONE );

	if( m_Rect.right )
	{
		// resize the tab control
		hTabWnd = (HWND)::GetDlgItem(m_hWnd, AFX_IDC_TAB_CONTROL);
		ASSERT(hTabWnd != NULL);
		::GetWindowRect(hTabWnd, &rectTabCtrl);  // Tab-control rectangle (screen coordinates)
		ScreenToClient(rectTabCtrl);

		//	Note : 탭을 보여주지 않을 때에는 사이즈의 조정이 필요 없으므로 무시한다.
		//
		// Is tab-control wider than specified width.
		if( !m_bNoTabs && rectTabCtrl.right > (m_Rect.Width() - 3) )
		{
			rectTabCtrl.right = m_Rect.Width() - 3; // Set new right position
		}

		// The tab-control does only need to show tabs
		rectTabCtrl.bottom = 32; // Height for tabs

		// move tab control
		::MoveWindow( hTabWnd, 0, m_iDistanceFromTop, rectTabCtrl.Width(), rectTabCtrl.bottom, TRUE );

		BOOL bResult = (BOOL)Default();          // Process messages

		// If propertysheet m_bNoTabs = TRUE hide the tabs
		if( m_bNoTabs )
		{
			::ShowWindow(hTabWnd, SW_HIDE);
			::EnableWindow(hTabWnd, FALSE);
		}

		// change tab style if scrolling tabs desired (stacked tabs are default)
		if (!m_bStacked)
		{
			hTabWnd = (HWND)SendMessage(PSM_GETTABCONTROL);
			if (hTabWnd != NULL)
			{
				CWnd::ModifyStyle(hTabWnd, TCS_MULTILINE, TCS_SINGLELINE, 0);
			}
		}

		// remove standard buttons 
		for (int i = 0; i < 4; i++)
		{
			HWND hWnd = ::GetDlgItem(m_hWnd, module_piButtons[i]);
			if (hWnd != NULL)
			{
				::ShowWindow(hWnd, SW_HIDE);
				::EnableWindow(hWnd, FALSE);
			}
		}

		// Move the propertysheet to specified position
		MoveWindow( m_Rect.left, m_Rect.top, m_Rect.right, m_Rect.bottom /*- cyDiff*/ );

		return bResult;
	}

	return CPropertySheet::OnInitDialog();
}



BEGIN_MESSAGE_MAP(CsheetWithTab, CPropertySheet)
	//{{AFX_MSG_MAP(CsheetWithTab)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



HBRUSH CsheetWithTab::CtlColor(CDC* pDc, UINT uCtlColor) 
{
	CWnd*    pwndParent;   // Parent window to property-sheet
	CDC*     pdcParent;    // Parent dc
	COLORREF color;        // color on parent dc           

	pwndParent = GetParent();
	ASSERT( pwndParent->IsKindOf( RUNTIME_CLASS( CWnd ) ) ); 

	pdcParent = pwndParent->GetDC();

	color = pdcParent->GetBkColor();    // Get parent color

	pwndParent->ReleaseDC( pdcParent );

	CBrush brush( color );              // Make a brush

	return (HBRUSH)brush;
}

BOOL CsheetWithTab::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CPropertySheet::PreTranslateMessage(pMsg);
}

void CsheetWithTab::ChangeDialogFont(CWnd* pWnd, CFont* pFont, int nFlag)
{
	CRect windowRect;

	// grab old and new text metrics
	TEXTMETRIC tmOld, tmNew;
	CDC * pDC = pWnd->GetDC();
	CFont * pSavedFont = pDC->SelectObject(pWnd->GetFont());
	pDC->GetTextMetrics(&tmOld);
	pDC->SelectObject(pFont);
	pDC->GetTextMetrics(&tmNew);
	pDC->SelectObject(pSavedFont);
	pWnd->ReleaseDC(pDC);


	CRect clientRect, newClientRect, newWindowRect;

	pWnd->GetWindowRect(windowRect);
	pWnd->GetClientRect(clientRect);

	double fHeigth = (double)(m_Rect.bottom-m_Rect.top) / clientRect.bottom;
	double fWidth = (double) (m_Rect.right-m_Rect.left) / clientRect.right;

	if (nFlag != CDF_NONE)
	{
		// calculate new dialog window rectangle

		long xDiff = windowRect.Width() - clientRect.Width();
		long yDiff = windowRect.Height() - clientRect.Height();
	
		newClientRect.left = newClientRect.top = 0;
		newClientRect.right = clientRect.right * fWidth;//tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		newClientRect.bottom = clientRect.bottom * fHeigth; //newHeight / oldHeight;

		if (nFlag == CDF_TOPLEFT) // resize with origin at top/left of window
		{
			newWindowRect.left = windowRect.left;
			newWindowRect.top = windowRect.top;
			newWindowRect.right = windowRect.left + newClientRect.right + xDiff;
			newWindowRect.bottom = windowRect.top + newClientRect.bottom + yDiff;
		}
		else if (nFlag == CDF_CENTER) // resize with origin at center of window
		{
			newWindowRect.left = windowRect.left - 
							(newClientRect.right - clientRect.right)/2;
			newWindowRect.top = windowRect.top -
							(newClientRect.bottom - clientRect.bottom)/2;
			newWindowRect.right = newWindowRect.left + newClientRect.right + xDiff;
			newWindowRect.bottom = newWindowRect.top + newClientRect.bottom + yDiff;
		}
		pWnd->MoveWindow(newWindowRect);
	}

	pWnd->SetFont(pFont);

	// iterate through and move all child windows and change their font.
	CWnd* pChildWnd = pWnd->GetWindow(GW_CHILD);

	while (pChildWnd)
	{
		pChildWnd->SetFont(pFont);
		pChildWnd->GetWindowRect(windowRect);

		CString strClass;
		::GetClassName(pChildWnd->m_hWnd, strClass.GetBufferSetLength(32), 31);
		strClass.MakeUpper();
		if(strClass==_T("COMBOBOX"))
		{
			CRect rect;
			pChildWnd->SendMessage(CB_GETDROPPEDCONTROLRECT,0,(LPARAM) &rect);
			windowRect.right = rect.right;
			windowRect.bottom = rect.bottom;
		}

		pWnd->ScreenToClient(windowRect);
		windowRect.left = windowRect.left * fWidth; //tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		windowRect.right = windowRect.right * fWidth; //tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		windowRect.top = windowRect.top * fHeigth; //newHeight / oldHeight;
		windowRect.bottom = windowRect.bottom *fHeigth; //newHeight / oldHeight;
		pChildWnd->MoveWindow(windowRect);
		
		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
	}
}

void CsheetWithTab::SetPage( int nPage )
{
	SetActivePage( nPage );

	switch ( nPage )
	{
	case ITEM_MIX_TOOL:	return m_ItemMixPage.Init();
	case MAPS_LIST_TOOL: return m_MapsListPage.Init();
	case TAXI_STATION: return m_TaxiStationPage.Init();
	case AUTO_LEVEL_TOOL: return m_AutoLevelPage.Init();
	}
}

bool CsheetWithTab::NewFile( int nPage )
{
	switch ( nPage )
	{
	case ITEM_MIX_TOOL:	return m_ItemMixPage.NewFile();
	case MAPS_LIST_TOOL: return m_MapsListPage.NewFile();
	case TAXI_STATION: return m_TaxiStationPage.NewFile();
	case AUTO_LEVEL_TOOL: return m_AutoLevelPage.NewFile();
	}

	return false;
}

bool CsheetWithTab::LoadFile( int nPage )
{
	switch ( nPage )
	{
	case ITEM_MIX_TOOL:	return m_ItemMixPage.LoadFile();
	case MAPS_LIST_TOOL: return m_MapsListPage.LoadFile();
	case TAXI_STATION: return m_TaxiStationPage.LoadFile();
	case AUTO_LEVEL_TOOL: return m_AutoLevelPage.LoadFile();
	}
	
	return false;
}

bool CsheetWithTab::LoadFileOld( int nPage )
{
	switch ( nPage )
	{
	case ITEM_MIX_TOOL:	return m_ItemMixPage.LoadFileOld(); 
	case MAPS_LIST_TOOL: return m_MapsListPage.LoadFileOld();
	case TAXI_STATION: return m_TaxiStationPage.LoadFileOld();
	case AUTO_LEVEL_TOOL: return m_AutoLevelPage.LoadFileOld();
	}

	return false;
}

bool CsheetWithTab::SaveFile( int nPage )
{
	switch ( nPage )
	{
	case ITEM_MIX_TOOL:	return m_ItemMixPage.SaveFile();
	case MAPS_LIST_TOOL: return m_MapsListPage.SaveFile();
	case TAXI_STATION: return m_TaxiStationPage.SaveFile();
	case AUTO_LEVEL_TOOL: return m_AutoLevelPage.SaveFile();
	}

	return false;
}

bool CsheetWithTab::IsPageEdit( int nPage )
{
	switch ( nPage )
	{
	case ITEM_MIX_TOOL:	return m_ItemMixPage.IsEdit();
	case MAPS_LIST_TOOL: return m_MapsListPage.IsEdit();
	case TAXI_STATION: return m_TaxiStationPage.IsEdit();
	case AUTO_LEVEL_TOOL: return m_AutoLevelPage.IsEdit();
	}
	
	return false;
}

bool CsheetWithTab::CsvLoad( int nPage )
{
	switch ( nPage )
	{
	case ITEM_MIX_TOOL:	  return m_ItemMixPage.LoadCsv();
	case MAPS_LIST_TOOL:  return m_MapsListPage.LoadCsv();
	case TAXI_STATION:    return m_TaxiStationPage.LoadCsv();
	case AUTO_LEVEL_TOOL: return m_AutoLevelPage.CsvLoadAll();
	}
	
	return false;
}

bool CsheetWithTab::CsvSave( int nPage )
{
	switch ( nPage )
	{
	case ITEM_MIX_TOOL:   return m_ItemMixPage.SaveCsv();
	case MAPS_LIST_TOOL:  return m_MapsListPage.SaveCsv();
	case TAXI_STATION:    return m_TaxiStationPage.SaveCsv();
	case AUTO_LEVEL_TOOL: return m_AutoLevelPage.CsvSaveAll();
	}
	
	return false;
}
