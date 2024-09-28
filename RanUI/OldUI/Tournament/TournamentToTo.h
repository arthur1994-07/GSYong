#pragma	once

#include "../Util/UIWindowObjectController.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"
#include "../../InnerInterface.h"
#include "../Util/UIWindowEx.h"

#include "./TournamentComboBox.h"


class TournamentToToWindow : public CUIWindowEx
{
public:
	enum
	{
		CONTROL_ID_NONE = NO_ID+5,
		CONTROL_ID_ENTRY_COMBO_BOX,
		CONTROL_ID_BUTTON_SELECTION,
		CONTROL_ID_BUTTON_EXIT,
		CONTROL_ID_BUTTON_CANSEL,
		CONTROL_ID_INFO_BUTTON0,
		CONTROL_ID_INFO_BUTTON1,
		CONTROL_ID_INFO_BUTTON2,
		CONTROL_ID_INFO_BUTTON3,
		CONTROL_ID_INFO_BUTTON4,
		CONTROL_ID_INFO_BUTTON5,
		CONTROL_ID_INFO_BUTTON6,
		CONTROL_ID_INFO_BUTTON7,
		CONTROL_ID_INFO_BUTTON8,
		CONTROL_ID_INFO_BUTTON_END,
		CONTROL_ID_HIT_BOX0,
		CONTROL_ID_HIT_BOX1,
		CONTROL_ID_HIT_BOX2,
		CONTROL_ID_HIT_BOX3,
		CONTROL_ID_HIT_BOX4,
		CONTROL_ID_HIT_BOX5,
		CONTROL_ID_HIT_BOX6,
		CONTROL_ID_HIT_BOX7,
		CONTROL_ID_HIT_BOX8,
		CONTROL_ID_HIT_BOX_END,
	};
public:
	TournamentToToWindow ( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual	~TournamentToToWindow ();

virtual void Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl);
		void OnSetPage(DWORD dwKagNum,DWORD dwPageNum);
public:
		void CreateSubControl ();
		virtual	void TranslateUIMessage( UIGUID ControlID, DWORD dwMsg );

public:

private:
	TournamentComboBox*			m_pComboBox2;
	CBasicTextButton*			m_pCanselButton;
	CBasicTextButton*			m_pSelectButton;
	CBasicTextButton*			m_pCloseButton;

	CBasicTextBox*				m_pCHR_LEVTEXT[9];
	CBasicTextBox*				m_pCHR_CHR_NAME_TEXT[9];

	CUIControl*					m_pImg_MINI_ICON[9];

	RnButton*					m_pImg_INFO_ICON[9];

	CUIControl*					m_pDashboard[7];

	CUIControl*					m_pOrengeBox[8];
	CUIControl*					m_pSelectHitBox[8];

	CUIControl*					m_pImg_GOLD_ICON;

	CBasicTextBox*		        m_pText_GOLD_TEXT;
	CBasicTextBox*		        m_pText_GOLD_BOX_TEXT;

	CBasicTextBox*		        m_pText_WIN_TEXT;
	CBasicTextBox*		        m_pText_WIN_BOX_TEXT;

	CBasicTextBox*		        m_pText_CHRNAME_TITLE;


	CBasicTextBox*		        m_pText_KANG_TEXTBOX;

	CBasicTextBox*		        m_pText_INFO_TITLE;
	CBasicTextBox*		        m_pText_RATE_TEXT;
	CBasicTextBox*		        m_pText_WINNER_TEXT;
	CBasicTextBox*		        m_pText_WINNER_TEXT_ME;

	DWORD						m_dwPageNum;
	DWORD						m_dwKangNum;

protected:
	GLGaeaClient* m_pGaeaClient;
};


class MyTournamentToToWindow : public ITournamentToToWindow, private TournamentToToWindow
{
public:
	MyTournamentToToWindow( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyTournamentToToWindow() {};

public:
	virtual void OnCreate();
	virtual void OnClose() {};
	virtual void OnVisible(bool visible);
	virtual bool GetVisible(){return false;}
	virtual void SetPage(DWORD dwKagNum,DWORD dwPageNum);
	virtual void OnFrameMove(){};
	virtual const UIRECT& GetGlobalPos();   
};
