// RanFieldServer.h : main header file for the RanFieldServer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// RanFieldServerApp:
// See RanFieldServer.cpp for the implementation of this class
//

class RanFieldServerApp : public CWinApp
{
public:
	RanFieldServerApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern RanFieldServerApp theApp;