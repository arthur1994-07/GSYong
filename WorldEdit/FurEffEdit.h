#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectFur.h"

// CFurEffEdit 대화 상자입니다.

class CFurEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CFurEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectFur			*m_pFurEff;
	
	FUR_PROPERTY		m_FurProperty;
	FUR_PROPERTY		m_OldFurProperty;

	D3DXMATRIX			m_matLocal;
	D3DXMATRIX			m_OldmatLocal;

public:
	void SetInstance ( DxEffectFur *pFurEff );

public:
	CFurEffEdit();
	virtual ~CFurEffEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_FUR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
};
