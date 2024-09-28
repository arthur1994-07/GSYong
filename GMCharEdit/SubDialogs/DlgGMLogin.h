#pragma once

namespace gmce
{
	class IDBManager;
}

// CDlgGMLogin 대화 상자입니다.

class CDlgGMLogin : public CDialog
{
	DECLARE_DYNAMIC(CDlgGMLogin)

public:
	CDlgGMLogin(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgGMLogin();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_strGMID;
	CString m_strPassword;
	int		m_nRetCode;


	gmce::IDBManager *m_pDbMan;
};
