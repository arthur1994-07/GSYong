#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharUserColor.h"
#include "EffCharPage.h"

// CEffUserColorPage ��ȭ �����Դϴ�.

class CEffUserColorPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffUserColorPage)

protected:
	EFFCHAR_PROPERTY_USERCOLOR	m_Property;

private:
	void CheckEnableTexWindow( int nCurSel );

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffUserColorPage();
	virtual ~CEffUserColorPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_USERCOLOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonColor();
	CComboBox m_ComboMatrials;
	afx_msg void OnCbnSelchangeComboMatrial();
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedCheckSrctex();
};
