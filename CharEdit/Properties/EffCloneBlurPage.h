#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharCloneBlur.h"
#include "EffCharPage.h"

// CEffCloneBlurPage 대화 상자입니다.

class CEffCloneBlurPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffCloneBlurPage)

protected:
	EFFCHAR_PROPERTY_CLONEBLUR	m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffCloneBlurPage();
	virtual ~CEffCloneBlurPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_CLONEBLUR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeComboBlend();
	afx_msg void OnBnClickedButtonDiffuseUp();
	afx_msg void OnBnClickedButtonDiffuseDown();
	afx_msg void OnBnClickedButtonTexture();
	CComboBox m_ComboVertsUP;
	CComboBox m_ComboVertsDOWN;
	CComboBox m_pComboBlend;
	afx_msg void OnBnClickedButtonApply();
};
