#pragma once

#include "WebDlg.h"
#include "afxwin.h"

class CTwitter;

// CPage02 대화 상자입니다.

class CPage02 : public CPropertyPage
{
	DECLARE_DYNAMIC(CPage02)

public:
	CPage02();
	virtual ~CPage02();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PAGE02 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonAuth();
	CListBox m_lbMsgTW;

	CTwitter *m_pTwitter;

	CWebDlg m_dlgWeb;
	CButton m_btTweetMsg;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonTweetmsg();
	CEdit m_ebTweetMsg;
	CEdit m_ebFollow;
	CButton m_btFollow;
	CButton m_btUnFollow;
	afx_msg void OnBnClickedButtonFollow();
	afx_msg void OnBnClickedButtonUnfollow();
	
	BOOL m_bUserID;
};
