#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectRainDrop.h"

// CRainPointEffEdit 대화 상자입니다.

class CRainPointEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CRainPointEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }
	void ChangeEnableWindow ( bool bValue );

protected:
	DxEffectRainPoint		*m_pRainPointEff;
	
	RAINPOINT_PROPERTY		m_RainPointProperty;
	RAINPOINT_PROPERTY		m_OldRainPointProperty;

	D3DXMATRIX				m_matLocal;
	D3DXMATRIX				m_OldmatLocal;

public:
	void SetInstance ( DxEffectRainPoint *pRainPointEff );

public:
	CRainPointEffEdit();
	virtual ~CRainPointEffEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_RAINPOINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonTexture2();
	afx_msg void OnBnClickedButtonTexture4();
	afx_msg void OnBnClickedButtonTexture5();
	//afx_msg void OnBnClickedButtonSandGTexture();
	//afx_msg void OnBnClickedButtonRainGTexture();
	//afx_msg void OnBnClickedButtonSnowGTexture();
	//afx_msg void OnBnClickedButtonLeavesGTexture();
	afx_msg void OnBnClickedButtonRainTexture();
	afx_msg void OnBnClickedCheckNeweffect();
};
