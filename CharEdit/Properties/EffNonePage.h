#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharNone.h"
#include "EffCharPage.h"

// CEffNonePage ��ȭ �����Դϴ�.

class CEffNonePage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffNonePage)

protected:
	EFFCHAR_PROPERTY_NONE	m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp=NULL );

public:
	CEffNonePage();
	virtual ~CEffNonePage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_NONE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
