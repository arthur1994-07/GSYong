#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharSingle.h"
#include "EffCharPage.h"

class CEffSinglePage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffSinglePage)

protected:
	EFFCHAR_PROPERTY_SINGLE		m_Property;

    D3DXMATRIX	m_matWorld;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp=NULL );

    void SetMatrixAndActiveEditMatrix();
    D3DXMATRIX SetRotateMatrix( DxSkinPiece* pSKinPiece, const D3DXMATRIX& matWorld, const TCHAR* pName );

public:
	CEffSinglePage();
	virtual ~CEffSinglePage();

	// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_SINGLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
	CComboBox m_ComboTraceVerts;
	afx_msg void OnBnClickedButtonSelect();
	afx_msg void OnBnClickedButtonApply();
	CComboBox m_pComboWeather;
	afx_msg void OnBnClickedButtonHigh();
	CComboBox m_ComboVertMove;
	afx_msg void OnBnClickedRadioAll();
	afx_msg void OnBnClickedRadioAttack();
	afx_msg void OnBnClickedRadioAttackExcept();
	afx_msg void OnBnClickedCheckRepeat();
	afx_msg void OnBnClickedRadio1point();
	afx_msg void OnBnClickedRadio2point();
	afx_msg void OnBnClickedCheckRepeatMove();
	afx_msg void OnBnClickedRadioMatrix();
	afx_msg void OnBnClickedCheckAutomove();
	afx_msg void OnBnClickedCheckTimeday();
	afx_msg void OnBnClickedCheckTimenight();
};
