#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharBlur.h"
#include "EffCharPage.h"

// CEffBlurPage ��ȭ �����Դϴ�.

class CEffBlurPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffBlurPage)

protected:
	EFFCHAR_PROPERTY_BLUR		m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffBlurPage();
	virtual ~CEffBlurPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_BLUR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CComboBox m_ComboVertsUP;
	CComboBox m_ComboVertsDOWN;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonDiffuseUp();
	afx_msg void OnBnClickedButtonDiffuseDown();
	CComboBox m_pComboBlend;
	afx_msg void OnCbnSelchangeComboBlend();
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedCheckRefract();
	afx_msg void OnBnClickedCheckAllAction();
};
