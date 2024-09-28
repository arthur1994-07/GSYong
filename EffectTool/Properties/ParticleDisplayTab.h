#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../EngineLib/DxEffect/Single/DxEffectParticleSys.h"
#include "../MfcExLib/ExLibs/StColor.h"
#include "../Util/ClrButton.h"
#include "afxwin.h"

struct PARTICLESYS_PROPERTY;

// ParticleDisplayTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CParticleDisplayTab dialog

class CParticleDisplayTab : public CDialog
{
// Construction
public:
	CParticleDisplayTab(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CParticleDisplayTab)
	enum { IDD = IDD_DIALOG_PARTICLE_DISPLAY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	PARTICLESYS_PROPERTY*	m_pProperty;

	CStColor	m_Color;

	CClrButton	m_cClrTexScale;
	CClrButton	m_cClrTexRotate;
	CClrButton	m_cClrSequence;
	CClrButton	m_cClrNormal;
	CClrButton	m_cClrBillALL;
	CClrButton	m_cClrBillUP;
	CClrButton	m_cClrGround;
	CClrButton	m_cClrGroundTex;

// Operations
public:
	void Init ( PARTICLESYS_PROPERTY* pProp );
	void Apply ();

protected:
	void ReSetButton ();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParticleDisplayTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CParticleDisplayTab)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTexScale();
	afx_msg void OnBnClickedButtonTexrotate();
	afx_msg void OnBnClickedButtonSequence();
	afx_msg void OnBnClickedButtonScale();
	afx_msg void OnBnClickedButtonBillall();
	afx_msg void OnBnClickedButtonBillup();
	afx_msg void OnBnClickedButtonGround();
	afx_msg void OnBnClickedButtonAlpha();
	afx_msg void OnBnClickedButtonColorS();
	afx_msg void OnBnClickedButtonColorV();
	afx_msg void OnBnClickedButtoncolorE();
	afx_msg void OnBnClickedCheckMesh();
	afx_msg void OnBnClickedCheckTex();
	afx_msg void OnBnClickedButtonMesh();
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedButtonTexScaleDisable();
	afx_msg void OnBnClickedButtonTexrotateDisable();
	afx_msg void OnBnClickedButtonSequenceDisable();
	CComboBox m_comboPOWER;
	CComboBox m_comboBLEND;
	afx_msg void OnCbnSelchangeComboPower2();
	afx_msg void OnCbnSelchangeComboBlend2();
	afx_msg void OnBnClickedButtonNormal();
	afx_msg void OnBnClickedButtonGroundTex();
	afx_msg void OnBnClickedCheckNeweff();
	afx_msg void OnBnClickedButtonNeweff();
	afx_msg void OnBnClickedRadioNeweffFirst();
	afx_msg void OnBnClickedRadioNeweffEnd();
	afx_msg void OnBnClickedRadioNeweffEndStop();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.