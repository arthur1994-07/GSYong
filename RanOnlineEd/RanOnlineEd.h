// RanOnlineEd.h : main header file for the RanOnlineEd application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "../Enginelib/ServiceProviderDefine.h"

class SUBPATH;

// RanOnlineEdApp:
// See RanOnlineEd.cpp for the implementation of this class
//

class RanOnlineEdApp : public CWinApp
{
public:
	RanOnlineEdApp();
    virtual ~RanOnlineEdApp();

protected:
    EMSERVICE_PROVIDER m_ServiceProvider;
    SUBPATH* m_pPath;

public:
    EMSERVICE_PROVIDER GetServiceProvider() { return m_ServiceProvider; }
    SUBPATH* GetSubPath() { return m_pPath; }

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
    virtual BOOL PreTranslateMessage(MSG* pMsg);
};

extern RanOnlineEdApp theApp;