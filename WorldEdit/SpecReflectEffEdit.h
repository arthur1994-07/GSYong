#pragma once


#include "../EngineLib/DxEffect/Frame/DxEffectSpecReflect.h"
#include "../MfcExLib/ExLibs/Separator.h"
#include "afxwin.h"

// Specular2EffEdit ��ȭ �����Դϴ�.
struct DXMATERIAL_SPECREFLECT;

class CsheetWithTab;

class CSpecReflectEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CSpecReflectEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectSpecReflect			*m_pEff;
	
	SPECREFLECT_PROPERTY		m_Property;
	SPECREFLECT_PROPERTY		m_OldProperty;

	D3DXMATRIX					m_matLocal;
	D3DXMATRIX					m_OldmatLocal;

protected:
	void CheckEnableWindow ( int nCurSel );

public:
	void SetInstance ( DxEffectSpecReflect *pEff );

public:
	CSpecReflectEffEdit();
	virtual ~CSpecReflectEffEdit();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SPEC_REFLECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedButtonReflectTex();
	CComboBox m_ComboColorOP;
	afx_msg void OnBnClickedCheckSrctex();
	CComboBox m_ComboMatrial;
	afx_msg void OnCbnSelchangeComboMatrial();
};
