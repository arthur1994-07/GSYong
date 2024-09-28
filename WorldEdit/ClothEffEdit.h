#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectCloth.h"
#include "afxwin.h"

// CClothEffEdit 대화 상자입니다.

class CClothEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CClothEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectCloth			*m_pClothEff;
	
	CLOTH_PROPERTY			m_ClothProperty;
	CLOTH_PROPERTY			m_OldClothProperty;

	D3DXMATRIX				m_matLocal;
	D3DXMATRIX				m_OldmatLocal;

protected:
	BOOL m_bInitDlg;

public:
	void UpdatePage();

public:
	void SetInstance ( DxEffectCloth *pClothEff );

public:
	CClothEffEdit();
	virtual ~CClothEffEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_CLOTH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedRadioStop();
	afx_msg void OnBnClickedRadioPlay();
	afx_msg void OnBnClickedButtonPointselect();
	CListBox m_ListPoint;
	afx_msg void OnBnClickedButtonPointdelete();
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButtonWinddir();
	afx_msg void OnBnClickedCheckEachRender();
};
