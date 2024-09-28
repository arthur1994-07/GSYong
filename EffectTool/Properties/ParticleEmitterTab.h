//#if _MSC_VER > 1000
#pragma once
//#endif // _MSC_VER > 1000

#include "EffSheetWithTab.h"
#include "afxwin.h"
#include "../../EngineLib/DxEffect/Single/DxEffectParticleSys.h"
#include "../Util/ClrButton.h"

struct PARTICLESYS_PROPERTY;

// ParticleEmitterTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CParticleEmitterTab dialog

class CParticleEmitterTab : public CDialog
{
// Construction
public:
	CParticleEmitterTab(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CParticleEmitterTab)
	enum { IDD = IDD_DIALOG_PARTICLE_EMITTER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CsheetWithTab*			m_pSheetTab;
	PARTICLESYS_PROPERTY*	m_pProperty;

	CClrButton	m_cClrRange;
	CClrButton	m_cClrRangeHigh;
	CClrButton	m_cClrRotate;

	//	Delete ´ë»ó.
	CClrButton	m_cClrDirect;
	CClrButton	m_cClrGravityS;
	CClrButton	m_cClrGravityE;
	CClrButton	m_cClrCollision;
	CClrButton	m_cClrGotoCenter;

// Operations
public:
	void Init ( PARTICLESYS_PROPERTY* pProp, CsheetWithTab* pSheetTab );
	void Apply ();
	void SetActive();

protected:
	void ReSetButton ();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParticleEmitterTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CParticleEmitterTab)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonRange();
	afx_msg void OnBnClickedButtonRangeHigh();
	afx_msg void OnBnClickedButtonInitDirect();
	afx_msg void OnBnClickedButtonRotate();
	afx_msg void OnBnClickedButtonGravityMain();
	afx_msg void OnBnClickedButtonGravityMainE();
	afx_msg void OnBnClickedButtonGotocenter();
	afx_msg void OnBnClickedButtonRangeDisable();
	afx_msg void OnBnClickedButtonRangeHighDisable();
	afx_msg void OnBnClickedButtonRotateDisable();
	afx_msg void OnBnClickedButtonDirectDisable();
	afx_msg void OnBnClickedButtonGravitySDisable();
	afx_msg void OnBnClickedButtonGravityEDisable();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.