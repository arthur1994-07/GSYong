// DlgExcelImport.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#pragma warning(disable:4311)
#pragma warning(disable:4312)

#include <boost/bind.hpp>
#include <process.h>
#include "../SigmaCore/String/StringFormat.h"
#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/NpcTalk/NpcTalkMan.h"

#include "NpcAction.h"
#include "DlgExcelImport.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// DlgExcelImport 대화 상자입니다.

IMPLEMENT_DYNAMIC(DlgExcelImport, CDialog)

DlgExcelImport::DlgExcelImport(const CString& LoadFile, CWnd* pParent /*=NULL*/)
	: CDialog(DlgExcelImport::IDD, pParent)
    , m_WorkProgress(false)
    , m_LoadSuccess(false)
    , m_LoadFile(LoadFile)
{

}

DlgExcelImport::~DlgExcelImport()
{
}

void DlgExcelImport::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS1, m_ProgressImport);
}


BEGIN_MESSAGE_MAP(DlgExcelImport, CDialog)
    ON_BN_CLICKED(IDOK, &DlgExcelImport::OnBnClickedOk)
    ON_WM_CLOSE()
    ON_WM_TIMER()
END_MESSAGE_MAP()


// DlgExcelImport 메시지 처리기입니다.
void DlgExcelImport::OnBnClickedOk()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    if (!m_WorkProgress)
        OnOK();
}

#define NPC_TALK_EXCEL_IMPORT_TIMER 1019

BOOL DlgExcelImport::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    m_ProgressImport.SetRange(0, 100);
    SetProgressPercentage(0);

    SetTimer(NPC_TALK_EXCEL_IMPORT_TIMER, 1500, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void DlgExcelImport::SetProgressPercentage(int Percentage)
{
    m_ProgressImport.SetPos(Percentage);
}

void DlgExcelImport::ExcelImport()
{
    m_WorkProgress = true;
    SetProgressPercentage(0);

    std::tr1::shared_ptr<NpcTalkMan> pTalkMan = GLogicData::GetInstance().GetNpcTalkMan();    
    if (pTalkMan->LoadExcel(m_LoadFile.GetString(), boost::bind(&DlgExcelImport::SetProgressPercentage, this, _1)))
    {
        m_LoadSuccess = true;
        m_WorkProgress = false;
        MessageBox("Success");            
    }
    else
    {
        m_WorkProgress = false;
        MessageBox("Fail");
    }    
}

void DlgExcelImport::OnClose()
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (!m_WorkProgress)
        CDialog::OnClose();
}

void DlgExcelImport::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (nIDEvent == NPC_TALK_EXCEL_IMPORT_TIMER)
    {
        KillTimer(NPC_TALK_EXCEL_IMPORT_TIMER);
        m_ImportThread = boost::thread(boost::bind(&DlgExcelImport::ExcelImport, this));   
    }
    CDialog::OnTimer(nIDEvent);
}
