// MobNpcEdit.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "./Resource.h"		// �� ��ȣ
#include "../EngineLib/ServiceProviderDefine.h"

class SUBPATH;

// CMobNpcEditApp:
// �� Ŭ������ ������ ���ؼ��� MobNpcEdit.cpp�� �����Ͻʽÿ�.
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

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CMobNpcEditApp theApp;
