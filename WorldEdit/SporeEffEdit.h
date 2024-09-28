#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectSpore.h"

class CsheetWithTab;

// CSporeEffEdit ��ȭ �����Դϴ�.

class CSporeEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CSporeEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectSpore			*m_pEff;
	
	SPORE_PROPERTY			m_Property;
	SPORE_PROPERTY			m_OldProperty;

	D3DXMATRIX				m_matLocal;
	D3DXMATRIX				m_OldmatLocal;

public:
	void SetInstance ( DxEffectSpore *pEff );

public:
	CSporeEffEdit();
	virtual ~CSporeEffEdit();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_SPORE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonTexture6();
	afx_msg void OnBnClickedButtonAlpha();
	afx_msg void OnBnClickedButtoncolor();
	afx_msg void OnBnClickedRadioFlat();
	afx_msg void OnBnClickedRadioSlant();
	afx_msg void OnBnClickedCheckTexrotate();
	afx_msg void OnBnClickedCheckSpore();
	afx_msg void OnBnClickedCheckFirefly();
	afx_msg void OnBnClickedButtonColor1();
	afx_msg void OnBnClickedButtonColor2();
};
