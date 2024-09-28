// DlgLog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EffectTool.h"
#include "DlgLog.h"


// DlgLog 대화 상자입니다.

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


// DlgLog 메시지 처리기입니다.

BOOL DlgLog::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.
    m_EditLog.SetLimitText((UINT) (512 * 1024));

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void DlgLog::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    // TODO: 여기에 메시지 처리기 코드를 추가합니다.

	// [2012.07.06] 문제가 있는 코드 같음.
	//				Release와 ReleaseD 는 되지만, Debug 에서는 작동이 안된다.
#ifndef _DEBUG
    m_EditLog.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
#endif
}

void DlgLog::OnEnMaxtextEditLog()
{
    // TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    m_EditLog.SetWindowText("");
}
