// StCreatePos.cpp : implementation file
//

#include "stdafx.h"
#include "../EffectTool.h"
#include "../MainFrm.h"
#include "../EffectToolDoc.h"
#include "../EffectToolView.h"

#include "StCreatePos.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CStCreatePos

CStCreatePos::CStCreatePos()
{
	MAX_SIZE = 10.0f;
	m_fScale = 8.0f;
	m_dwType = _CT_POS;

	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( FALSE );
}

CStCreatePos::~CStCreatePos()
{
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if( !pFrame )	return;
	CEffectToolView *pView = (CEffectToolView*) pFrame->GetActiveView ();
	if( !pView )	return;
	pView->SetCurClick( TRUE );
}


BEGIN_MESSAGE_MAP(CStCreatePos, CStatic)
	//{{AFX_MSG_MAP(CStCreatePos)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStCreatePos message handlers

CPoint g_Screen;
CPoint CStCreatePos::Projection ( float x, float y, float z, float AngX, float AngY )
{
	x *= m_fScale;
	y *= m_fScale;
	z *= m_fScale;

	CPoint Point;

	Point.x = int ( x * cos ( AngY ) + z * sin ( AngX ) );
	Point.y = int ( y * sin ( AngY ) * sin ( AngX ) + y * cos ( AngX )
		+ z * sin ( AngX ) * cos ( AngY ) );

	Point.x = g_Screen.x/2 + Point.x;
	Point.y = g_Screen.y - ( g_Screen.y/2 + Point.y );

	return Point;
}

void CStCreatePos::UpdateContrl ( float fSize )
{
	MAX_SIZE = fSize;
	m_fScale = 80.f/fSize;
}

void CStCreatePos::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CRect WndRect;
	GetClientRect ( WndRect );

	g_Screen.x = WndRect.Width ();
	g_Screen.y = WndRect.Height ();

	CmyMemDC pDC ( &dc, WndRect );
	pDC->SetBkFill ();

	D3DVECTOR Vt1 = D3DXVECTOR3 ( -MAX_SIZE, 0, -MAX_SIZE );
	D3DVECTOR Vt2 = D3DXVECTOR3 ( +MAX_SIZE, 0, -MAX_SIZE );
	D3DVECTOR Vt3 = D3DXVECTOR3 ( +MAX_SIZE, 0, +MAX_SIZE );
	D3DVECTOR Vt4 = D3DXVECTOR3 ( -MAX_SIZE, 0, +MAX_SIZE );

	D3DVECTOR Vt1a = D3DXVECTOR3 ( -MAX_SIZE, +MAX_SIZE, -MAX_SIZE );
	D3DVECTOR Vt1b = D3DXVECTOR3 ( -MAX_SIZE, -MAX_SIZE, -MAX_SIZE );
	
	D3DVECTOR Vt2a = D3DXVECTOR3 ( +MAX_SIZE, +MAX_SIZE, -MAX_SIZE );
	D3DVECTOR Vt2b = D3DXVECTOR3 ( +MAX_SIZE, -MAX_SIZE, -MAX_SIZE );

	D3DVECTOR Vt3a = D3DXVECTOR3 ( +MAX_SIZE, +MAX_SIZE, +MAX_SIZE );
	D3DVECTOR Vt3b = D3DXVECTOR3 ( +MAX_SIZE, -MAX_SIZE, +MAX_SIZE );

	D3DVECTOR Vt4a = D3DXVECTOR3 ( -MAX_SIZE, +MAX_SIZE, +MAX_SIZE );
	D3DVECTOR Vt4b = D3DXVECTOR3 ( -MAX_SIZE, -MAX_SIZE, +MAX_SIZE );


	float AngX = 25.26f * 3.141592f / 180.0f;
	float AngY = 20.00f * 3.141592f / 180.0f;
	CPoint Point1, Point2, Point3, Point4;
	
	Point1 = Projection ( Vt1.x, Vt1.y, Vt1.z, AngX, AngY );
	Point2 = Projection ( Vt2.x, Vt2.y, Vt2.z, AngX, AngY );
	Point3 = Projection ( Vt3.x, Vt3.y, Vt3.z, AngX, AngY );
	Point4 = Projection ( Vt4.x, Vt4.y, Vt4.z, AngX, AngY );


	CPen Pen ( PS_SOLID, 1, RGB ( 206, 206, 206 ) );
	CPen *pOldPen;
	pOldPen = pDC->SelectObject ( &Pen );

	pDC->MoveTo ( Point1 );
	pDC->LineTo ( Point2 );
	pDC->LineTo ( Point3 );
	pDC->LineTo ( Point4 );
	pDC->LineTo ( Point1 );

	Point1 = Projection ( Vt1a.x, Vt1a.y, Vt1a.z, AngX, AngY );
	Point2 = Projection ( Vt2a.x, Vt2a.y, Vt2a.z, AngX, AngY );
	Point3 = Projection ( Vt3a.x, Vt3a.y, Vt3a.z, AngX, AngY );
	Point4 = Projection ( Vt4a.x, Vt4a.y, Vt4a.z, AngX, AngY );

	pDC->MoveTo ( Point1 );
	pDC->LineTo ( Point2 );
	pDC->LineTo ( Point3 );
	pDC->LineTo ( Point4 );
	pDC->LineTo ( Point1 );


	Point1 = Projection ( Vt1b.x, Vt1b.y, Vt1b.z, AngX, AngY );
	Point2 = Projection ( Vt2b.x, Vt2b.y, Vt2b.z, AngX, AngY );
	Point3 = Projection ( Vt3b.x, Vt3b.y, Vt3b.z, AngX, AngY );
	Point4 = Projection ( Vt4b.x, Vt4b.y, Vt4b.z, AngX, AngY );

	pDC->MoveTo ( Point1 );
	pDC->LineTo ( Point2 );
	pDC->LineTo ( Point3 );
	pDC->LineTo ( Point4 );
	pDC->LineTo ( Point1 );

	pDC->SelectObject ( pOldPen );


	CPen Pen2 ( PS_SOLID, 1, RGB ( 186, 186, 186 ) );
	pOldPen = pDC->SelectObject ( &Pen2 );

	D3DVECTOR vt1 = D3DXVECTOR3 ( -MAX_SIZE, 0, 0 );
	D3DVECTOR vt2 = D3DXVECTOR3 ( +MAX_SIZE, 0, 0 );
	D3DVECTOR vt3 = D3DXVECTOR3 ( 0, 0, -MAX_SIZE );
	D3DVECTOR vt4 = D3DXVECTOR3 ( 0, 0, +MAX_SIZE );

	Point1 = Projection ( vt1.x, vt1.y, vt1.z, AngX, AngY );
	Point2 = Projection ( vt2.x, vt2.y, vt2.z, AngX, AngY );
	Point3 = Projection ( vt3.x, vt3.y, vt3.z, AngX, AngY );
	Point4 = Projection ( vt4.x, vt4.y, vt4.z, AngX, AngY );

	pDC->MoveTo ( Point1 );
	pDC->LineTo ( Point2 );

	pDC->MoveTo ( Point3 );
	pDC->LineTo ( Point4 );


	D3DVECTOR vt1a = D3DXVECTOR3 ( -MAX_SIZE, MAX_SIZE, 0 );
	D3DVECTOR vt2a = D3DXVECTOR3 ( +MAX_SIZE, MAX_SIZE, 0 );
	D3DVECTOR vt3a = D3DXVECTOR3 ( 0, MAX_SIZE, -MAX_SIZE );
	D3DVECTOR vt4a = D3DXVECTOR3 ( 0, MAX_SIZE, +MAX_SIZE );

	Point1 = Projection ( vt1a.x, vt1a.y, vt1a.z, AngX, AngY );
	Point2 = Projection ( vt2a.x, vt2a.y, vt2a.z, AngX, AngY );
	Point3 = Projection ( vt3a.x, vt3a.y, vt3a.z, AngX, AngY );
	Point4 = Projection ( vt4a.x, vt4a.y, vt4a.z, AngX, AngY );

	pDC->MoveTo ( Point1 );
	pDC->LineTo ( Point2 );

	pDC->MoveTo ( Point3 );
	pDC->LineTo ( Point4 );

	D3DVECTOR vt1b = D3DXVECTOR3 ( -MAX_SIZE, -MAX_SIZE, 0 );
	D3DVECTOR vt2b = D3DXVECTOR3 ( +MAX_SIZE, -MAX_SIZE, 0 );
	D3DVECTOR vt3b = D3DXVECTOR3 ( 0, -MAX_SIZE, -MAX_SIZE );
	D3DVECTOR vt4b = D3DXVECTOR3 ( 0, -MAX_SIZE, +MAX_SIZE );

	Point1 = Projection ( vt1b.x, vt1b.y, vt1b.z, AngX, AngY );
	Point2 = Projection ( vt2b.x, vt2b.y, vt2b.z, AngX, AngY );
	Point3 = Projection ( vt3b.x, vt3b.y, vt3b.z, AngX, AngY );
	Point4 = Projection ( vt4b.x, vt4b.y, vt4b.z, AngX, AngY );

	pDC->MoveTo ( Point1 );
	pDC->LineTo ( Point2 );

	pDC->MoveTo ( Point3 );
	pDC->LineTo ( Point4 );

	pDC->SelectObject ( pOldPen );


	Point1 = Projection ( m_vtCreatePos.x, m_vtCreatePos.y, m_vtCreatePos.z, AngX, AngY );
	
	Point2 = Projection ( m_vtCreatePos.x, 0, 0, AngX, AngY );
	Point3 = Projection ( 0, 0, m_vtCreatePos.z, AngX, AngY );
	Point4 = Projection ( m_vtCreatePos.x, 0, m_vtCreatePos.z, AngX, AngY );

	pDC->MoveTo ( Point2 );
	pDC->LineTo ( Point4 );

	pDC->MoveTo ( Point3 );
	pDC->LineTo ( Point4 );

	pDC->MoveTo ( Point4 );
	pDC->LineTo ( Point1 );


	{
		Point2 = Projection ( 0, 0, 0, AngX, AngY );

		pDC->MoveTo ( Point2 );
		pDC->LineTo ( Point1 );
	}


	CRect Rect = CRect(-3,-3,3,3) + Point1;

	CBrush BrushSel ( RGB(200,50,50) );
	pDC->FrameRect ( Rect, &BrushSel );


	CFont font;
	font.CreateFont ( 12,0,0,0,0,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Tahoma" );

	CFont *pOldFont = pDC->SelectObject ( &font );
	COLORREF OldColor = pDC->SetTextColor ( RGB(106,106,106) );
	pDC->SetBkMode ( TRANSPARENT );

	D3DVECTOR Out = m_vtCreatePos;
	CString Str;
	Str.Format ( "( %3.2f,  %3.2f,  %3.2f )", Out.x, Out.y, Out.z );

	pDC->TextOut ( Point1.x, Point1.y, Str );

	pDC->SelectObject ( pOldFont );

	// Do not call CStatic::OnPaint() for painting messages
}
