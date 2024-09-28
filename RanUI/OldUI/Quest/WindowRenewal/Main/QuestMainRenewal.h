#pragma	once

#include "../../../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../../../EngineLib/G-Logic/GLDefine.h"

class	CBasicLineBox;
class	CBasicTextBox;
class	CBasicScrollBarEx;
class	CItemSlot;
class	CBasicTextButton;
struct	SINVENITEM;
class	CQuestGiftView;
class	CItemImage;
class GLGaeaClient;
class CInnerInterface;

#define nINDEMNIFY_SLOT_SIZE_X 3
#define nINDEMNIFY_SLOT_TOTAL 6

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_BUTTON_GIVEUP_RENEWAL = UIMSG_USER3;
const DWORD UIMSG_BUTTON_COMPLETE_RENEWAL = UIMSG_USER6;
////////////////////////////////////////////////////////////////////

class	CQuestMainRenewal : public CUIGroup
{
private:
	static	const	float	fANSWER_LINE_INTERVAL;
	static	const	DWORD	dwMINITE_BY_HOUR;

private:
	enum
	{
		QUEST_MAIN_BACK = NO_ID + 1,
		QUEST_MAIN_TITLE_BACK,
		QUEST_MAIN_TITLE_TEXT,
		QUEST_MAIN_ICON,

		QUEST_MAIN_EXPLAIN_BACK,
		QUEST_MAIN_EXPLAIN_TITLE,
		QUEST_MAIN_EXPLAIN_TITLE_BACK,
		QUEST_MAIN_EXPLAIN_TEXT,
		QUEST_MAIN_EXPLAIN_SCROLLBAR,

		QUEST_MAIN_MORE_BACK,
		QUEST_MAIN_MORE_TITLE,
		QUEST_MAIN_MORE_TITLE_BACK,
		QUEST_MAIN_MORE_TEXT,
		QUEST_MAIN_MORE_SCROLLBAR,

		QUEST_MAIN_STEP_BACK,
		QUEST_MAIN_STEP_TEXT,
		QUEST_MAIN_STEP_SCROLLBAR,

		QUEST_MAIN_COLL_BACK,
		QUEST_MAIN_COLL_TITLE,
		QUEST_MAIN_COLL_TITLE_BACK,
		QUEST_MAIN_COLL_SLOT_BACK,
		QUEST_MAIN_COLL_SLOT,

		QUEST_MAIN_INDEMNIFY_BACK,
		QUEST_MAIN_INDEMNIFY_TITLE,
		QUEST_MAIN_INDEMNIFY_TITLE_BACK,
		QUEST_MAIN_INDEMNIFY_TEXT,
		QUEST_MAIN_INDEMNIFY_TEXT_SCROLLBAR,
		QUEST_MAIN_INDEMNIFY_ITEM_BACK,
		QUEST_MAIN_INDEMNIFY_ITEM_SCROLLBAR,
		QUEST_MAIN_INDEMNIFY_ITEM0,
		QUEST_MAIN_INDEMNIFY_ITEM_END = QUEST_MAIN_INDEMNIFY_ITEM0 + nINDEMNIFY_SLOT_TOTAL,

		QUEST_MAIN_GIVE_UP_BUTTON,
		QUEST_MAIN_COMPLETE_BUTTON,
	};

public:
	CQuestMainRenewal(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CQuestMainRenewal ();

protected:
    GLGaeaClient* m_pGaeaClient;
    CInnerInterface* m_pInterface;

public:
	void	CreateSubControl ();

private:
	CItemSlot*			CreateItemSlot ( CString strKeyword, UIGUID ControlID );
	CItemImage*			CreateIndemnifyItemSlot ( CString strKeyword, UIGUID ControlID );
	CBasicTextButton*	CreateTextButton ( char* szButton, UIGUID ControlID , char* szText );
	CUIControl*			CreateControl ( char* szControl );
	CBasicTextBox*		CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );

public:
	virtual	void	SetVisibleSingle ( BOOL bVisible );
	virtual	void	Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void	TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

	void	LoadItem ( SINVENITEM& ref_InvenItem );
	const SINVENITEM* GetItem(int nPosX) const;
	void	UnLoadItem ( int nPosX );

	bool	LoadQuestMain ( DWORD dwQuestID );
	void	UnLoadMain();

	DWORD	GetQuestID()	{ return m_dwQuestID; }

	CBasicLineBox*	GetStepBack()	{ return m_pStepBack; }

private:
	CBasicLineBox*		m_pLineBox;
	CBasicLineBox*		m_pTitleBack;
	CBasicTextBox*		m_pTitle;
	CUIControl*			m_pIngIcon;

	CBasicLineBox*		m_pExplainBack;
	CBasicTextBox*		m_pExplain;
	CUIControl*			m_pExplainTitleBack;
	CBasicTextBox*		m_pExplainText;
	CBasicScrollBarEx*	m_pExplainScrollBar;

	CBasicLineBox*		m_pMoreBack;
	CBasicTextBox*		m_pMore;
	CUIControl*			m_pMoreTitleBack;
	CBasicTextBox*		m_pMoreText;
	CBasicScrollBarEx*	m_pMoreScrollBar;

	CBasicLineBox*		m_pStepBack;
	CBasicTextBox*		m_pStepText;
	CBasicScrollBarEx*	m_pStepScrollBar;
	CItemSlot*			m_pItemSlot;

	CBasicLineBox*		m_pColItemBack;
	CBasicTextBox*		m_pColItemTitle;
	CUIControl*			m_pColItemTitleBack;

	CBasicLineBox*		m_pIndemnifyBack;
	CBasicTextBox*		m_pIndemnifyTitle;
	CUIControl*			m_pIndemnifyTitleBack;
	CBasicTextBox*		m_pIndemnifyText;
	CBasicScrollBarEx*	m_pIndemnifyTextScrollBar;
	SNATIVEID			m_sID[ nINDEMNIFY_SLOT_TOTAL ];
	CItemImage*			m_pIndemnifyItem[ nINDEMNIFY_SLOT_TOTAL ];
	CBasicScrollBarEx*	m_pIndemnifyItemScrollBar;
    CUIControl*		    m_pIndemnifyItemMouseOver;

	CBasicTextButton*	m_pGiveUpButton;
	CBasicTextButton*	m_pCompleteButton;

private:
	DWORD m_dwQuestID;

	int m_iStartIndex;
	int m_nTotalLine;
};