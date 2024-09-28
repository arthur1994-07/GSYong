#if !defined(AFX_STPROG_H__A3CFB534_CDFC_478B_9E38_264E96C37832__INCLUDED_)
#define AFX_STPROG_H__A3CFB534_CDFC_478B_9E38_264E96C37832__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StProg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CStProg window

#include "../../MfcExLib/ExLibs/MemDc.h"
#include "../../SigmaCore/Container/MList.h"

struct	TIMELINE
{
	DWORD	IndexID;

	float	x1;
	float	x2;

	TIMELINE () :
		IndexID(-1),
		x1(0.0f),
		x2(0.0f)
	{
	}
};


class CStProg : public CStatic
{
protected:  
    sc::CMList<TIMELINE> m_TimeLines;

public:
	CPoint		ToAxis ( CPoint P );
	CPoint		ToAxis ( int x, int y );
	CRect		ToAxisRect ( CRect R, int x, int y );

	CPoint		ToAAxis ( CPoint P );
	CPoint		ToAAxis ( int x, int y );
	CRect		ToAAxisRect ( CRect R, int x, int y );

	CPoint		ToAAxisScale ( float x, float y );
	CRect		ToAAxisRectScale ( CRect R, float x, float y );

	D3DXVECTOR2	FromScreenScale ( D3DXVECTOR2 P );

	void		ToAAxisTextOut ( CmyMemDC &pDC, CPoint P, CString Str );

public:
	BOOL		m_bIsx1;
	TIMELINE	m_ThisLine;
	float		m_fTimeLine;

public:
	void		AddTimeLine ( TIMELINE addLine );
	void		DelAllLines ();
	void		ActiveLine ( DWORD nIndex );

public:
	BOOL		m_bLDownSel;

protected:
	int			m_ScrollPosX;
	int			m_ScrollPosY;

public:
	void		SetScrollPosX ( DWORD newPos );
	void		SetScrollPosY ( DWORD newPos );

protected:
	float		m_ScaleX;
	float		m_ScaleY;

public:
	//	Note : GRIDSIZEX, GRIDSIZEY ¿¡ °öÇÑ°ª.
	//			GRIDSIZEX*0.25 = 5
	//			
	void		SetScale ( float Sx, float Sy );

protected:
	float		m_SnapX;
	float		m_SnapY;

// Construction
public:
	CStProg();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStProg)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CStProg();

	// Generated message map functions
protected:
	//{{AFX_MSG(CStProg)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STPROG_H__A3CFB534_CDFC_478B_9E38_264E96C37832__INCLUDED_)
