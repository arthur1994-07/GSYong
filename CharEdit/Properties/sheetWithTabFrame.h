#pragma once

#include "FramePage.h"
#include "FrameSetPage.h"
/////////////////////////////////////////////////////////////////////////////
// CsheetWithTab

	

class CsheetWithTabFrame : public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTabFrame)

public:
	CWnd*			m_pWndParent;

// Construction
public:
	CFramePage		m_FramePage;
	CFrameSetPage	m_FrameSetPage;


public:
	void ReSetPages ();
	void FrameSetPageActive ( CString sName );
	void SetFrameEditType( EMFRAMEEDITTYPE emFrameEditType );

	
public:
	CsheetWithTabFrame ( CWnd* pParentWnd = NULL );
	CsheetWithTabFrame ( CRect& rect, CWnd* pParentWnd = NULL );
	virtual ~CsheetWithTabFrame ();

// Attributes
public:
   CRect      m_Rect;

   BOOL       m_bNoTabs;
   int        m_iDistanceFromTop;

// Get/Set
public:
   void NoTabs () { m_bNoTabs = TRUE; }
   void SetDistanceFromTop ( int iDistance ) { m_iDistanceFromTop = iDistance; }

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	public:
	virtual BOOL OnInitDialog ();

// Implementation
public:

// Generated message map functions
protected:
   afx_msg HBRUSH CtlColor ( CDC* pDc, UINT uCtlColor );	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

