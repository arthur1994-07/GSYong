// Basic.h : main header file for the BASIC application
//

#if !defined(AFX_BASIC_H__9FF379E7_FAE2_11D1_BFC5_D41F722B624A__INCLUDED_)
#define AFX_BASIC_H__9FF379E7_FAE2_11D1_BFC5_D41F722B624A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/*
#if !defined(MY_PARAM) && !defined(MYE_PARAM) && !defined(PH_PARAM) //&& !defined(_RELEASED)  && !defined( VN_PARAM ) //&& !defined(TH_PARAM) && !defined ( HK_PARAM ) && !defined(TW_PARAM) // Nprotect ����
#define NO_GAMEGUARD
#endif

#ifdef _RELEASED
#define NO_GAMEGUARD
#endif
*/


/*
#if defined( TH_PARAM ) 
#include "../=include_th/NPGameLib_20120326.h"
#else
#include "../=include/NPGameLib_20120326.h"
#endif


extern CNPGameLib* pNpgl;
*/

//#define NO_GAMEGARD // Ŭ���̾�Ʈ���� ������ϱ����ؼ� ���Ӱ��带 �Ͻ� ������Ŵ

#ifndef _NP30
//#include "../=include/NPGameLib_20120326.h"
#include "../=nProtect/NPGameLib_20120326.h"
#pragma comment(lib, "../=nProtect/NPGameLib_20120326.lib") // ������Ʈ-�Ӽ�-��Ŀ-�Է�-�߰����Ӽ����� "$(SolutionDir)=nProtect/NPGameLib_20120326.lib" �����ϰ� �ҽ����� �����ϵ��� �߰�

#else
//#include "../=nProtect/NPGameLib_Ver30_20140731.h"
//#pragma comment(lib, "../=nProtect/NPGameLib_103_MD_DE.lib")
#include "../=nProtect/NPGameLib_20160624.h"
#pragma comment(lib, "../=nProtect/NPGameLib_20160624.lib") // ������Ʈ-�Ӽ�-��Ŀ-�Է�-�߰����Ӽ����� "$(SolutionDir)=nProtect/NPGameLib_20120326.lib" �����ϰ� �ҽ����� �����ϵ��� �߰�
#endif


extern CNPGameLib* pNpgl;

class VtcGuard;
class CBasicWnd;

/////////////////////////////////////////////////////////////////////////////
// CBasicApp:
// See Basic.cpp for the implementation of this class
//
class CBasicApp : public CWinApp
{
	void ErrorMsgByNP( DWORD dwResult );

public:
	CBasicApp();
	virtual ~CBasicApp();

protected:
	CBasicWnd*	m_pWnd;
	BOOL		m_bIsActive;    
	BOOL		m_bRedrawScreen;
	BOOL		m_bShutDown;
    EMSERVICE_PROVIDER m_ServiceProvider;
    SUBPATH* m_pPath;
    VtcGuard* m_pVtcGuard;
	BOOL		m_bLoginProcessLog;

public:
	void SetActive(BOOL bActive);
	void ShutDown() { m_bShutDown = TRUE; }
	void SetAppPath();
    inline EMSERVICE_PROVIDER GetServiceProvider() { return m_ServiceProvider; }
    SUBPATH* GetSubPath() { return m_pPath; }
    void DeleteLogFile();

public:
	CString m_strAppPath;
	TCHAR	m_szAppPath[MAX_PATH];

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBasicApp)
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

	// Implementation

	//{{AFX_MSG(CBasicApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BASIC_H__9FF379E7_FAE2_11D1_BFC5_D41F722B624A__INCLUDED_)
