#pragma once


// CRange ��ȭ �����Դϴ�.

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
	CRange(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CRange();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_RANGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCheckRandomDirect();
};
