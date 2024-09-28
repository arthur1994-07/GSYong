#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"

class CInnerInterface;
class CBasicButton;
class CBasicVarTextBox;
class GLGaeaClient;

class	CBasicGameMenu : public CUIGroup
{
protected:
	enum
	{
		MENU_INVENTORY_BUTTON = NO_ID + 1,
		MENU_CHARACTER_BUTTON,
		MENU_SKILL_BUTTON,
		MENU_PARTY_BUTTON,
		MENU_GUILD_BUTTON,
		MENU_QUEST_BUTTON,
		MENU_FRIEND_BUTTON,
		MENU_LARGEMAP_BUTTON,
		MENU_CHATMACRO_BUTTON,
		MENU_ITEMBANK_BUTTON,
		MENU_POINT_SHOP_BUTTON,
        MENU_SNS_BUTTON,
		MENU_RUN_BUTTON,
		
		MENU_CLOSE_BUTTON
	};

	enum
	{
		BASIC_MENU_INVENTORY = 0,
		BASIC_MENU_CHARACTER,
		BASIC_MENU_SKILL,
		BASIC_MENU_PARTY,
		BASIC_MENU_QUEST,
		BASIC_MENU_GUILD,
		BASIC_MENU_FRIEND,
		BASIC_MENU_LARGEMAP,
		BASIC_MENU_CHATMACRO,
		BASIC_MENU_ITEMBANK,
		BASIC_MENU_POINTSHOP,
		BASIC_MENU_SNS,
		BASIC_MENU_RUN,

		BASIC_MENU_NUM,
	};

protected:
	static const FLOAT fBUTTON_GAP_X;
	static const int nMenuTextIndex[BASIC_MENU_NUM];
	static const int nMenuShotcut[BASIC_MENU_NUM];

private:
	int	m_nOverMenu;
	CBasicVarTextBox* m_pInfo;
	CBasicButton* m_pRunButton;
	CString m_ShotcutText[BASIC_MENU_NUM];

public:
    CBasicGameMenu(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CBasicGameMenu ();

protected:
    GLGaeaClient* m_pGaeaClient;

public:
	void CreateSubControl ();
	void SetFlipRunButton( BOOL bRun )			{ m_pRunButton->SetFlip( bRun ); }
	void SetShotcutText ( DWORD nID, CString& strTemp );
	void InitShotCutString();

public:
	virtual void Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

protected:
	CInnerInterface* m_pInterface;
};