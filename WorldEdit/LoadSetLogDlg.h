#pragma once
#include "afxwin.h"


// CLoadSetLogDlg ��ȭ �����Դϴ�.

class CLoadSetLogDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoadSetLogDlg)

public:
	CLoadSetLogDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLoadSetLogDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_LOADSET_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ctrlEditLog;
	CString m_strEditLog;
	void LoadSetLog(void);
	virtual BOOL OnInitDialog();
};
