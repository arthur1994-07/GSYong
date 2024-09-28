#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EffSheetWithTab.h"
#include "../../EngineLib/DxEffect/Single/DxEffectParticleSys.h"
#include "../Util/ClrButton.h"
#include "afxwin.h"

struct PARTICLESYS_PROPERTY;

// ParticleParticleTab.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CParticleParticleTab dialog

class CParticleParticleTab : public CDialog
{
// Construction
public:
	CParticleParticleTab(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CParticleParticleTab)
	enum { IDD = IDD_DIALOG_PARTICLE_PARTICLE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CsheetWithTab*			m_pSheetTab;
	PARTICLESYS_PROPERTY*	m_pProperty;

	CClrButton	m_cClrThrow;
	CClrButton	m_cClrFromTo;
	CClrButton	m_cClrCollision;
	CClrButton	m_cClrAttraction;
	CClrButton	m_cClrCenter;
	CClrButton	m_cClrDirection;
	CClrButton	m_cClrHeightNO;
	CClrButton	m_cClrMoveMatrix;
	CClrButton	m_cClrRotateMatrix;
	CClrButton	m_cClrRotateWorld;
	CClrButton	m_cClrRandomPlay;

// Operations
public:
	void Init ( PARTICLESYS_PROPERTY* pProp, CsheetWithTab* pSheetTab );
	void Apply ();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParticleParticleTab)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CParticleParticleTab)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLife();
	afx_msg void OnBnClickedButtonVelocity();
	afx_msg void OnBnClickedButtonWinddir();
	afx_msg void OnBnClickedButtonThrow();
	afx_msg void OnBnClickedButtonFromto();
	afx_msg void OnBnClickedButtonGravity();
	afx_msg void OnBnClickedButtonCollision();
	afx_msg void OnBnClickedButtonAttraction();
	afx_msg void OnBnClickedButtonCenter();
	afx_msg void OnBnClickedButtonDirection();
	afx_msg void OnBnClickedButtonHeightNo();
	afx_msg void OnBnClickedButtonMoveMatrix();
	afx_msg void OnBnClickedButtonRotateMatrix();
	afx_msg void OnBnClickedButtonRotateWorld();
	afx_msg void OnBnClickedButtonRandomPlay();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.