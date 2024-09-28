#pragma once


#include "../EngineLib/DxEffect/Frame/DxEffectRiver.h"
#include "../MfcExLib/ExLibs/Separator.h"

// RiverEffEdit ��ȭ �����Դϴ�.
class CsheetWithTab;

class CRiverEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CRiverEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectRiver			*m_pEff;
	
	RIVER_PROPERTY			m_Property;
	RIVER_PROPERTY			m_OldProperty;

	D3DXMATRIX				m_matLocal;
	D3DXMATRIX				m_OldmatLocal;

public:
	void SetInstance ( DxEffectRiver *pEff );

public:
	CRiverEffEdit();
	virtual ~CRiverEffEdit();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_RIVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedCheckDark();
	afx_msg void OnBnClickedCheckFlash();
	afx_msg void OnBnClickedCheckReflect();
	afx_msg void OnBnClickedButtonDarkDiffuse();
	afx_msg void OnBnClickedButtonFlashDiffuse();
	afx_msg void OnBnClickedButtonDarkTex();
	afx_msg void OnBnClickedButtonFlashTex();
	afx_msg void OnBnClickedRadioOceane();
	afx_msg void OnBnClickedRadioTransLight();
	afx_msg void OnBnClickedCheckSee();
	afx_msg void OnBnClickedCheckSameHeight();
};
