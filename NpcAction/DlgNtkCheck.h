#pragma once

#pragma warning(disable:4312)
#pragma warning(disable:4311)

#include "afxcmn.h"
#include "afxwin.h"
#include <boost/thread.hpp>

// DlgNtkCheck ��ȭ �����Դϴ�.

class DlgNtkCheck : public CDialog
{
	DECLARE_DYNAMIC(DlgNtkCheck)

public:
	DlgNtkCheck(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~DlgNtkCheck();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_NTK_CHECK };

private:
    boost::thread m_CheckThread;
    
protected:
    void GetNtkList(std::vector<std::string>& vFileList);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
    CProgressCtrl m_ProgressCheck;    
    CStatic m_CurrentFile;
    bool m_ThreadRun;
    afx_msg void OnBnClickedButtonClose();
    virtual BOOL OnInitDialog();
    void NtkCheckThreadFunc();
    void SetProgressPos(int nPos);

protected:
    virtual void OnOK();

public:
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    CButton m_CloseButton;
    afx_msg void OnClose();
};
