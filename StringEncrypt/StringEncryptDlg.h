
// StringEncryptDlg.h : header file
//

#pragma once


// CStringEncryptDlg dialog
class CStringEncryptDlg : public CDialog
{
// Construction
public:
	CStringEncryptDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_STRINGENCRYPT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonDecrypt();
	afx_msg void OnBnClickedButtonEncrypt();
};
