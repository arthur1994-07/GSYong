#pragma once

#include "afxcmn.h"

#include "../../RanLogic/NpcTalk/NpcDialogue.h"
#include "../../RanLogic/NpcTalk/NpcDialogueSet.h"

// CNpcActionPage ��ȭ �����Դϴ�.
class	CsheetWithTab;
class	CNpcDialogue;

class CNpcActionPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CNpcActionPage)

public:
	CNpcActionPage( LOGFONT logfont );
	virtual ~CNpcActionPage();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_NPCACTIONPAGE };

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;

    CNpcDialogue	m_DummyDialogue;
    std::tr1::shared_ptr<CNpcDialogue> m_spDialogue;

	spNpcDialogSet	m_spNpcDialogueSet;
	BOOL		 	m_bDlgInit;
	CFont*			m_pFont;

public:
	void	SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }
    void	SetDialogue ( spNpcDialogSet spNpcDialogueSet, std::tr1::shared_ptr<CNpcDialogue> spDialogue );
	const DWORD GetCurrentDialogueNID()	{ return m_spDialogue->GetDlgNID(); }

public:
	void	UpdateItems ();
	void	UpdateCase ();

	void	SetReadMode();

private:
	void	InverseUpdateItems ();
	void	InitDefaultCtrls ();

private:
	std::string GetBoolString ( BOOL bValue );
	void NpcTalkStringtableSynchronization();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:	
	CListCtrl m_listctrlCase;
	CListCtrl m_listctrlAnswer;

	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	afx_msg void OnBnClickedButtonCaseEdit();
	afx_msg void OnBnClickedButtonCaseDel();
	afx_msg void OnBnClickedButtonCaseNew();
	afx_msg void OnLvnItemchangedListPostiveAnswer2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnNMDblclkListPostiveAnswer2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonCaseCopy();
};
