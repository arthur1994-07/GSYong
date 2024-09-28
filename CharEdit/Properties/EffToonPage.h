#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharToon.h"
#include "EffCharPage.h"

// CEffToonPage ��ȭ �����Դϴ�.

class CEffToonPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffToonPage)

protected:
	EFFCHAR_PROPERTY_TOON	m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffToonPage();
	virtual ~CEffToonPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_TOON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedCheckObliqueLine();
	afx_msg void OnBnClickedButtonObliqueTex();
};
