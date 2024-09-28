// AdoTestDlg.h : header file
//

#pragma once

#include <string>
#include <boost/tr1/memory.hpp>
#include "afxwin.h"

namespace sc {
    namespace db {
        class CjADO;
    }
}

// AdoTestDlg dialog
class AdoTestDlg : public CDialog
{
// Construction
public:
	AdoTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ADOTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
    std::string m_ConnectionString;
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    CString m_ServerIP;
    CString m_ServerPort;
    CString m_ServerID;
    CString m_ServerPassword;
    CString m_DatabaseName;
    CComboBox m_Provider;
    CEdit m_EditLog;
    afx_msg void OnBnClickedButtonConnect();
    afx_msg void OnEnMaxtextEditLog();    
    afx_msg void OnClose();
    afx_msg void OnBnClickedButtonTest1();
    afx_msg void OnBnClickedButtonTest2();
    afx_msg void OnBnClickedButtonTest3();
    afx_msg void OnBnClickedButtonTest4();
    afx_msg void OnBnClickedButtonTest5();
    afx_msg void OnBnClickedButtonTest6();
    afx_msg void OnBnClickedButtonLogItemExchangeInsertNew();
};
