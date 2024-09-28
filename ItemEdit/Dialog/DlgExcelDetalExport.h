#pragma once

#include "afxcmn.h"
#include <boost/thread.hpp>
#include "../resource.h"

// DlgExcelExport ��ȭ �����Դϴ�.

class CsheetWithTab;

class DlgExcelDetailExport : public CDialog
{
	DECLARE_DYNAMIC(DlgExcelDetailExport)

public:
	DlgExcelDetailExport(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DlgExcelDetailExport();

	// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EXCEL_EXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_ProgressExport;
	bool m_WorkProgress;
	boost::thread m_SaveThread;

	void SetProgressPercentage(int Percentage);
	void ExcelExport();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
