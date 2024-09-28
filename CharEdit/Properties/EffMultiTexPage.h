#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharMultiTex.h"
#include "EffCharPage.h"

// CEffMultiTexPage 대화 상자입니다.

class CEffMultiTexPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffMultiTexPage)

protected:
	EFFCHAR_PROPERTY_MULTITEX	m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

protected:
	void Check_All_Select ();

public:
	CEffMultiTexPage();
	virtual ~CEffMultiTexPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_MULTITEX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTex1();
	afx_msg void OnBnClickedButtonTex2();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CComboBox m_ComboColorOP;
	afx_msg void OnBnClickedRadioTex1();
	afx_msg void OnBnClickedRadioTex2();
	afx_msg void OnBnClickedButtonMoveTex1();
	afx_msg void OnBnClickedButtonMoveTex2();
	afx_msg void OnBnClickedButtonColor1();
	afx_msg void OnBnClickedButtonColor2();
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedCheckViewTex();
	afx_msg void OnBnClickedButtonTexRe1();
	afx_msg void OnBnClickedButtonTexRe2();
	afx_msg void OnBnClickedCheckSrctex();
	CComboBox m_ComboMatrials;
	afx_msg void OnCbnSelchangeComboMatrial();
	afx_msg void OnBnClickedRadioAll();
	afx_msg void OnBnClickedRadioSelect();
};
