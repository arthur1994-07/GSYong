// Page04.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "RanStatistics.h"
#include "Page04.h"
#include "ODBCManager.h"
#include "ADOManager.h"


// CPage04 ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CPage04, CPropertyPage)
CPage04::CPage04( CMainPropertySheet *p )
	: CPropertyPage(CPage04::IDD)
{
	m_pPropertySheet = p;
}

CPage04::~CPage04()
{
	CConsoleMessage::GetInstance()->SetConsole( NULL );	//Dettach the console
}

void CPage04::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DAILY_PLAYTIME, m_lbDailyPlayTime);
	DDX_Control(pDX, IDC_LIST_MSG_04, m_lbMsg);
	DDX_Control(pDX, IDC_LIST_DAILY_PLAYTIME_DETAIL, m_lbDailyPlayTimeDetail);
}


BEGIN_MESSAGE_MAP(CPage04, CPropertyPage)
	ON_WM_CHILDACTIVATE()
	ON_BN_CLICKED(IDC_BUTTON_PROCESS_04, OnBnClickedButtonProcess04)
END_MESSAGE_MAP()


// CPage04 �޽��� ó�����Դϴ�.

BOOL CPage04::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CPage04::OnChildActivate()
{
	CPropertyPage::OnChildActivate();

	CConsoleMessage::GetInstance()->SetConsole( &m_lbMsg );
}

void CPage04::OnBnClickedButtonProcess04()
{
	CDBManager *p = GetDBManager();

	if (!p)
		return;

	GetDlgItem(IDC_BUTTON_PROCESS_04)->SetWindowText(_T("Now loading..."));
	GetDlgItem(IDC_BUTTON_PROCESS_04)->EnableWindow(FALSE);

	
	CTime &ctFrom = GetFrom();
	CTime &ctTo = GetTo();


	m_lbDailyPlayTime.ResetContent();	
	m_lbDailyPlayTimeDetail.ResetContent();
	p->SQLPage04_Daily_PlayTime( ctFrom, ctTo, &m_lbDailyPlayTime, &m_lbDailyPlayTimeDetail );


	if ( IsExported() )
	{
		CExport2File ef1(EF_DAILY_GAMETIME);
		CExport2File ef2(EF_DAILY_GAMETIME_DETAIL);
		
		ef1.ExportFromListBox(&m_lbDailyPlayTime);
		ef2.ExportFromListBox(&m_lbDailyPlayTimeDetail);		
	}
	
	GetDlgItem(IDC_BUTTON_PROCESS_04)->SetWindowText(_T("Process"));
	GetDlgItem(IDC_BUTTON_PROCESS_04)->EnableWindow(TRUE);
}

BOOL CPage04::IsODBCReady()
{
	return m_pPropertySheet->IsODBCReady();
}

BOOL CPage04::IsExported()
{
	return m_pPropertySheet->IsExported();
}

CTime& CPage04::GetFrom()
{
	return m_pPropertySheet->GetFrom();
}

CTime& CPage04::GetTo()
{
	return m_pPropertySheet->GetTo();
}

CDBManager *CPage04::GetDBManager()
{
	return m_pPropertySheet->GetDBManager();
}