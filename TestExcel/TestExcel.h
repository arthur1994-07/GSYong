// TestExcel.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CTestExcelApp:
// �� Ŭ������ ������ ���ؼ��� TestExcel.cpp�� �����Ͻʽÿ�.
//

class CTestExcelApp : public CWinApp
{
public:
	CTestExcelApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CTestExcelApp theApp;