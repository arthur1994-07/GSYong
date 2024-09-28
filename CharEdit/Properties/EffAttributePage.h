#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharAttribute.h"
#include "EffCharPage.h"

// CEffAttributePage 대화 상자입니다.

class CEffAttributePage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffAttributePage)

protected:
	EFFCHAR_PROPERTY_ATTRIBUTE	m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffAttributePage();
	virtual ~CEffAttributePage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_ATTRIBUTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonDiffuse();
	afx_msg void OnBnClickedButtonTexture();
	CComboBox m_ComboVertsUP;
	CComboBox m_ComboVertsDOWN;
	CComboBox m_pComboBlend;
};
