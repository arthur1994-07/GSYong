#pragma once


// CDlgMakeCronCharacter ��ȭ �����Դϴ�.

class CDlgMakeCronCharacter : public CDialog
{
	DECLARE_DYNAMIC(CDlgMakeCronCharacter)

public:
	CDlgMakeCronCharacter(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgMakeCronCharacter();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_MAKE_CRON_CHARACTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	int m_nUserNum;
	CString m_strChaName;
	afx_msg void OnBnClickedOk();
};
