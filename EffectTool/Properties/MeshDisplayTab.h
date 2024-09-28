//#if _MSC_VER > 1000
#pragma once
//#endif // _MSC_VER > 1000

#include "EffSheetWithTab.h"
#include "afxwin.h"
#include "../../EngineLib/DxEffect/Single/DxEffectMesh.h"
#include "../MfcExLib/ExLibs/StColor.h"
#include "../Util/ClrButton.h"

struct MESH_PROPERTY;

// MeshDisplayTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMeshDisplayTab dialog

class CMeshDisplayTab : public CDialog
{
// Construction
public:
	CMeshDisplayTab(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMeshDisplayTab)
	enum { IDD = IDD_DIALOG_EFFMESH_DISPLAY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CsheetWithTab*	m_pSheetTab;
	MESH_PROPERTY*	m_pProperty;

	CStColor	m_Color;

	CClrButton	m_cClrCullMode;
	CClrButton	m_cClrNormal;
	CClrButton	m_cClrAlpha;
	CClrButton	m_cClrTexMove;
	CClrButton	m_cClrSequence;
	CClrButton	m_cClrGroundTex;

// Operations
public:
	void Init ( MESH_PROPERTY* pProp, CsheetWithTab* pSheetTab );
	void Apply ();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMeshDisplayTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMeshDisplayTab)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboPOWER;
	CComboBox m_comboBLEND;
	afx_msg void OnBnClickedButtonAlpha();
	afx_msg void OnBnClickedButtonColorS();
	afx_msg void OnBnClickedButtonColorE();
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedButtonNormal();
	afx_msg void OnBnClickedButtoemCullMode();
	afx_msg void OnBnClickedButtonTexmove();
	afx_msg void OnBnClickedButtonSequence2();
	afx_msg void OnCbnSelchangeComboPower();
	afx_msg void OnCbnSelchangeComboBlend();
	afx_msg void OnBnClickedCheckTexture();
	afx_msg void OnBnClickedButtonTexmoveDisable();
	afx_msg void OnBnClickedButtonSequenceDisable();
	afx_msg void OnBnClickedButtonGroundTex();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.