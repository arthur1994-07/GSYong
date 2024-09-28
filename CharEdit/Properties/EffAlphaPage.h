#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharAlpha.h"
#include "EffCharPage.h"

// CEffAlphaPage 대화 상자입니다.

class CEffAlphaPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffAlphaPage)

protected:
	EFFCHAR_PROPERTY_ALPHA		m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffAlphaPage();
	virtual ~CEffAlphaPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_ALPHA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCheckTranslucent();
};
