#pragma once

#include "afxcmn.h"
#include <boost/thread.hpp>
#include "../resource.h"

// DlgExcelExport 대화 상자입니다.

class CsheetWithTab;

class DlgExcelDetailExport : public CDialog
{
	DECLARE_DYNAMIC(DlgExcelDetailExport)

public:
	DlgExcelDetailExport(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DlgExcelDetailExport();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EXCEL_EXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
