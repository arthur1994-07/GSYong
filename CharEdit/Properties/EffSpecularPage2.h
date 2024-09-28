#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharSpecular2.h"
#include "EffCharPage.h"

// CEffSpecularPage2 대화 상자입니다.

class CEffSpecularPage2 : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffSpecularPage2)

protected:
	EFFCHAR_PROPERTY_SPECULAR2	m_Property;

protected:
	void CheckEnableTexWindow ( int nCurSel );
	void CubeEnableWindow( BOOL bUse );

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp=NULL );

public:
	CEffSpecularPage2();
	virtual ~CEffSpecularPage2();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_SPECULAR2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
	afx_msg void OnCbnSetfocusComboMatrial();
	afx_msg void OnBnClickedRadioVertex();
	afx_msg void OnBnClickedRadioCubemap();
};
