#pragma once
#include "CWebBrowser2.h"
#include "afxwin.h"


// CWebDlg 대화 상자입니다.

class CWebDlg : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CWebDlg)

public:
	CWebDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWebDlg();
// 재정의
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

	void SetURL ( const TCHAR *pURL, const BOOL bTwitter ) 
					{ m_pURL = (TCHAR *)pURL; m_bTwitter = bTwitter; };

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WEB_DLG, IDH = IDR_HTML_WEBDLG };

protected:
	TCHAR	*m_pURL;
	BOOL	m_bTwitter;
	CString m_csLastURL;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();	

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()


public:
	CExplorer1 m_Browser;
	CString m_csPIN;

	//Contol variables only for Twitter.
	CEdit m_ebPIN;
	CStatic m_sttPIN;
	CButton m_btPIN;

	virtual INT_PTR DoModal();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonPin();	

};
