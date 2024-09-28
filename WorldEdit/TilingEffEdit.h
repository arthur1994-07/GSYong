#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectCommandTiling.h"
#include "../EngineLib/DxEffect/Frame/DxEffectTiling.h"	
#include "afxwin.h"
#include "afxcmn.h"

class CsheetWithTab;

class CTilingEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CTilingEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectTiling	*m_pTilingEff;
	
	TILING_PROPERTY	m_sProperty;
	TILING_PROPERTY	m_sProperty_Old;

	D3DXMATRIX		m_matLocal;
	D3DXMATRIX		m_OldmatLocal;

    BOOL                    m_bTileChange;
    CommandTilingManager    m_sCommand;

    // 이전 정보
    DWORD			            m_dwPointEX;
	DxEffectTiling::MATERIALMAP	m_matMaterials_BackUp;
    //DxEffectTiling::POINTEX*	m_pPointEX_BackUp;

public:
	void SetInstance ( DxEffectTiling *pTiling );
	void UpdatePage ();
    void UpdateKeyBoardMouse();

private:
	void BackUpTileMaterial();

public:
	CTilingEffEdit();
	virtual ~CTilingEffEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_TILING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedCheckSelectMaterial();
	afx_msg void OnBnClickedButtonGetmaterialD();
	afx_msg void OnBnClickedButtonGetmaterialF();
	afx_msg void OnBnClickedButtonChange();
	afx_msg void OnBnClickedButtonDelete();
	CListBox m_ListMaterials;
	afx_msg void OnBnClickedButtonTileDraw();
	afx_msg void OnLbnSelchangeListMaterials();
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedButtonColorRandom();
	afx_msg void OnEnChangeEditAlpha();
	afx_msg void OnBnClickedCheckTexDraw();
	afx_msg void OnBnClickedCheckColorDraw();
	afx_msg void OnBnClickedButtonTileDelete();
	afx_msg void OnBnClickedButtonColorDelete();
	CSliderCtrl m_slider_Priority;
	afx_msg void OnNMCustomdrawSliderPriority(NMHDR *pNMHDR, LRESULT *pResult);
    virtual BOOL OnKillActive();
	afx_msg void OnBnClickedButtonMaterialUp();
	afx_msg void OnBnClickedButtonMaterialDown();
};
