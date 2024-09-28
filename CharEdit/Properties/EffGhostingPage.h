#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharGhosting.h"
#include "EffCharPage.h"

// CEffGhostingPage 대화 상자입니다.

class CEffGhostingPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffGhostingPage)

protected:
	EFFCHAR_PROPERTY_GHOSTING	m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffGhostingPage();
	virtual ~CEffGhostingPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_GHOSTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonColor();
	CComboBox m_comboMType;
	CComboBox m_comboSType;
	afx_msg void OnCbnSelchangeComboMType();
	afx_msg void OnCbnSelchangeComboSType();
};
