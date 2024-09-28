#include "stdafx.h"
#include "../GMCharEdit.h"
#include "DlgItemDisguise.h"

IMPLEMENT_DYNAMIC(CDlgItemDisguise, CDialog)
CDlgItemDisguise::CDlgItemDisguise(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgItemDisguise::IDD, pParent)
	, m_uiYear( 2006 )
	, m_uiMonth( 12 )
	, m_uiDay( 26 )
	, m_uiHour( 12 )
	, m_uiMinute( 31 )
	, m_bDisguise( TRUE )
{
}

CDlgItemDisguise::~CDlgItemDisguise()
{
}

void CDlgItemDisguise::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDT_YEAR, m_ctrlYear);
	DDX_Control(pDX, IDC_EDT_MONTH, m_ctrlMonth);
	DDX_Control(pDX, IDC_EDT_DAY, m_ctrlDay);
	DDX_Control(pDX, IDC_EDT_TIME, m_ctrlHour);
	DDX_Control(pDX, IDC_EDT_MINUTE, m_ctrlMinute);
	DDX_Text(pDX, IDC_EDT_YEAR, m_uiYear);
	DDV_MinMaxUInt(pDX, m_uiYear, 1970, 2082);
	DDX_Text(pDX, IDC_EDT_MONTH, m_uiMonth);
	DDV_MinMaxUInt(pDX, m_uiMonth, 1, 12);
	DDX_Text(pDX, IDC_EDT_DAY, m_uiDay);
	DDV_MinMaxUInt(pDX, m_uiDay, 1, 31);
	DDX_Text(pDX, IDC_EDT_TIME, m_uiHour);
	DDV_MinMaxUInt(pDX, m_uiHour, 0, 23);
	DDX_Text(pDX, IDC_EDT_MINUTE, m_uiMinute);
	DDV_MinMaxUInt(pDX, m_uiMinute, 0, 59);
	DDX_Check(pDX, IDC_CHECK_VALID, m_bDisguise);
}


BEGIN_MESSAGE_MAP(CDlgItemDisguise, CDialog)
	ON_BN_CLICKED(IDC_CHECK_VALID, OnBnClickedCheckValid)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


BOOL CDlgItemDisguise::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_uiYear = m_cTime.GetYear();
	m_uiMonth = m_cTime.GetMonth();
	m_uiDay = m_cTime.GetDay();
	m_uiHour = m_cTime.GetHour();
	m_uiMinute = m_cTime.GetMinute();

	UpdateData( FALSE );

	OnBnClickedCheckValid();

	return TRUE;
}

void CDlgItemDisguise::OnBnClickedCheckValid()
{
	UpdateData();

	if( m_bDisguise )
	{
		m_ctrlYear.EnableWindow();
		m_ctrlMonth.EnableWindow();
		m_ctrlDay.EnableWindow();
		m_ctrlHour.EnableWindow();
		m_ctrlMinute.EnableWindow();
	}
	else
	{
		m_ctrlYear.EnableWindow( FALSE );
		m_ctrlMonth.EnableWindow( FALSE );
		m_ctrlDay.EnableWindow( FALSE );
		m_ctrlHour.EnableWindow( FALSE );
		m_ctrlMinute.EnableWindow( FALSE );
	}
}

void CDlgItemDisguise::OnBnClickedOk()
{
	UpdateData();

	m_cTime = CTime( m_uiYear, m_uiMonth, m_uiDay, m_uiHour, m_uiMinute, 0 );

	OnOK();
}