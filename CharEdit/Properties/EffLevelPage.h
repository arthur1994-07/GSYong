#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharLevel.h"
#include "EffCharPage.h"

// CEffLevelPage ��ȭ �����Դϴ�.

class CEffLevelPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffLevelPage)

protected:
	EFFCHAR_PROPERTY_LEVEL		m_Property;

protected:
	void CheckEnableWindow ( int nCurSel );

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp=NULL );

public:
	CEffLevelPage();
	virtual ~CEffLevelPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_LEVEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CComboBox m_ComboColorOP;
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedCheckSrctex();
	CComboBox m_ComboMatrials;
	afx_msg void OnCbnSelchangeComboMatrial();
};
