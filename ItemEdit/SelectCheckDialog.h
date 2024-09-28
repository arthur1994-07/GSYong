#pragma once

#include <map>

// CSelectCheckDlg 대화 상자입니다.

class CSelectCheckDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectCheckDlg)

public:
	CSelectCheckDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSelectCheckDlg();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CHECK };

public :
	
	DWORD   m_dwCheckList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	afx_msg void OnSelchangeList();
	afx_msg void OnCheckchangeList();

	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:

	int m_nSelect;
	DWORD m_dwData;

	CCheckListBox	  m_SelectList;

public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	
};
