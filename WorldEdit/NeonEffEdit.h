#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectNeon.h"
#include "afxwin.h"

// CNeonEffEdit 대화 상자입니다.
struct DXMATERIAL_NEON;

class CNeonEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CNeonEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectNeon			*m_pEff;
	
	NEON_PROPERTY			m_Property;
	NEON_PROPERTY			m_OldProperty;

	D3DXMATRIX				m_matLocal;
	D3DXMATRIX				m_OldmatLocal;

protected:
	void CheckEnableWindow ( int nCurSel );

public:
	void SetInstance ( DxEffectNeon *pEff );

public:
	CNeonEffEdit();
	virtual ~CNeonEffEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_NEON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedButtonDiffuse();
	afx_msg void OnBnClickedCheckSrctex();
	CComboBox m_ComboMatrial;
	afx_msg void OnCbnSelchangeComboMatrial();
};
