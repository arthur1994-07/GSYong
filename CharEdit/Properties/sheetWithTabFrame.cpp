#include "stdafx.h"
#include "../resource.h"

#include "sheetWithTabFrame.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

IMPLEMENT_DYNAMIC(CsheetWithTabFrame, CPropertySheet)

CsheetWithTabFrame::CsheetWithTabFrame ( CWnd* pWndParent )
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent),
	 m_bNoTabs(FALSE),
	 m_pWndParent(NULL),
	 m_iDistanceFromTop(0)
{
	m_pWndParent = pWndParent;

	m_bNoTabs = TRUE;
	m_bStacked = FALSE;

	m_FramePage.SetSheetTab ( this );
	AddPage(&m_FramePage);

	m_FrameSetPage.SetSheetTab ( this );
	AddPage(&m_FrameSetPage);
}


CsheetWithTabFrame::CsheetWithTabFrame ( CRect& rect, CWnd* pWndParent )
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent),
	 m_bNoTabs(FALSE),
	 m_pWndParent(NULL),
	 m_iDistanceFromTop(0)
{
	m_pWndParent = pWndParent;
	m_Rect = rect;

	m_bNoTabs = TRUE;
	m_bStacked = FALSE;

	m_FramePage.SetSheetTab ( this );
	AddPage(&m_FramePage);
	
	m_FrameSetPage.SetSheetTab ( this );
	AddPage(&m_FrameSetPage);
}

CsheetWithTabFrame::~CsheetWithTabFrame()
{
}

static int module_piButtons[] = { IDOK, IDCANCEL, ID_APPLY_NOW, IDHELP };

BOOL CsheetWithTabFrame::OnInitDialog() 
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



BEGIN_MESSAGE_MAP(CsheetWithTabFrame, CPropertySheet)
	//{{AFX_MSG_MAP(CsheetWithTabFrame)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



HBRUSH CsheetWithTabFrame::CtlColor(CDC* pDc, UINT uCtlColor) 
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

void CsheetWithTabFrame::FrameSetPageActive ( CString sName )
{
	SetActivePage ( 1 );
	m_FrameSetPage.SetFrameName ( sName );
}

void CsheetWithTabFrame::ReSetPages ()
{
	SetActivePage (0);
	m_FramePage.ResetPage ();
}

void CsheetWithTabFrame::SetFrameEditType( EMFRAMEEDITTYPE emFrameEditType )
{
	m_FramePage.SetFrameEditType( emFrameEditType );
}
