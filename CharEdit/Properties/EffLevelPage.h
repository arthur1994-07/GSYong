#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharLevel.h"
#include "EffCharPage.h"

// CEffLevelPage 대화 상자입니다.

class CEffLevelPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffLevelPage)

protected:
	EFFCHAR_PROPERTY_LEVEL		m_Property;

protected:
	void CheckEnableWindow ( int nCurSel );

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp=NULL );

public:
	CEffLevelPage();
	virtual ~CEffLevelPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_LEVEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CComboBox m_ComboColorOP;
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedCheckSrctex();
	CComboBox m_ComboMatrials;
	afx_msg void OnCbnSelchangeComboMatrial();
};
