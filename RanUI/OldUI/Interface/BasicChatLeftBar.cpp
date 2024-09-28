#include "StdAfx.h"
#include "BasicChatLeftBar.h"
#include "../Interface/ChatType.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CBasicChatLeftBar::CBasicChatLeftBar(EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
{
}

CBasicChatLeftBar::~CBasicChatLeftBar ()
{
}

void CBasicChatLeftBar::CreateSubControl ()
{
	CUIControl* pTop = new CUIControl(m_pEngineDevice);
	pTop->CreateSub ( this, "CHAT_LEFT_BAR_TOP", UI_FLAG_DEFAULT, CHAT_LEFT_BAR_TOP );
	pTop->SetProtectSizePoint ();
	RegisterControl ( pTop );

	CUIControl* pBody = new CUIControl(m_pEngineDevice);
	pBody->CreateSub ( this, "CHAT_LEFT_BAR_BODY", UI_FLAG_YSIZE );
	pBody->SetProtectSizePoint ();
	RegisterControl ( pBody );

	CUIControl* pBottom = new CUIControl(m_pEngineDevice);
	pBottom->CreateSub ( this, "CHAT_LEFT_BAR_BOTTOM", UI_FLAG_BOTTOM );
	pBottom->SetProtectSizePoint ();
	RegisterControl ( pBottom );

	CUIControl* pChatState = new CUIControl(m_pEngineDevice);
	pChatState->CreateSub ( this, "CHAT_CHANGE_STATE", UI_FLAG_BOTTOM, CHAT_CHANGE_STATE );
	RegisterControl ( pChatState );
	m_pChatState = pChatState;
}

void CBasicChatLeftBar::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case CHAT_LEFT_BAR_TOP:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( CHECK_LB_DOWN_LIKE ( dwMsg ) )
				{
					AddMessageEx ( UIMSG_MOUSEIN_LEFTBAR_DRAG );
				}
			}
		}
		break;

	case CHAT_CHANGE_STATE:
		{
			if ( CHECK_MOUSE_IN ( dwMsg ) )
			{
				if ( UIMSG_LB_UP & dwMsg )
				{
					AddMessageEx( UIMSG_MOUSEIN_CHAT_TYPE_CHANGE );
				}
			}			
		}
		break;
	}
}

const UIRECT& CBasicChatLeftBar::GetChangStateButtonGlobalPos ()
{
    if ( !m_pChatState )
    {
        GASSERT( !"CBasicChatLeftBar::GetChangStateButtonGlobalPos - m_pChatState is NULL" );
    }

    return m_pChatState->GetGlobalPos();
}

