#pragma once

#include "afxcmn.h"
#include <boost/thread.hpp>
#include "resource.h"

// DlgExcelImport 대화 상자입니다.

class DlgExcelImport : public CDialog
{
	DECLARE_DYNAMIC(DlgExcelImport)

public:
	DlgExcelImport(const CString& LoadFile, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DlgExcelImport();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EXCEL_IMPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
