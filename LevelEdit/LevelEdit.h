// LevelEdit.h : LevelEdit 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "./Resource.h"       // 주 기호
#include "../EngineLib/ServiceProviderDefine.h"

class SUBPATH;

// CLevelEditApp:
// 이 클래스의 구현에 대해서는 LevelEdit.cpp을 참조하십시오.
//
class CLevelEditApp : public CWinApp
{
public:
	void  SetActive(BOOL bActive) { m_bIsActive = bActive; m_bRedrawScreen = bActive; };

protected:
	HACCEL m_hAccel;
	BOOL  m_bIsActive;
	BOOL  m_bRedrawScreen;
	BOOL  m_bShutDown;
    EMSERVICE_PROVIDER m_emServiceProvider;
    SUBPATH* m_pPath;

public:
	CLevelEditApp();
    virtual ~CLevelEditApp();

    EMSERVICE_PROVIDER GetServiceProvider() { return m_emServiceProvider; }
    SUBPATH* GetSubPath() { return m_pPath; }

// 재정의
public:
	virtual BOOL InitInstance();

// 구현
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int Run();
};

extern CLevelEditApp theApp;
