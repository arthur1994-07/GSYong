#pragma once
#include "afxwin.h"
#include "../resource.h"

class CsheetWithTab;

// CLevelMainPage 대화 상자입니다.
class CLevelMainPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CLevelMainPage)

protected:
	CsheetWithTab*	m_pSheetTab;
	BOOL			m_bInit;

public:
	void	SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	void UpdatePage ();

public:
	CLevelMainPage();
	virtual ~CLevelMainPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LEVELMAINPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonWldfilename();
	CComboBox m_comboDiv;
	afx_msg void OnBnClickedButtonCreatetree();
	afx_msg void OnCbnSelchangeComboDiv();
	afx_msg void OnBnClickedCheckRendquad();
	afx_msg void OnBnClickedCheckCrowsch();
	afx_msg void OnBnClickedButtonNaviFilename();
	afx_msg void OnBnClickedButtonNaviFilenameClear();
};
