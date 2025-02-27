#pragma	once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class CInnerInterface;
class CBasicTextBox;
class CBasicScrollBarEx;
class CItemSlot;
class CBasicTextButton;
class GLGaeaClient;
struct SINVENITEM;

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_BUTTON_BACK_LBUPLIKE = UIMSG_USER4;
const DWORD UIMSG_BUTTON_COMPLETE_STEP = UIMSG_USER5;
const DWORD	UIMSG_BUTTON_PRINT_STEP	   = UIMSG_USER7;
////////////////////////////////////////////////////////////////////

struct	GLQUEST_STEP;
struct	GLQUESTPROG_STEP;
class CQuestStep : public CUIGroup
{
private:
	static	const	float	fANSWER_LINE_INTERVAL;

private:
	enum
	{
		QUEST_STEP_DESC_TEXTBOX = NO_ID + 1,
		QUEST_STEP_DESC_SCROLLBAR,
		QUEST_STEP_TEXTBOX,
		QUEST_STEP_SCROLLBAR,
		QUEST_STEP_ITEMSLOT,
		QUEST_STEP_BUTTON_L,
		QUEST_STEP_BUTTON_R,
		QUEST_STEP_BUTTON_PRINT,
		QUEST_MAIN_BUTTON_COMPLETE
	};

public:
	CQuestStep(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CQuestStep ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void	CreateSubControl ();

private:
	CItemSlot*	CreateItemSlot ( CString strKeyword, UIGUID ControlID );
	CBasicTextButton* CreateTextButton ( char* szButton, UIGUID ControlID , char* szText );
	CUIControl*		CreateControl ( char* szControl );
	CBasicTextBox*	CreateStaticControl (const char* szControlKeyword, CD3DFontPar* pFont, D3DCOLOR D3DCOLOR, int nAlign );

public:
	bool	LoadQuestStep ( DWORD dwQuestID );

public:
	virtual	void	TranslateUIMessage ( UIGUID cID, DWORD dwMsg );

	void	LoadItem ( SINVENITEM& ref_InvenItem );
	const SINVENITEM* GetItem(int nPosX) const;
	void	UnLoadItem ( int nPosX );
	CBasicTextBox*	GetRunText(){ return m_pRunText; }

private:
	CBasicTextBox*		m_pDescText;
	CBasicScrollBarEx*	m_pDescScrollBar;
	CBasicTextBox*		m_pRunText;
	CBasicScrollBarEx*	m_pRunScrollBar;

	CItemSlot*	m_pItemSlot;

//	CBasicTextButton*	m_pButtonL;
	CBasicTextButton*	m_pButtonR;
	CBasicTextButton*	m_pButtonPrint;

	CBasicTextButton*	m_pButtonCOMPLETE;

protected:
	CInnerInterface* m_pInterface;

};