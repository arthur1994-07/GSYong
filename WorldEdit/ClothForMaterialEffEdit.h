#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectClothForMaterial.h"
#include "afxwin.h"

// CClothForMaterialEffEdit ��ȭ �����Դϴ�.

class CClothForMaterialEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CClothForMaterialEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectClothForMaterial	*m_pClothEff;
	
	CLOTH_FOR_MATERIAL_PROPERTY			m_ClothProperty;
	CLOTH_FOR_MATERIAL_PROPERTY			m_OldClothProperty;

	D3DXMATRIX				m_matLocal;
	D3DXMATRIX				m_OldmatLocal;

protected:
	BOOL m_bInitDlg;

public:
	void UpdatePage();

public:
	void SetInstance ( DxEffectClothForMaterial *pClothEff );

public:
	CClothForMaterialEffEdit();
	virtual ~CClothForMaterialEffEdit();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_CLOTH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
