#pragma once

#include "afxcmn.h"

#include "../../RanLogic/NpcTalk/NpcDialogue.h"
#include "../../RanLogic/NpcTalk/NpcDialogueCase.h"
#include "../../RanLogic/NpcTalk/NpcTalkControl.h"
#include "../../RanLogic/NpcTalk/NpcDialogueSet.h"

// CNpcActionAnswer 대화 상자입니다.
class	CsheetWithTab;

class CNpcActionAnswer : public CPropertyPage
{
	DECLARE_DYNAMIC(CNpcActionAnswer)

public:
	CNpcActionAnswer( LOGFONT logfont );
	virtual ~CNpcActionAnswer();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NPCACTIONANSWER };

	DWORD			m_dwItemIndex;

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;

	CNpcTalkControl		m_DummyTalkControl;
	CNpcDialogueCase	m_DummyDialogueCase;

	std::tr1::shared_ptr<CNpcDialogueSet>	m_spNpcDialogueSet;
	std::tr1::shared_ptr<CNpcDialogue>		m_spNpcDialogue;
	std::tr1::shared_ptr<CNpcDialogueCase>	m_spOrgDialogueCase;
	BOOL			  m_bDlgInit;
	CFont*			  m_pFont;

public:
	void SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	void SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }
	void SetDialogueCase(spNpcDialogSet pNpcDialogueSet, std::tr1::shared_ptr<CNpcDialogueCase> spDialogueCase, const DWORD dwItemIndex );

public:
	void	UpdateItems ();

	void	SetReadMode ();

private:	
	void	InitDefaultCtrls ();
	void	InverseUpdateItems ();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOk();
	CListCtrl m_ctrlAnswer;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonModify();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonDelall();
	afx_msg void OnBnClickedButtonConditonEdit();
	afx_msg void OnBnClickedButtonConditonDel();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnNMDblclkListAnswer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonCopy();
};
