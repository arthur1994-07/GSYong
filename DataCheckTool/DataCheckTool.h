// DataCheckTool.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"		// �� ��ȣ


// CDataCheckToolApp:
// �� Ŭ������ ������ ���ؼ��� DataCheckTool.cpp�� �����Ͻʽÿ�.
//

class CDataCheckToolApp : public CWinApp
{
public:
	CDataCheckToolApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CDataCheckToolApp theApp;
