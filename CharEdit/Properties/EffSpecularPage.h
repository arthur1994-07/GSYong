#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharSpecular.h"
#include "EffCharPage.h"

// CEffSpecularPage ��ȭ �����Դϴ�.

class CEffSpecularPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffSpecularPage)

protected:
	EFFCHAR_PROPERTY_SPECULAR	m_Property;

protected:
	void ResetCheck ();

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp=NULL );

public:
	CEffSpecularPage();
	virtual ~CEffSpecularPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_SPECULAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCheckSrctex1();
	afx_msg void OnBnClickedCheckSrctex2();
	afx_msg void OnBnClickedCheckSrctex3();
};
