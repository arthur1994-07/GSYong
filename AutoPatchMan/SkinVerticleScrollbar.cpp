// SkinVerticleScrollbar.cpp : implementation file
//

#include "stdafx.h"
#include "SkinVerticleScrollbar.h"
#include "AutoPatchMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkinVerticleScrollbar

CSkinVerticleScrollbar::CSkinVerticleScrollbar()
{
	bMouseDown = false;
	bMouseDownArrowUp = false;
	bMouseDownArrowDown = false;
	bDragging = false;

	nThumbTop = 12;
	dbThumbInterval = 0.000000;
	pList = NULL;

}

CSkinVerticleScrollbar::~CSkinVerticleScrollbar()
{
}


BEGIN_MESSAGE_MAP(CSkinVerticleScrollbar, CStatic)
	//{{AFX_MSG_MAP(CSkinVerticleScrollbar)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkinVerticleScrollbar message handlers

BOOL CSkinVerticleScrollbar::OnEraseBkgnd(CDC* pDC) 
{
	return CStatic::OnEraseBkgnd(pDC);
}

void CSkinVerticleScrollbar::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
	CRect clientRect;
	GetClientRect(&clientRect);

	int nHeight = clientRect.Height() - 13;
	

	CRect rectUpArrow(0,0,11,19);
	CRect rectDownArrow(0,nHeight,12,nHeight+13);
	CRect rectThumb(0,nThumbTop,12,nThumbTop+13);

	if(rectThumb.PtInRect(point))
	{
		bMouseDown = true;
	}

	if(rectDownArrow.PtInRect(point))
	{
		bMouseDownArrowDown = true;
		SetTimer(2,250,NULL);
	}

	if(rectUpArrow.PtInRect(point))
	{
		bMouseDownArrowUp = true;
		SetTimer(2,250,NULL);
	}
	
	CStatic::OnLButtonDown(nFlags, point);
}

void CSkinVerticleScrollbar::OnLButtonUp(UINT nFlags, CPoint point) 
{
	UpdateThumbPosition();
	KillTimer(1);
	ReleaseCapture();
	
	bool bInChannel = true;

	CRect clientRect;
	GetClientRect(&clientRect);
	int nHeight = clientRect.Height() - 13;
	CRect rectUpArrow(0,0,11,19);
	CRect rectDownArrow(0,nHeight,12,nHeight+13);
	CRect rectThumb(0,nThumbTop,12,nThumbTop+13);


	if(rectUpArrow.PtInRect(point) && bMouseDownArrowUp)
	{
		ScrollUp();	
		bInChannel = false;
	}

	if(rectDownArrow.PtInRect(point) && bMouseDownArrowDown)
	{
		ScrollDown();
		bInChannel = false;
	}

	if(rectThumb.PtInRect(point))
	{
		bInChannel = false;
	}

	if(bInChannel == true)
	{
		if(point.y > nThumbTop)
		{
			PageDown();
		}
		else
		{
			PageUp();
		}
	}

	bMouseDown = false;
	bDragging = false;
	bMouseDownArrowUp = false;
	bMouseDownArrowDown = false;
	
	CStatic::OnLButtonUp(nFlags, point);
}

void CSkinVerticleScrollbar::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect clientRect;
	GetClientRect(&clientRect);

	if(bMouseDown)
	{
		
		int nPreviousThumbTop = nThumbTop;
		nThumbTop = point.y-6; //-13 so mouse is in middle of thumb
		
		double nMax = pList->GetScrollLimit(SB_VERT);
		int nPos = pList->GetScrollPos(SB_VERT);

		double nHeight = clientRect.Height()-39;
		double nVar = nMax;

		if ( nVar != 0.0f ) dbThumbInterval = nHeight/nVar;
		else dbThumbInterval = nHeight;

		//figure out how many times to scroll total from top
		//then minus the current position from it
		
		int nScrollTimes = (int)((nThumbTop-13)/dbThumbInterval)-nPos;
		
		CSize size;
		size.cx = 0;
		size.cy = nScrollTimes*13; //13 is the height of each row at current font
								   //I cant figure out how to grab this value
								   //dynamically

		nPos += nScrollTimes;
		if ( nPos < 0 ) nPos = 0;

		pList->SendMessage(WM_VSCROLL, MAKELONG(SB_THUMBTRACK,nPos),NULL);

		LimitThumbPosition();

		Draw();
		
	}
	CStatic::OnMouseMove(nFlags, point);
}

void CSkinVerticleScrollbar::OnPaint() 
{
	CPaintDC dc(this); 
	
	Draw();
}

void CSkinVerticleScrollbar::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 1)
	{
		if(bMouseDownArrowDown)
		{
			ScrollDown();
		}
		
		if(bMouseDownArrowUp)
		{
			ScrollUp();
		}
	}
	else if(nIDEvent == 2)
	{
		if(bMouseDownArrowDown)
		{
			KillTimer(2);
			SetTimer(1, 50, NULL);
		}
		
		if(bMouseDownArrowUp)
		{
			KillTimer(2);
			SetTimer(1, 50, NULL);
		}
	}
	CStatic::OnTimer(nIDEvent);
}

void CSkinVerticleScrollbar::PageDown()
{
	pList->SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEDOWN,0),NULL);
	UpdateThumbPosition();
}

void CSkinVerticleScrollbar::PageUp()
{
	pList->SendMessage(WM_VSCROLL, MAKELONG(SB_PAGEUP,0),NULL);
	UpdateThumbPosition();
}

void CSkinVerticleScrollbar::ScrollUp()
{
	pList->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEUP,0),NULL);
	UpdateThumbPosition();
}

void CSkinVerticleScrollbar::ScrollDown()
{
	pList->SendMessage(WM_VSCROLL, MAKELONG(SB_LINEDOWN,0),NULL);
	UpdateThumbPosition();
}

void CSkinVerticleScrollbar::UpdateThumbPosition()
{
	CRect clientRect;
	GetClientRect(&clientRect);

	double nPos = pList->GetScrollPos(SB_VERT);
	double nMax = pList->GetScrollLimit(SB_VERT);
	double nHeight = (clientRect.Height()-37);
	double nVar = nMax;

	if ( nVar != 0.0f ) dbThumbInterval = nHeight/nVar;
	else dbThumbInterval = nHeight;


	double nNewdbValue = (dbThumbInterval * nPos);
	int nNewValue = (int)nNewdbValue;


	nThumbTop = 12+nNewValue;

	LimitThumbPosition();

	Draw();
}


void CSkinVerticleScrollbar::Draw()
{

	CClientDC dc(this);
	CRect clientRect;
	GetClientRect(&clientRect);
	CmyMemDC memDC(&dc, &clientRect);
	memDC.FillSolidRect(&clientRect,  RGB(255,0,0));
	CDC bitmapDC;
	bitmapDC.CreateCompatibleDC(&dc);

	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_VERTICLE_SCROLLBAR_UPARROW);
	CBitmap* pOldBitmap = bitmapDC.SelectObject(&bitmap);
	memDC.BitBlt(clientRect.left,clientRect.top,12,13,&bitmapDC,0,0,SRCCOPY);
	bitmapDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	pOldBitmap = NULL;
	
	//draw the background (span)
	bitmap.LoadBitmap(IDB_VERTICLE_SCROLLBAR_SPAN);
	pOldBitmap = bitmapDC.SelectObject(&bitmap);
	int nHeight = clientRect.Height() - 13;

	for(int i=0; i<nHeight; i++)
	{
		memDC.BitBlt(clientRect.left,(clientRect.top+13)+(i),12,1,&bitmapDC,0,0,SRCCOPY);
	}

	bitmapDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	pOldBitmap = NULL;
	
	//draw the down arrow of the scrollbar
	bitmap.LoadBitmap(IDB_VERTICLE_SCROLLBAR_DOWNARROW);
	pOldBitmap = bitmapDC.SelectObject(&bitmap);
	memDC.BitBlt(clientRect.left,nHeight,12,13,&bitmapDC,0,0,SRCCOPY);
	bitmapDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	pOldBitmap = NULL;


	//draw the thumb control
	bitmap.LoadBitmap(IDB_VERTICLE_SCROLLBAR_THUMB);
	pOldBitmap = bitmapDC.SelectObject(&bitmap);
	memDC.BitBlt(clientRect.left,clientRect.top+nThumbTop,12,13,&bitmapDC,0,0,SRCCOPY);
	bitmapDC.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
	pOldBitmap = NULL;


}

void CSkinVerticleScrollbar::LimitThumbPosition()
{
	CRect clientRect;
	GetClientRect(&clientRect);

	if(nThumbTop+13 > (clientRect.Height()-13))
	{
		nThumbTop = clientRect.Height()-25;
	}

	if(nThumbTop < (clientRect.top+12))
	{
		nThumbTop = clientRect.top+12;
	}
}

