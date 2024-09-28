// DlgLog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "EffectTool.h"
#include "DlgLog.h"


// DlgLog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(DlgLog, CDialog)

DlgLog::DlgLog(CWnd* pParent /*=NULL*/)
	: CDialog(DlgLog::IDD, pParent)
{

}

DlgLog::~DlgLog()
{
}

void DlgLog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_LOG, m_EditLog);
}


BEGIN_MESSAGE_MAP(DlgLog, CDialog)
    ON_WM_SIZE()
    ON_EN_MAXTEXT(IDC_EDIT_LOG, &DlgLog::OnEnMaxtextEditLog)
END_MESSAGE_MAP()


// DlgLog �޽��� ó�����Դϴ�.

BOOL DlgLog::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
    m_EditLog.SetLimitText((UINT) (512 * 1024));

    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void DlgLog::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.

	// [2012.07.06] ������ �ִ� �ڵ� ����.
	//				Release�� ReleaseD �� ������, Debug ������ �۵��� �ȵȴ�.
#ifndef _DEBUG
    m_EditLog.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
#endif
}

void DlgLog::OnEnMaxtextEditLog()
{
    // TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    m_EditLog.SetWindowText("");
}
