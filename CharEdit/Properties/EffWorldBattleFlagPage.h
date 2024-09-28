#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharWorldBattleFlag.h"
#include "EffCharPage.h"

// CEffWorldBattleFlagPage 대화 상자입니다.

class CEffWorldBattleFlagPage : public CEffCharPage
{
	DECLARE_DYNAMIC(CEffWorldBattleFlagPage)

protected:
	EFFCHAR_PROPERTY_WORLD_BATTLE_FLAG	m_Property;

public:
	void SetProperty ( EFFCHAR_PROPERTY *pProp = NULL );

public:
	CEffWorldBattleFlagPage();
	virtual ~CEffWorldBattleFlagPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_EFF_WB_FLAG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CComboBox m_ComboMatrials;
	afx_msg void OnCbnSelchangeComboMatrial();
	afx_msg void OnBnClickedCheckSrctex();
};
