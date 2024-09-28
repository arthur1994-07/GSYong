#pragma once
#include "afxcmn.h"

struct	SParam
{
	TSTRING m_strExportName;
};

// CProgressBarDlg ��ȭ �����Դϴ�.

class CProgressBarDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressBarDlg)

public:
	SParam m_sParam;
	void Create( const TSTRING& strName );

public:
	CProgressBarDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CProgressBarDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_PROGRESS_BAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_ctrlProgress;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
