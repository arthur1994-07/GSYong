// DlgExcelImport.cpp : ���� �����Դϴ�.
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

// DlgExcelImport ��ȭ �����Դϴ�.

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


// DlgExcelImport �޽��� ó�����Դϴ�.
void DlgExcelImport::OnBnClickedOk()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    if (!m_WorkProgress)
        OnOK();
}

#define NPC_TALK_EXCEL_IMPORT_TIMER 1019

BOOL DlgExcelImport::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
    m_ProgressImport.SetRange(0, 100);
    SetProgressPercentage(0);

    SetTimer(NPC_TALK_EXCEL_IMPORT_TIMER, 1500, NULL);

    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
    if (!m_WorkProgress)
        CDialog::OnClose();
}

void DlgExcelImport::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
    if (nIDEvent == NPC_TALK_EXCEL_IMPORT_TIMER)
    {
        KillTimer(NPC_TALK_EXCEL_IMPORT_TIMER);
        m_ImportThread = boost::thread(boost::bind(&DlgExcelImport::ExcelImport, this));   
    }
    CDialog::OnTimer(nIDEvent);
}
