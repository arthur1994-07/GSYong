#pragma once
#include "afxwin.h"


// CItemSuit 대화 상자입니다.
class	CsheetWithTab;
struct	SITEM;
class CItemSuit : public CPropertyPage
{
	DECLARE_DYNAMIC(CItemSuit)

public:
	CItemSuit( LOGFONT logfont );
	virtual ~CItemSuit();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ITEMSUIT_DIALOG };

    enum
    {
        TIMER_ITEMSUIT_SAVE_STATE_ID = 10,

        SAVE_MESSAGE_APPEAR_TIME = 1000,
    };

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;

	SITEM*			m_pDummyItem;
	SITEM*			m_pItem;

	BOOL			m_bDlgInit;
	CFont*			m_pFont;


public:
	void	SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }
	BOOL	SetItem ( SITEM* pItem );
	void	SetReadMode();
    void    ShowSaveState();

protected:
	void	UpdateItems ();
	bool	InverseUpdateItems ();
	void	TypeOfUpdateItems ();
	bool	TypeOfInverseUpdateItems ();
	void	InitDefaultCtrls ();

	//////////////////////////////////////////////////////////////////////////////
	//	리팩토링 적용펑션
public:
	void	InitAllItem ();

	void	InitBox ();
	void	InitBox_Rand ();
	void	InitPetSkinPack();

	void	SelectAddonType( int nIndex, int nEditID );

private:
	void ListBoxNameUpdate(void);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_list_Mob;
	CListBox  m_listBOX;
	CListBox  m_listBox_Rand;
	CEdit m_Edit_QueTime;
	CEdit m_Edit_QueExp;
	CEdit m_Edit_QueParam1;
	CEdit m_Edit_QueParam2;

	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonPrev();
	afx_msg void OnBnClickedButtonBoxAdd();
	afx_msg void OnBnClickedButtonBoxDel();
	afx_msg void OnBnClickedButtonBoxAdd2();
	afx_msg void OnBnClickedButtonBoxDel2();
	afx_msg void OnBnClickedButtonBoxUp1();
	afx_msg void OnBnClickedButtonBoxDw1();
	afx_msg void OnBnClickedButtonBoxUp2();
	afx_msg void OnBnClickedButtonBoxDw2();
	afx_msg void OnBnClickedButtonAddPetskin();
	afx_msg void OnBnClickedButtonDelPetskin();
	afx_msg void OnBnClickedButtonBoxEdit();
	afx_msg void OnBnClickedButtonBoxEdit2();
	afx_msg void OnBnClickedButtonSuit();
	afx_msg void OnBnClickedButtonAttType();
	afx_msg void OnBnClickedButtonAddonNo1();
	afx_msg void OnBnClickedButtonAddonNo2();
	afx_msg void OnBnClickedButtonAddonNo3();
	afx_msg void OnBnClickedButtonAddonNo4();
	afx_msg void OnBnClickedButtonAddonNo5();
	afx_msg void OnBnClickedButtonAddonNo6();
	afx_msg void OnBnClickedButtonBlow();
	afx_msg void OnBnClickedButtonQuestion();
	afx_msg void OnBnClickedButtonDrug();
	afx_msg void OnCbnSelchangeComboVehicleType();
	afx_msg void OnCbnSelchangeComboPartsType();
	afx_msg void OnBnClickedCheckEnableCostum();
	afx_msg void OnBnClickedCheckEnableUnique();
	afx_msg void OnBnClickedDrugErList();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
//	afx_msg void OnEnChangeSkillRate();
	afx_msg void OnCbnSelchangeSkillTarget();
};