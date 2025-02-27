// UIEdit.h : UIEdit 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error PCH에서 이 파일을 포함하기 전에 'stdafx.h'를 포함하십시오.
#endif

#include "resource.h"       // 주 기호


// CUIEditApp:
// 이 클래스의 구현에 대해서는 UIEdit.cpp을 참조하십시오.
//

class CUIEditApp : public CWinApp
{
public:
	CUIEditApp();

public:
	void  SetActive(BOOL bActive) { m_bIsActive = bActive; };

public:
	BOOL  m_bIsActive;
	BOOL  m_bIsShutDown;

	CString	m_strAppPath;
	TCHAR	m_szAppPath[MAX_PATH];

public:
	void ShutDown() { m_bIsShutDown = TRUE; }


// 재정의
public:
	virtual BOOL InitInstance();
	virtual int Run();

// 구현
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CUIEditApp theApp;
