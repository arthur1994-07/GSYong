#pragma once

#include "afxwin.h"
#include "../resource.h"
#include "../MfcExLib/ExLibs/StColor.h"

// CBlurSysPage ��ȭ �����Դϴ�.
class CsheetWithTab;
struct BLURSYS_PROPERTY;

class CBlurSysPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CBlurSysPage)

protected:
	CStColor	m_Color;

protected:
	CsheetWithTab*			m_pSheetTab;
	BLURSYS_PROPERTY*		m_pProperty;

public:
	void SetProperty ( EFF_PROPERTY* pProp );
	void SetSheetTab ( CsheetWithTab *pSheetTab )	{ m_pSheetTab = pSheetTab; }

public:
	CBlurSysPage();
	virtual ~CBlurSysPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_BLURSYS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedButtonColorS();
	afx_msg void OnBnClickedButtoncolorE();
	afx_msg void OnBnClickedButtonScale();
	afx_msg void OnBnClickedButtonAlpha();
	CComboBox m_pComboBlend;
	afx_msg void OnBnClickedRadioAbsolute();
	afx_msg void OnBnClickedRadioAbsoluteno();
	afx_msg void OnBnClickedCheckLoopReset();
};
