#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"

#include "../../InnerInterface.h"
#include "../Util/UIWindowEx.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "PostBoxWindow.h"
#include "PostBoxWindowMain.h"
#include "PostBoxWindowSub.h"
#include "PostBoxWindowNaked.h"

#include "PostBoxReceivePage.h"
#include "PostBoxReadPage.h"
#include "PostBoxSendPage.h"
#include "PostBoxFriendFindPage.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogic/GLogicData.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CPostBoxWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case POSTBOX_WINDOW_MAIN:
		{
			// Note : ∏ﬁ¿Œ ≈«º±≈√
			if ( dwMsg&UIMSG_POSTBOX_WINDOW_MAIN_TAP_CHANGE )
			{
				if ( !m_pWindow_Main )
				{
					break;
				}

				RefreshMainPage();
			}

			// Note : ¥›±‚
			if ( dwMsg&UIMSG_POSTBOX_WINDOW_OBJECT_CLOSE )
			{
				WindowPageClose ( POSTBOX_WINDOW_MAIN );
			}
		}
		break;
	
	case POSTBOX_WINDOW_SUB:
		{
			// Note : ¥›±‚
			if ( dwMsg&UIMSG_POSTBOX_WINDOW_OBJECT_CLOSE )
			{
				WindowPageClose ( POSTBOX_WINDOW_SUB );
			}
		}
		break;

	case POSTBOX_WINDOW_NAKED:
		{
			// Note : ¥›±‚
			if ( dwMsg&UIMSG_POSTBOX_WINDOW_OBJECT_CLOSE )
			{
				WindowPageClose ( POSTBOX_WINDOW_NAKED );
			}
		}
		break;
	};
}

