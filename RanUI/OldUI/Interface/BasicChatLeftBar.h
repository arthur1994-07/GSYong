#pragma once

#include "../../../EngineLib/GUInterface/UIGroup.h"

////////////////////////////////////////////////////////////////////
//	사용자 메시지 정의
const DWORD UIMSG_MOUSEIN_LEFTBAR_DRAG		= UIMSG_USER1;
const DWORD UIMSG_MOUSEIN_CHAT_TYPE_CHANGE	= UIMSG_USER2;
////////////////////////////////////////////////////////////////////

class CBasicChatLeftBar : public CUIGroup
{
public:
	enum
	{
		CHAT_LEFT_BAR_TOP = 1,
		CHAT_CHANGE_STATE, 
	};

public:
	CBasicChatLeftBar(EngineDeviceMan* pEngineDevice);
	virtual	~CBasicChatLeftBar ();

public:
	void	CreateSubControl ();

public:
    const UIRECT& GetChangStateButtonGlobalPos ();

public:
	virtual	void TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg );

private:
	CUIControl*	m_pChatState;
};