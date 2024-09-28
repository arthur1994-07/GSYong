#pragma once

#include "BoneEdit.h"

class CsheetWithTabBone : public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTabBone)

public:
	CWnd*			m_pWndParent;

public:
	DxSkinAniControl*	m_pSkinAniContrl;
	DxSkinAniControl*	GetSkinAniContrl ()		{	return m_pSkinAniContrl; }
	void ApplyAniContrl ();

	// Construction
public:
	CBoneEdit		m_BonePage;

public:
	void SetPages( DxSkinAniControl* pSkinAniContrl );
	void ReSetPages ();
	void FrameSetPageActive ( CString sName );

public:
	CsheetWithTabBone ( CWnd* pParentWnd = NULL );
	CsheetWithTabBone ( CRect& rect, CWnd* pParentWnd = NULL );
	virtual ~CsheetWithTabBone ();

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
};

/////////////////////////////////////////////////////////////////////////////