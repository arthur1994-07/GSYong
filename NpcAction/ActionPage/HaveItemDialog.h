#pragma once


// CHaveItemDialog ��ȭ �����Դϴ�.

class CHaveItemDialog : public CDialog
{
	DECLARE_DYNAMIC(CHaveItemDialog)

public:
	CHaveItemDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CHaveItemDialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_HAVEITEMDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

private:
	DWORD	m_dwItemID;
	int		m_nNum;
	BOOL	m_bConfirm;

public:
	DWORD	GetItemID ();
	DWORD	GetNum() { return m_nNum; }

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonFind();
	virtual BOOL OnInitDialog();
};
