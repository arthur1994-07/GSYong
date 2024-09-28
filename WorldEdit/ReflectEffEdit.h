#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectReflect.h"
#include "afxwin.h"

// ReflectEffEdit ��ȭ �����Դϴ�.

class CReflectEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CReflectEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

protected:
	DxEffectReflect			*m_pReflectEff;
	
	REFLECT_PROPERTY		m_ReflectProperty;
	REFLECT_PROPERTY		m_OldReflectProperty;

	D3DXMATRIX				m_matLocal;
	D3DXMATRIX				m_OldmatLocal;

public:
	void SetInstance ( DxEffectReflect *pReflectEff );

public:
	CReflectEffEdit();
	virtual ~CReflectEffEdit();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_REFLECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
};
