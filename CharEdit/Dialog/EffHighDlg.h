#pragma once
#include "afxwin.h"


// CEffHighDlg 대화 상자입니다.

class CEffHighDlg : public CDialog
{
	DECLARE_DYNAMIC(CEffHighDlg)

public:
	BOOL	m_bMType;
	BOOL	m_bSType;
	BOOL	m_bKeyTime;

	DWORD	m_dwMType;
	DWORD	m_dwSType;

	DWORD	m_dwStartKey;
	DWORD	m_dwEndKey;

public:
	CEffHighDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CEffHighDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_HIGH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCheckMtype();
	afx_msg void OnBnClickedCheckStype();
	afx_msg void OnCbnSelchangeComboMtype();
	afx_msg void OnCbnSelchangeComboStype();
	afx_msg void OnBnClickedCheckKeytime();
	afx_msg void OnEnChangeEditStartkey();
	afx_msg void OnEnChangeEditEndkey();
	virtual BOOL OnInitDialog();
	CComboBox m_pComboMType;
	CComboBox m_pComboSType;
};