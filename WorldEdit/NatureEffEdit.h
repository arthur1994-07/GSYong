#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectNature.h"

// CNatureEffEdit 대화 상자입니다.

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

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_NATURE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
};
