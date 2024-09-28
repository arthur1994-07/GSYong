#pragma once

#include "../MfcExLib/ExLibs/StColor.h"
#include "../EngineLib/DxEffect/Char/DxEffCharShock.h"
#include "EffCharPage.h"

// CEffShockPage 대화 상자입니다.

class CEffShockPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffShockPage)

protected:
	CStColor	m_Color;

protected:
	EFFCHAR_PROPERTY_SHOCK			m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp=NULL );

public:
	CEffShockPage();
	virtual ~CEffShockPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_SHOCK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonColor();
};
