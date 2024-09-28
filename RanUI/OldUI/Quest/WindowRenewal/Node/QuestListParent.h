#pragma once

#include "../../../../../EngineLib/GUInterface/UIGroup.h"
#include "../List/QuestListRenewal.h"

class CBasicTextBox;
class CTipBox;
class GLGaeaClient;
class CInnerInterface;

class	CQuestListParent : public CUIGroup
{
private:
	enum
	{
		QUEST_LIST_PARENT_BACK = NO_ID + 1,
		QUEST_LIST_PARENT_TEXT,
		QUEST_LIST_PARENT_OPEN,
		QUEST_LIST_PARENT_CLOSE,
		QUEST_LIST_PARENT_OPEN_OVER,
		QUEST_LIST_PARENT_CLOSE_OVER,
	};

public:
	CQuestListParent(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CQuestListParent ();

protected:
    CInnerInterface* m_pInterface;

public:
	void	CreateSubControl ();

public:
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

public:
	bool	IsOpen();
	void	Open();
	void	Close();

	void	SetText( CString strText );

	void	MouseOver( bool bOver );

	void	SetID( DWORD dwID, bool bOpen = false );
	DWORD	GetID()	{	return m_dwID;	}

private:
	CBasicTextBox*	m_pText;
	CUIControl*		m_pOpen;
	CUIControl*		m_pClose;
	CUIControl*		m_pOpenOver;
	CUIControl*		m_pCloseOver;

	CTipBox*		m_pBack;

private:
	DWORD			m_dwID;
};