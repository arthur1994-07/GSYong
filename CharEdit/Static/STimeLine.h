#if !defined(AFX_STIMELINE_H__EE0DC602_A848_47AD_A812_C6DD279059D8__INCLUDED_)
#define AFX_STIMELINE_H__EE0DC602_A848_47AD_A812_C6DD279059D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// STimeLine.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSTimeLine window

class CSTimeLine : public CStatic
{
protected:
	DWORD	STime;
	DWORD	ETime;

	DWORD	TTime;

	BOOL	m_bLDownSel;
	BOOL	m_bTick;
	float	m_fxStep;

	WORD	m_wMarkCount;
	WORD*	m_pwMarkArray;

public:
	BOOL IsTick ()			{ return m_bTick; }
	DWORD GetTickTime ()	{ return TTime; }
	void ResetTick ()		{ m_bTick = FALSE; }

	void SetFrameMarking ( WORD wMarkCount, WORD* pMarkArray=NULL );

// Construction
public:
	CSTimeLine();

// Attributes
public:
	void SetTimeToTime ( DWORD dwToTime, BOOL bRedraw=TRUE );
	void SetTimeState ( DWORD dwStart, DWORD dwEnd, BOOL bRedraw=TRUE );

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSTimeLine)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSTimeLine();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSTimeLine)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STIMELINE_H__EE0DC602_A848_47AD_A812_C6DD279059D8__INCLUDED_)
