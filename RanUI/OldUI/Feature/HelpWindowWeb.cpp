#include "StdAfx.h"
#include "HelpWindowWeb.h"

#include "../../../EngineLib/DxTools/RENDERPARAM.h"
#include "../../../EngineLib/Common/SubPath.h"
#include "../../../RanLogic/RANPARAM.h"
#include "../../../RanLogic/GLUseFeatures.h"
#include "../../InnerInterface.h"

#include "../../../EngineLib/Common/CommonWeb.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CHelpWindowWeb::CHelpWindowWeb( CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CWebWindowBase( pInterface, pEngineDevice )
{
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
	{
		SetAddress( RANPARAM::MultiHelpAddress[RANPARAM::emProvideLangFlag] );
	}
	else
	{
		SetAddress( RANPARAM::HelpAddress );
	}
}

void CHelpWindowWeb::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CWebWindowBase::TranslateUIMessage ( ControlID, dwMsg );

	switch ( ControlID )
	{
	case ET_CONTROL_TITLE:
	case ET_CONTROL_TITLE_F:
		{
			if ( (dwMsg & UIMSG_LB_DUP) && CHECK_MOUSE_IN ( dwMsg ) )
			{
				m_pInterface->SetDefaultPosInterface( HELP_WINDOW );
			}
		}
		break;	
	}
}