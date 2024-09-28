#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectShadow.h"
#include "afxwin.h"

// CShadowEffEdit 대화 상자입니다.

class CShadowEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CShadowEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectShadow		*m_pShadowEff;
	
	SHADOW_PROPERTY		m_ShadowProperty;
	SHADOW_PROPERTY		m_OldShadowProperty;

	D3DXMATRIX				m_matLocal;
	D3DXMATRIX				m_OldmatLocal;

public:
	void SetInstance ( DxEffectShadow *pShadowEff );

public:
	CShadowEffEdit();
	virtual ~CShadowEffEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SHADOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonMakess();
	afx_msg void OnBnClickedCheckDirect();
	afx_msg void OnBnClickedCheckPoint();
	afx_msg void OnBnClickedButtonTextureFilter();
	afx_msg void OnBnClickedCheckUsess();
	afx_msg void OnBnClickedButtonTexDaytime();
	afx_msg void OnBnClickedButtonTexNight();
	afx_msg void OnBnClickedButtonCreateUv();
	CComboBox m_pComboSSSize;
	afx_msg void OnCbnSelchangeComboSsSize();
};
