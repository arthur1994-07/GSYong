#pragma once

#include <set>
#include "afxcmn.h"


// CPassWordDlg ��ȭ �����Դϴ�.

class CPassWordDlg : public CDialog
{
	DECLARE_DYNAMIC(CPassWordDlg)

public:
	CPassWordDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CPassWordDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_PASSWORD };

	BOOL CheckPassWord();
    std::set<int> GetStorageUserNum() { return m_setStorageUserNum; }

	DWORD m_dwCreateTime;

private:
    std::set<int> m_setStorageUserNum;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CProgressCtrl m_prog_Load;
	afx_msg void OnTimer(UINT nIDEvent);
};
