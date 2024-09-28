#pragma once

#include <map>

// CSelectCheckDlg ��ȭ �����Դϴ�.

class CSelectCheckDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectCheckDlg)

public:
	CSelectCheckDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSelectCheckDlg();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_CHECK };

public :
	
	DWORD   m_dwCheckList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
