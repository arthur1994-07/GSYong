#include "stdafx.h"

#include "../resource.h"
#include "./DlgSpecialActivity.h"
#include "SheetWithTab.h"
#include "../../RanLogic/Activity/Activity.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../../enginelib/GUInterface/GameTextControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

IMPLEMENT_DYNAMIC(CsheetWithTab, CPropertySheet)

CsheetWithTab::CsheetWithTab(CRect& rect, LOGFONT logfont, CWnd* pParentWnd)
	 : CPropertySheet(IDS_PROPSHT_CAPTION, pParentWnd),
	 m_bNoTabs(FALSE),
	 m_pWndParent(NULL),
	 m_iDistanceFromTop(0),
	 m_HighPage ( logfont ),
	 m_MediumPage ( logfont ),
	 m_LowPage ( logfont ),
	 m_pFont ( NULL )
{
	m_pWndParent = pParentWnd;
	m_Rect = rect;

	m_bNoTabs = FALSE;
	m_bStacked = FALSE;

    m_LowPage.InitData( ACTIVITY_CLASS_LOW );
    m_MediumPage.InitData( ACTIVITY_CLASS_MEDIUM );
    m_HighPage.InitData( ACTIVITY_CLASS_HIGH );

    m_LowPage.SetSheetTab ( this );	        //0
	m_MediumPage.SetSheetTab( this );		//1
	m_HighPage.SetSheetTab ( this );		//2


	AddPage ( &m_LowPage );	
	AddPage ( &m_MediumPage );	
    AddPage ( &m_HighPage );


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

	// PropertyPage ��Ʈ ���� ����
	ChangeDialogFont( this, m_pFont, CDF_NONE );
   
	if( m_Rect.right )
	{
        // resize the tab control
		hTabWnd = (HWND)::GetDlgItem(m_hWnd, AFX_IDC_TAB_CONTROL);
		ASSERT(hTabWnd != NULL);
		::GetWindowRect(hTabWnd, &rectTabCtrl);  // Tab-control rectangle (screen coordinates)
		ScreenToClient(rectTabCtrl);

		//	Note : ���� �������� ���� ������ �������� ������ �ʿ� �����Ƿ� �����Ѵ�.
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

        CTabCtrl* pTab = GetTabControl();
        if ( pTab )
        {
            for ( int i = 0; i < PAGE_NUM; ++i ) 
            {
                TC_ITEM tcItem;
                char buffer[256] = {0};

                tcItem.pszText = buffer;
                tcItem.cchTextMax = 256;
                tcItem.mask = TCIF_TEXT;
                pTab->GetItem(i, &tcItem);
           
				if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
				{
					tcItem.pszText = (LPSTR)CGameTextMan::GetInstance().GetCommentText("ACTIVITY_CLASS",i).GetString();
				}
				else
				{
					//  Set the new text for the item.
					tcItem.pszText = (LPSTR)COMMENT::ACTIVITY_CLASS[i].c_str();
				}
                //  Set the item in the tab control.
                pTab->SetItem(i, &tcItem);
            }
        }

		return TRUE;
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
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

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
		newClientRect.right = static_cast<LONG>(clientRect.right * fWidth);//tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		newClientRect.bottom = static_cast<LONG>(clientRect.bottom * fHeigth); //newHeight / oldHeight;

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
		windowRect.left = static_cast<LONG>(windowRect.left * fWidth); //tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		windowRect.right = static_cast<LONG>(windowRect.right * fWidth); //tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		windowRect.top = static_cast<LONG>(windowRect.top * fHeigth); //newHeight / oldHeight;
		windowRect.bottom = static_cast<LONG>(windowRect.bottom *fHeigth); //newHeight / oldHeight;
		pChildWnd->MoveWindow(windowRect);
		
		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
	}
}


void CsheetWithTab::InitAllPage()
{
    m_HighPage.UpdateTree();
    m_MediumPage.UpdateTree();
    m_LowPage.UpdateTree();
    SetActivePage ( HIGH_PAGE );
}
