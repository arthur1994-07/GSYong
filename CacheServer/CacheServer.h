// CacheServer.h : main header file for the CacheServer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "../EngineLib/ServiceProviderDefine.h"

class SUBPATH;

// CacheServerApp:
// See CacheServer.cpp for the implementation of this class
//

class CacheServerApp : public CWinApp
{
public:
	CacheServerApp();
	virtual ~CacheServerApp();

protected:
    EMSERVICE_PROVIDER m_ServiceProvider;
    SUBPATH* m_pPath;

public:
    inline EMSERVICE_PROVIDER GetServiceProvider() { return m_ServiceProvider; }
    inline SUBPATH* GetSubPath() { return m_pPath; }

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CacheServerApp theApp;