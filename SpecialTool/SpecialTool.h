// SpecialTool.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"		// �� ��ȣ
#include "../Enginelib/ServiceProviderDefine.h"

class SUBPATH;
// CSpecialToolApp:
// �� Ŭ������ ������ ���ؼ��� SpecialTool.cpp�� �����Ͻʽÿ�.
//

class CSpecialToolApp : public CWinApp
{
public:
	CSpecialToolApp();
    virtual ~CSpecialToolApp();

protected:	
    EMSERVICE_PROVIDER m_emServiceProvider;
    SUBPATH* m_pPath;

public:
    EMSERVICE_PROVIDER GetServiceProvider() const { return m_emServiceProvider; }
    SUBPATH* GetSubPath() { return m_pPath; }

public:
// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CSpecialToolApp theApp;