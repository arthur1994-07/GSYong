// STimeLine.cpp : implementation file
//

#include "stdafx.h"

#include "../MfcExLib/ExLibs/memdc.h"
#include "../MainFrm.h"
#include "../charedit.h"

#include "STimeLine.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
// CSTimeLine

CSTimeLine::CSTimeLine()
{
	STime = 0;
	ETime = 30;

	TTime = 0;

	m_bLDownSel = FALSE;
	m_bTick = FALSE;
	m_fxStep = 0;

	m_wMarkCount = 0;
	m_pwMarkArray = 0;
}

CSTimeLine::~CSTimeLine()
{
	SAFE_DELETE(m_pwMarkArray);
}


BEGIN_MESSAGE_MAP(CSTimeLine, CStatic)
	//{{AFX_MSG_MAP(CSTimeLine)
	ON_WM_PAINT()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSTimeLine message handlers

void CSTimeLine::OnPaint() 
{
	CPaintDC dcpaint(this); // device context for painting
	
	CRect WndRect;
	GetClientRect ( WndRect );

	CmyMemDC dc ( &dcpaint, WndRect );
	dc.SetBkFill ( RGB(225,225,225) );

	//	Note : 외각선 그리기
	//
	dc.MoveTo ( WndRect.left, WndRect.top+1 );
	dc.LineTo ( WndRect.left, WndRect.bottom-1 );
	dc.LineTo ( WndRect.right-1, WndRect.bottom-1 );
	dc.LineTo ( WndRect.right-1, WndRect.top );
	dc.LineTo ( WndRect.left-1, WndRect.top );

	//	Note : 그리드 그리기
	//
	CPen Pen1 ( PS_SOLID, 1, RGB ( 138, 138, 138 ) );
	CPen Pen2 ( PS_SOLID, 1, RGB ( 168, 168, 168 ) );

	CFont font;
	font.CreateFont ( 14,0,0,0,0,FALSE,FALSE,0,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH,"Tahoma" );

	CFont *pOldFont = dc.SelectObject ( &font );
	COLORREF OldColor = dc.SetTextColor ( RGB(0,0,0) );
	dc.SetBkMode ( TRANSPARENT );

	float xStep = (float)WndRect.Width() / (float)ETime;
	m_fxStep = xStep;

	for ( DWORD i=1; i<=ETime; ++i )
	{
		if ( i%10 == 0 )
		{
			CPen *pOldPen;
			pOldPen = dc.SelectObject ( &Pen1 );

			dc.MoveTo ( WndRect.left + int(i*xStep), WndRect.top+1 );
			dc.LineTo ( WndRect.left + int(i*xStep), WndRect.bottom-1 );

			dc.SelectObject ( pOldPen );

			//	Note : 눈금 숫자.
			//

			CString Str;
			Str.Format ( "%d", i );

			CSize textSize = dc.GetTextExtent ( Str );

			dc.TextOut ( WndRect.left + int(i*xStep-textSize.cx/2.0f),
				WndRect.bottom-WndRect.Height()/2, Str );
		}
		else
		{
			CPen *pOldPen;
			pOldPen = dc.SelectObject ( &Pen2 );

			dc.MoveTo ( WndRect.left + int(i*xStep), WndRect.top+1 );
			dc.LineTo ( WndRect.left + int(i*xStep), WndRect.bottom-WndRect.Height()/2 );

			dc.SelectObject ( pOldPen );
		}
	}

	dc.SelectObject ( pOldFont );
	
	//	Note : 현제 위치 그리기
	//
	CPen Pen3 ( PS_SOLID, 1, RGB ( 255, 55, 55 ) );
	CPen *pOldPen;
	pOldPen = dc.SelectObject ( &Pen3 );

	int xPos = WndRect.left + int(TTime*xStep);
	dc.MoveTo ( xPos - int(xStep/2), WndRect.top+1 );
	dc.LineTo ( xPos - int(xStep/2), WndRect.bottom-1 );
	dc.LineTo ( xPos + int(xStep/2), WndRect.bottom-1 );
	dc.LineTo ( xPos + int(xStep/2), WndRect.top );
	dc.LineTo ( xPos - int(xStep/2), WndRect.top );


	dc.SelectObject ( pOldPen );

	CPen Pen4 ( PS_SOLID, 1, RGB ( 155, 55, 55 ) );
	pOldPen = dc.SelectObject ( &Pen4 );

	xPos = 0;
	for ( WORD i=0; i<m_wMarkCount; ++i )
	{
		xPos = WndRect.left + int(m_pwMarkArray[i]*xStep);
		dc.MoveTo ( xPos - int(xStep/3), WndRect.top+1 );
		dc.LineTo ( xPos - int(xStep/3), WndRect.bottom-1 );
		dc.LineTo ( xPos + int(xStep/3), WndRect.bottom-1 );
		dc.LineTo ( xPos + int(xStep/3), WndRect.top );
		dc.LineTo ( xPos - int(xStep/3), WndRect.top );
	}

	dc.SelectObject ( pOldPen );

	// Do not call CStatic::OnPaint() for painting messages
}

void CSTimeLine::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	RedrawWindow();
}

void CSTimeLine::SetTimeToTime ( DWORD dwToTime, BOOL bRedraw )
{
	TTime = dwToTime;

	if ( bRedraw )
		RedrawWindow();

	// User 가 TimeLine 의 시간을 변경한다면 그와 연관된 작업을 한다.
	CMainFrame *pFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	pFrame->ChangeTimeOfTimeLine( TTime );
}

void CSTimeLine::SetTimeState ( DWORD dwStart, DWORD dwEnd, BOOL bRedraw )
{
	STime = dwStart;
	ETime = dwEnd;

	if ( bRedraw )
		RedrawWindow();
}

void CSTimeLine::SetFrameMarking ( WORD wMarkCount, WORD* pMarkArray )
{
	m_wMarkCount = wMarkCount;

	SAFE_DELETE(m_pwMarkArray);
	if ( m_wMarkCount > 0 )
	{
		m_pwMarkArray = new WORD[m_wMarkCount];
		memcpy ( m_pwMarkArray, pMarkArray, sizeof(WORD)*m_wMarkCount );
	}

	RedrawWindow();
}

void CSTimeLine::OnLButtonDown ( UINT nFlags, CPoint point )
{
	//	Note : 마우스 켑쳐.
	//
	SetCapture();
	m_bLDownSel = TRUE;

	CStatic::OnLButtonDown(nFlags, point);
}

void CSTimeLine::OnLButtonUp(UINT nFlags, CPoint point)
{
	if ( m_bLDownSel )	ReleaseCapture();
	m_bLDownSel = FALSE;

	CStatic::OnLButtonUp(nFlags, point);
}

void CSTimeLine::OnMouseMove(UINT nFlags, CPoint point)
{
	if ( m_bLDownSel )
	{
		//	Note : m_fxStep, 이 한 프래임의 간격을 나타냄. m_fxStep/2, 를 더해주는 것은
		//		계산시에 반올림을 적용해주기 위한 것이다.
		//
		int xPos = (int) ( (point.x+m_fxStep/2) / m_fxStep );

		m_bTick = TRUE;
		SetTimeToTime ( xPos, TRUE);
	}

	CStatic::OnMouseMove(nFlags, point);
}
