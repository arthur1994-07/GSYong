// Page02.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RanStatistics.h"
#include "Page02.h"
#include "ODBCManager.h"
#include "ADOManager.h"


// CPage02 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPage02, CPropertyPage)
CPage02::CPage02( CMainPropertySheet *p )
	: CPropertyPage(CPage02::IDD)
{
	m_pPropertySheet = p;
}

CPage02::~CPage02()
{
	CConsoleMessage::GetInstance()->SetConsole( NULL );	//Dettach the console
}

void CPage02::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MSG_02, m_lbMsg);
	DDX_Control(pDX, IDC_LIST_ANAYSIS_GAMETIME, m_lbAnalaysisGameTime);
	DDX_Control(pDX, IDC_LIST_CONCURRENT, m_lbConcurrent);
	DDX_Control(pDX, IDC_LIST_CHALEVEL, m_lbChaLevel);
	DDX_Control(pDX, IDC_LIST_IP2NATION, m_lbIP2Nation);
}


BEGIN_MESSAGE_MAP(CPage02, CPropertyPage)
	ON_WM_CHILDACTIVATE()
	ON_BN_CLICKED(IDC_BUTTON_PROCESS_02, OnBnClickedButtonProcess02)
END_MESSAGE_MAP()


// CPage02 메시지 처리기입니다.

BOOL CPage02::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CPage02::OnChildActivate()
{
	CPropertyPage::OnChildActivate();	

	CConsoleMessage::GetInstance()->SetConsole( &m_lbMsg );
}

void CPage02::OnBnClickedButtonProcess02()
{
	CDBManager *p = GetDBManager();

	if (!p)
		return;

	GetDlgItem(IDC_BUTTON_PROCESS_02)->SetWindowText(_T("Now loading..."));
	GetDlgItem(IDC_BUTTON_PROCESS_02)->EnableWindow(FALSE);	


	CTime &ctFrom = GetFrom();
	CTime &ctTo = GetTo();


	m_lbAnalaysisGameTime.ResetContent();	
	p->SQLPage02_AnaysisGameTime(  ctFrom, ctTo, &m_lbAnalaysisGameTime );

	m_lbConcurrent.ResetContent();	
	p->SQLPage02_ConcurrentUsers(  ctFrom, ctTo, &m_lbConcurrent );


	m_lbChaLevel.ResetContent();	
	p->SQLPage02_ChaLevel(  ctFrom, ctTo, &m_lbChaLevel );

	m_lbIP2Nation.ResetContent();	
	p->SQLPage02_IP2Nation(  ctFrom, ctTo, &m_lbIP2Nation );


	if ( IsExported() )
	{
		CExport2File ef1(EF_DAILY_GAMETIME);
		CExport2File ef2(EF_CONCURRENT_USRES);
		CExport2File ef3(EF_CHAR_LEVEL);
		CExport2File ef4(EF_IP_2_NATION);

		ef1.ExportFromListBox(&m_lbAnalaysisGameTime);
		ef2.ExportFromListBox(&m_lbConcurrent);
		ef3.ExportFromListBox(&m_lbChaLevel);
		ef4.ExportFromListBox(&m_lbIP2Nation);
	}

	GetDlgItem(IDC_BUTTON_PROCESS_02)->SetWindowText(_T("Process"));
	GetDlgItem(IDC_BUTTON_PROCESS_02)->EnableWindow(TRUE);
}

BOOL CPage02::IsODBCReady()
{
	return m_pPropertySheet->IsODBCReady();
}

BOOL CPage02::IsExported()
{
	return m_pPropertySheet->IsExported();
}

CTime& CPage02::GetFrom()
{
	return m_pPropertySheet->GetFrom();
}

CTime& CPage02::GetTo()
{
	return m_pPropertySheet->GetTo();
}

CDBManager *CPage02::GetDBManager()
{
	return m_pPropertySheet->GetDBManager();
}