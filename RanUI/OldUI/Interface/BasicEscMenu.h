//	'ESC' 키 눌렀을때 나오는 메뉴
//
//	최초 작성자 : 성기엽
//	이후 수정자 :
//	로그
//		[2003.12.4]
//			@ 최초 작성

#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

class	CInnerInterface;
class	CBasicButton;
class	CBasicTextButton;

class CBasicEscMenu : public CUIGroup
{
protected:
	enum
	{
//		ESC_MENU_CHARACTER_CHANGE = 1,
		ESC_MENU_SERVER_CHANGE = NO_ID + 1,
		ESC_MENU_HWOPTION,
		ESC_MENU_HELP,
		ESC_MENU_CLOSEGAME,
		ESC_MENU_CLOSE,
		ESC_MENU_KEYSETTING,
		ESC_MENU_GRADE_DISPLAY,
        ESC_MENU_CUSTOMER_CENTER,
		ESC_MENU_OPENFORUM,
		//ESC_MENU_JOIN_PVP_SERVER,
	};

public:
    CBasicEscMenu(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice);
	virtual	~CBasicEscMenu ();

public:
	void	CreateSubControl ();

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CBasicButton*	CreateFlipButton ( char* szButton, char* szButtonFlip, UIGUID ControlID );
	CBasicTextButton*	CreateTextButton ( char* szButton, UIGUID ControlID, char* szText );

protected:
	CInnerInterface* m_pInterface;

};