
// RandomOptionEdit.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

class SUBPATH;

#include "./resource.h"
#include "../Enginelib/ServiceProviderDefine.h"

class CRandomOptionEditApp : public CWinApp
{
public:
	CRandomOptionEditApp();
	virtual ~CRandomOptionEditApp();

protected:	
	EMSERVICE_PROVIDER m_emServiceProvider;
	SUBPATH* m_pPath;	

public:
	EMSERVICE_PROVIDER GetServiceProvider() const { return m_emServiceProvider; }
	SUBPATH* GetSubPath() { return m_pPath; }

// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// 구현입니다.

	DECLARE_MESSAGE_MAP()
};

extern CRandomOptionEditApp theApp;