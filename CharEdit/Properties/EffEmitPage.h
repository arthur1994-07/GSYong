#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharEmit.h"
#include "EffCharPage.h"

// CEffEmitPage ��ȭ �����Դϴ�.

class CEffEmitPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffEmitPage)

protected:
	EFFCHAR_PROPERTY_EMIT	m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffEmitPage();
	virtual ~CEffEmitPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_SPECULAR2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonTexture();
};
