#pragma once


// CCollisionMapDlg ��ȭ �����Դϴ�.

class CCollisionMapDlg : public CDialog
{
	DECLARE_DYNAMIC(CCollisionMapDlg)

public:
	CCollisionMapDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CCollisionMapDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_COLLISIONMAP };

private:
	HANDLE	m_hThread;
	DWORD	m_ThreadID;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
};
