// MMPC.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "./Resource.h"		// �� ��ȣ


// CMMPCApp:
// �� Ŭ������ ������ ���ؼ��� MMPC.cpp�� �����Ͻʽÿ�.
//

class CMMPCApp : public CWinApp
{
public:
	CMMPCApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CMMPCApp theApp;
