// DlgExcelExport.cpp : 구현 파일입니다.
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

// DlgExcelExport 대화 상자입니다.

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


// DlgExcelExport 메시지 처리기입니다.
void DlgExcelExport::OnBnClickedOk()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (!m_WorkProgress)
        OnOK();
}

#define NPC_TALK_EXCEL_EXPORT_TIMER 1009

BOOL DlgExcelExport::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    m_ProgressExport.SetRange(0, 100);
    SetProgressPercentage(0);

    SetTimer(NPC_TALK_EXCEL_EXPORT_TIMER, 1500, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
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
            "%1% 이 저장되었습니다.",
            SavedExcel));
        AfxMessageBox(Msg.c_str(), MB_OK);
    }

    m_WorkProgress = false;
}

void DlgExcelExport::OnClose()
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (!m_WorkProgress)
        CDialog::OnClose();
}

void DlgExcelExport::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (nIDEvent == NPC_TALK_EXCEL_EXPORT_TIMER)
    {
        KillTimer(NPC_TALK_EXCEL_EXPORT_TIMER);
        m_SaveThread = boost::thread(boost::bind(&DlgExcelExport::ExcelExport, this));   
    }
    CDialog::OnTimer(nIDEvent);
}
