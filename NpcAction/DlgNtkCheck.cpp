// DlgNtkCheck.cpp : ���� �����Դϴ�.
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

// DlgNtkCheck ��ȭ �����Դϴ�.

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


// DlgNtkCheck �޽��� ó�����Դϴ�.

void DlgNtkCheck::OnBnClickedButtonClose()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    OnOK();
}

#define NTK_CHECK_TIMER 110725

BOOL DlgNtkCheck::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
    m_ProgressCheck.SetRange(0, 100);
    SetTimer(NTK_CHECK_TIMER, 1000, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void DlgNtkCheck::OnOK()
{
    // TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
    if (m_ThreadRun)
        return;

    CDialog::OnOK();
}

void DlgNtkCheck::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
    if (nIDEvent == NTK_CHECK_TIMER)
    {
        KillTimer(NTK_CHECK_TIMER);
        m_CheckThread = boost::thread(boost::bind(&DlgNtkCheck::NtkCheckThreadFunc, this));   
    }

    CDialog::OnTimer(nIDEvent);
}

void DlgNtkCheck::OnClose()
{
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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
    m_CurrentFile.SetWindowText("�غ����Դϴ�...");
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
    m_CurrentFile.SetWindowText("���� �м��� �������ϴ�. �α׸� �� Ȯ���Ͻð� �����ϼ���.");
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