// StProg.cpp : implementation file
//

#include "stdafx.h"
#include "../Resource.h"
#include "../MainFrm.h"

#include "../EngineLib/DxEffect/Single/DxEffSinglePropGMan.h"

#include "StProg.h"

#define MARGIN_X	(25)
#define MARGIN_Y	(25)

#define GRIDSIZEX	(20)
#define GRIDSIZEY	(20)


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CStProg

CStProg::CStProg() :
	m_bLDownSel(FALSE),
	m_ScrollPosX(0),
	m_ScrollPosY(0),
	m_ScaleX(0.005f),
	m_ScaleY(0.5f),
	m_SnapX(1.0f),
	m_SnapY(0.0f),
	m_fTimeLine(0.0f),
	m_bIsx1(FALSE)
{
}

CStProg::~CStProg()
{
}


BEGIN_MESSAGE_MAP(CStProg, CStatic)
	//{{AFX_MSG_MAP(CStProg)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStProg message handlers

BOOL CStProg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CStProg::SetScale ( float Sx, float Sy )
{
	m_ScaleX = Sx;
	m_ScaleY = Sy;
}

CPoint CStProg::ToAxis ( CPoint P )
{
	CRect WndRect;
	GetClientRect ( WndRect );

	P.y = WndRect.Height() - P.y;

	return P;
}

CPoint CStProg::ToAxis ( int x, int y )
{
	CRect WndRect;
	GetClientRect ( WndRect );

	CPoint P;
	P.x = x;
	P.y = WndRect.Height() - y;

	return P;
}

CRect CStProg::ToAxisRect ( CRect R, int x, int y )
{
	CRect WndRect;
	GetClientRect ( WndRect );

	R.top += WndRect.Height() - y;
	R.bottom += WndRect.Height() - y;

	R.left += x;
	R.right += x;

	return R;
}

CPoint CStProg::ToAAxis ( CPoint P )
{
	CRect WndRect;
	GetClientRect ( WndRect );

	P.x = P.x + MARGIN_X - m_ScrollPosX;
	P.y = WndRect.Height() - ( P.y + MARGIN_Y - m_ScrollPosY );

	return P;
}

CPoint CStProg::ToAAxis ( int x, int y )
{
	CRect WndRect;
	GetClientRect ( WndRect );

	CPoint P;
	P.x = x + MARGIN_X - m_ScrollPosX;
	P.y = WndRect.Height() - ( y + MARGIN_Y - m_ScrollPosY );

	return P;
}

CRect CStProg::ToAAxisRect ( CRect R, int x, int y )
{
	CRect WndRect;
	GetClientRect ( WndRect );

	R.top += WndRect.Height() - ( y + MARGIN_Y - m_ScrollPosY );
	R.bottom += WndRect.Height() - ( y + MARGIN_Y - m_ScrollPosY );

	R.left += x - m_ScrollPosX + MARGIN_X +1;
	R.right += x - m_ScrollPosX + MARGIN_X +1;

	return R;
}

CPoint CStProg::ToAAxisScale ( float x, float y )
{
	CRect WndRect;
	GetClientRect ( WndRect );

	CPoint P;
	P.x = int ( x / m_ScaleX + MARGIN_X - m_ScrollPosX );
	P.y = int ( WndRect.Height() - ( y / m_ScaleY + MARGIN_Y - m_ScrollPosY ) );

	return P;
}

CRect CStProg::ToAAxisRectScale ( CRect R, float x, float y )
{
	CRect WndRect;
	GetClientRect ( WndRect );

	R.top += int ( WndRect.Height() - ( y / m_ScaleY + MARGIN_Y - m_ScrollPosY ) );
	R.bottom += int ( WndRect.Height() - ( y / m_ScaleY + MARGIN_Y - m_ScrollPosY ) );

	R.left += int ( x / m_ScaleX - m_ScrollPosX + MARGIN_X +1 );
	R.right += int ( x / m_ScaleX - m_ScrollPosX + MARGIN_X +1 );

	return R;
}

void CStProg::ToAAxisTextOut ( CmyMemDC &pDC, CPoint P, CString Str )
{
	CRect WndRect;
	GetClientRect ( WndRect );

	P.x = P.x + MARGIN_X;
	P.y = WndRect.Height() - ( P.y + MARGIN_Y );

	pDC->TextOut ( P.x, P.y, Str );
}

D3DXVECTOR2 CStProg::FromScreenScale ( D3DXVECTOR2 P )
{
	CRect WndRect;
	GetClientRect ( WndRect );

	P.x = ( ( P.x -  MARGIN_X ) + m_ScrollPosX ) * m_ScaleX;
	P.y = ( ( ( WndRect.Height() - MARGIN_Y ) - P.y ) + m_ScrollPosY ) * m_ScaleY;

	return P;
}

void CStProg::AddTimeLine ( TIMELINE addLine )
{
	m_TimeLines.AddTail ( addLine );
	this->RedrawWindow ();
}

void CStProg::DelAllLines ()
{
	m_TimeLines.RemoveAll ();

	m_ThisLine.IndexID = -1;
}

void CStProg::ActiveLine ( DWORD nIndex )
{
	for ( m_TimeLines.SetHead(); !m_TimeLines.IsEnd(); m_TimeLines.GoNext() )
	{
		TIMELINE Line;
		m_TimeLines.GetCurrent ( Line );

		if ( Line.IndexID == nIndex )
		{
			m_bIsx1 = TRUE;
			m_ThisLine = Line;
			break;
		}

	}

	this->RedrawWindow();
}

void CStProg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CRect WndRect;
	GetClientRect ( WndRect );

	CmyMemDC pDC ( &dc, WndRect );
	pDC->SetBkFill ();


	//	Note : 그리드 그리기.
	//
	CPen Pen ( PS_SOLID, 1, RGB ( 206, 206, 206 ) );

	CPen *pOldPen;
	pOldPen = pDC->SelectObject ( &Pen );

	CFont font;
	font.CreateFont ( 12,0,0,0,0,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Tahoma" );

	pDC->SetBkMode ( TRANSPARENT );

	CFont *pOldFont = pDC->SelectObject ( &font );
	COLORREF OldColor = pDC->SetTextColor ( RGB(106,106,106) );

	int X = m_ScrollPosX;
	int Y = m_ScrollPosY;
	CString Str;

	pDC->MoveTo ( ToAxis ( MARGIN_X, MARGIN_Y-2 ) );
	pDC->LineTo ( ToAxis ( MARGIN_X, WndRect.Height()+1 ) );

	Str.Format ( "%3.1f", float(X*m_ScaleX) );
	CSize TextSize = pDC->GetTextExtent ( Str );
	ToAAxisTextOut ( pDC, CPoint(-TextSize.cx/2,-2), Str );

	int xGridSize = GRIDSIZEX;
	BOOL bDown = FALSE;
	for ( int i = MARGIN_X + xGridSize; i < WndRect.Width(); i += xGridSize, bDown=!bDown )
	{
		pDC->MoveTo ( ToAxis ( i, MARGIN_Y-2 ) );
		pDC->LineTo ( ToAxis ( i, WndRect.Height()+1 ) );

		X += GRIDSIZEX;
		float	SumX = (float)(X*m_ScaleX);
		if ( SumX < 0.01f )			Str.Format ( "%3.3f", float(SumX) );
		else if ( SumX < 0.1f )		Str.Format ( "%3.2f", float(SumX) );
		else						Str.Format ( "%3.1f", float(SumX) );
		TextSize = pDC->GetTextExtent ( Str );
		
		if ( bDown )
			ToAAxisTextOut ( pDC, CPoint(i-MARGIN_X-TextSize.cx/2,-2), Str );
		else
			ToAAxisTextOut ( pDC, CPoint(i-MARGIN_X-TextSize.cx/2,-1-TextSize.cy), Str );
	}

	pDC->MoveTo ( ToAxis ( MARGIN_X-2, MARGIN_Y ) );
	pDC->LineTo ( ToAxis ( WndRect.Width()+1, MARGIN_Y ) );


	Str.Format ( "%3.1f", float(Y*m_ScaleY) );
	TextSize = pDC->GetTextExtent ( Str );
	ToAAxisTextOut ( pDC, CPoint(-1-TextSize.cx,0+TextSize.cy/2), Str );

	int yGridSize = GRIDSIZEY;
	for ( int j = MARGIN_Y + yGridSize; j < WndRect.Height(); j += yGridSize )
	{
		pDC->MoveTo ( ToAxis ( MARGIN_X-2, j ) );
		pDC->LineTo ( ToAxis ( WndRect.Width()+1, j  ) );

		Y += GRIDSIZEY;
		if ( Y*m_ScaleY < 0.1f )	Str.Format ( "%3.2f", float(Y*m_ScaleY) );
		else						Str.Format ( "%3.1f", float(Y*m_ScaleY) );
		TextSize = pDC->GetTextExtent ( Str );
		ToAAxisTextOut ( pDC, CPoint(-1-TextSize.cx,j-MARGIN_Y+TextSize.cy/2), Str );
	}

	pDC->SelectObject ( pOldPen );
	pDC->SetTextColor ( OldColor );

	
	
	//	Note : 노드들 그리기.
	//

	//	Note : 클리핑 영역 설정.
	//
	CRgn Rgn;
	CRect rgnRect ( MARGIN_X, WndRect.top + 1,
		WndRect.right - 1, WndRect.Height() - MARGIN_Y );
	Rgn.CreateRectRgnIndirect ( &rgnRect );
	pDC->SelectClipRgn ( &Rgn );

	DWORD dwCount = 1;
	BOOL bStart = TRUE;
	D3DXVECTOR2 Point1, Point2;
	for ( m_TimeLines.SetHead(); !m_TimeLines.IsEnd(); m_TimeLines.GoNext(), ++dwCount )
	{
		TIMELINE Line;
		m_TimeLines.GetCurrent ( Line );

		Point1 = D3DXVECTOR2 ( Line.x1, (float)dwCount*10 );
		Point2 = D3DXVECTOR2 ( Line.x2, (float)dwCount*10 );

		pDC->MoveTo ( ToAAxisScale ( Point1.x, Point1.y ) );
		pDC->LineTo ( ToAAxisScale ( Point2.x, Point2.y ) ); 


		CBrush Brush ( RGB(50,50,50) );
		CBrush BrushSel ( RGB(200,50,50) );

		if ( m_ThisLine.IndexID == Line.IndexID )
		{
			pDC->FrameRect ( &ToAAxisRectScale ( CRect(-3,-3,3,3), Point1.x, Point1.y ), &BrushSel );
			pDC->FrameRect ( &ToAAxisRectScale ( CRect(-3,-3,3,3), Point2.x, Point2.y ), &BrushSel );

			if ( m_bIsx1 )
				pDC->FrameRect ( &ToAAxisRectScale ( CRect(-5,-5,5,5), Point1.x, Point1.y ), &BrushSel );
			else
				pDC->FrameRect ( &ToAAxisRectScale ( CRect(-5,-5,5,5), Point2.x, Point2.y ), &BrushSel );

			CPoint point1 = ToAAxisScale ( Point1.x, Point1.y );

			CString Str;
			Str.Format ( "[%d]", dwCount );
			pDC->TextOut ( point1.x, point1.y-14, Str );

			Str.Format ( "(%3.2f)", Line.x1 );
			pDC->TextOut ( point1.x, point1.y+2, Str );

			CPoint point2 = ToAAxisScale ( Point2.x, Point2.y );
			Str.Format ( "(%3.2f)", Line.x2 );
			pDC->TextOut ( point2.x, point2.y+2, Str );
		}
		else
		{
			pDC->FrameRect ( &ToAAxisRectScale ( CRect(-3,-3,3,3), Point1.x, Point1.y ), &Brush );
			pDC->FrameRect ( &ToAAxisRectScale ( CRect(-3,-3,3,3), Point2.x, Point2.y ), &Brush );
		}

		bStart = FALSE;
	}

	pDC->SelectObject ( pOldFont );


	{
		int xTime = int ( m_fTimeLine / m_ScaleX + MARGIN_X - m_ScrollPosX );

		CPen PenTime ( PS_SOLID, 1, RGB ( 255, 122, 122 ) );
		pOldPen = pDC->SelectObject ( &PenTime );

		pDC->MoveTo ( xTime, 0 );
		pDC->LineTo ( xTime, WndRect.Height() );

		pDC->SelectObject ( pOldPen );
	}

	// Do not call CStatic::OnPaint() for painting messages
}

void CStProg::OnLButtonDown(UINT nFlags, CPoint mpoint) 
{
	// TODO: Add your message handler code here and/or call default
	m_ThisLine.IndexID = -1;

	D3DXVECTOR2 MouseAPos = FromScreenScale ( D3DXVECTOR2((float)mpoint.x,(float)mpoint.y) );

	DWORD dwCount = 1;
	for ( m_TimeLines.SetHead(); !m_TimeLines.IsEnd(); m_TimeLines.GoNext(), ++dwCount )
	{
		TIMELINE Line;
		m_TimeLines.GetCurrent ( Line );

		
		D3DXVECTOR2 Point1 = D3DXVECTOR2 ( Line.x1, dwCount*10.0f );
		
		float x1 = MouseAPos.x - Point1.x;
		float y1 = MouseAPos.y - Point1.y;

		float Delta1 = (float) sqrt( x1*x1 + y1*y1 );

		D3DXVECTOR2 Point2 = D3DXVECTOR2 ( Line.x2, dwCount*10.0f );
	
		float x2 = MouseAPos.x - Point2.x;
		float y2 = MouseAPos.y - Point2.y;

		float Delta2 = (float) sqrt( x2*x2 + y2*y2 );

		if ( Delta1 < Delta2 )
		{
			if ( Delta1 <= 6*m_ScaleY )
			{
				m_ThisLine = Line;
				m_bIsx1 = TRUE;

				SetCapture();
				m_bLDownSel = TRUE;
			}
		}
		else
		{
			if ( Delta2 <= 6*m_ScaleY )
			{
				m_ThisLine = Line;
				m_bIsx1 = FALSE;

				SetCapture();
				m_bLDownSel = TRUE;
			}
		}
	}

	this->RedrawWindow ();


	CStatic::OnLButtonDown(nFlags, mpoint);
}

void CStProg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if ( m_bLDownSel )	ReleaseCapture();
	m_bLDownSel = FALSE;

	CStatic::OnLButtonUp(nFlags, point);
}

#include "../Enginelib/DxTools/DebugSet.h"
void CStProg::OnMouseMove(UINT nFlags, CPoint mpoint) 
{
	// TODO: Add your message handler code here and/or call default

	if ( m_bLDownSel )
	{
		TIMELINE *pPoint;
		for ( m_TimeLines.SetHead(); !m_TimeLines.IsEnd(); m_TimeLines.GoNext() )
		{
			pPoint = m_TimeLines.GetCurrent ();

			if ( m_ThisLine.IndexID == pPoint->IndexID )
			{
				D3DXVECTOR2 MouseAPos = FromScreenScale ( D3DXVECTOR2((float)mpoint.x,(float)mpoint.y) );

				if ( MouseAPos.x < 0 ) MouseAPos.x = 0;

				if ( m_SnapX != 0 )
					MouseAPos.x = (MouseAPos.x/m_SnapX) * m_SnapX;

				if ( m_bIsx1 )
				{
					pPoint->x2 = MouseAPos.x + ( pPoint->x2 - pPoint->x1 );
					pPoint->x1 = MouseAPos.x;
				}
				else
				{
					if ( pPoint->x1 < MouseAPos.x )
					{
						pPoint->x2 = MouseAPos.x;
					}
					else
					{
						pPoint->x2 = pPoint->x1 + m_SnapX;
					}
				}

				//	Note :  편집된 시간 적용.
				//
				EFF_PROPERTY* pProp = (EFF_PROPERTY*) pPoint->IndexID;
				pProp->m_fGBeginTime = float ( pPoint->x1 );
				pProp->m_fGLifeTime = float ( pPoint->x2 - pPoint->x1 );

				break;
			}
		}

		this->RedrawWindow ();
	}

	CStatic::OnMouseMove(nFlags, mpoint);
}

void CStProg::SetScrollPosX ( DWORD newPos )
{
	m_ScrollPosX = (newPos/GRIDSIZEX)*GRIDSIZEX;
	this->RedrawWindow ();
}

void CStProg::SetScrollPosY ( DWORD newPos )
{
	m_ScrollPosY = (newPos/GRIDSIZEY)*GRIDSIZEY;
	this->RedrawWindow ();
}

void CStProg::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	CMenu Menu;
	
	int pMenuID = 0;

	DWORD SelectionMade;
	VERIFY ( Menu.LoadMenu ( IDR_MENU_LINEBAR ) );

	CMenu* pPopup = Menu.GetSubMenu ( pMenuID );
	ASSERT ( pPopup != NULL );

	Menu.RemoveMenu( IDMENU_COPY, 0 );
	Menu.RemoveMenu( IDMENU_PASTE, 0 );
	Menu.RemoveMenu( IDMENU_DELETE, 0 );	


	POINT pp;
	GetCursorPos (&pp);
	SelectionMade = pPopup->TrackPopupMenu (
		TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD,
		pp.x, pp.y, this );

	pPopup->DestroyMenu ();

	switch ( SelectionMade )
	{
	case IDMENU_SELECT:
		{
			if ( m_ThisLine.IndexID != -1 )
			{
				CMainFrame *pFrame = (CMainFrame *) AfxGetApp()->m_pMainWnd;
				pFrame->m_DlgEditBar.SelectEffProp ( m_ThisLine.IndexID );
			}
		}
		break;
	};
}
