#pragma once
#include "afxwin.h"


// DlgLog ��ȭ �����Դϴ�.

class DlgLog : public CDialog
{
	DECLARE_DYNAMIC(DlgLog)

public:
	DlgLog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DlgLog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

private:
    CEdit m_EditLog;

public:
    HWND GetEditWindow() { return m_EditLog.m_hWnd; }
    virtual BOOL OnInitDialog();
    afx_msg void OnEnMaxtextEditLog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
};
