#pragma once

#include "../EngineLib/DxEffect/Frame/DxEffectGrass.h"
#include "afxwin.h"

// GrassEffEdit 대화 상자입니다.

class CGrassEffEdit : public CPropertyPage
{
	DECLARE_DYNAMIC(CGrassEffEdit)

protected:
	CsheetWithTab	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

private:
    enum BLUSH_MODE
    {
        BLUSH_INIT          = 0,
        BLUSH_PART_DELETE   = 1,
    };

protected:
	DxEffectGrass	*m_pGrassEff;
	
	GRASS_PROPERTY	m_Property;
	GRASS_PROPERTY	m_OldProperty;

	D3DXMATRIX		m_matLocal;
	D3DXMATRIX		m_OldmatLocal;

   BLUSH_MODE       m_emBlushMode;

public:
	void SetInstance ( DxEffectGrass *pGrassEff );
    void UpdateKeyBoardMouse();

private:
    void SetBlushDlg();

public:
	CGrassEffEdit();
	virtual ~CGrassEffEdit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_GRASS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonPointselect();
	afx_msg void OnBnClickedButtonPointdelete();
	afx_msg void OnLbnSelchangeListPoint();
	CListBox m_ListPoint;
	afx_msg void OnBnClickedButtonPointreset();
	afx_msg void OnBnClickedButtonDiffuse();
    afx_msg void OnBnClickedButtonPartDelete();
};
