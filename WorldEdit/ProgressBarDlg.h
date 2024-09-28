#pragma once
#include "afxcmn.h"

struct	SParam
{
	TSTRING m_strExportName;
};

// CProgressBarDlg 대화 상자입니다.

class CProgressBarDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressBarDlg)

public:
	SParam m_sParam;
	void Create( const TSTRING& strName );

public:
	CProgressBarDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CProgressBarDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_PROGRESS_BAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_ctrlProgress;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
