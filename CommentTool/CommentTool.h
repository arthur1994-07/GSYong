// CommentTool.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"		// �� ��ȣ


// CCommentToolApp:
// �� Ŭ������ ������ ���ؼ��� CommentTool.cpp�� �����Ͻʽÿ�.
//

class CCommentToolApp : public CWinApp
{
public:
	CCommentToolApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CCommentToolApp theApp;
