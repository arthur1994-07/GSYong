// GMCharEdit.h : main header file for the GMCharEdit application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "../Enginelib/ServiceProviderDefine.h"

class SUBPATH;

// CGMCharEditApp:
// See GMCharEdit.cpp for the implementation of this class
//
class CGMCharEditApp : public CWinApp
{
public:
	CGMCharEditApp();
    virtual ~CGMCharEditApp();

protected:
    SUBPATH* m_pPath;
    EMSERVICE_PROVIDER m_ServiceProvier;

public:
    SUBPATH* GetSubPath() { return m_pPath; }
    EMSERVICE_PROVIDER GetServiceProvider() const { return m_ServiceProvier; }

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CGMCharEditApp theApp;