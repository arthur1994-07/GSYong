#pragma once

#include "BoardEditDlg.h"

class CsheetWithTabBoardEdit : public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTabBoardEdit)

public:
	CWnd*			m_pWndParent;

	// Construction
public:
	CBoardEditDlg	m_BoardEditPage;

public:
	DxSkinAniControl*	m_pSkinAniContrl;
	DxSkinAniControl*	GetSkinAniContrl ()		{	return m_pSkinAniContrl; }

public:
	void SetPages( DxSkinAniControl* pSkinAniContrl );
	void ReSetPages ();

	// Attributes
public:
	CRect      m_Rect;

	BOOL       m_bNoTabs;
	int        m_iDistanceFromTop;

	// Get/Set
public:
	void NoTabs () { m_bNoTabs = TRUE; }
	void SetDistanceFromTop ( int iDistance ) { m_iDistanceFromTop = iDistance; }
	void SetEditView( const bool bView );

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
	DECLARE_MESSAGE_MAP()

public:
	CsheetWithTabBoardEdit ( CWnd* pParentWnd = NULL );
	CsheetWithTabBoardEdit ( CRect& rect, CWnd* pParentWnd = NULL );
	virtual ~CsheetWithTabBoardEdit(void);
};
