#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharNormal.h"
#include "EffCharPage.h"

// CEffNormalMapPage 대화 상자입니다.

class CEffNormalMapPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffNormalMapPage)

private:
	EFFCHAR_PROPERTY_NORMAL	m_Property;

public:
	void SetProperty( EFFCHAR_PROPERTY *pProp=NULL );

public:
	CEffNormalMapPage();
	virtual ~CEffNormalMapPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_NORMALMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonTextureNormal();
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedButtonTextureSpecular();
	afx_msg void OnBnClickedCheckSpecular();
	afx_msg void OnBnClickedButtonColor3();
	afx_msg void OnBnClickedButtonColor2();
	afx_msg void OnBnClickedButtonTextureSpecular2();
	afx_msg void OnBnClickedCheckSpecular2();
	afx_msg void OnBnClickedCheckGlowtex();
	afx_msg void OnBnClickedButtonTextureGlowtex();
};
