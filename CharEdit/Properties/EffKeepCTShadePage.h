#pragma once

#include "../EngineLib/DxEffect/EffKeep/DxEffKeepCTShade.h"

class CsheetWithTabChar;

// CEffKeepCTShadePage 대화 상자입니다.

class CEffKeepCTShadePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CEffKeepCTShadePage)

protected:
	CsheetWithTabChar	*m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTabChar *pSheetTab ) { m_pSheetTab = pSheetTab; }

private:
	DxEffKeepData*				m_pEff;
	EFFKEEP_PROPERTY_CTSHADE*	m_rProperty;
	BOOL						m_bCreateEff;

public:
	void SetInstance( DxEffKeepData* pEff );

public:
	CEffKeepCTShadePage();
	virtual ~CEffKeepCTShadePage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFFKEEP_CTSHADE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonTex();
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
