// CharEdit.h : main header file for the CHAREDIT application
//

#if !defined(AFX_CHAREDIT_H__6A40CDDF_2373_4E9A_8B90_507A5B7E9272__INCLUDED_)
#define AFX_CHAREDIT_H__6A40CDDF_2373_4E9A_8B90_507A5B7E9272__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class SUBPATH;

/////////////////////////////////////////////////////////////////////////////
// CCharEditApp:
// See CharEdit.cpp for the implementation of this class
//

class CCharEditApp : public CWinApp
{
public:
	CCharEditApp();
    virtual ~CCharEditApp();

public:
	void  SetActive(BOOL bActive) { m_bIsActive = bActive; };
	void  ShutDown() { m_bShutDown = TRUE; }
	BOOL  IsActive() { return m_bIsActive; }
    void OnColorPickerWnd( bool bOnColorPicker ) { m_bOnColorPicker = bOnColorPicker; }
    SUBPATH* GetSubPath() { return m_pPath; }

protected:
	BOOL  m_bIsActive;
	BOOL  m_bShutDown;
    bool m_bOnColorPicker;
    SUBPATH* m_pPath;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCharEditApp)
	public:
	virtual BOOL InitInstance();
	virtual int Run();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CCharEditApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHAREDIT_H__6A40CDDF_2373_4E9A_8B90_507A5B7E9272__INCLUDED_)
