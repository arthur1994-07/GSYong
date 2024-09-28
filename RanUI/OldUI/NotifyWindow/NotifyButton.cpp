#include "StdAfx.h"
#include "./NotifyButton.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/GUInterface/NSGUI.h"
#include "../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../../../RanLogicClient/GLGaeaClient.h"

#include "../../../RanLogicClient\Notify\NotifyClientBase.h"
#include "../../../RanLogicClient\Notify\NotifyClientMan.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CNotifyButton::CNotifyButton( GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
    : CUIGroup( pEngineDevice )
    , m_pGaeaClient( pGaeaClient )
    , m_pInterface( pInterface )
    , m_pButtonActiveImg( NULL )
    , m_fTimer( ALARM_TIME )
{
}

void CNotifyButton::CreateSubControl()
{
	CBasicButton* pButton = new CBasicButton( m_pEngineDevice );
	pButton->CreateSub( this, "NOTIFY_ICON_NORMAL", UI_FLAG_DEFAULT, ID_NOTIFY_BUTTON );
    pButton->SetUseGlobalAction( FALSE );
    RegisterControl( pButton );

    m_pButtonActiveImg = new CBasicButton( m_pEngineDevice );
    m_pButtonActiveImg->CreateSub( this, "NOTIFY_ICON_ACTIVE", UI_FLAG_DEFAULT );
    m_pButtonActiveImg->SetUseGlobalAction( FALSE );
    RegisterControl( m_pButtonActiveImg );


    m_pNotiftMan = m_pGaeaClient->GetNotifyClientMan();
}

void CNotifyButton::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
    CUIGroup::TranslateUIMessage( ControlID, dwMsg );
    switch( ControlID )
    {
    case ID_NOTIFY_BUTTON:
        {   
            if( CHECK_MOUSE_IN( dwMsg ) )
            {
                NSGUI::SetCharMoveBlock();

				m_pInterface->SHOW_COMMON_LINEINFO(
					ID2GAMEINTEXT( "NOTIFY_BUTTON_MESSAGE" ),
					NS_UITEXTCOLOR::WHITE );

                if( CHECK_LB_UP_LIKE( dwMsg ) )
                {
                    if( m_pInterface->UiIsVisibleGroup( NOTIFY_WINDOW ) )
                        m_pInterface->UiHideGroup( NOTIFY_WINDOW );
                    else
                    {
                        m_fTimer = ALARM_TIME;
                        m_pInterface->UiShowGroupFocus( NOTIFY_WINDOW );
                    }
                }
            }
			else
			{
            	if( CHECK_LB_UP_LIKE( dwMsg ) )
                {
                    if( ALARM_TIME <= m_fTimer )
			    	    m_pInterface->UiHideGroup( NOTIFY_WINDOW );
                }
				
			}

        }
		break;
    }
}

void CNotifyButton::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
    CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
    
    if( m_pButtonActiveImg == NULL )
        return;


    if( m_fTimer < ALARM_TIME )
    {
        if( m_pNotiftMan != NULL )
            if( m_pNotiftMan->GetData().size() <= 0 )
                m_fTimer = ALARM_TIME;

        m_fTimer += fElapsedTime*1000;

        int nTemp = (int)m_fTimer % (ALARM_FLASH_CYCLE+ALARM_FLASH_TIME);
        if( nTemp < ALARM_FLASH_CYCLE )
            m_pButtonActiveImg->SetVisibleSingle( FALSE );
        else
            m_pButtonActiveImg->SetVisibleSingle( TRUE );

    }
    else
        m_pButtonActiveImg->SetVisibleSingle( FALSE );

}

void CNotifyButton::SetNotifyAlarm()
{
    m_fTimer = 1;
}
