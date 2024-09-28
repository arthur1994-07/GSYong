// DlgNtkCheck.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include <boost/bind.hpp>
#include <process.h>
#include "../RanLogic/LogicDataCheck.h"
#include "../RanLogic/NpcTalk/NpcDialogueSet.h"
#include "../RanLogic/NpcTalk/NpcTalkMan.h"
#include "../RanLogic/GLogicData.h"
#include "../RanLogic/GLogicDataMan.h"

#include "NpcAction.h"
#include "DlgNtkCheck.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

// DlgNtkCheck 대화 상자입니다.

IMPLEMENT_DYNAMIC(DlgNtkCheck, CDialog)

DlgNtkCheck::DlgNtkCheck(CWnd* pParent /*=NULL*/)
	: CDialog(DlgNtkCheck::IDD, pParent)
    , m_ThreadRun(false)
{

}

DlgNtkCheck::~DlgNtkCheck()
{
}

void DlgNtkCheck::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS_NTK_CHECK, m_ProgressCheck);
    DDX_Control(pDX, IDC_STATIC_NTK_CHECK, m_CurrentFile);
    DDX_Control(pDX, IDC_BUTTON_CLOSE, m_CloseButton);
}


BEGIN_MESSAGE_MAP(DlgNtkCheck, CDialog)
    ON_BN_CLICKED(IDC_BUTTON_CLOSE, &DlgNtkCheck::OnBnClickedButtonClose)
    ON_WM_TIMER()
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// DlgNtkCheck 메시지 처리기입니다.

void DlgNtkCheck::OnBnClickedButtonClose()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    OnOK();
}

#define NTK_CHECK_TIMER 110725

BOOL DlgNtkCheck::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    m_ProgressCheck.SetRange(0, 100);
    SetTimer(NTK_CHECK_TIMER, 1000, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void DlgNtkCheck::OnOK()
{
    // TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
    if (m_ThreadRun)
        return;

    CDialog::OnOK();
}

void DlgNtkCheck::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (nIDEvent == NTK_CHECK_TIMER)
    {
        KillTimer(NTK_CHECK_TIMER);
        m_CheckThread = boost::thread(boost::bind(&DlgNtkCheck::NtkCheckThreadFunc, this));   
    }

    CDialog::OnTimer(nIDEvent);
}

void DlgNtkCheck::OnClose()
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    if (m_ThreadRun)
        return;

    CDialog::OnClose();
}

void DlgNtkCheck::SetProgressPos(int nPos)
{
    m_ProgressCheck.SetPos(nPos);
}

void DlgNtkCheck::NtkCheckThreadFunc()
{
    m_CurrentFile.SetWindowText("준비중입니다...");
    m_ThreadRun = true;
    SetProgressPos(0);
    m_CloseButton.EnableWindow(FALSE);    
    // ------------------------------------------------------------------------
    std::tr1::shared_ptr<NpcTalkMan> spTalkMan = GLogicData::GetInstance().GetNpcTalkMan();
    if (!spTalkMan)
        return;

    int ErrorNum = 0;
    std::vector<std::string> vFileList;
    spTalkMan->GetDlgList(vFileList);

    size_t TotalSize = vFileList.size();
    for (size_t i=0; i<TotalSize; ++i)
    {
        int Percentage = (((i+1)*100) / TotalSize);
        SetProgressPos(Percentage);
        m_CurrentFile.SetWindowText(vFileList[i].c_str());
        LOGIC_DATA_CHECK::NtkCheck(vFileList[i], ErrorNum);
    }

    // ------------------------------------------------------------------------
    m_CloseButton.EnableWindow(TRUE);
    m_ThreadRun = false;
    m_CurrentFile.SetWindowText("오류 분석이 끝났습니다. 로그를 꼭 확인하시고 수정하세요.");
}

void DlgNtkCheck::GetNtkList(std::vector<std::string>& vFileList)
{
    std::string FilePath(CNpcDialogueSet::GetPath());
    FilePath.append("\\*.ntk");

    CFileFind finder;
    BOOL bWorking = finder.FindFile(FilePath.c_str());
    while (bWorking)
    {
        bWorking = finder.FindNextFile();
        CString FileName = finder.GetFileName();
        CString Path = finder.GetFilePath();
        std::string NtkFileName(FileName.GetString());
        vFileList.push_back(NtkFileName);
    }
}