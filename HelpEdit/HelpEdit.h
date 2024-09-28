// HelpEdit.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH에서 이 파일을 포함하기 전에 'stdafx.h'를 포함하십시오.
#endif

#include "resource.h"		// 주 기호
#include "../Enginelib/ServiceProviderDefine.h"

class SUBPATH;

// CHelpEditApp:
// 이 클래스의 구현에 대해서는 HelpEdit.cpp을 참조하십시오.
//

class CHelpEditApp : public CWinApp
{
public:
	CHelpEditApp();
    virtual ~CHelpEditApp();

protected:
    SUBPATH* m_pPath;
    EMSERVICE_PROVIDER m_ServiceProvider;

public:
    SUBPATH* GetSubPath() { return m_pPath; }
    EMSERVICE_PROVIDER GetServiceProvider() { return m_ServiceProvider; }

public:
// 재정의
	public:
	virtual BOOL InitInstance();

// 구현

	DECLARE_MESSAGE_MAP()
};

extern CHelpEditApp theApp;
