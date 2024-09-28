#pragma once

#include "../EngineLib/DxEffect/Char/DxEffCharWorldBattleFlag.h"
#include "EffCharPage.h"

// CEffWorldBattleFlagPage ��ȭ �����Դϴ�.

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

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DIALOG_EFF_WB_FLAG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonApply();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	CComboBox m_ComboMatrials;
	afx_msg void OnCbnSelchangeComboMatrial();
	afx_msg void OnBnClickedCheckSrctex();
};
