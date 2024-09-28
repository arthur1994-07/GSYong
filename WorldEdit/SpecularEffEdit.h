#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectSpecular.h"

// CShadowEffEdit ��ȭ �����Դϴ�.

class CSpecularEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CSpecularEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectSpecular		*m_pEff;
	
	SPECULAR_PROPERTY		m_Property;
	SPECULAR_PROPERTY		m_OldProperty;

	D3DXMATRIX				m_matLocal;
	D3DXMATRIX				m_OldmatLocal;

public:
	void SetInstance ( DxEffectSpecular *pEff );

public:
	CSpecularEffEdit();
	virtual ~CSpecularEffEdit();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SPECULAR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedRadioAll();
	afx_msg void OnBnClickedRadioAlpha();
};
