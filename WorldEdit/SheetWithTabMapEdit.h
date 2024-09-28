#ifndef __SHEETWITHTABMAPEDIT_H_
#define __SHEETWITHTABMAPEDIT_H_

#pragma once

#include "MapEditGridPage.h"
#include "MapEditHeightPage.h"
#include "MapEditTilePage.h"
#include "MapEditPiecePage.h"

enum MAPEDITSHEET
{
	GRIDPAGEINDEX	= 0,
	HEIGHTPAGEINDEX	= 1,
	TILEPAGEINDEX	= 2,
	PIECEPAGEINDEX	= 3,
};

extern CString g_strEditSheetMapEdit[10];

class CsheetWithTabMapEdit : public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTabMapEdit)

public:
	CWnd*				m_pWndParent;

public:
	CMapEditGridPage	m_sGridPage;
	CMapEditHeightPage	m_sHeightPage;
	CMapEditTilePage	m_sTilePage;
	CMapEditPiecePage	m_sPiecePage;

public:
	void ActiveGridPage ()
	{
		SetActivePage ( GRIDPAGEINDEX );
	}

	void ActiveHeightPage ()
	{
		SetActivePage ( HEIGHTPAGEINDEX );
	}

	void ActiveTilePage ()
	{
		SetActivePage ( TILEPAGEINDEX );
	}

	void ActivePiecePage ()
	{
		SetActivePage ( PIECEPAGEINDEX );
	}

public:
	void UpdatePage ();

	// Construction
public:
	CsheetWithTabMapEdit(CRect& rect, CWnd* pParentWnd = NULL);
	virtual ~CsheetWithTabMapEdit();

public:
	CRect			m_Rect;

	BOOL			m_bNoTabs;
	int				m_iDistanceFromTop;

// Get/Set
public:
   void NoTabs() { m_bNoTabs = TRUE; }
   void SetDistanceFromTop( int iDistance ) { m_iDistanceFromTop = iDistance; }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CsheetWithTab)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:

// Generated message map functions
protected:
	//{{AFX_MSG(CsheetWithTab)
   afx_msg HBRUSH CtlColor(CDC* pDc, UINT uCtlColor);	
   //}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif	// __SHEETWITHTABMAPEDIT_H_