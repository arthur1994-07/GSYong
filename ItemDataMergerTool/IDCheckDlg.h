#pragma once
#include "afxcmn.h"

// CIDCheckDlg ��ȭ �����Դϴ�.

class CIDCheckDlg : public CDialog
{
	DECLARE_DYNAMIC(CIDCheckDlg)

public:
	CIDCheckDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	CIDCheckDlg(const SNATIVEID sNativeID, const CString strItemName, GLItemMan *pItemMan );  
	virtual ~CIDCheckDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_IDCHECK_DIALOG };

	SNATIVEID		   m_sItemID;

private:
	GLItemMan		  *m_pItemMan;
	CString			   m_strItemName;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSearchButton();
};
