#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharNoAlpha.h"
#include "EffCharPage.h"

// CEffNoAlphaPage 대화 상자입니다.

class CEffNoAlphaPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffNoAlphaPage)

protected:
	EFFCHAR_PROPERTY_NOALPHA	m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffNoAlphaPage();
	virtual ~CEffNoAlphaPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_NOALPHA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
