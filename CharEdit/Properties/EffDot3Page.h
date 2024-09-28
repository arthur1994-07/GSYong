#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharDot3.h"
#include "EffCharPage.h"

// CEffDot3Page 대화 상자입니다.

class CEffDot3Page : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffDot3Page)

protected:
	EFFCHAR_PROPERTY_DOT3		m_Property;

protected:
	void ResetCheck ();

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffDot3Page();
	virtual ~CEffDot3Page();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_DOT3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCheckSrctex1();
	afx_msg void OnBnClickedCheckSrctex2();
	afx_msg void OnBnClickedCheckSrctex3();
	afx_msg void OnBnClickedButtonTexture1();
	afx_msg void OnBnClickedButtonTexture2();
	afx_msg void OnBnClickedButtonTexture3();
	afx_msg void OnBnClickedButtonTextureBump1();
	afx_msg void OnBnClickedButtonTextureBump2();
	afx_msg void OnBnClickedButtonTextureBump3();
	CComboBox m_ComboOption1;
	CComboBox m_ComboOption2;
	CComboBox m_ComboOption3;
	afx_msg void OnBnClickedButtonReflectTex();
};
