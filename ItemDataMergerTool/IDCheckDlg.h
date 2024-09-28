#pragma once
#include "afxcmn.h"

// CIDCheckDlg 대화 상자입니다.

class CIDCheckDlg : public CDialog
{
	DECLARE_DYNAMIC(CIDCheckDlg)

public:
	CIDCheckDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	CIDCheckDlg(const SNATIVEID sNativeID, const CString strItemName, GLItemMan *pItemMan );  
	virtual ~CIDCheckDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_IDCHECK_DIALOG };

	SNATIVEID		   m_sItemID;

private:
	GLItemMan		  *m_pItemMan;
	CString			   m_strItemName;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSearchButton();
};
