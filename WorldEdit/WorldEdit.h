// WorldEdit.h : main header file for the WORLDEDIT application
//

#if !defined(AFX_WORLDEDIT_H__F80DF342_F103_48A4_9EBF_A6DE466F4FED__INCLUDED_)
#define AFX_WORLDEDIT_H__F80DF342_F103_48A4_9EBF_A6DE466F4FED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
 
#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "../EngineLib/ServiceProviderDefine.h"

class SUBPATH;

static void SetWindowText_gcvt( CWnd* pWnd, double value, int digits )
{
	char szValue[256];
	_gcvt_s( szValue, 256, value, digits );
	pWnd->SetWindowText( szValue );
}

//	szValueW[256] 이여야 한다.
static void GetText_gcvt( CHAR* szOut, double value, int digits )
{
	char szValue[256];
	_gcvt_s( szValue, 256, value, digits );

	StringCchCopy( szOut, 256, szValue );
	//szOut[256-1] = 0;
}

/////////////////////////////////////////////////////////////////////////////
// CWorldEditApp:
// See WorldEdit.cpp for the implementation of this class
//

class CWorldEditApp : public CWinApp
{
public:
	CWorldEditApp();
    virtual ~CWorldEditApp();

public:
	void  SetActive(BOOL bActive) { m_bIsActive = bActive; m_bRedrawScreen = bActive; };
    void  ShutDown() { m_bShutDown = TRUE; }
	BOOL  IsActive() { return m_bIsActive; }

protected:
	BOOL  m_bIsActive;
	BOOL  m_bRedrawScreen;
	BOOL  m_bShutDown;
    EMSERVICE_PROVIDER m_emServiceProvider;
    SUBPATH* m_pPath;

public:
    EMSERVICE_PROVIDER GetServiceProvider() const { return m_emServiceProvider; }
    SUBPATH* GetSubPath() { return m_pPath; }

protected:
    // Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWorldEditApp)
	public:
	virtual BOOL InitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CWorldEditApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WORLDEDIT_H__F80DF342_F103_48A4_9EBF_A6DE466F4FED__INCLUDED_)
