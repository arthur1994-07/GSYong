// RanSNS.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"		// �� ��ȣ


// CRanSNSApp:
// �� Ŭ������ ������ ���ؼ��� RanSNS.cpp�� �����Ͻʽÿ�.
//

class CRanSNSApp : public CWinApp
{
public:
	CRanSNSApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};

extern CRanSNSApp theApp;
