#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharCloneBlur.h"
#include "EffCharPage.h"

// CEffCloneBlurPage ��ȭ �����Դϴ�.

class CEffCloneBlurPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffCloneBlurPage)

protected:
	EFFCHAR_PROPERTY_CLONEBLUR	m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffCloneBlurPage();
	virtual ~CEffCloneBlurPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_CLONEBLUR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnCbnSelchangeComboBlend();
	afx_msg void OnBnClickedButtonDiffuseUp();
	afx_msg void OnBnClickedButtonDiffuseDown();
	afx_msg void OnBnClickedButtonTexture();
	CComboBox m_ComboVertsUP;
	CComboBox m_ComboVertsDOWN;
	CComboBox m_pComboBlend;
	afx_msg void OnBnClickedButtonApply();
};
