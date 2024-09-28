#include "stdafx.h"
#include "../resource.h"


#include ".\sheetwithtabts.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

IMPLEMENT_DYNAMIC(CsheetWithTabTS, CPropertySheet)
CsheetWithTabTS::CsheetWithTabTS ( CRect& rect, CWnd* pWndParent )
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent),
	 m_bNoTabs(FALSE),
	 m_pWndParent(NULL),
	 m_iDistanceFromTop(0)
{
	m_pWndParent = pWndParent;
	m_Rect = rect;

	m_bNoTabs = TRUE;
	m_bStacked = FALSE;

	m_EffTS_NormalMap.SetSheetTab ( this );		//	0
	AddPage(&m_EffTS_NormalMap);

	m_EffTS_Overlay.SetSheetTab ( this );		//	1
	AddPage(&m_EffTS_Overlay);

	m_EffTS_Reflection.SetSheetTab ( this );	//	2
	AddPage(&m_EffTS_Reflection);

	m_EffTS_TexUpDown.SetSheetTab ( this );		//	3
	AddPage(&m_EffTS_TexUpDown);
}

CsheetWithTabTS::~CsheetWithTabTS()
{
}

static int module_piButtons[] = { IDOK, IDCANCEL, ID_APPLY_NOW, IDHELP };


BOOL CsheetWithTabTS::OnInitDialog() 
{
	HWND hTabWnd;
	CRect rectTabCtrl;

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
		HWND hWnd = NULL;
		for (int i = 0; i < 4; ++i)
		{
			hWnd = ::GetDlgItem(m_hWnd, module_piButtons[i]);
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



BEGIN_MESSAGE_MAP(CsheetWithTabTS, CPropertySheet)
	//{{AFX_MSG_MAP(CsheetWithTabTS)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



HBRUSH CsheetWithTabTS::CtlColor(CDC* pDc, UINT uCtlColor) 
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

void CsheetWithTabTS::TSSetPageActive( int nPage, EFFCHAR_PROPERTY_TOTALSHADER Property )
{
	SetActivePage ( nPage );
	switch (nPage)
	{
	case 0:
		m_EffTS_NormalMap.SetProperty( Property.m_ShaderNormalMap );
		break;
	case 1:
		m_EffTS_Overlay.SetProperty( Property.m_ShaderOverlay );
		break;
	case 2:
		m_EffTS_Reflection.SetProperty( Property.m_ShaderReflection );
		break;
	case 3:
		m_EffTS_TexUpDown.SetProperty( Property.m_ShaderTexUpDown );
		break;
	}
}

void CsheetWithTabTS::ReSetPages ( EFFCHAR_PROPERTY_TOTALSHADER Property )
{
	TSSetPageActive (0, Property);
}

