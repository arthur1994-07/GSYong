#include "StdAfx.h"
#include "EscMenuOpen.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CEscMenuOpen::CEscMenuOpen(CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
    : CUIGroup(pEngineDevice)
    , m_pInterface(pInterface)
{
}

CEscMenuOpen::~CEscMenuOpen ()
{
}

void CEscMenuOpen::CreateSubControl ()
{
	CBasicButton* pButton = new CBasicButton(m_pEngineDevice);
	pButton->CreateSub ( this, "ESCMENU_OPEN_BUTTON", UI_FLAG_DEFAULT, ESCMENU_OPEN_BUTTON );
	pButton->CreateFlip ( "ESCMENU_OPEN_BUTTON_F", CBasicButton::MOUSEIN_FLIP );
	pButton->SetUseGlobalAction ( TRUE );
	RegisterControl ( pButton );
}

void CEscMenuOpen::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case ESCMENU_OPEN_BUTTON:
		{
			if ( CHECK_MOUSEIN_LBUPLIKE ( dwMsg ) )
			{
				if( !m_pInterface->UiIsVisibleGroup ( ESC_MENU ) )
				{
					m_pInterface->UiShowGroupFocus ( ESC_MENU );
				}
				else
				{
					m_pInterface->UiHideGroup ( ESC_MENU );
				}
			}
		}
		break;
	}
}
