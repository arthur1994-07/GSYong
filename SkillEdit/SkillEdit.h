// SkillEdit.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// 주 기호
#include "../Enginelib/ServiceProviderDefine.h"

class SUBPATH;

// CSkillEditApp:
// 이 클래스의 구현에 대해서는 SkillEdit.cpp을 참조하십시오.
//

class CSkillEditApp : public CWinApp
{
public:
	CSkillEditApp();
    virtual ~CSkillEditApp();

protected:
    EMSERVICE_PROVIDER m_emServiceProvider;
    SUBPATH* m_pPath;

public:
    EMSERVICE_PROVIDER GetServiceProvider() const { return m_emServiceProvider; }
    SUBPATH* GetSubPath() { return m_pPath; }

public:

// 재정의
	public:
	virtual BOOL InitInstance();

// 구현

	DECLARE_MESSAGE_MAP()
};

extern CSkillEditApp theApp;
