// RanStatistics.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//
#pragma once


#include "MainPropertySheet.h"
#include "Page01.h"


#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"		// �� ��ȣ


// CRanStatisticsApp:
// �� Ŭ������ ������ ���ؼ��� RanStatistics.cpp�� �����Ͻʽÿ�.
//

class CRanStatisticsApp : public CWinApp
{
public:
	CRanStatisticsApp();

// ������
	public:
	virtual BOOL InitInstance();

	//CMainPropertySheet m_PropSheet ( _T("MainPropertySheet") );
	//CPage01 m_Page01;

// ����

	DECLARE_MESSAGE_MAP()
};

extern CRanStatisticsApp theApp;
