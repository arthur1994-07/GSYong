// EffectTool.h : main header file for the EFFECTTOOL application
//

#if !defined(AFX_EFFECTTOOL_H__16F62766_CA95_4A65_993D_BEE95634CEB1__INCLUDED_)
#define AFX_EFFECTTOOL_H__16F62766_CA95_4A65_993D_BEE95634CEB1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "../EngineLib/ServiceProviderDefine.h"

class SUBPATH;

/////////////////////////////////////////////////////////////////////////////
// CEffectToolApp:
// See EffectTool.cpp for the implementation of this class
//

class CEffectToolApp : public CWinApp
{
protected:
	BOOL  m_bIsActive;    
	BOOL  m_bRedrawScreen;
	BOOL  m_bShutDown;
    EMSERVICE_PROVIDER m_emServiceProvider;
    SUBPATH* m_pPath;

public:
	void  SetActive(BOOL bActive) { m_bIsActive = bActive; m_bRedrawScreen = bActive; };
	void  ShutDown() { m_bShutDown = TRUE; };
    EMSERVICE_PROVIDER GetServiceProvider() const { return m_emServiceProvider; }
    SUBPATH* GetSubPath() { return m_pPath; }

public:
	CEffectToolApp();
    virtual ~CEffectToolApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEffectToolApp)
	public:
	virtual BOOL InitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CEffectToolApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EFFECTTOOL_H__16F62766_CA95_4A65_993D_BEE95634CEB1__INCLUDED_)
