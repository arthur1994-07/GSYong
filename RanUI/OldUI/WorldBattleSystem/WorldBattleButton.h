#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"
#include "../../InnerInterface.h"

#include "./IWorldBattleUI.h"

class CInnerInterface;
class EngieDeviceMan;
class GLGaeaClient;

class CSwapImage;
class CBasicTextBox;

// World Battle 입장 버튼;
class CWorldBattleButton : public CUIGroup
{
private:
	enum
	{
		EMWORLD_BATTLE_BUTTON		=	NO_ID +1
	};

public:
	CWorldBattleButton ( GLGaeaClient* pClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~CWorldBattleButton ( );

public:
	void				CreateSubControl();
private:
	virtual void		Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl );
	virtual void		TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	std::string			m_strTooltip;

private:
	CSwapImage*			m_pWorldBattleImage;

protected:
	CInnerInterface*	m_pInterface;
	GLGaeaClient*		m_pClient;
};

class MyWorldBattleButton : public IWorldBattleButton, public CWorldBattleButton
{
public:
	MyWorldBattleButton ( GLGaeaClient* pClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice );
	virtual ~MyWorldBattleButton ( ) { };

public:
	virtual void CreateUIWindowAndRegisterOwnership();

	virtual bool IsVisible();

	virtual void ToggleWorldBattleButton( bool bOn );
};