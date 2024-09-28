// Tool_VisualMaterial.h : Tool_VisualMaterial 응용 프로그램에 대한 주 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"       // 주 기호입니다.

//#pragma comment( lib, "../=Library/ijl15.lib" )


#pragma warning( disable : 4995 ) // disable unreference formal parameter warnings for /W4 builds
#pragma warning( disable : 4996 ) // disable unreference formal parameter warnings for /W4 builds

class SUBPATH;

// CTool_VisualMaterialApp:
// 이 클래스의 구현에 대해서는 Tool_VisualMaterial.cpp을 참조하십시오.
//

class CTool_VisualMaterialApp : public CWinApp
{
public:
	CTool_VisualMaterialApp();
    virtual ~CTool_VisualMaterialApp();

public:
	void  SetActive(BOOL bActive) { m_bIsActive = bActive; };

protected:
	BOOL  m_bIsActive;
    BOOL  m_bRedrawScreen;
	BOOL  m_bShutDown;
    SUBPATH* m_pPath;

public:
    SUBPATH* GetSubPath() { return m_pPath; }

// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
    virtual int Run();
};

extern CTool_VisualMaterialApp theApp;