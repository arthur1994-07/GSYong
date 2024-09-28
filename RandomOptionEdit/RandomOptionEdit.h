
// RandomOptionEdit.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
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

// �������Դϴ�.
public:
	virtual BOOL InitInstance();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CRandomOptionEditApp theApp;