// MobNpcEdit.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "./Resource.h"		// 주 기호
#include "../EngineLib/ServiceProviderDefine.h"

class SUBPATH;

// CMobNpcEditApp:
// 이 클래스의 구현에 대해서는 MobNpcEdit.cpp을 참조하십시오.
//

class CMobNpcEditApp : public CWinApp
{
public:
    CMobNpcEditApp();
    virtual ~CMobNpcEditApp();

protected:
    EMSERVICE_PROVIDER m_emServiceProvider;
    SUBPATH* m_pPath;

public:
    EMSERVICE_PROVIDER GetServiceProvider() const { return m_emServiceProvider; }
    SUBPATH* GetSubPath() { return m_pPath; }

public:
	char		m_szAppPath[MAX_PATH];
	CString		m_strAppPath;

// 재정의
	public:
	virtual BOOL InitInstance();

// 구현

	DECLARE_MESSAGE_MAP()
};

extern CMobNpcEditApp theApp;
