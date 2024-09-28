#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharTotalShader.h"
#include "EffCharPage.h"

// CEffTotalShaderPage 대화 상자입니다.

class CEffTotalShaderPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffTotalShaderPage)

protected:
	class CsheetWithTabTS		*m_pSheetTS;

protected:
	EFFCHAR_PROPERTY_TOTALSHADER	m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

	void UpdateCheckWindow();
	void UpdateEnableWindow();

public:
	CEffTotalShaderPage();
	virtual ~CEffTotalShaderPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_TOTALSHADER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckUseeffect1();
	afx_msg void OnBnClickedCheckUseeffect2();
	afx_msg void OnBnClickedCheckUseeffect3();
	afx_msg void OnBnClickedCheckUseeffect4();
	afx_msg void OnTcnSelchangeTabEfftype(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioShadereffect1();
	afx_msg void OnBnClickedRadioShadereffect2();
	afx_msg void OnBnClickedRadioShadereffect3();
	afx_msg void OnBnClickedRadioShadereffect4();
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedButtonColor2();
	afx_msg void OnBnClickedButtonColor3();
	CSliderCtrl m_slider_SpecPW;
	afx_msg void OnNMCustomdrawSliderSpecpower(NMHDR *pNMHDR, LRESULT *pResult);
};
