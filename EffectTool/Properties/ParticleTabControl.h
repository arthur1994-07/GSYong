#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EffSheetWithTab.h"
#include "../../EngineLib/DxEffect/Single/DxEffectMesh.h"

struct PARTICLESYS_PROPERTY;

// ParticleTabControl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CParticleTabControl window

class CParticleTabControl : public CTabCtrl
{
// Construction
public:
	CParticleTabControl();

	enum { NUMBEROFPAGE = 3, };
	CDialog *m_tabPages[NUMBEROFPAGE];
	int m_tabCurrent;

// Attributes
public:

// Operations
public:
	void Init ( PARTICLESYS_PROPERTY* pProp, CsheetWithTab* pSheetTab );
	void SetRectangle();
	void Apply();
	void SetActive();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParticleTabControl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CParticleTabControl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CParticleTabControl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
