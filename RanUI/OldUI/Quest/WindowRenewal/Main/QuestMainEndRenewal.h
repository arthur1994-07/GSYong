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

class CQuestMainEndRenewal : public CUIGroup
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
		QUEST_MAIN_DONE_ICON,
		QUEST_MAIN_FAIL_ICON,

		QUEST_MAIN_INDEMNIFY_BACK,
		QUEST_MAIN_INDEMNIFY_TITLE,
		QUEST_MAIN_INDEMNIFY_TITLE_BACK,
		QUEST_MAIN_INDEMNIFY_TEXT,
		QUEST_MAIN_INDEMNIFY_TEXT_SCROLLBAR,
		QUEST_MAIN_INDEMNIFY_ITEM_BACK,
		QUEST_MAIN_INDEMNIFY_ITEM_SCROLLBAR,
		QUEST_MAIN_INDEMNIFY_ITEM0,
		QUEST_MAIN_INDEMNIFY_ITEM_END = QUEST_MAIN_INDEMNIFY_ITEM0 + nINDEMNIFY_SLOT_TOTAL,

		QUEST_MAIN_END_RESULT_BACK,
		QUEST_MAIN_END_RESULT_TITLE,
		QUEST_MAIN_END_RESULT_TITLE_BACK,
		QUEST_MAIN_END_RESULT_TEXT,

		QUEST_MAIN_END_EXPLAIN_BACK,
		QUEST_MAIN_END_EXPLAIN_TITLE,
		QUEST_MAIN_END_EXPLAIN_TITLE_BACK,
		QUEST_MAIN_END_EXPLAIN_TEXT,
		QUEST_MAIN_END_EXPLAIN_SCROLLBAR,

		QUEST_MAIN_END_MORE_BACK,
		QUEST_MAIN_END_MORE_TITLE,
		QUEST_MAIN_END_MORE_TITLE_BACK,
		QUEST_MAIN_END_MORE_TEXT,
		QUEST_MAIN_END_MORE_SCROLLBAR,

		QUEST_MAIN_END_STEP_BACK,
		QUEST_MAIN_END_STEP_TEXT,
		QUEST_MAIN_END_STEP_SCROLLBAR,
	};

public:
	CQuestMainEndRenewal(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CQuestMainEndRenewal ();

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

	bool	LoadQuestMain ( DWORD dwQuestID );
	void	UnLoadMain();

	DWORD	GetQuestID()	{ return m_dwQuestID; }

private:
	CBasicLineBox*		m_pLineBox;
	CBasicLineBox*		m_pTitleBack;
	CBasicTextBox*		m_pTitle;
	CUIControl*			m_pDoneIcon;
	CUIControl*			m_pFailIcon;

	CBasicLineBox*		m_pIndemnifyBack;
	CBasicTextBox*		m_pIndemnifyTitle;
	CUIControl*			m_pIndemnifyTitleBack;
	CBasicTextBox*		m_pIndemnifyText;
	CBasicScrollBarEx*	m_pIndemnifyTextScrollBar;
	SNATIVEID			m_sID[ nINDEMNIFY_SLOT_TOTAL ];
	CItemImage*			m_pIndemnifyItem[ nINDEMNIFY_SLOT_TOTAL ];
	CBasicScrollBarEx*	m_pIndemnifyItemScrollBar;

	CBasicLineBox*		m_pEndResultBack;
	CBasicTextBox*		m_pEndResultTitle;
	CUIControl*			m_pEndResultTitleBack;
	CBasicTextBox*		m_pEndResultText;

	CBasicLineBox*		m_pEndExplainBack;
	CBasicTextBox*		m_pEndExplain;
	CUIControl*			m_pEndExplainTitleBack;
	CBasicTextBox*		m_pEndExplainText;
	CBasicScrollBarEx*	m_pEndExplainScrollBar;

	CBasicLineBox*		m_pEndMoreBack;
	CBasicTextBox*		m_pEndMore;
	CUIControl*			m_pEndMoreTitleBack;
	CBasicTextBox*		m_pEndMoreText;
	CBasicScrollBarEx*	m_pEndMoreScrollBar;

	CBasicLineBox*		m_pEndStepBack;
	CBasicTextBox*		m_pEndStepText;
	CBasicScrollBarEx*	m_pEndStepScrollBar;

private:
	DWORD m_dwQuestID;

	int m_iStartIndex;
	int m_nTotalLine;
};