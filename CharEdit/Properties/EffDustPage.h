#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharDust.h"
#include "EffCharPage.h"

// CEffDustPage 대화 상자입니다.

class CEffDustPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffDustPage)

protected:
	EFFCHAR_PROPERTY_DUST	m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffDustPage();
	virtual ~CEffDustPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_DUST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonReflectTex();
};
