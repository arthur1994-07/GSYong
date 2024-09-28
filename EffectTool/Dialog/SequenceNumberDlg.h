#pragma once


// CSequenceNumberDlg ��ȭ �����Դϴ�.

class CSequenceNumberDlg : public CDialog
{
	DECLARE_DYNAMIC(CSequenceNumberDlg)

public:
	int m_nTexSize;
	int m_nNumber;
	BOOL m_bRepeat;

protected:
	void ChangeNumber();

public:
	CSequenceNumberDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSequenceNumberDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SEQUENCE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadioNumber16();
	afx_msg void OnBnClickedRadioNumber32();
	afx_msg void OnBnClickedRadioNumber64();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRadioTexsize128();
	afx_msg void OnBnClickedRadioTexsize256();
	afx_msg void OnBnClickedCheckRepeat();
};
