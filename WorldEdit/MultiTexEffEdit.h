#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectMultiTex.h"
#include "afxwin.h"

// CMultiTexEffEdit 대화 상자입니다.

class CMultiTexEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CMultiTexEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectMultiTex		*m_pEff;
	
	MULTITEX_PROPERTY		m_Property;
	MULTITEX_PROPERTY		m_OldProperty;

	D3DXMATRIX				m_matLocal;
	D3DXMATRIX				m_OldmatLocal;

public:
	void SetInstance ( DxEffectMultiTex *pEff );

public:
	CMultiTexEffEdit();
	virtual ~CMultiTexEffEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_MULTITEX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedCheckBase();
	afx_msg void OnBnClickedCheckAdd01();
	afx_msg void OnBnClickedCheckAdd02();
	afx_msg void OnBnClickedCheckSequence01();
	afx_msg void OnBnClickedCheckSequence02();
	afx_msg void OnBnClickedButtonTextureAdd01();
	afx_msg void OnBnClickedButtonTextureAdd02();
	afx_msg void OnBnClickedButtonSequence01();
	afx_msg void OnBnClickedButtonSequence02();
	afx_msg void OnBnClickedButtonDiffuse();
	CComboBox m_ComboSrcBlend;
	CComboBox m_ComboDestBlend;
	afx_msg void OnBnClickedButtonAlpha();
};
