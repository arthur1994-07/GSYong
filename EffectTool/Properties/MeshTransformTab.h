#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../EngineLib/DxEffect/Single/DxEffectMesh.h"
#include "../MfcExLib/ExLibs/StColor.h"
#include "../Util/ClrButton.h"
#include "afxwin.h"

struct MESH_PROPERTY;

// MeshTransformTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMeshTransformTab dialog

class CMeshTransformTab : public CDialog
{
// Construction
public:
	CMeshTransformTab(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMeshTransformTab)
	enum { IDD = IDD_DIALOG_EFFMESH_TRANSFORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	MESH_PROPERTY*	m_pProperty;

	CClrButton	m_cClrGround;
	CClrButton	m_cClrDirect;
	CClrButton	m_cClrHeight;
	CClrButton	m_cClrScale;
	CClrButton	m_cClrXYZScale;

	float		m_fRotateAngle;

// Operations
public:
	void Init ( MESH_PROPERTY* pProp );
	void Apply ();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeshTransformTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMeshTransformTab)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRead();
	afx_msg void OnBnClickedButtonRead2();
	afx_msg void OnBnClickedButtonRead3();
	afx_msg void OnBnClickedButtonGround();
	afx_msg void OnBnClickedButtonDirect();
	CComboBox m_ctrlRotateDirection;
	afx_msg void OnBnClickedCheckBlendMesh();
	afx_msg void OnBnClickedCheckBlur();
	afx_msg void OnBnClickedCheckRotate();
	afx_msg void OnBnClickedButtonHeight();
	afx_msg void OnBnClickedButtonScale();
	afx_msg void OnBnClickedButtonScalexyz();
	afx_msg void OnBnClickedButtonHeightDisable();
	afx_msg void OnBnClickedButtonXyzscaleDisable();
	afx_msg void OnBnClickedButtonScaleDisable();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.