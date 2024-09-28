#pragma once

#include "afxcmn.h"
#include <boost/thread.hpp>
#include "resource.h"

// DlgExcelExport ��ȭ �����Դϴ�.

class DlgExcelExport : public CDialog
{
	DECLARE_DYNAMIC(DlgExcelExport)

public:
	DlgExcelExport(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DlgExcelExport();

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
