#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "../Enginelib/ServiceProviderDefine.h"

class SUBPATH;

class CPreDownloadManApp : public CWinApp
{
public:
	CPreDownloadManApp();
	virtual ~CPreDownloadManApp();

protected:
	EMSERVICE_PROVIDER m_ServiceProvider;
	SUBPATH* m_pPath;

public:
	inline EMSERVICE_PROVIDER GetServiceProvider() const { return m_ServiceProvider; }
	inline SUBPATH* GetSubPath() { return m_pPath; }

public:
	virtual BOOL InitInstance();

protected:

#ifdef _DEBUG
	CMemoryState	*m_memState;
#endif

	// ±¸Çö

	DECLARE_MESSAGE_MAP()
	virtual BOOL InitApplication();
};

extern CPreDownloadManApp theApp;
