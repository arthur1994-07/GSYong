#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharOverlay.h"
#include "EffCharPage.h"
#include "../Dialog/ColorPickerDlg.h" // by luxes.

// CEffOverlayPage 대화 상자입니다.

class CEffOverlayPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffOverlayPage)

protected:
	EFFCHAR_PROPERTY_OVERLAY	m_Property;

	CColorPickerDlg*			m_pColorPicker; // by luxes.
	D3DXCOLOR					m_originColor; // by luxes.

protected: // by luxes.
	enum { CREATE_OVERLAY, MODIFY_OVERLAY };
	int	m_iState; // by luxes.
	int m_nColorIndex; // by luxes.

	enum { COLOR1 = 1, COLOR2, COLOR4, COLOR5, COLOR6 } COLOR_PAGE;

	void DrawColor( int nIndex );
	void DrawSelectColor( int nIndex );

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );
	void SetProperty ( EFFCHAR_PROPERTY_OVERLAY *pProp );

	void FrameMove();
	void SetSpinControl( int nColorHeight );

public:
	CEffOverlayPage();
	virtual ~CEffOverlayPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_OVERLAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedButtonColor1();
	afx_msg void OnBnClickedButtonColor2();
	afx_msg void OnBnClickedButtonColor4();
	afx_msg void OnBnClickedButtonColor5();
	afx_msg void OnBnClickedButtonColor6();
	afx_msg void OnNMCustomdrawSliderSpecpower(NMHDR *pNMHDR, LRESULT *pResult);
	CSliderCtrl m_slider_SpecPW;
	CComboBox m_combo_Material;
	CSliderCtrl m_slider_SpecLV;
	CSliderCtrl m_slider_AlphaRef; // by luxes.
	afx_msg void OnNMCustomdrawSliderSpeclevel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckGlowtex();
	afx_msg void OnBnClickedButtonTextureGlowtex();
	afx_msg void OnBnClickedCheckReflecttex();
	afx_msg void OnBnClickedButtonTextureReflecttex();
	afx_msg void OnBnClickedButtonTextureReflecttex2();
	afx_msg void OnBnClickedCheckReflecttex2();
	afx_msg void OnBnClickedUseSpecular();
	afx_msg void OnBnClickedCheckUseOverlay();
	afx_msg void OnCbnSelchangeComboMatrial();
	afx_msg void OnNMCustomdrawSliderAlphaRef(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckEnableChange();
};
