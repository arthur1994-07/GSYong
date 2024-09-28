#pragma once

#include "../resource.h"


// CDlgConfig 대화 상자입니다.

class CDlgConfig : public CDialog
{
	DECLARE_DYNAMIC(CDlgConfig)

public:
	CDlgConfig(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgConfig();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_strOdbcName;
	CString m_strUserName;
	CString m_strPassword;
	CString m_strPassword2;
};
