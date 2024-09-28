#pragma once
#include "afxwin.h"


// CCrowAttack 대화 상자입니다.
class	CsheetWithTab;
struct	SCROWDATA;
class CCrowAttack : public CPropertyPage
{
	DECLARE_DYNAMIC(CCrowAttack)

public:
	CCrowAttack( LOGFONT logfont );
	virtual ~CCrowAttack();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CROWATTACK_DIALOG };

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;

	SCROWDATA*		m_pItem;
	SCROWDATA*		m_pDummyItem;

	SNATIVEID		m_sSelectedItem;
	//CROWATTACK_VEC	m_vecDummyCrowAttack;
	//CROWATTACK_VEC*	m_pVecCrowAttack;

	BOOL			m_bDlgInit;

	CFont*			m_pFont;

	int				m_nSelectAttack;

public:
	void	SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }
	BOOL	SetItem ( SCROWDATA* pItem, DWORD dwID );

protected:
	void	UpdateItems ();
	void	UpdateMaxattack();
	void	InverseUpdateItems ();
	void	InverseUpdateMaxattack();
	void	InitDefaultCtrls ();

	void	UpdateAttackAni ( SCROWATTACK &rCrowAttack, SANIMCONINFO *pAnimConInfo );

public:
	void	InitAllItem ();
	void	SetReadMode ();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonPrev();
	afx_msg void OnCbnSelchangeComboAniAttacktype();
	afx_msg void OnBnClickedButtonSelfbodyeffect();
	afx_msg void OnBnClickedButtonTargetbodyeffect();
	afx_msg void OnBnClickedButtonTargeteffect();
	afx_msg void OnBnClickedButtonSkill();
	afx_msg void OnBnClickedButtonBlow();
	afx_msg void OnBnClickedButtonCrowAttackAdd();
	afx_msg void OnBnClickedButtonCrowAttackDel();
	afx_msg void OnLbnSelchangeListCrowAttack();
	CListBox m_listCrowAttack;
};
