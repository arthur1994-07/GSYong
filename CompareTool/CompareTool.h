// CompareTool.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"		// �� ��ȣ


// CCompareToolApp:
// �� Ŭ������ ������ ���ؼ��� CompareTool.cpp�� �����Ͻʽÿ�.
//

class CCompareToolApp : public CWinApp
{
public:
	CCompareToolApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CCompareToolApp theApp;
