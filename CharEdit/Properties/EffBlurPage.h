#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharBlur.h"
#include "EffCharPage.h"

// CEffBlurPage 대화 상자입니다.

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

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_BLUR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
