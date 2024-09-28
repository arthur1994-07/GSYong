#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectWaterLight.h"

// CWaterLightEffEdit 대화 상자입니다.

class CWaterLightEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CWaterLightEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectWaterLight		*m_pWaterEff;
	
	WATERLIGHT_PROPERTY		m_WaterProperty;
	WATERLIGHT_PROPERTY		m_OldWaterProperty;

	D3DXMATRIX				m_matLocal;
	D3DXMATRIX				m_OldmatLocal;

public:
	void SetInstance ( DxEffectWaterLight *pWaterEff );

public:
	CWaterLightEffEdit();
	virtual ~CWaterLightEffEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_WATERLIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonTextureMove();
	afx_msg void OnBnClickedButtonTextureRotate();
	afx_msg void OnBnClickedButtonDiffuse();
	afx_msg void OnBnClickedButtonWinddir();
	virtual BOOL OnSetActive();
};