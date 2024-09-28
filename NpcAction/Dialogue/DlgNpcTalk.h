#pragma once

#include "../../RanLogic/NpcTalk/NpcDialogueSet.h"

// CDlgNpcTalk 대화 상자입니다.

class CDlgNpcTalk : public CDialog
{
	DECLARE_DYNAMIC(CDlgNpcTalk)

public:

	DWORD m_dwNpcID;
	spNpcDialogSet m_spNpcDialogueSet;

public:
	CDlgNpcTalk(CWnd* pParent = NULL);   // 표준 생성자입니다.
	CDlgNpcTalk(spNpcDialogSet pNpcDialogueSet, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgNpcTalk();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLGNPCTALK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CListBox	m_listNpcTalk;
	virtual BOOL OnInitDialog();
	void	SetDialogueSet(spNpcDialogSet pNpcDialogueSet ) { m_spNpcDialogueSet = pNpcDialogueSet; }

protected:
	virtual void OnOK();
	virtual void OnCancel();
};
