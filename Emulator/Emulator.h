// Emulator.h : Emulator 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "../EngineLib/ServiceProviderDefine.h"
#include "resource.h"       // 주 기호

class SUBPATH;

// CEmulatorApp:
// 이 클래스의 구현에 대해서는 Emulator.cpp을 참조하십시오.
//

class CEmulatorApp : public CWinApp
{
public:
	void  SetActive(BOOL bActive) { m_bIsActive = bActive; };

protected:
    EMSERVICE_PROVIDER m_emServiceProvider;
    SUBPATH* m_pPath;

public:
	BOOL  m_bIsActive;
	CString	m_strAppPath;
	char	m_szAppPath[MAX_PATH];

public:
	CEmulatorApp();
    virtual ~CEmulatorApp();

    EMSERVICE_PROVIDER GetServiceProvider() const { return m_emServiceProvider; }
    SUBPATH* GetSubPath() { return m_pPath; }

// 재정의
public:
	virtual BOOL InitInstance();

// 구현
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int Run();
};

extern CEmulatorApp theApp;
