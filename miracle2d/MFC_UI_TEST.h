// MFC_UI_TEST.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CMFC_UI_TESTApp:
// �� Ŭ������ ������ ���ؼ��� MFC_UI_TEST.cpp�� �����Ͻʽÿ�.
//

class CMFC_UI_TESTApp : public CWinApp
{
public:
	CMFC_UI_TESTApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CMFC_UI_TESTApp theApp;