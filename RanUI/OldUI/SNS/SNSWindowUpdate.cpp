
// bjju.sns

#include "StdAfx.h"


#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/BasicLineBox.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogic/GLogicData.h"
#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../InnerInterface.h"
#include "../Util/UIWindowEx.h"
#include "../Util/UIWindowObject.h"

#include "./SNSWindow.h"
#include "./SNSMenuPage.h"
#include "./SNSFacebookPage.h"
#include "./SNSTwitterPage.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void CSNSWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowObjectController::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CSNSWindow::UpdatePageRequest	( UIGUID ControlID, const SPAGEREQUEST& sRequest )
{
	switch ( sRequest.dwID )
	{
	case SNS_PAGE_REQ_MENU_PAGE_OPEN:
		{
			m_pUIObject->PageOpen( m_pPage_Menu );
		}
		break;

	case SNS_PAGE_REQ_FACEBOOK_PAGE_OPEN:
		{
			m_pUIObject->PageOpen( m_pPage_Facebook );
		}
		break;

	case SNS_PAGE_REQ_TWITTER_PAGE_OPEN:
		{
			m_pUIObject->PageOpen( m_pPage_Twitter );
		}
		break;
	};
}
