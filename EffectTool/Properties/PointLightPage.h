#pragma once

#include "../resource.h"

// CPointLightPage 대화 상자입니다.
class CsheetWithTab;
struct POINTLIGHT_PROPERTY;

class CPointLightPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CPointLightPage)

protected:
	CsheetWithTab*			m_pSheetTab;
	POINTLIGHT_PROPERTY*	m_pProperty;

public:
	void SetProperty ( EFF_PROPERTY* pProp );
	void SetSheetTab ( CsheetWithTab *pSheetTab )	{ m_pSheetTab = pSheetTab; }

public:
	CPointLightPage();
	virtual ~CPointLightPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_POINTLIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonPower();
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedButtonScale();
};
