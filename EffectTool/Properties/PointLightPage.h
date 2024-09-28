#pragma once

#include "../resource.h"

// CPointLightPage ��ȭ �����Դϴ�.
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

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_POINTLIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonPower();
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedButtonScale();
};
