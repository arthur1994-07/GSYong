// HelpEdit.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"		// �� ��ȣ
#include "../Enginelib/ServiceProviderDefine.h"

class SUBPATH;

// CHelpEditApp:
// �� Ŭ������ ������ ���ؼ��� HelpEdit.cpp�� �����Ͻʽÿ�.
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
// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CHelpEditApp theApp;
