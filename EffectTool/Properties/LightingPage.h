#pragma once

#include "../MfcExLib/ExLibs/StColor.h"
#include "afxwin.h"
#include "../resource.h"

// CLightingPage 대화 상자입니다.
class CsheetWithTab;
struct LIGHTNING_PROPERTY;


// CLightingPage 대화 상자입니다.

class CLightingPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CLightingPage)

protected:
	CStColor	m_Color;

protected:
	CsheetWithTab*			m_pSheetTab;
	LIGHTNING_PROPERTY*		m_pProperty;

public:
	void SetProperty ( EFF_PROPERTY* pProp );
	void SetSheetTab ( CsheetWithTab *pSheetTab )	{ m_pSheetTab = pSheetTab; }

public:
	CLightingPage();
	virtual ~CLightingPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LIGHTNING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CComboBox m_pComboBlend;
	afx_msg void OnCbnSelchangeComboBlend();
	afx_msg void OnBnClickedButtonAlpha();
	afx_msg void OnBnClickedButtonColorS();
	afx_msg void OnBnClickedButtonColorE();
	afx_msg void OnBnClickedButtonEditsound();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedCheckIn();
	afx_msg void OnBnClickedCheckOut();
	afx_msg void OnBnClickedCheckRanrotate();
	afx_msg void OnBnClickedButtonTextureIn();
	afx_msg void OnBnClickedButtonTextureOut();
	afx_msg void OnBnClickedCheckStart();
	afx_msg void OnBnClickedCheckEnd();
};
