// UIEdit.h : UIEdit ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"       // �� ��ȣ


// CUIEditApp:
// �� Ŭ������ ������ ���ؼ��� UIEdit.cpp�� �����Ͻʽÿ�.
//

class CUIEditApp : public CWinApp
{
public:
	CUIEditApp();

public:
	void  SetActive(BOOL bActive) { m_bIsActive = bActive; };

public:
	BOOL  m_bIsActive;
	BOOL  m_bIsShutDown;

	CString	m_strAppPath;
	TCHAR	m_szAppPath[MAX_PATH];

public:
	void ShutDown() { m_bIsShutDown = TRUE; }


// ������
public:
	virtual BOOL InitInstance();
	virtual int Run();

// ����
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CUIEditApp theApp;
