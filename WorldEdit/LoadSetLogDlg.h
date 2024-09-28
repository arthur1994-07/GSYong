#pragma once
#include "afxwin.h"


// CLoadSetLogDlg 대화 상자입니다.

class CLoadSetLogDlg : public CDialog
{
	DECLARE_DYNAMIC(CLoadSetLogDlg)

public:
	CLoadSetLogDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLoadSetLogDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LOADSET_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ctrlEditLog;
	CString m_strEditLog;
	void LoadSetLog(void);
	virtual BOOL OnInitDialog();
};
