// NpcAction.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "./Resource.h"		// �� ��ȣ
#include "../Enginelib/ServiceProviderDefine.h"

class SUBPATH;
// CNpcActionApp:
// �� Ŭ������ ������ ���ؼ��� NpcAction.cpp�� �����Ͻʽÿ�.
//

class CNpcActionApp : public CWinApp
{
public:
	CNpcActionApp();
    virtual ~CNpcActionApp();

protected:
    EMSERVICE_PROVIDER m_ServiceProver;
    SUBPATH* m_pPath;

public:
    EMSERVICE_PROVIDER GetServiceProvider() const { return m_ServiceProver; }
    SUBPATH* GetSubPath() { return m_pPath; }

public:
// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CNpcActionApp theApp;
