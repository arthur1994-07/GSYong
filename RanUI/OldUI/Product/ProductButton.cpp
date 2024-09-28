#include "StdAfx.h"
#include "./ProductButton.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CProductButton::CProductButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroup( pEngineDevice )
    , m_pGaeaClient( pGaeaClient )
    , m_pInterface( pInterface )
{
}

void CProductButton::CreateSubControl()
{
	CBasicButton* pButton = new CBasicButton( m_pEngineDevice );
	pButton->CreateSub( this, "PRODUCT_BUTTON_IMAGE", UI_FLAG_DEFAULT, ID_PRODUCT_BUTTON );
    pButton->SetUseGlobalAction( FALSE );
    RegisterControl( pButton );
}

void CProductButton::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    switch( ControlID )
    {
    case ID_PRODUCT_BUTTON:
        {   
            if( CHECK_MOUSE_IN( dwMsg ) )
            {
				m_pInterface->SHOW_COMMON_LINEINFO(
					ID2GAMEINTEXT( "PRODUCT_BUTTON_MESSAGE" ),
					NS_UITEXTCOLOR::WHITE );

                if( CHECK_LB_UP_LIKE( dwMsg ) )
                {
					m_pInterface->ToggleWindowProduct();
                }
            }
        }
		break;
    }
}

MyProductButton::MyProductButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
	: CProductButton( pGaeaClient, pInterface, pEngineDevice )
{
}

void MyProductButton::CreateUIWindowAndRegisterOwnership()
{
    Create( PRODUCT_BUTTON, "PRODUCT_BUTTON", UI_FLAG_RIGHT | UI_FLAG_BOTTOM );
    CreateSubControl();
    m_pInterface->UiRegisterControl( this );
    m_pInterface->UiShowGroupBottom( GetWndID() );
}