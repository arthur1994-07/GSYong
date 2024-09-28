// SliderCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "AutoPatchManDlg.h"
#include "SliderCtrlEx.h"
#include ".\sliderctrlex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSliderCtrlEx

IMPLEMENT_DYNAMIC(CSliderCtrlEx, CSliderCtrl)
CSliderCtrlEx::CSliderCtrlEx()
	: m_nBitIDBack ( 0 ) 
	, m_nBitIDDrag ( 0 ) 
{
}

CSliderCtrlEx::~CSliderCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CSliderCtrlEx, CSliderCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_MESSAGE( WM_NOTIFY_TRANSPARENT, OnNotifyTransparent)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CSliderCtrlEx message handlers


void CSliderCtrlEx::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW lpNMCustomDraw = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	CDC* dcPaint = CDC::FromHandle(lpNMCustomDraw->hdc) ;

	CRect rcPaint( lpNMCustomDraw->rc );

	switch( lpNMCustomDraw->dwDrawStage ) 
	{
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;

	case CDDS_ITEMPREPAINT:  // Before an item is drawn. This is where we perform our item-specific custom drawing
		if( lpNMCustomDraw->dwItemSpec == TBCD_THUMB )
		{
			if( OnSliderDrawThumb( dcPaint, rcPaint,lpNMCustomDraw->uItemState) )
				*pResult = CDRF_SKIPDEFAULT;
		} 
		else if( lpNMCustomDraw->dwItemSpec == TBCD_CHANNEL )
		{
			CRect rcClient;
			GetClientRect( &rcClient);

			if( OnSliderDrawChannel( dcPaint,rcClient,lpNMCustomDraw->uItemState))
				*pResult = CDRF_SKIPDEFAULT;
		}
		else if( lpNMCustomDraw->dwItemSpec == TBCD_TICS )
		{
			CRect rcClient;
		    GetClientRect( &rcClient);
			if( OnSliderDrawTics( dcPaint,rcClient, lpNMCustomDraw->uItemState))
				*pResult = CDRF_SKIPDEFAULT;
		}
		else
		{
			_ASSERT( FALSE );
		}

		break;
	} // switc

}



BOOL CSliderCtrlEx::OnSliderDrawThumb( CDC* pDC,  CRect& rect, UINT nState)
{

	CDC memdc;
	memdc.CreateCompatibleDC( pDC);
	memdc.SelectObject(m_bmpDrag);
	BITMAP bmpInfo;
	m_bmpDrag.GetBitmap( &bmpInfo);


	if ( nState == CDIS_SELECTED)
	{
		//DrawDisplayItem( hdc, rect, STATE_DOWN);
		pDC->TransparentBlt( rect.left, rect.top, rect.Width(), rect.Height(), &memdc, 0,0, bmpInfo.bmWidth, bmpInfo.bmHeight, RGB(255,0,255));
	}
	else if ( nState == CDIS_HOT)
	{
		//you may have another hot bitmap to draw the thumb. I doesn't have another picture,so use the same picture.
		pDC->TransparentBlt( rect.left, rect.top, rect.Width(), rect.Height(), &memdc, 0,0, bmpInfo.bmWidth, bmpInfo.bmHeight, RGB(255,0,255));
	}
	else 
	{
		pDC->TransparentBlt( rect.left, rect.top, rect.Width(), rect.Height(), &memdc, 0,0, bmpInfo.bmWidth, bmpInfo.bmHeight, RGB(255,0,255));
	}

	return TRUE;

}


BOOL CSliderCtrlEx::OnSliderDrawChannel( CDC* pDC,  CRect& rect, UINT nState)
{
	if ( m_objTrans.IsValid() )
	{
		m_objTrans.TransparentBk( pDC->GetSafeHdc(),  GetSafeHwnd());
	}

	CDC memdc;
	memdc.CreateCompatibleDC( pDC);
	memdc.SelectObject(m_bmpBg);
	BITMAP bmpInfo;
	m_bmpBg.GetBitmap( &bmpInfo);

    pDC->TransparentBlt( rect.left, rect.top, rect.Width(), rect.Height(), &memdc, 0,0, bmpInfo.bmWidth, bmpInfo.bmHeight, RGB(255,0,255));
	return TRUE;
}

void CSliderCtrlEx::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class

	CSliderCtrl::PreSubclassWindow();
	m_objTrans.Install( GetSafeHwnd());
	m_bmpBg.LoadBitmap( m_nBitIDBack );
	m_bmpDrag.LoadBitmap( m_nBitIDDrag);
}

BOOL CSliderCtrlEx::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
    return TRUE;
	//return CSliderCtrl::OnEraseBkgnd(pDC);
}

LRESULT CSliderCtrlEx::OnNotifyTransparent( WPARAM wParam, LPARAM lParam) 
{
	if ( ::IsWindowEnabled( GetSafeHwnd()))
	{
		::EnableWindow( GetSafeHwnd(),FALSE); 
		::EnableWindow( GetSafeHwnd(),TRUE);
	}
	else
	{
		::EnableWindow( GetSafeHwnd(),TRUE); 
		::EnableWindow( GetSafeHwnd(),FALSE);
	}

	//This operation is for the repaint of slider control, because Invalidate cann't bring the NM_CUSTOMDRAW message.
	//M..., this may not the best method to solve the problem.
	return TRUE;
}

void CSliderCtrlEx::SetBitmapID( int nBitIDBack, int nBitIDDrag )
{
	m_nBitIDBack = nBitIDBack;
	m_nBitIDDrag = nBitIDDrag;
}
