#pragma once

namespace gmce
{
	class IDBManager;
}

// CDlgGMLogin ��ȭ �����Դϴ�.

class CDlgGMLogin : public CDialog
{
	DECLARE_DYNAMIC(CDlgGMLogin)

public:
	CDlgGMLogin(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgGMLogin();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_LOGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_strGMID;
	CString m_strPassword;
	int		m_nRetCode;


	gmce::IDBManager *m_pDbMan;
};
