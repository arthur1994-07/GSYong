#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharTexDiff.h"
#include "EffCharPage.h"

// CEffTexDiffPage ��ȭ �����Դϴ�.

class CEffTexDiffPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffTexDiffPage)

protected:
	EFFCHAR_PROPERTY_TEXDIFF	m_Property;

protected:
	void CheckEnableWindow ( int nCurSel );

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp=NULL );

public:
	CEffTexDiffPage();
	virtual ~CEffTexDiffPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_TEXDIFF };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_ComboMatrials;
	afx_msg void OnCbnSelchangeComboMatrial();
	afx_msg void OnBnClickedCheckSrctex();
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedRadio1x();
	afx_msg void OnBnClickedRadio2x();
	afx_msg void OnBnClickedRadio4x();
	afx_msg void OnBnClickedCheckUpdown();
	afx_msg void OnEnChangeEditAlpha();
};
