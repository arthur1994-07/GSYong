#pragma once


#include "../../RanLogic/Skill/GLSkill.h"
#include "../../EngineLib/G-Logic/GLogic.h"

#include "afxwin.h"

// CSkillData 대화 상자입니다.
class	CsheetWithTab;
struct	GLSKILL;
class CSkillData : public CPropertyPage
{
	DECLARE_DYNAMIC(CSkillData)

public:
	CSkillData( LOGFONT logfont );
	virtual ~CSkillData();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SKILLDATA_DIALOG };

    enum
    {
        TIMER_SKILLDATA_SAVE_STATE_ID = 12,

        SAVE_MESSAGE_APPEAR_TIME = 1000,
    };

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;
	
	GLSKILL		m_Item;

	BOOL		m_bDlgInit;

	CFont*			m_pFont;

	std::string		strKeyName;
	std::string		strName;
	std::string		strKeyDesc;
	std::string		strDesc;

	int	m_nSelEffElmt;

	CListBox	m_listImpactAddon;
	CListBox	m_listSpecAddon;
	CListBox	m_listAttStrikeAni;

public:
	void	SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }
	BOOL	SetItem ( DWORD GUID );

	void	SetReadMode();

protected:
	void	UpdateItems ();
	void	InverseUpdateItems ();
	void	InitDefaultCtrls ();
	void	InverseUpdateEffElmtLeft();
	void	UpdateEffElmtLeft();

	bool    IsAddEnableCountAttack(EMSPEC_ADDON eSpec);
	bool	IsInputListCountAttack(DWORD dwSpec);

private:
	void	UpdateAnimInfo(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedButtonSelfbodyeffect();
	afx_msg void OnBnClickedButtonTargetbodyeffect();
	afx_msg void OnBnClickedButtonTargeteffect();
	afx_msg void OnBnClickedButtonIconfile();
	afx_msg void OnBnClickedButtonSelfzoneeffect();
	afx_msg void OnBnClickedButtonSelfzoneeffect2();
    afx_msg void OnBnClickedButtonSelfzoneeffect3();
	afx_msg void OnBnClickedButtonTargzoneeffect();
	afx_msg void OnBnClickedButtonTargzoneeffect2();
	afx_msg void OnBnClickedButtonTargzoneeffect3();
	afx_msg void OnBnClickedButtonTargetbodyeffect2();
	afx_msg void OnBnClickedButtonHoldout();
	afx_msg void OnEnChangeEditSkillname();
	afx_msg void OnEnChangeEditSkilldesc();
	afx_msg void OnBnClickedButtonEffElmtLeft();
	afx_msg void OnBnClickedButtonEffElmtRight();
	afx_msg void OnBnClickedButtonSkillType();
	afx_msg void OnBnClickedButtonElement();
	afx_msg void OnBnClickedButtonBlowType();
	afx_msg void OnBnClickedButtonSpecial();
	afx_msg void OnBnClickedButtonMaintype();
	afx_msg void OnBnClickedButtonSubtype();
	afx_msg void OnBnClickedButtonAttackleft();
	afx_msg void OnBnClickedButtonAttackright();
	afx_msg void OnBnClickedButtonImpactAddonAdd();
	afx_msg void OnBnClickedButtonImpactAddonDel();
	afx_msg void OnBnClickedButtonSpecTypeAdd();
	afx_msg void OnBnClickedButtonSpecTypeDel();
	afx_msg void OnBnClickedButtonVehicleMaintype();
    afx_msg void OnBnClickedButtonTargzoneeffect4();
	afx_msg void OnBnClickedCheckHiddenWeapon();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
};
