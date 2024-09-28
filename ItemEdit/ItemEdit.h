// ItemEdit.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// �� ��ȣ
#include "../Enginelib/ServiceProviderDefine.h"

class SUBPATH;

struct SSearchItemList
{
	int MID;
	int SID;
	char szItemName[MAX_PATH];
	char szItemValue[MAX_PATH];

	SSearchItemList()
	{
		ZeroMemory( szItemName, MAX_PATH );
		ZeroMemory( szItemValue, MAX_PATH );
	}

};


// CItemEditApp:
// �� Ŭ������ ������ ���ؼ��� ItemEdit.cpp�� �����Ͻʽÿ�.
//

class CItemEditApp : public CWinApp
{
public:
	CItemEditApp();
    virtual ~CItemEditApp();

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

extern CItemEditApp theApp;
