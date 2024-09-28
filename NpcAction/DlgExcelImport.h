#pragma once

#include "afxcmn.h"
#include <boost/thread.hpp>
#include "resource.h"

// DlgExcelImport ��ȭ �����Դϴ�.

class DlgExcelImport : public CDialog
{
	DECLARE_DYNAMIC(DlgExcelImport)

public:
	DlgExcelImport(const CString& LoadFile, CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DlgExcelImport();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EXCEL_IMPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
    CProgressCtrl m_ProgressImport;
    bool m_WorkProgress;
    bool m_LoadSuccess;
    boost::thread m_ImportThread;
    CString m_LoadFile;

    void SetProgressPercentage(int Percentage);
    void ExcelImport();
    afx_msg void OnBnClickedOk();
    virtual BOOL OnInitDialog();
    afx_msg void OnClose();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
