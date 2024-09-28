#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectNature.h"

// CNatureEffEdit ��ȭ �����Դϴ�.

class CNatureEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CNatureEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectNature			*m_pNatureEff;
	
	NATURE_PROPERTY			m_NatureProperty;
	NATURE_PROPERTY			m_OldNatureProperty;

	D3DXMATRIX				m_matLocal;
	D3DXMATRIX				m_OldmatLocal;

public:
	void SetInstance ( DxEffectNature *pNatureEff );

public:
	CNatureEffEdit();
	virtual ~CNatureEffEdit();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_NATURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
};
