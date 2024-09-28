#pragma once


#include "../EngineLib/DxEffect/Frame/DxEffectDot3.h"

// CDot3EffEdit ��ȭ �����Դϴ�.
class CsheetWithTab;

class CDot3EffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CDot3EffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectDot3	*m_pDot3Eff;
	
	DOT3_PROPERTY	m_Dot3Property;
	DOT3_PROPERTY	m_OldDot3Property;

	D3DXMATRIX		m_matLocal;
	D3DXMATRIX		m_OldmatLocal;

public:
	void SetInstance ( DxEffectDot3 *pDot3Eff );

public:
	CDot3EffEdit();
	virtual ~CDot3EffEdit();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_DOT3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedRadioDot3();
	afx_msg void OnBnClickedRadioDetail();
};
