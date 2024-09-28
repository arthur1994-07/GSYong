#pragma once


// CRange 대화 상자입니다.

class CRange : public CDialog
{
	DECLARE_DYNAMIC(CRange)

public:
	BOOL	m_bRandomDirect;
	float	m_fPlayTimeS;
	float	m_fPlayTimeE;
	float	m_fSleepTimeS;
	float	m_fSleepTimeE;

public:
	CRange(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CRange();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_RANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheckRandomDirect();
};
