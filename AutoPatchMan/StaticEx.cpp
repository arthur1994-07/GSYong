// StaticEx.cpp : implementation file
//

#include "stdafx.h"
#include "AutoPatchManDlg.h"
#include "StaticEx.h"
#include ".\staticex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CStaticEx

IMPLEMENT_DYNAMIC(CStaticEx, CStatic)
CStaticEx::CStaticEx()
	: m_Textcolor( RGB(192,192,192) )
{
}

CStaticEx::~CStaticEx()
{
}


BEGIN_MESSAGE_MAP(CStaticEx, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CStaticEx message handlers


void CStaticEx::PreSubclassWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	CStatic::PreSubclassWindow();
	m_objHelper.Install( GetSafeHwnd());
}

void CStaticEx::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
	m_objHelper.TransparentBk( dc.GetSafeHdc(), GetSafeHwnd());
	CString strText;
	GetWindowText( strText);

	CRect rtText;
	GetClientRect( &rtText);
	dc.SetBkMode( TRANSPARENT);

	dc.SetTextColor( m_Textcolor );
	
	dc.SelectObject( GetFont());
	dc.DrawText( strText, &rtText, DT_LEFT|DT_SINGLELINE);
}


BOOL CStaticEx::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
    return TRUE;
	//return CStatic::OnEraseBkgnd(pDC);
}

void CStaticEx::SetFontSize ( int nSize, bool bBold )
{
	CFont* pOldFont = GetFont();
	LOGFONT pLogFont;
	pOldFont->GetLogFont( &pLogFont );
	
	pLogFont.lfHeight = nSize;
	if ( bBold ) pLogFont.lfWeight = FW_BOLD;

	CFont pNewFont;
	pNewFont.CreateFontIndirect( &pLogFont );
	SetFont( &pNewFont );
}
