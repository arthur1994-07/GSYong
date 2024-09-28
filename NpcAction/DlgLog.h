#pragma once
#include "afxwin.h"


// DlgLog 대화 상자입니다.

class DlgLog : public CDialog
{
	DECLARE_DYNAMIC(DlgLog)

public:
	DlgLog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~DlgLog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
    CEdit m_EditLog;

public:
    HWND GetEditWindow() { return m_EditLog.m_hWnd; }
    virtual BOOL OnInitDialog();
    afx_msg void OnEnMaxtextEditLog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
};
