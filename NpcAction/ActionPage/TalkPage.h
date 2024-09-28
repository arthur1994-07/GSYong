#pragma once

#include "afxwin.h"

#include "../../RanLogic/NpcTalk/NpcTalk.h"
#include "../../RanLogic/NpcTalk/NpcDialogueSet.h"

class	CNpcTalkControl;
class	CsheetWithTab;

// CTalkPage 대화 상자입니다.

class CTalkPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CTalkPage)

protected:
	CsheetWithTab*	m_pSheetTab;
	int				m_CallPage;

private:
	BOOL		m_bModify;
	std::tr1::shared_ptr<SNpcTalk> m_spOrgTalk;
	SNpcTalk	m_Talk;
	CFont*		m_pFont;

private:
	CNpcTalkControl*	m_pTalkControl;
	spNpcDialogSet	m_spNpcDialogueSet;

public:
	CListCtrl	m_ctrlRandomID;
	CListCtrl	m_ctrNeedItem;
	CListCtrl	m_ctrTradeItem;

public:
	void	UpdateItems ();
	void	SetWndView ( int nACTION, DWORD dwNO );

public:
	void SetTalkData ( spNpcDialogSet pNpcDialogueSet, CNpcTalkControl* pTalkControl );
	void SetModifyData(BOOL bModify, std::tr1::shared_ptr<SNpcTalk> spTalk);

	SITEM_NUM	GetNeedItem( DWORD dwItemNum );
	void		InsertNeedItem( SITEM_NUM sItemNum );
	void		ResetNeedItem() { m_Talk.m_vecNeedItem.clear(); }

	int		GetActionType() { return m_Talk.m_nACTION; }
	int		GetActionNO()	{ return m_Talk.m_dwACTION_NO; }

public:
	void	SetCallPage ( int CallPage ) { m_CallPage = CallPage; }
	void	SetSheetTab ( CsheetWithTab* pSheetTab ) { m_pSheetTab = pSheetTab; }

	void	SetReadMode ();

public:
	CTalkPage( LOGFONT logfont );
	virtual ~CTalkPage();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NPCTALKDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboActionType();
	afx_msg void OnCbnSelchangeComboActionData();
	afx_msg void OnBnClickedButtonConditonSet();
	afx_msg void OnBnClickedButtonConditonReset();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonCancel();
	CComboBox m_comboQUESTSTART[MAX_QUEST_START];
	CComboBox m_comboQUEST_PROC;
	CComboBox m_comboQUEST_PROC_STEP;
	afx_msg void OnCbnSelchangeComboQuestStart(UINT nID);
	afx_msg void OnCbnSelchangeComboQuestProg();
	afx_msg void OnCbnSelchangeComboQuestProgStep();
	afx_msg void OnBnClickedButtonBus();
	afx_msg void OnBnClickedRandomInsert();
	afx_msg void OnBnClickedRandomDelete();
	afx_msg void OnBnClickedRandomReset();
	afx_msg void OnBnClickedButtonTradeItemAdd();
	afx_msg void OnBnClickedButtonTradeItemDel();
	afx_msg void OnBnClickedButtonTradeItemDelall();
	afx_msg void OnBnClickedButtonHireSummon();
};
