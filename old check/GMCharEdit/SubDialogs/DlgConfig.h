#pragma once

#include "../resource.h"


// CDlgConfig ��ȭ �����Դϴ�.

class CDlgConfig : public CDialog
{
	DECLARE_DYNAMIC(CDlgConfig)

public:
	CDlgConfig(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgConfig();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_strOdbcName;
	CString m_strUserName;
	CString m_strPassword;
	CString m_strPassword2;
};
