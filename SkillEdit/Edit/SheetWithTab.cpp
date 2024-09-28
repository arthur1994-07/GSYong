#include "stdafx.h"

#include "../Resource.h"

#include "./SheetWithTab.h"
#include "../SkillEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString g_strEditSheet[30] =
{
	"SKILLTREEPAGE",	//	0
	"SKILLDATAPAGE",	//	1
	"SKILLLEVELDATAPAGE",
};

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

IMPLEMENT_DYNAMIC(CsheetWithTab, CPropertySheet)

CsheetWithTab::CsheetWithTab(CRect& rect, LOGFONT logfont, CWnd* pWndParent)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent),
	 m_bNoTabs(FALSE),
	 m_pWndParent(NULL),
	 m_iDistanceFromTop(0),
	 m_pFont ( NULL ),
	 m_SkillTreePage ( logfont ),
	 m_SkillDataPage ( logfont ),
	 m_SkillLevelDataPage ( logfont )

{
	m_pWndParent = pWndParent;
	m_Rect = rect;

	m_bNoTabs = TRUE;
	m_bStacked = FALSE;

	m_SkillTreePage.SetSheetTab ( this );	//	0
	AddPage ( &m_SkillTreePage );	
	
	m_SkillDataPage.SetSheetTab ( this );	//	1
	AddPage ( &m_SkillDataPage );

	m_SkillLevelDataPage.SetSheetTab ( this );	//	2
	AddPage ( &m_SkillLevelDataPage );

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

void CsheetWithTab::ActiveSkillTreePage ()
{
	CSkillEditDlg* pDlg = (CSkillEditDlg*)m_pWndParent;
	pDlg->EnableMenu ( ID_MENUITEM_NEW,  TRUE );
	pDlg->EnableMenu ( ID_MENUITEM_SAVE, TRUE );
	pDlg->EnableMenu ( ID_MENUITEM_LOAD, TRUE );

	pDlg->EnableToobar( ID_MENUITEM_NEW,  TRUE );
	pDlg->EnableToobar( ID_MENUITEM_SAVE, TRUE );
	pDlg->EnableToobar( ID_MENUITEM_LOAD, TRUE );

	pDlg->SetReadMode();

	m_SkillTreePage.UpdateName ();
	SetActivePage ( SKILLTREEPAGE );
}

void CsheetWithTab::ActiveSkillDataPage ( int CurPage, DWORD GUID )
{
	CSkillEditDlg* pDlg = (CSkillEditDlg*)m_pWndParent;
	pDlg->EnableMenu ( ID_MENUITEM_NEW,  FALSE );
	pDlg->EnableMenu ( ID_MENUITEM_LOAD, FALSE );

	pDlg->EnableToobar( ID_MENUITEM_NEW,  FALSE );	
	pDlg->EnableToobar( ID_MENUITEM_LOAD, FALSE );

	pDlg->SetReadMode();

	m_SkillDataPage.SetCallPage ( CurPage );
	m_SkillDataPage.SetItem ( GUID );		
	SetActivePage ( SKILLDATAPAGE );
}

void CsheetWithTab::ActiveSkillLevelDataPage ( int CurPage, DWORD GUID )
{
	CSkillEditDlg* pDlg = (CSkillEditDlg*)m_pWndParent;
	pDlg->EnableMenu ( ID_MENUITEM_NEW,  FALSE );
	pDlg->EnableMenu ( ID_MENUITEM_LOAD, FALSE );

	pDlg->EnableToobar( ID_MENUITEM_NEW,  FALSE );	
	pDlg->EnableToobar( ID_MENUITEM_LOAD, FALSE );

	m_SkillLevelDataPage.SetCallPage ( CurPage );
	m_SkillLevelDataPage.SetItem ( GUID );		
	SetActivePage ( SKILLLEVELDATAPAGE );
}

void CsheetWithTab::UpdateTreePage()
{
	m_SkillTreePage.UpdateTreeItem();
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
	
		newClientRect.left = newClientRect.top = static_cast<long>( 0 );
		newClientRect.right = static_cast<long>( clientRect.right * fWidth );//tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		newClientRect.bottom = static_cast<long>( clientRect.bottom * fHeigth ); //newHeight / oldHeight;

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
		windowRect.left = static_cast<long>( windowRect.left * fWidth ); //tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		windowRect.right = static_cast<long>( windowRect.right * fWidth ); //tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		windowRect.top = static_cast<long>( windowRect.top * fHeigth ); //newHeight / oldHeight;
		windowRect.bottom = static_cast<long>( windowRect.bottom *fHeigth ); //newHeight / oldHeight;
		pChildWnd->MoveWindow(windowRect);
		
		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
	}
}

void CsheetWithTab::ExportExcelDetail( boost::function<void (int)> PercentCallbackFunc )
{
	std::string strSavedFileName;
	if ( m_SkillTreePage.ExportExcelDetail_CheckItem(
		strSavedFileName,
		PercentCallbackFunc ) )
	{
		std::string Msg(
			sc::string::format(
			"%1% 이 저장되었습니다.",
			strSavedFileName ) );
		AfxMessageBox( Msg.c_str(), MB_OK );
	}
}