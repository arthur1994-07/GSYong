#pragma once
#include "afxcmn.h"

#include <set>

// CCompareTargetDlg 대화 상자입니다.

class CCompareTargetDlg : public CDialog
{
	DECLARE_DYNAMIC(CCompareTargetDlg)

private:
	CStringArray m_strHeadArray;
    std::set<CString>  m_setCompareTargetStr;

public:
	CCompareTargetDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CCompareTargetDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_COMPARE_TARGET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
