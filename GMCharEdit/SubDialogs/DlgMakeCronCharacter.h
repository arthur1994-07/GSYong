#pragma once


// CDlgMakeCronCharacter 대화 상자입니다.

class CDlgMakeCronCharacter : public CDialog
{
	DECLARE_DYNAMIC(CDlgMakeCronCharacter)

public:
	CDlgMakeCronCharacter(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgMakeCronCharacter();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MAKE_CRON_CHARACTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	int m_nUserNum;
	CString m_strChaName;
	afx_msg void OnBnClickedOk();
};
