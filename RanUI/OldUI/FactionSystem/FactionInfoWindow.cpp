#include "stdafx.h"

#include "./FactionInfoWindow.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"

#include "../../../SigmaCore/DebugInclude.h"

#include "../../../RanLogicClient/Land/Faction/FactionManagerClient.h"

#include "../../InnerInterface.h"

CFactionInfoWindow::CFactionInfoWindow ( GLGaeaClient* pClient
									   , CInnerInterface* pInterface
									   , EngineDeviceMan* pEngineDevice )
									   : CUIWindowEx( pInterface, pEngineDevice )
									   , m_pClient ( pClient )
{
}


void CFactionInfoWindow::CreateSubControl ( )
{
	CreateLineBox( "LOTTO_MAIN_UI_OUTLINE_REGION", "RAN_UI_BASIC_BACKGROUND_1" );

}


void CFactionInfoWindow::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}


void CFactionInfoWindow::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}


void CFactionInfoWindow::SetVisibleSingle ( BOOL bVisible )
{
	CUIWindowEx::SetVisibleSingle( bVisible );
}





MyFactionInfoWindow::MyFactionInfoWindow ( GLGaeaClient* pClient
										 , CInnerInterface* pInterface
										 , EngineDeviceMan* pEngineDevice )
										 : CFactionInfoWindow ( pClient
										 , pInterface
										 , pEngineDevice )
{

}


void MyFactionInfoWindow::CreateUIWindowAndRegisterOwnership ( )
{
	CFactionInfoWindow::Create( FACTION_INFO_WINDOW, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
	CFactionInfoWindow::CreateBaseWindowLightGray( "LOTTO_MAIN_UI", (char*)ID2GAMEINTEXT("LOTTO_MAIN_UI_TITLE_TEXT") );
	CFactionInfoWindow::CreateSubControl();
	CFactionInfoWindow::SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );

	if ( CFactionInfoWindow::m_pInterface )
	{
		CFactionInfoWindow::m_pInterface->UiRegisterControl( this, true );
		CFactionInfoWindow::m_pInterface->UiShowGroupFocus( FACTION_INFO_WINDOW );
	}
}