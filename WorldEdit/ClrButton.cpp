// ClrButton.cpp : implementation file
//

#include "stdafx.h"
#include "ClrButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const COLORREF CLR_BTN_WHITE  = RGB(255, 255, 255);
const COLORREF CLR_BTN_BLACK  = RGB(0, 0, 0);
const COLORREF CLR_BTN_DGREY  = RGB(128, 128, 128);
const COLORREF CLR_BTN_GREY   = RGB(192, 192, 192);
const COLORREF CLR_BTN_LLGREY = RGB(223, 223, 223);

const int BUTTON_IN			  = 0x01;
const int BUTTON_OUT		  = 0x02;
const int BUTTON_BLACK_BORDER = 0x04;

/////////////////////////////////////////////////////////////////////////////
// CClrButton

CClrButton::CClrButton()
{
	text_colour					= GetSysColor(COLOR_BTNTEXT);
	background_colour			= GetSysColor(COLOR_BTNFACE); 
	disabled_background_colour	= background_colour;
	light						= GetSysColor(COLOR_3DLIGHT);
	highlight					= GetSysColor(COLOR_BTNHIGHLIGHT);
	shadow						= GetSysColor(COLOR_BTNSHADOW);
	dark_shadow					= GetSysColor(COLOR_3DDKSHADOW);
}

CClrButton::~CClrButton()
{
}

void CClrButton::SetColour(COLORREF new_text_colour, COLORREF new_background_colour)
{
	text_colour					= new_text_colour;
	background_colour			= new_background_colour; 
	disabled_background_colour	= GetSysColor(COLOR_BTNFACE); 

	Invalidate(FALSE);
}

void CClrButton::SetColour(COLORREF new_text_colour, COLORREF new_background_colour, COLORREF new_disabled_background_colour)
{
	text_colour					= new_text_colour;
	background_colour			= new_background_colour; 
	disabled_background_colour	= new_disabled_background_colour; 

	Invalidate(FALSE);
}

void CClrButton::ResetColour()
{
	text_colour					= GetSysColor(COLOR_BTNTEXT);
	background_colour			= GetSysColor(COLOR_BTNFACE); 
	disabled_background_colour	= background_colour;
	light						= GetSysColor(COLOR_3DLIGHT);
	highlight					= GetSysColor(COLOR_BTNHIGHLIGHT);
	shadow						= GetSysColor(COLOR_BTNSHADOW);
	dark_shadow					= GetSysColor(COLOR_3DDKSHADOW);

	Invalidate(FALSE);
}

BEGIN_MESSAGE_MAP(CClrButton, CButton)
	//{{AFX_MSG_MAP(CClrButton)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClrButton message handlers

void CClrButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	CDC		*dc;
	CRect	focus_rect, button_rect, text_rect, offset_text_rect;
	UINT	state;
	
	dc    = CDC::FromHandle(lpDrawItemStruct->hDC);
	state = lpDrawItemStruct->itemState;

	focus_rect.CopyRect(&lpDrawItemStruct->rcItem); 
	button_rect.CopyRect(&lpDrawItemStruct->rcItem); 

	text_rect = button_rect;
	text_rect.OffsetRect(-1, -1); 
	offset_text_rect = text_rect;
	offset_text_rect.OffsetRect(1, 1);

	// Set the focus rectangle to just past the border decoration
	focus_rect.left   += 4;
    focus_rect.right  -= 4;
    focus_rect.top    += 4;
    focus_rect.bottom -= 4;
      
	// Retrieve the button's caption
    const int bufSize = 512;
    TCHAR buffer[bufSize];
    GetWindowText(buffer, bufSize);

	if (state & ODS_DISABLED)
    {
		DrawFilledRect(dc, button_rect, disabled_background_colour);
	}
	else
	{
		DrawFilledRect(dc, button_rect, background_colour);
	}
		
	if (state & ODS_SELECTED)
	{ 
    	DrawFrame(dc, button_rect, BUTTON_IN);
	}
	else
	{
		if ((state & ODS_DEFAULT) || (state & ODS_FOCUS))
		{
			DrawFrame(dc, button_rect, BUTTON_OUT | BUTTON_BLACK_BORDER);			
		}
		else
		{
			DrawFrame(dc, button_rect, BUTTON_OUT);
		}
	}

	if (state & ODS_DISABLED)
	{
		DrawButtonText(dc, offset_text_rect, buffer, CLR_BTN_WHITE);
		DrawButtonText(dc, text_rect,		 buffer, CLR_BTN_DGREY);
    }
	else
	{
		if (state & ODS_SELECTED)
		{
			DrawButtonText(dc, offset_text_rect, buffer, text_colour);
		}
		else
		{
  			DrawButtonText(dc, text_rect, buffer, text_colour);
		}
	}

	if (state & ODS_FOCUS)
	{
		DrawFocusRect(lpDrawItemStruct->hDC, (LPRECT)&focus_rect);
	}
} 

void CClrButton::DrawFrame(CDC *dc, CRect r, int state)
{ 
	COLORREF colour;

	if (state & BUTTON_BLACK_BORDER)
	{
		colour = CLR_BTN_BLACK;

		DrawLine(dc, r.left, r.top, r.right, r.top,    colour); // Across top
		DrawLine(dc, r.left, r.top, r.left,  r.bottom, colour); // Down left

		DrawLine(dc, r.left,      r.bottom - 1, r.right,     r.bottom - 1, colour); // Across bottom
		DrawLine(dc, r.right - 1, r.top,        r.right - 1, r.bottom,     colour); // Down right

		r.InflateRect(-1, -1);
	}
	
	if (state & BUTTON_OUT)
	{
		colour = highlight;

		DrawLine(dc, r.left, r.top, r.right, r.top,    colour); // Across top
		DrawLine(dc, r.left, r.top, r.left,  r.bottom, colour); // Down left

		colour = dark_shadow;
	
		DrawLine(dc, r.left,      r.bottom - 1, r.right,     r.bottom - 1, colour); // Across bottom
		DrawLine(dc, r.right - 1, r.top,        r.right - 1, r.bottom,     colour); // Down right
	
		r.InflateRect(-1, -1);

		colour = light;
	
		DrawLine(dc, r.left, r.top, r.right, r.top,    colour); // Across top
		DrawLine(dc, r.left, r.top, r.left,  r.bottom, colour); // Down left

		colour = shadow;
	
		DrawLine(dc, r.left,      r.bottom - 1, r.right,     r.bottom - 1, colour); // Across bottom
		DrawLine(dc, r.right - 1, r.top,        r.right - 1, r.bottom,     colour); // Down right
	}

	if (state & BUTTON_IN)
	{
		colour = dark_shadow;

		DrawLine(dc, r.left, r.top, r.right, r.top,    colour); // Across top
		DrawLine(dc, r.left, r.top, r.left,  r.bottom, colour); // Down left
		DrawLine(dc, r.left,      r.bottom - 1, r.right,     r.bottom - 1, colour); // Across bottom
		DrawLine(dc, r.right - 1, r.top,        r.right - 1, r.bottom,     colour); // Down right
	
		r.InflateRect(-1, -1);

		colour = shadow;
	
		DrawLine(dc, r.left, r.top, r.right, r.top,    colour); // Across top
		DrawLine(dc, r.left, r.top, r.left,  r.bottom, colour); // Down left
		DrawLine(dc, r.left,      r.bottom - 1, r.right,     r.bottom - 1, colour); // Across bottom
		DrawLine(dc, r.right - 1, r.top,        r.right - 1, r.bottom,     colour); // Down right
	}
}

void CClrButton::DrawFilledRect(CDC *dc, CRect r, COLORREF colour)
{ 
	CBrush B;

	B.CreateSolidBrush(colour);
	dc->FillRect(r, &B);
}

void CClrButton::DrawLine(CDC *dc, long sx, long sy, long ex, long ey, COLORREF colour)
{ 
	CPen new_pen;
	CPen *old_pen;

	new_pen.CreatePen(PS_SOLID, 1, colour);
	old_pen = dc->SelectObject(&new_pen);
	dc->MoveTo(sx, sy);
	dc->LineTo(ex, ey);
	dc->SelectObject(old_pen);
    new_pen.DeleteObject();
}


void CClrButton::DrawButtonText(CDC *dc, CRect r, const char *Buf, COLORREF text_colour)
{
	if( !Buf )	return;

    COLORREF previous_colour;
	previous_colour = dc->SetTextColor(text_colour);
    dc->SetBkMode(TRANSPARENT);
	dc->DrawText(Buf, strlen(Buf), r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	dc->SetTextColor(previous_colour);
}
