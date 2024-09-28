#pragma once

// SplashDlg ��ȭ �����Դϴ�.
#include <string>
#include "resource.h"

class SplashDlg : public CDialog
{
	DECLARE_DYNAMIC(SplashDlg)

public:
	SplashDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~SplashDlg();
    static SplashDlg* g_pSplashDlg;

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SPLASHDLG };

    static void ShowSplashScreen(CWnd* pParentWnd /*= NULL*/);
    void HideSplashScreen();
    static BOOL PreTranslateAppMessage(MSG* pMsg);
    static void DisplayMessage(const std::string& Message);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL OnInitDialog();
};
