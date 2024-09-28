#pragma once
#include "../EngineLib/DxEffect/Char/DxEffCharTotalShader.h"
#include "EffTS_NormalMap.h"
#include "EffTS_TexUpDown.h"
#include "EffTS_Reflection.h"
#include "EffTS_Overlay.h"

class CsheetWithTabTS :	public CPropertySheet
{
	DECLARE_DYNAMIC(CsheetWithTabTS)

public:
	CWnd*				m_pWndParent;

public:
	CsheetWithTabTS ( CRect& rect, CWnd* pParentWnd = NULL );
	virtual ~CsheetWithTabTS ();

public:
	void ReSetPages ( EFFCHAR_PROPERTY_TOTALSHADER Property );
	void TSSetPageActive ( int nPage, EFFCHAR_PROPERTY_TOTALSHADER Property );

	// Attributes
public:
	CRect      m_Rect;

	BOOL       m_bNoTabs;
	int        m_iDistanceFromTop;

	// 1: Normal Map
	// 2: Overlay
	// 3: Reflection
	// 4: Texture Up Down

	CEffTS_NormalMap  m_EffTS_NormalMap;
	CEffTS_Overlay    m_EffTS_Overlay;
	CEffTS_Reflection m_EffTS_Reflection;
	CEffTS_TexUpDown  m_EffTS_TexUpDown;

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
