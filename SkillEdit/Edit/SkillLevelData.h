#pragma once

#include "../../RanLogic/Skill/GLSkill.h"
#include "../../EngineLib/G-Logic/GLogic.h"
#include "./TransformDlg.h"
#include "./CreatureSetDlg.h"

// CSkillLevelData 대화 상자입니다.
class	CsheetWithTab;
struct	GLSKILL;
class CSkillLevelData : public CPropertyPage
{
	DECLARE_DYNAMIC(CSkillLevelData)

public:
	CSkillLevelData( LOGFONT logfont );
	virtual ~CSkillLevelData();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SKILL_LEVEL_DATA_DIALOG };

    enum
    {
        TIMER_SKILLLEVELDATA_SAVE_STATE_ID = 13,

        SAVE_MESSAGE_APPEAR_TIME = 1000,
    };

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;

	GLSKILL		m_Item;

	BOOL		m_bDlgInit;

	CFont*		m_pFont;

	int			m_nSelectLevel;
	int			m_nSelectImpact;
	int			m_nSelectSpec;

	CListBox	m_listImpactAddon;
	CListBox	m_listSpecAddon;
	CListBox    m_listSpecOption;
	CListBox	m_listSpecCAOption;
	CListBox	m_listSpecImmune;

public:
	void	SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }
	BOOL	SetItem ( DWORD GUID );
	void	SetReadMode();
	void	UpdateSpecOptionSkillSpecial();
    void	UpdateToolTipSkillSpecial();
    void	UpdateToolTipSkillImpact();

protected:
	void	UpdateItems ();
	void	UpdateSkillLevelData();
	void	UpdateImpactAddon();
	void	UpdateSpecAddon();

	void	InverseUpdateItems ();
	void	InverseUpdateSkillLevelData();
	void	InverseUpdateImpactAddon();
	void	InverseUpdateSpecAddon();
	void	InitDefaultCtrls ();

	void	OnBnClickedRadioSpec(const DWORD dwButtonIndex);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEditSpecMid();
	afx_msg void OnEnChangeEditSpecSid();

	afx_msg void OnBnClickedCheckTarget();
	afx_msg void OnBnClickedCheckBufDel();
	afx_msg void OnBnClickedButtonSpecImmuneAdd();
	afx_msg void OnBnClickedButtonSpecImmuneDel();
	afx_msg void OnBnClickedButtonSpecCaAdd();
	afx_msg void OnBnClickedButtonSpecCaDel();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonPrev();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedSpecialButton1();
	afx_msg void OnBnClickedButtonLevelDataLeft();
	afx_msg void OnBnClickedButtonLevelDataRight();
	afx_msg void OnBnClickedButtonLevelDataBunchLeft();
	afx_msg void OnBnClickedButtonLevelDataBunchRight();
	afx_msg void OnLbnSelchangeListImpactAddonSel();
	afx_msg void OnLbnSelchangeListSpecTypeSel();
	afx_msg void OnLbnSelchangeListSpecCAOption();
	afx_msg void OnLbnSelchangeListSpecVar();
	afx_msg void OnBnClickedSpecialButton2();
	afx_msg void OnEnChangeEditSpecVar2();	
	afx_msg void OnBnClickedRadioSpec1();
	afx_msg void OnBnClickedRadioSpec2();
	afx_msg void OnBnClickedRadioSpec3();
	afx_msg void OnBnClickedRadioSpec4();
    afx_msg void OnEnUpdateEditSpecVar2();
    afx_msg void OnEnChangeEditSpecVar1();
    afx_msg void OnEnChangeEditAge();
    afx_msg void OnBnClickedCheckChk1();
    afx_msg void OnBnClickedCheckChk2();
    afx_msg void OnBnClickedCheckChk3();
    afx_msg void OnBnClickedCheckChk4();
    afx_msg void OnBnClickedCheckChk5();
    afx_msg void OnBnClickedCheckChk6();
    afx_msg void OnBnClickedCheckChk7();
    afx_msg void OnBnClickedCheckChk8();
	afx_msg void OnBnClickedCheckCADamage();
	afx_msg void OnBnClickedCheckCAAll();
    afx_msg void OnCbnSelchangeComboRemoveActionType();
    afx_msg void OnEnChangeEditImpactVar();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
