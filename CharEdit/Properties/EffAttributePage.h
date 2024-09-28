#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharAttribute.h"
#include "EffCharPage.h"

// CEffAttributePage ��ȭ �����Դϴ�.

class CEffAttributePage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffAttributePage)

protected:
	EFFCHAR_PROPERTY_ATTRIBUTE	m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffAttributePage();
	virtual ~CEffAttributePage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_ATTRIBUTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonDiffuse();
	afx_msg void OnBnClickedButtonTexture();
	CComboBox m_ComboVertsUP;
	CComboBox m_ComboVertsDOWN;
	CComboBox m_pComboBlend;
};
