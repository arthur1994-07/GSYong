#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharNone.h"
#include "EffCharPage.h"

// CEffNonePage 대화 상자입니다.

class CEffNonePage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffNonePage)

protected:
	EFFCHAR_PROPERTY_NONE	m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp=NULL );

public:
	CEffNonePage();
	virtual ~CEffNonePage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_NONE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
