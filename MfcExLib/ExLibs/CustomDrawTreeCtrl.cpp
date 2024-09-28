#include "stdafx.h"
#include "CustomDrawTreeCtrl.h"
#include "GradientUtil.h"
#include "MemBitmap.h"
#include "CustomDrawHelper.h"

IMPLEMENT_DYNCREATE(CCustomDrawTreeCtrl, CTreeCtrl)

BEGIN_MESSAGE_MAP(CCustomDrawTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)	
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

CCustomDrawTreeCtrl::CCustomDrawTreeCtrl() : 
	m_eTreeCtrlStyle(TCST_RIGHTROW)
,	m_hHotTreeItem(0)
,	m_bMouseEventsTracked(false)
{

}

CCustomDrawTreeCtrl::~CCustomDrawTreeCtrl()
{
	
}

bool CCustomDrawTreeCtrl::HasValidFullRowSelectStyle()
{
	DWORD dwStyle = GetStyle();
	return (dwStyle & TVS_FULLROWSELECT) && ((dwStyle & TVS_HASLINES) == 0);
}

BOOL CCustomDrawTreeCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

void CCustomDrawTreeCtrl::OnPaint()
{
	CRect rect;

	if (GetUpdateRect(&rect, FALSE) && rect.IsRectEmpty() == false)
	{
		CPaintDC dc(this);
		CMemBitmap kMemBitmap(&dc, dc.m_ps.rcPaint);
		DefWindowProc(WM_PRINT, reinterpret_cast<WPARAM>(kMemBitmap.GetMemDC().m_hDC), static_cast<LPARAM>(PRF_CLIENT | PRF_CHECKVISIBLE));
	}
}

void CCustomDrawTreeCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVCUSTOMDRAW pNMTVCD = reinterpret_cast<LPNMTVCUSTOMDRAW>(pNMHDR);

	if (pNMTVCD == 0)
    {
		return;
    }

	switch (pNMTVCD->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;

	case CDDS_ITEMPREPAINT:
		{
			// set the background and foreground color of the item 
			// to the background,
			// so you don't see the default drawing of the text
			pNMTVCD->clrTextBk = pNMTVCD->clrText = ::GetSysColor(COLOR_WINDOW);

			if (m_eTreeCtrlStyle == TCST_FULLROW)
			{
   				HTREEITEM hItem = reinterpret_cast<HTREEITEM>(pNMTVCD->nmcd.dwItemSpec);
				CDC* pDC = CDC::FromHandle(pNMTVCD->nmcd.hdc);

				CRect rect = GetFillRect(hItem);
				OnFillBackground(pDC, rect, hItem, pNMTVCD->nmcd.uItemState);
			}

			// we want to get the CDDS_ITEMPOSTPAINT notification
			*pResult = CDRF_NOTIFYPOSTPAINT;
		}
		break;

	case CDDS_ITEMPOSTPAINT:
		{
		    HTREEITEM hItem = reinterpret_cast<HTREEITEM>(pNMTVCD->nmcd.dwItemSpec);
			CDC* pDC = CDC::FromHandle(pNMTVCD->nmcd.hdc);

			// draw the item
			OnDrawTreeItem(pDC, hItem, pNMTVCD->nmcd.uItemState);
			
			*pResult = CDRF_SKIPDEFAULT;
		}
		break;

	default:
		*pResult = CDRF_DODEFAULT;
		break;
	}
}

CRect CCustomDrawTreeCtrl::GetFillRect(HTREEITEM hItem)
{
	CRect rcText;
	GetItemRect(hItem, rcText, TRUE);

	if (m_eTreeCtrlStyle == TCST_TEXT)
	{
		return rcText;
	}

	CRect rcBounds;
	GetItemRect(hItem, &rcBounds, FALSE);

	SCROLLINFO si = { 0 };
	GetScrollInfo(SB_HORZ, &si, SIF_POS | SIF_RANGE);
	
	rcBounds.right = max(rcBounds.right, si.nMax - si.nPos);

	if (m_eTreeCtrlStyle == TCST_FULLROW)
	{
		rcBounds.left = min(rcBounds.left, -si.nPos);
	}
	else
	{
		rcBounds.left = rcText.left;
	}
	
	if (m_eTreeCtrlStyle == TCST_FULLROW)
	{
		return rcBounds;
	}

	rcText.right = rcBounds.right;

	return rcText;
}

void CCustomDrawTreeCtrl::OnDrawTreeItem(CDC* pDC, HTREEITEM hItem, UINT nItemState)
{
	CRect rcItem;
	GetItemRect(hItem, rcItem, TRUE);

	CRect rcFill = GetFillRect(hItem);
	rcItem.right = rcFill.right;

	OnFillBackground(pDC, rcFill, hItem, nItemState);

	//rcItem.left += 5;	// makes some gap to the text.
	OnDrawText(pDC, rcItem, hItem, nItemState, FALSE);
}

void CCustomDrawTreeCtrl::OnFillBackground(CDC* pDC, CRect rect, HTREEITEM hItem, UINT nItemState)
{
	// 선택 영역의 배경을 윈도우 색상과 동일하게 칠해 준다.
	pDC->FillSolidRect(rect, ::GetSysColor(COLOR_WINDOW));

	rect.left -= 4;	// make some gap

	CCustomDrawHelper itemDrawer;
	itemDrawer.m_bWndHasFocus			= ::GetFocus() == GetSafeHwnd();
	itemDrawer.m_bFocusItem				= (nItemState & CDIS_FOCUS) ? true : false;
	itemDrawer.m_bSelected				= (nItemState & CDIS_SELECTED) ? true : false;
	itemDrawer.m_bIsHotItem				= m_hHotTreeItem == hItem;
	itemDrawer.m_bDrawBorderWhenFill	= true;
	itemDrawer.DrawItemBackground(pDC, rect);
}

CSize CCustomDrawTreeCtrl::OnDrawText(CDC* pDC, CRect rect, HTREEITEM hItem, UINT nItemState, bool bCalcOnly)
{
	CString strItem = GetItemText(hItem);
	
	COLORREF clrTxtColor;

	// 선택 또는 디저블 색상 설정
	if (nItemState & CDIS_DISABLED)
	{
		clrTxtColor = ::GetSysColor(COLOR_GRAYTEXT);
	}
	else
	{
		clrTxtColor = ::GetSysColor(COLOR_WINDOWTEXT);
	}
	
	return DrawItemText(pDC, rect, hItem, strItem, clrTxtColor, bCalcOnly);
}

CSize CCustomDrawTreeCtrl::DrawItemText(CDC* pDC, CRect rect, HTREEITEM hItem, LPCTSTR lpcszText, COLORREF clrTxtColor, bool bCalcOnly)
{
	int nOldBkMode			= pDC->SetBkMode(TRANSPARENT);
	COLORREF clrOldTxtColor	= pDC->SetTextColor(clrTxtColor);
	
	DRAWTEXTPARAMS dtp = {sizeof(DRAWTEXTPARAMS)};
	dtp.iTabLength = 4;
	
	UINT nBoldState = GetItemState(hItem, TVIS_BOLD);
	BOOL bBoldText = nBoldState & TVIS_BOLD;
	
	CFont* pOldFont = NULL;
	CFont fnt;

	if (bBoldText)
	{
		CFont* pCurFont = pDC->GetCurrentFont();
		LOGFONT lf = {0};
		pCurFont->GetLogFont(&lf);
		lf.lfWeight = FW_BOLD;	// make it bold
		
		fnt.CreateFontIndirect(&lf);
		pOldFont = pDC->SelectObject(&fnt);
	}
	
	UINT uDTFormat = DT_VCENTER | DT_SINGLELINE | DT_TABSTOP | DT_EXPANDTABS | DT_END_ELLIPSIS;

	if (bCalcOnly)
	{
		uDTFormat |= DT_CALCRECT;
	}
	
	int nTextHeight = DrawTextEx(pDC->GetSafeHdc(), (LPTSTR)lpcszText, _tcslen(lpcszText), rect, uDTFormat, &dtp);	

	CSize sizeText(rect.Width(), nTextHeight);	

	if (bBoldText)
	{
		pDC->SelectObject(pOldFont);
		fnt.DeleteObject();
	}
	
	pDC->SetTextColor(clrOldTxtColor);
	pDC->SetBkMode(nOldBkMode);

	return sizeText;
}

void CCustomDrawTreeCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	CTreeCtrl::OnMouseMove(nFlags, point);

	UINT uHitTestFlags = 0;
	HTREEITEM hItem = HitTest(point, &uHitTestFlags);

	if ( hItem )
	{		
		CRect rcSel = GetFillRect(hItem);

		if (rcSel.PtInRect(point) == FALSE)
		{
			// 포함되지 않았을 때는 이전 아이템을 넣는다.
			// 그러므로 이전 hotitem 을 계속 그린다.
			hItem = 0;
		}
	}

	if (hItem != m_hHotTreeItem)
	{
		if (m_bMouseEventsTracked == false)
		{
			m_bMouseEventsTracked		= true;

			TRACKMOUSEEVENT trackmouseevent;
			trackmouseevent.cbSize		= sizeof(trackmouseevent);
			trackmouseevent.dwFlags		= TME_LEAVE;
			trackmouseevent.hwndTrack	= GetSafeHwnd();
			trackmouseevent.dwHoverTime	= HOVER_DEFAULT;
			//::AFXTrackMouse(&trackmouseevent);
			_TrackMouseEvent(&trackmouseevent);
		}

		// It seems that SetRedraw() will actually produce flickering,
		// so better do invalidate instead.
		if (m_hHotTreeItem != 0)
		{
			InvalidateItem(m_hHotTreeItem, TRUE);
		}

		if (hItem != 0)
		{
			InvalidateItem(hItem, TRUE);
		}

		m_hHotTreeItem = hItem;
		UpdateWindow();
	}
}

LRESULT CCustomDrawTreeCtrl::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	LRESULT result = Default();
	m_bMouseEventsTracked = false;

	if (m_hHotTreeItem)
	{
		InvalidateItem(m_hHotTreeItem, FALSE);
		m_hHotTreeItem = NULL;
	}

	return result;
}

void CCustomDrawTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	UINT uHitTestFlags = 0;
	HTREEITEM hItem = HitTest(point, &uHitTestFlags);

	if (hItem != 0)
	{
		CRect rcSel = GetFillRect(hItem);

		if (rcSel.PtInRect(point) == TRUE)
		{
			Select(hItem, TVGN_CARET);
		}
	}

	CTreeCtrl::OnLButtonUp(nFlags, point);	
}

int CCustomDrawTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	OnInitTreeCtrl();
	return 0;
}

void CCustomDrawTreeCtrl::PreSubclassWindow()
{
	CTreeCtrl::PreSubclassWindow();

	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	if (pThreadState->m_pWndInit == NULL)
	{
		OnInitTreeCtrl();
	}
}

void CCustomDrawTreeCtrl::OnInitTreeCtrl()
{
	if ( HasValidFullRowSelectStyle() )
	{
		m_eTreeCtrlStyle = TCST_FULLROW;
	}
}

void CCustomDrawTreeCtrl::InvalidateItem(HTREEITEM hItem, BOOL bErase)
{
	CRect rcItem = GetFillRect(hItem);
	InvalidateRect(rcItem, bErase);
}

void CCustomDrawTreeCtrl::RedrawSelectedItem(BOOL bErase)
{
    if (GetSafeHwnd() != 0)
    {
        HTREEITEM hSelItem = GetSelectedItem();

        if (hSelItem)
        {
            InvalidateItem(hSelItem, bErase);
        }
    }
}

HTREEITEM CCustomDrawTreeCtrl::FindItemFromData(DWORD dwData)
{
    HTREEITEM hRoot = GetRootItem();
    return RecursiveFindItemFromData(hRoot, dwData);
}

HTREEITEM CCustomDrawTreeCtrl::RecursiveFindItemFromData(HTREEITEM hNode, DWORD dwData)
{
    if (hNode)
    {
        if (GetItemData(hNode) == dwData)
        {
            return hNode;
        }
        else
        {
            HTREEITEM hSibling = RecursiveFindItemFromData(GetNextSiblingItem(hNode), dwData);

            if (hSibling)
            {
                return hSibling;
            }

            HTREEITEM hChild = RecursiveFindItemFromData(GetChildItem(hNode), dwData);

            if (hChild)
            {
                return hChild;
            }
        }
    }

    return 0;
}
