// MoneyManager.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "./Resource.h"		// �� ��ȣ


// CMoneyManagerApp:
// �� Ŭ������ ������ ���ؼ��� MoneyManager.cpp�� �����Ͻʽÿ�.
//

class CMoneyManagerApp : public CWinApp
{
public:
	CMoneyManagerApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CMoneyManagerApp theApp;
