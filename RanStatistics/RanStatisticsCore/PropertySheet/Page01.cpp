// Page01.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RanStatistics.h"
#include "Page01.h"
#include "ODBCManager.h"
#include "ADOManager.h"


// CPage01 대화 상자입니다.

IMPLEMENT_DYNAMIC(CPage01, CPropertyPage)
CPage01::CPage01( CMainPropertySheet *p )
	: CPropertyPage(CPage01::IDD)
{
	m_pPropertySheet = p;
}

CPage01::~CPage01()
{	
}

void CPage01::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_NEWUSER, m_lbNewUser);
	DDX_Control(pDX, IDC_LIST_MSG_01, m_lbMsg);	
	DDX_Control(pDX, IDC_LIST_NEWCHAR, m_lbNewChar);
	DDX_Control(pDX, IDC_MONTHLYUSERS, m_lbMonthlyUser);
	DDX_Control(pDX, IDC_MONTHLYSHOPINCOME, m_lbMonthlyShopIncome);
}


BEGIN_MESSAGE_MAP(CPage01, CPropertyPage)
	ON_WM_CHILDACTIVATE()
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, OnBnClickedButtonProcess)
END_MESSAGE_MAP()


// CPage01 메시지 처리기입니다.

BOOL CPage01::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CPage01::OnChildActivate()
{
	CPropertyPage::OnChildActivate();	

	CConsoleMessage::GetInstance()->SetConsole( &m_lbMsg );
}

void CPage01::OnBnClickedButtonProcess()
{
	CDBManager *p = GetDBManager();

	if (!p)
		return;

	GetDlgItem(IDC_BUTTON_PROCESS_01)->SetWindowText(_T("Now loading..."));
	GetDlgItem(IDC_BUTTON_PROCESS_01)->EnableWindow(FALSE);
	

	CTime &ctFrom = GetFrom();
	CTime &ctTo = GetTo();
	

	m_lbNewUser.ResetContent();
	p->SQLPage01_NewUser( ctFrom, ctTo, &m_lbNewUser );

	m_lbNewChar.ResetContent();
	p->SQLPage01_NewChar(  ctFrom, ctTo, &m_lbNewChar );

	m_lbMonthlyUser.ResetContent();
	p->SQLPage01_MonthlyUser(  ctFrom, ctTo, &m_lbMonthlyUser );

	m_lbMonthlyShopIncome.ResetContent();
	p->SQLPage01_MonthlyShopIncome(  ctFrom, ctTo, &m_lbMonthlyShopIncome );


	if ( IsExported() )
	{
		CExport2File ef1(EF_NEW_USER);
		CExport2File ef2(EF_NEW_CHAR);
		CExport2File ef3(EF_DISTINCT_USER);
		CExport2File ef4(EF_SHOP_INCOME);

		ef1.ExportFromListBox(&m_lbNewUser);
		ef2.ExportFromListBox(&m_lbNewChar);
		ef3.ExportFromListBox(&m_lbMonthlyUser);
		ef4.ExportFromListBox(&m_lbMonthlyShopIncome);
	}

    
	GetDlgItem(IDC_BUTTON_PROCESS_01)->SetWindowText(_T("Process"));
	GetDlgItem(IDC_BUTTON_PROCESS_01)->EnableWindow(TRUE);	
}


BOOL CPage01::IsODBCReady()
{
	return m_pPropertySheet->IsODBCReady();
}

BOOL CPage01::IsExported()
{
	return m_pPropertySheet->IsExported();
}

CTime& CPage01::GetFrom()
{
	return m_pPropertySheet->GetFrom();
}

CTime& CPage01::GetTo()
{
	return m_pPropertySheet->GetTo();
}

CDBManager *CPage01::GetDBManager()
{
	return m_pPropertySheet->GetDBManager();
}
