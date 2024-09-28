#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharArrow.h"
#include "EffCharPage.h"

// CEffArrowPage ��ȭ �����Դϴ�.

class CEffArrowPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffArrowPage)

protected:
	EFFCHAR_PROPERTY_ARROW		m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffArrowPage();
	virtual ~CEffArrowPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_ARROW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonColor();
	CComboBox m_pComboBlend;
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedCheckSec();
	afx_msg void OnBnClickedCheckSize();
	afx_msg void OnBnClickedCheckEffect();
	afx_msg void OnBnClickedButtonSelect2();
	afx_msg void OnBnClickedButtonSelect3();
};
