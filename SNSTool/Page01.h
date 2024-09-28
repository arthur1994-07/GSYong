#pragma once
//#include "afxwin.h"

#include "WebDlg.h"
#include "afxwin.h"

class CFacebook;

// CPage01 ��ȭ �����Դϴ�.

class CPage01 : public CPropertyPage
{
	DECLARE_DYNAMIC(CPage01)

public:
	CPage01();
	virtual ~CPage01();


// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_PAGE01 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
