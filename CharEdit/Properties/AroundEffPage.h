#pragma once

#include "../EngineLib/DxEffect/Char/DxCharAroundEff.h"
#include "EffCharPage.h"

// CAroundEffPage ��ȭ �����Դϴ�.

class CAroundEffPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CAroundEffPage)


protected:
	DxCharAroundEff				 *m_pEffChar;
	EFFCHAR_PROPERTY_CHAR_AROUND m_Property; 

public:
	void SetProperty( EFFCHAR_PROPERTY *pProp = NULL );
	void UpdateEffectLIST();

public:
	CAroundEffPage();
	virtual ~CAroundEffPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_AROUND_EFFECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonGet1();
	afx_msg void OnBnClickedButtonInsert();
	CListCtrl m_AroundEffList;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedButtonTextureFind();
	afx_msg void OnBnClickedButtonTextureFind2();
	CSliderCtrl m_TexSpeedSlider1;
	CSliderCtrl m_TexSpeedSlider2;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonColor3();
};
