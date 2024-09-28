// LevelEdit.h : LevelEdit ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "./Resource.h"       // �� ��ȣ
#include "../EngineLib/ServiceProviderDefine.h"

class SUBPATH;

// CLevelEditApp:
// �� Ŭ������ ������ ���ؼ��� LevelEdit.cpp�� �����Ͻʽÿ�.
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

// ������
public:
	virtual BOOL InitInstance();

// ����
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	virtual int Run();
};

extern CLevelEditApp theApp;
