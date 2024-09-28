#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class CsheetWithTabTS;
struct EFFCHAR_PROPERTY_TS_OVERLAY;
// CEffTS_Overlay ��ȭ �����Դϴ�.

class CEffTS_Overlay : public CPropertyPage
{
	DECLARE_DYNAMIC(CEffTS_Overlay)

protected:
	CsheetWithTabTS	*m_pSheetTab;
	EFFCHAR_PROPERTY_TS_OVERLAY m_PropOverlay;

public:
	void SetSheetTab ( CsheetWithTabTS *pSheetTab ) { m_pSheetTab = pSheetTab; }
	EFFCHAR_PROPERTY_TS_OVERLAY GetProperty() { return m_PropOverlay; }
	void SetProperty( EFFCHAR_PROPERTY_TS_OVERLAY Property ) ;
	void Apply();

public:
	CEffTS_Overlay();
	virtual ~CEffTS_Overlay();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFFTS_OVERLAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedButtonColor1();
	afx_msg void OnBnClickedButtonColor2();
	CSliderCtrl m_slider_SpecLV;
	afx_msg void OnNMCustomdrawSliderSpeclevel(NMHDR *pNMHDR, LRESULT *pResult);
	CComboBox m_combo_Material;
};
