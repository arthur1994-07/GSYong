#pragma once
#include "afxcmn.h"

#include <set>

// CCompareTargetDlg ��ȭ �����Դϴ�.

class CCompareTargetDlg : public CDialog
{
	DECLARE_DYNAMIC(CCompareTargetDlg)

private:
	CStringArray m_strHeadArray;
    std::set<CString>  m_setCompareTargetStr;

public:
	CCompareTargetDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CCompareTargetDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_COMPARE_TARGET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CListCtrl m_list_CompareTarget;
	CListCtrl m_list_CompareSearch;
	CListCtrl m_list_Compare;
	virtual BOOL OnInitDialog();
    void InitStrHeadArray( const LPCSTR szFile, std::set<CString> setCompareTargetStr, DWORD compareType );
	std::set<CString>  GetCompareTargetStr() { return m_setCompareTargetStr; }

	afx_msg void OnBnClickedButtonHeadSearch();
	afx_msg void OnLvnItemchangedListCompareTarget(NMHDR *pNMHDR, LRESULT *pResult);
};
