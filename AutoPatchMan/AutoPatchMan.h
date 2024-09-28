// AutoPatchMan.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// �� ��ȣ
#include "../Enginelib/ServiceProviderDefine.h"

class SUBPATH;

// CAutoPatchManApp:
// �� Ŭ������ ������ ���ؼ��� AutoPatchMan.cpp�� �����Ͻʽÿ�.
//

class CAutoPatchManApp : public CWinApp
{
public:
	CAutoPatchManApp();
	virtual ~CAutoPatchManApp();

protected:
    EMSERVICE_PROVIDER m_ServiceProvider;
    SUBPATH* m_pPath;

public:
    inline EMSERVICE_PROVIDER GetServiceProvider() const { return m_ServiceProvider; }
    inline SUBPATH* GetSubPath() { return m_pPath; }

// ������
public:
	virtual BOOL InitInstance();

public:
	class CImpIDispatch* m_pDispOM;

protected:

#ifdef _DEBUG
	CMemoryState	*m_memState;
#endif

// ����

	DECLARE_MESSAGE_MAP()
	virtual BOOL InitApplication();
};

extern CAutoPatchManApp theApp;
