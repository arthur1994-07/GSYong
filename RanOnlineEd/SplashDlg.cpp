// SplashDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "RanOnlineEd.h"
#include "SplashDlg.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


SplashDlg* SplashDlg::g_pSplashDlg = NULL;

// SplashDlg ��ȭ �����Դϴ�.

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


// SplashDlg �޽��� ó�����Դϴ�.

void SplashDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

    CDialog::OnTimer(nIDEvent);
}

BOOL SplashDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

    return TRUE;  // return TRUE unless you set the focus to a control
    // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
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