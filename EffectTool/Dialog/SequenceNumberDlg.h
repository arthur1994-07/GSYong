#pragma once


// CSequenceNumberDlg 대화 상자입니다.

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
	CSequenceNumberDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSequenceNumberDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SEQUENCE_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
