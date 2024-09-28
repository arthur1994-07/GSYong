
#include "stdafx.h"
#include "RanStatistics.h"
#include "Page03.h"
#include "ODBCManager.h"
#include "ADOManager.h"


IMPLEMENT_DYNAMIC(CPage03, CPropertyPage)
CPage03::CPage03( CMainPropertySheet *p )
	: CPropertyPage(CPage03::IDD)
	, m_bExceptKOR(FALSE)
{
	m_pPropertySheet = p;
}

CPage03::~CPage03()
{
	CConsoleMessage::GetInstance()->SetConsole( NULL );	//Dettach the console
}

void CPage03::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CC_NUM, m_lbCCNum);
	DDX_Control(pDX, IDC_LIST_CC_DETAIL, m_lbCCDetail);
	DDX_Control(pDX, IDC_LIST_MSG_03, m_lbMsg);
	DDX_Control(pDX, IDC_CHECK_EXCEPT_KOR, m_cbExceptKOR);
	DDX_Control(pDX, IDC_LIST_UID_SET, m_lbUID);
	DDX_Check(pDX, IDC_CHECK_EXCEPT_KOR, m_bExceptKOR);
}


BEGIN_MESSAGE_MAP(CPage03, CPropertyPage)
	ON_WM_CHILDACTIVATE()
	ON_BN_CLICKED(IDC_BUTTON_PROCESS_03, OnBnClickedButtonProcess03)	
END_MESSAGE_MAP()


// CPage03 메시지 처리기입니다.

BOOL CPage03::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	
	//
	//Default : Exept KOR data.
	//
	m_cbExceptKOR.SetCheck(TRUE);

	//COdbcManager::GetInstance()->SetExceptKOR( &m_cbExceptKOR );


	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CPage03::OnChildActivate()
{
	CPropertyPage::OnChildActivate();

	CConsoleMessage::GetInstance()->SetConsole( &m_lbMsg );
}

void CPage03::OnBnClickedButtonProcess03()
{
	CDBManager *p = GetDBManager();

	if (!p)
		return;

	GetDlgItem(IDC_BUTTON_PROCESS_03)->SetWindowText(_T("Now loading..."));
	GetDlgItem(IDC_BUTTON_PROCESS_03)->EnableWindow(FALSE);	

	
	CTime &ctFrom = GetFrom();
	CTime &ctTo = GetTo();


	m_lbCCNum.ResetContent();	
	m_lbCCDetail.ResetContent();
	p->SQLPage03_CC_Num( ctFrom, ctTo, IsExceptKOR(), &m_lbCCNum, &m_lbCCDetail, &m_lbUID );


	if ( IsExported() )
	{
		CExport2File ef1(EF_IP_2_CCODE);
		CExport2File ef2(EF_IP_2_CC_DETAIL);
		CExport2File ef3(EF_IP_2_CC_UID);

		ef1.ExportFromListBox(&m_lbCCNum);
		ef2.ExportFromListBox(&m_lbCCDetail);
		ef3.ExportFromListBox(&m_lbUID);
		
	}

	GetDlgItem(IDC_BUTTON_PROCESS_03)->SetWindowText(_T("Process"));
	GetDlgItem(IDC_BUTTON_PROCESS_03)->EnableWindow(TRUE);
	
}

BOOL CPage03::IsODBCReady()
{
	return m_pPropertySheet->IsODBCReady();
}

BOOL CPage03::IsExported()
{
	return m_pPropertySheet->IsExported();
}

CTime& CPage03::GetFrom()
{
	return m_pPropertySheet->GetFrom();
}

CTime& CPage03::GetTo()
{
	return m_pPropertySheet->GetTo();
}

BOOL CPage03::IsExceptKOR()
{
	return m_bExceptKOR;
}

CDBManager *CPage03::GetDBManager()
{
	return m_pPropertySheet->GetDBManager();
}