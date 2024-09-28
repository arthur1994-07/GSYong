// SplashDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "RanOnlineEd.h"
#include "SplashDlg.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


SplashDlg* SplashDlg::g_pSplashDlg = NULL;

// SplashDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(SplashDlg, CDialog)

SplashDlg::SplashDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SplashDlg::IDD, pParent)
{

}

SplashDlg::~SplashDlg()
{
}

void SplashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(SplashDlg, CDialog)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// SplashDlg 메시지 처리기입니다.

void SplashDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

    CDialog::OnTimer(nIDEvent);
}

BOOL SplashDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  여기에 추가 초기화 작업을 추가합니다.

    return TRUE;  // return TRUE unless you set the focus to a control
    // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void SplashDlg::ShowSplashScreen(CWnd* pParentWnd /*= NULL*/)
{

    // Allocate a new splash screen, and create the window.
    g_pSplashDlg = new SplashDlg;
    if (!g_pSplashDlg->Create(SplashDlg::IDD, pParentWnd))
    {
        delete g_pSplashDlg;
        g_pSplashDlg = NULL;
    }
    else
    {
        g_pSplashDlg->ShowWindow(SW_SHOW);
    }
    g_pSplashDlg->UpdateWindow();

    g_pSplashDlg->SetTimer(1,2000, NULL);
}

void SplashDlg::DisplayMessage(const std::string& Message)
{
    //if (g_pSplashDlg )
        g_pSplashDlg->GetDlgItem( IDC_STATIC_SPLASH )->SetWindowText( Message.c_str() );
}

void SplashDlg::HideSplashScreen()
{
    // Destroy the window, and update the mainframe.
    g_pSplashDlg->KillTimer(1);
    DestroyWindow();
    AfxGetMainWnd()->UpdateWindow();
    delete g_pSplashDlg;
    g_pSplashDlg = NULL;
}

BOOL SplashDlg::PreTranslateAppMessage(MSG* pMsg)
{

    if (!g_pSplashDlg)
        return FALSE;

    // If you receive a keyboard or mouse message, hide the splash screen.
    if (g_pSplashDlg->m_hWnd != NULL && pMsg->message == WM_KEYDOWN ||
        pMsg->message == WM_SYSKEYDOWN ||
        pMsg->message == WM_LBUTTONDOWN ||
        pMsg->message == WM_RBUTTONDOWN ||
        pMsg->message == WM_MBUTTONDOWN ||
        pMsg->message == WM_NCLBUTTONDOWN ||
        pMsg->message == WM_NCRBUTTONDOWN ||
        pMsg->message == WM_NCMBUTTONDOWN)
    {
        g_pSplashDlg->HideSplashScreen();
        return TRUE;	// message handled here
    }
    else
    {
        return FALSE;	// message not handled
    }
}