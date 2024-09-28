#pragma once

#include <set>
#include "afxcmn.h"


// CPassWordDlg 대화 상자입니다.

class CPassWordDlg : public CDialog
{
	DECLARE_DYNAMIC(CPassWordDlg)

public:
	CPassWordDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CPassWordDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PASSWORD };

	BOOL CheckPassWord();
    std::set<int> GetStorageUserNum() { return m_setStorageUserNum; }

	DWORD m_dwCreateTime;

private:
    std::set<int> m_setStorageUserNum;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CProgressCtrl m_prog_Load;
	afx_msg void OnTimer(UINT nIDEvent);
};
