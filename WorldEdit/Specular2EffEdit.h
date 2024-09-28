#pragma once


#include "../EngineLib/DxEffect/Frame/DxEffectSpecular2.h"
#include "../MfcExLib/ExLibs/Separator.h"
#include "afxwin.h"

// Specular2EffEdit 대화 상자입니다.
struct DXMATERIAL_SPEC2;

class CsheetWithTab;

class CSpecular2EffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CSpecular2EffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectSpecular2			*m_pEff;
	
	SPECULAR2_PROPERTY			m_Property;
	SPECULAR2_PROPERTY			m_OldProperty;

	D3DXMATRIX				m_matLocal;
	D3DXMATRIX				m_OldmatLocal;

protected:
	void CheckEnableWindow ( int nCurSel );

public:
	void SetInstance ( DxEffectSpecular2 *pEff );

public:
	CSpecular2EffEdit();
	virtual ~CSpecular2EffEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_SPECULAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonTexture();
	CComboBox m_ComboColorOP;
	afx_msg void OnBnClickedCheckSrctex();
	CComboBox m_ComboMatrial;
	afx_msg void OnCbnSelchangeComboMatrial();
};
