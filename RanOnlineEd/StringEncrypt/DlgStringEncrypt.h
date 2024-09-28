#pragma once

#include "../Resource.h"

// DlgStringEncrypt 대화 상자입니다.

class DlgStringEncrypt : public CDialog
{
	DECLARE_DYNAMIC(DlgStringEncrypt)

public:
	DlgStringEncrypt(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DlgStringEncrypt();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_STRING_ENCTYPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonStringEncrypt();
    afx_msg void OnBnClickedButtonStringDecrypt();
    afx_msg void OnBnClickedButtonStringCopyToClipboard();
};
