#pragma once

#include "../../../../../EngineLib/GUInterface/UIGroup.h"
#include "../List/QuestListRenewal.h"

class CBasicTextBox;
class GLGaeaClient;
class CInnerInterface;

class	CQuestListChild : public CUIGroup
{
private:
	enum
	{
		QUEST_LIST_CHILD_BACK = NO_ID + 1,
		QUEST_LIST_CHILD_TEXT,
		QUEST_LIST_CHILD_CHECK,
		QUEST_LIST_CHILD_UNCHECK,
		QUEST_LIST_CHILD_CHECK_BACK,
		QUEST_LIST_CHILD_DONE,
		QUEST_LIST_CHILD_FAIL,
	};

public:
	CQuestListChild(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CQuestListChild ();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
	void	CreateSubControl ();

public:
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	bool	IsCheck();
	bool	Check( bool bPrintMsg = TRUE );
	bool	Uncheck();

	void	SetText( CString strText );
	CString	GetText();

	void	SetSelect();
	void	UnSelect();

	void	SetID( DWORD dwID, bool bCheck = false, bool bComplete = false );
	DWORD	GetID()	{return m_dwID; }

private:
	CBasicTextBox*	m_pText;
	CUIControl*		m_pCheck;
	CUIControl*		m_pUnCheck;
	CUIControl*		m_pCheckBack;

	CUIControl*		m_pDone;
	CUIControl*		m_pFail;

	CUIControl*		m_pBack;

private:
	DWORD			m_dwID;
	CString			m_strText;
};