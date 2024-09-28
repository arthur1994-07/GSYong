#pragma once

// SplashDlg 대화 상자입니다.
#include <string>
#include "resource.h"

class SplashDlg : public CDialog
{
	DECLARE_DYNAMIC(SplashDlg)

public:
	SplashDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~SplashDlg();
    static SplashDlg* g_pSplashDlg;

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SPLASHDLG };

    static void ShowSplashScreen(CWnd* pParentWnd /*= NULL*/);
    void HideSplashScreen();
    static BOOL PreTranslateAppMessage(MSG* pMsg);
    static void DisplayMessage(const std::string& Message);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual BOOL OnInitDialog();
};
