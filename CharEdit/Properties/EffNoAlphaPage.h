#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharNoAlpha.h"
#include "EffCharPage.h"

// CEffNoAlphaPage ��ȭ �����Դϴ�.

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

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_NOALPHA };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
