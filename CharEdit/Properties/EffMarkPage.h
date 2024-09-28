#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharMark.h"
#include "EffCharPage.h"

// CEffMarkPage 대화 상자입니다.

class CEffMarkPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffMarkPage)

protected:
	EFFCHAR_PROPERTY_MARK		m_Property;

protected:
	void ResetCheck ();

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffMarkPage();
	virtual ~CEffMarkPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_MARK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonTex1();
	afx_msg void OnBnClickedButtonTex2();
	afx_msg void OnBnClickedButtonTexRe1();
	afx_msg void OnBnClickedButtonTexRe2();
	afx_msg void OnBnClickedButtonMoveTex1();
	afx_msg void OnBnClickedButtonMoveTex2();
	afx_msg void OnBnClickedButtonColor1();
	afx_msg void OnBnClickedButtonColor2();
	afx_msg void OnBnClickedRadioTex1();
	afx_msg void OnBnClickedRadioTex2();
};
