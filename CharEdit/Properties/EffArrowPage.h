#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharArrow.h"
#include "EffCharPage.h"

// CEffArrowPage 대화 상자입니다.

class CEffArrowPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffArrowPage)

protected:
	EFFCHAR_PROPERTY_ARROW		m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffArrowPage();
	virtual ~CEffArrowPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_ARROW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonColor();
	CComboBox m_pComboBlend;
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedCheckSec();
	afx_msg void OnBnClickedCheckSize();
	afx_msg void OnBnClickedCheckEffect();
	afx_msg void OnBnClickedButtonSelect2();
	afx_msg void OnBnClickedButtonSelect3();
};
