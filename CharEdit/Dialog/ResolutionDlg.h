#pragma once

#include "../resource.h"

// CResolutionDlg 대화 상자입니다.

class CResolutionDlg : public CDialog
{
	DECLARE_DYNAMIC(CResolutionDlg)

public:
	float m_fAddTime;
	float m_fColorPower;
	DWORD m_dwResolutionINDEX;
	DWORD m_dwAA_INDEX;
	DWORD m_dwFPS;
	bool m_bOnlyCurrentFrame;

public:
	CResolutionDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CResolutionDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_RESOLUTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadioResolution1();
	afx_msg void OnBnClickedRadioResolution2();
	afx_msg void OnBnClickedRadioResolution3();
	afx_msg void OnBnClickedRadioResolution4();
	afx_msg void OnBnClickedRadioResolution5();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRadioAa1x();
	afx_msg void OnBnClickedRadioAa2x();
	afx_msg void OnBnClickedRadioAa4x();
	afx_msg void OnEnChangeCustomScnWidth();
	afx_msg void OnEnChangeCustomScnHeight();
};
