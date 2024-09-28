// Tool_VisualMaterial.h : Tool_VisualMaterial ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.

//#pragma comment( lib, "../=Library/ijl15.lib" )


#pragma warning( disable : 4995 ) // disable unreference formal parameter warnings for /W4 builds
#pragma warning( disable : 4996 ) // disable unreference formal parameter warnings for /W4 builds

class SUBPATH;

// CTool_VisualMaterialApp:
// �� Ŭ������ ������ ���ؼ��� Tool_VisualMaterial.cpp�� �����Ͻʽÿ�.
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

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
    virtual int Run();
};

extern CTool_VisualMaterialApp theApp;