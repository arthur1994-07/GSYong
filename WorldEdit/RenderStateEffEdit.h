#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectRenderState.h"
#include "afxwin.h"

// CRenderStateEffEdit 대화 상자입니다.

class CRenderStateEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CRenderStateEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectRenderState		*m_pRenderStateEff;
	
	RENDERSTATE_PROPERTY	m_RenderStateProperty;
	RENDERSTATE_PROPERTY	m_OldRenderStateProperty;

	D3DXMATRIX				m_matLocal;
	D3DXMATRIX				m_OldmatLocal;

public:
	void SetInstance ( DxEffectRenderState *pRenderStateEff );

public:
	CRenderStateEffEdit();
	virtual ~CRenderStateEffEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_RENDERSTATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	CCJFlatComboBox m_ComboFillMode;
	CCJFlatComboBox m_ComboCullMode;
	CCJFlatComboBox m_ComboLighting;
	CCJFlatComboBox m_ComboZnEable;
	CCJFlatComboBox m_ComboAlphaBlend;
	CCJFlatComboBox m_ComboSrcBlend;
	CCJFlatComboBox m_ComboDestBlend;
	CCJFlatComboBox m_ComboColorOP;
	CCJFlatComboBox m_ComboColorARG1;
	CCJFlatComboBox m_ComboColorARG2;
	CCJFlatComboBox m_ComboAlphaOP;
	CCJFlatComboBox m_ComboAlphaARG1;
	CCJFlatComboBox m_ComboAlphaARG2;
	CCJFlatComboBox m_ComboType;
	afx_msg void OnCbnSelchangeComboType();
	afx_msg void OnBnClickedCheckLight();
	afx_msg void OnBnClickedButtonLight();
};