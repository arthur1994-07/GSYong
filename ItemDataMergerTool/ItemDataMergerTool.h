// ItemDataMergerTool.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"		// �� ��ȣ


// CItemDataMergerToolApp:
// �� Ŭ������ ������ ���ؼ��� ItemDataMergerTool.cpp�� �����Ͻʽÿ�.
//

class CItemDataMergerToolApp : public CWinApp
{
public:
	CItemDataMergerToolApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CItemDataMergerToolApp theApp;
