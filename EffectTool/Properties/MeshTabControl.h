#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EffSheetWithTab.h"
#include "../../EngineLib/DxEffect/Single/DxEffectMesh.h"

struct MESH_PROPERTY;

// MeshTabControl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMeshTabControl window

class CMeshTabControl : public CTabCtrl
{
// Construction
public:
	CMeshTabControl();

	enum { NUMBEROFPAGE = 2, };
	CDialog *m_tabPages[NUMBEROFPAGE];
	int m_tabCurrent;

// Attributes
public:

// Operations
public:
	void Init ( MESH_PROPERTY* pProp, CsheetWithTab* pSheetTab );
	void SetRectangle();
	void Apply();
	void SetActive();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeshTabControl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMeshTabControl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMeshTabControl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
