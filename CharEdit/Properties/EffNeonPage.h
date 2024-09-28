#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharNeon.h"
#include "EffCharPage.h"

// CEffNeonPage 대화 상자입니다.

class CEffNeonPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffNeonPage)

protected:
	EFFCHAR_PROPERTY_NEON		m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

protected:
	void Check_All_Select ();
	void Check_Src_User_Tex ();

public:
	CEffNeonPage();
	virtual ~CEffNeonPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_NEON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCheckSrctex();
	CComboBox m_ComboMatrials;
	afx_msg void OnCbnSelchangeComboMatrial();
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedRadioAll();
	afx_msg void OnBnClickedRadioSelect();
	afx_msg void OnBnClickedRadioOri();
	afx_msg void OnBnClickedRadioUser();
	afx_msg void OnBnClickedRadioGeneral();
	afx_msg void OnBnClickedRadioBurn();
};
