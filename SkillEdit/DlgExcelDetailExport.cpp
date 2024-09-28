// DlgExcelExport.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#pragma warning(disable:4311)
#pragma warning(disable:4312)

#include <boost/bind.hpp>
#include <process.h>
#include "../SigmaCore/String/StringFormat.h"
#include "SkillEditDlg.h"
#include "SkillEdit.h"

//#include "../Properties/SheetWithTab.h"
#include "DlgExcelDetailExport.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// DlgExcelExport ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(DlgExcelDetailExport, CDialog)

DlgExcelDetailExport::DlgExcelDetailExport(CWnd* pParent /*=NULL*/)
: CDialog(DlgExcelDetailExport::IDD, pParent)
, m_WorkProgress(false)
{
}

DlgExcelDetailExport::~DlgExcelDetailExport()
{
}

void DlgExcelDetailExport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_EXCEL_EXPORT, m_ProgressExport);
}


BEGIN_MESSAGE_MAP(DlgExcelDetailExport, CDialog)
	ON_BN_CLICKED(IDOK, &DlgExcelDetailExport::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// DlgExcelExport �޽��� ó�����Դϴ�.
void DlgExcelDetailExport::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	if (!m_WorkProgress)
		OnOK();
}

#define NPC_TALK_EXCEL_EXPORT_TIMER 1009

BOOL DlgExcelDetailExport::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_ProgressExport.SetRange(0, 100);
	SetProgressPercentage(0);

	SetTimer(NPC_TALK_EXCEL_EXPORT_TIMER, 1500, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void DlgExcelDetailExport::SetProgressPercentage(int Percentage)
{
	m_ProgressExport.SetPos(Percentage);
}

void DlgExcelDetailExport::ExcelExport()
{
	m_WorkProgress = true;
	SetProgressPercentage(0);

	CSkillEditDlg* pSkillEditDlg =
		static_cast< CSkillEditDlg* >( theApp.m_pMainWnd );
	if ( pSkillEditDlg )
	{
		pSkillEditDlg->ExportExcel(
			boost::bind(
			&DlgExcelDetailExport::SetProgressPercentage, this, _1 ) );
	}
	/*m_pSheetTab->ExportExcelDetail(
		boost::bind(
		&DlgExcelDetailExport::SetProgressPercentage, this, _1 ) );*/

	m_WorkProgress = false;
}

void DlgExcelDetailExport::OnClose()
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (!m_WorkProgress)
		CDialog::OnClose();
}

void DlgExcelDetailExport::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (nIDEvent == NPC_TALK_EXCEL_EXPORT_TIMER)
	{
		KillTimer(NPC_TALK_EXCEL_EXPORT_TIMER);
		m_SaveThread = boost::thread(boost::bind(&DlgExcelDetailExport::ExcelExport, this));   
	}
	CDialog::OnTimer(nIDEvent);
}
