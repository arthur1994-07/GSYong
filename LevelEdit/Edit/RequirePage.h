#pragma once

#include "../resource.h"
#include "../../EngineLib/G-Logic/GLDefine.h"

// CRequirePage 대화 상자입니다.
class CsheetWithTab;
class CRequirePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CRequirePage)

protected:
	CsheetWithTab*	m_pSheetTab;
	BOOL			m_bInit;

public:
	void SetSheetTab ( CsheetWithTab *pSheetTab ) { m_pSheetTab = pSheetTab; }

public:
	void SetReqItem ( SNATIVEID sNID, int iBtnNum );
	void SetReqSkill ( SNATIVEID sNID );

public:
	void InversePage ();
	void UpdatePage ();

public:
	CRequirePage();
	virtual ~CRequirePage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_LEVELREQUIREPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonSaveSet();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonItem();
	afx_msg void OnBnClickedButtonSkill();
	afx_msg void OnBnClickedButtonResetItem();
	afx_msg void OnBnClickedButtonResetSkill();
	afx_msg void OnBnClickedButtonResetQuestCom();
	afx_msg void OnBnClickedButtonResetQuestProc();
	afx_msg void OnBnClickedButtonComQuest();
	afx_msg void OnBnClickedButtonActQuest();
	afx_msg void OnCbnSelchangeComboLevel();
	afx_msg void OnBnClickedCheckForceExit();
	afx_msg void OnBnClickedButtonItem2();
	afx_msg void OnBnClickedButtonItem3();
	afx_msg void OnBnClickedButtonItem4();
	afx_msg void OnBnClickedButtonItem5();
	afx_msg void OnBnClickedButtonResetItem2();
	afx_msg void OnBnClickedButtonResetItem3();
	afx_msg void OnBnClickedButtonResetItem4();
	afx_msg void OnBnClickedButtonResetItem5();
};
