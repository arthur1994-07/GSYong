#pragma once
//#include "afxwin.h"

#include "WebDlg.h"
#include "afxwin.h"

class CFacebook;

// CPage01 대화 상자입니다.

class CPage01 : public CPropertyPage
{
	DECLARE_DYNAMIC(CPage01)

public:
	CPage01();
	virtual ~CPage01();


// 대화 상자 데이터입니다.
	enum { IDD = IDD_PAGE01 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonAuth();

	CWebDlg m_dlgWeb;

	CFacebook *m_pFacebook;

	CString m_csPathPhoto;


	CListBox m_lbMsgFB;
	afx_msg void OnBnClickedButtonWallpost();
	CButton m_btWallPost;
	CEdit m_ebWallMsg;
	CButton m_btPublishPhotos;
	afx_msg void OnBnClickedButtonPublishPhotos();
	CEdit m_ebCaption;
	CEdit m_ebPath;
	afx_msg void OnBnClickedButtonBrowse();
};
