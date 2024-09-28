#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectGlow.h"
#include "../MfcExLib/ExLibs/Separator.h"
#include "afxwin.h"

// CGlowEffEdit 대화 상자입니다.
struct DXUSERMATERIAL;

class CGlowEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CGlowEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectGlow			*m_pEff;
	
	GLOW_PROPERTY			m_Property;
	GLOW_PROPERTY			m_OldProperty;

	D3DXMATRIX				m_matLocal;
	D3DXMATRIX				m_OldmatLocal;

protected:
	void CheckEnableWindow ( int nCurSel );

public:
	void SetInstance ( DxEffectGlow *pEff );

public:
	CGlowEffEdit();
	virtual ~CGlowEffEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_GLOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	CComboBox m_ComboMatrial;
	afx_msg void OnCbnSelchangeComboMatrial();
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedCheckSrctex();
	afx_msg void OnBnClickedCheckColor();
	afx_msg void OnBnClickedCheckDay();
	afx_msg void OnBnClickedCheckNight();
};
