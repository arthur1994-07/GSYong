#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharGhosting.h"
#include "EffCharPage.h"

// CEffGhostingPage ��ȭ �����Դϴ�.

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

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_GHOSTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
