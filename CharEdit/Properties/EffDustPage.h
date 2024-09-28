#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharDust.h"
#include "EffCharPage.h"

// CEffDustPage ��ȭ �����Դϴ�.

class CEffDustPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffDustPage)

protected:
	EFFCHAR_PROPERTY_DUST	m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffDustPage();
	virtual ~CEffDustPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_DUST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonReflectTex();
};
