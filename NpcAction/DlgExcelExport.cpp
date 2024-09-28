// DlgExcelExport.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#pragma warning(disable:4311)
#pragma warning(disable:4312)

#include <boost/bind.hpp>
#include <process.h>
#include "../SigmaCore/String/StringFormat.h"
#include "../RanLogic/GLogicDataMan.h"
#include "NpcAction.h"
#include "DlgExcelExport.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// DlgExcelExport ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(DlgExcelExport, CDialog)

DlgExcelExport::DlgExcelExport(CWnd* pParent /*=NULL*/)
	: CDialog(DlgExcelExport::IDD, pParent)
    , m_WorkProgress(false)
{
}

DlgExcelExport::~DlgExcelExport()
{
}

void DlgExcelExport::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS_EXCEL_EXPORT, m_ProgressExport);
}


BEGIN_MESSAGE_MAP(DlgExcelExport, CDialog)
    ON_BN_CLICKED(IDOK, &DlgExcelExport::OnBnClickedOk)
    ON_WM_CLOSE()
    ON_WM_TIMER()
END_MESSAGE_MAP()


// DlgExcelExport �޽��� ó�����Դϴ�.
void DlgExcelExport::OnBnClickedOk()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    if (!m_WorkProgress)
        OnOK();
}

#define NPC_TALK_EXCEL_EXPORT_TIMER 1009

BOOL DlgExcelExport::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
    m_ProgressExport.SetRange(0, 100);
    SetProgressPercentage(0);

    SetTimer(NPC_TALK_EXCEL_EXPORT_TIMER, 1500, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void DlgExcelExport::SetProgressPercentage(int Percentage)
{
    m_ProgressExport.SetPos(Percentage);
}

void DlgExcelExport::ExcelExport()
{
    m_WorkProgress = true;
    SetProgressPercentage(0);

    std::string SavedExcel;    
    if (GLogicData::GetInstance().SaveNpcTalkExcel(SavedExcel, boost::bind(&DlgExcelExport::SetProgressPercentage, this, _1)))
    {
        //ShellExecute(0, "open", SavedExcel.c_str(), 0, 0, SW_SHOWNORMAL);

        std::string Msg(
            sc::string::format(
            "%1% �� ����Ǿ����ϴ�.",
            SavedExcel));
        AfxMessageBox(Msg.c_str(), MB_OK);
    }

    m_WorkProgress = false;
}

void DlgExcelExport::OnClose()
{
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
    if (!m_WorkProgress)
        CDialog::OnClose();
}

void DlgExcelExport::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
    if (nIDEvent == NPC_TALK_EXCEL_EXPORT_TIMER)
    {
        KillTimer(NPC_TALK_EXCEL_EXPORT_TIMER);
        m_SaveThread = boost::thread(boost::bind(&DlgExcelExport::ExcelExport, this));   
    }
    CDialog::OnTimer(nIDEvent);
}
