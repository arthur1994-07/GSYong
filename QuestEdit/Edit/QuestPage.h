#pragma once

#include "../../RanLogic/Quest/GLQuest.h"

#include "afxwin.h"

// CQuestPage ��ȭ �����Դϴ�.
class CsheetWithTab;
class CQuestPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CQuestPage)

public:
	CsheetWithTab* m_pSheetTab;

public:
	void SetSheetTab ( CsheetWithTab* pSheetTab )		{ m_pSheetTab = pSheetTab; }

public:
	void UpdateItems();
    void UpdateItemsGiftItem();
    void UpdateItemsGiftSkill();    
    void InverseUpdateItems();
	void SetReadMode ();
	void UpdateProgress();
	void InverseUpdatePrograss();
	void InitDefaultCtrls();
	void InitAttendenceType();

public:
	bool m_bDlgInit;
	CFont* m_pFont;
	CListCtrl m_GiftItemList;
    
public:
	CQuestPage( LOGFONT logfont );
	virtual ~CQuestPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_FORMVIEW_QUEST };
	void QuestStringtableSynchronization();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
    void InitGiftItemList();
    afx_msg void OnBnClickedButtonGiftItemNew();
	afx_msg void OnBnClickedButtonGiftItemEdit();
	afx_msg void OnBnClickedButtonGiftItemDel();
	afx_msg void OnBnClickedButtonGiftSkillNew();
	afx_msg void OnBnClickedButtonGiftSkillDel();
	afx_msg void OnBnClickedButtonPrev();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedButtonEditcancel();
	afx_msg void OnBnClickedButtonEditcomplate();
	afx_msg void OnBnClickedUseProgressCheck();
	afx_msg void OnCbnSelchangeStateCombo1();
	afx_msg void OnCbnSelchangeStateCombo2();
	afx_msg void OnCbnSelchangeStateCombo3();
	afx_msg void OnBnClickedMapEditButton();
	afx_msg void OnBnClickedMapEditButton2();
	afx_msg void OnBnClickedMapEditButton3();
	afx_msg void OnBnClickedRadioAll();
	afx_msg void OnBnClickedRadioMan();
	afx_msg void OnBnClickedRadioWoman();
    afx_msg void OnBnClickedButtonQuestArea();
	afx_msg void OnEnChangeEditLimitTime();
	afx_msg void OnBnClickedCheckDaily();
};