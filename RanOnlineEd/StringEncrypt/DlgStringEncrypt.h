#pragma once

#include "../Resource.h"

// DlgStringEncrypt ��ȭ �����Դϴ�.

class DlgStringEncrypt : public CDialog
{
	DECLARE_DYNAMIC(DlgStringEncrypt)

public:
	DlgStringEncrypt(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DlgStringEncrypt();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_STRING_ENCTYPT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonStringEncrypt();
    afx_msg void OnBnClickedButtonStringDecrypt();
    afx_msg void OnBnClickedButtonStringCopyToClipboard();
};
