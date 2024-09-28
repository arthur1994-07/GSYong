#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharReflection2.h"
#include "EffCharPage.h"

// CEffReflection2Page ��ȭ �����Դϴ�.

class CEffReflection2Page : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffReflection2Page)

protected:
	EFFCHAR_PROPERTY_REFLECTION2	m_Property;

protected:
	void CheckEnableWindow ( int nCurSel );

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp=NULL );

public:
	CEffReflection2Page();
	virtual ~CEffReflection2Page();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_REFLECTION2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonReflectTex();
	CComboBox m_ComboColorOP;
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedCheckSrctex();
	afx_msg void OnBnClickedButtonTexture();
	CComboBox m_ComboMatrials;
	afx_msg void OnCbnSelchangeComboMatrial();
	afx_msg void OnCbnSetfocusComboMatrial();
};
