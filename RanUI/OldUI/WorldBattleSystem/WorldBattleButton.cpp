#include "stdafx.h"

#include "./WorldBattleButton.h"

#include "../../../SigmaCore/String/StringFormat.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/InterfaceBaseDefine.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../RanLogic/GLUseFeatures.h"
#include "../../../RanLogic/RANPARAM.h"

#include "../../InnerInterface.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CWorldBattleButton::CWorldBattleButton ( GLGaeaClient* pClient
									   , CInnerInterface* pInterface
									   , EngineDeviceMan* pEngineDevice )
									   : CUIGroup ( pEngineDevice )
									   , m_pClient ( pClient )
									   , m_pInterface ( pInterface )
									   , m_pWorldBattleImage ( NULL )
{

}

CWorldBattleButton::~CWorldBattleButton ( ) { }

void CWorldBattleButton::CreateSubControl ( )
{
	// 기본 버튼 UI;
	m_pWorldBattleImage = new CSwapImage( m_pEngineDevice );
	m_pWorldBattleImage->CreateSub( this, "WORLD_BATTLE_BUTTON_REGION", UI_FLAG_DEFAULT, EMWORLD_BATTLE_BUTTON );
	if ( GLUseFeatures::GetInstance().IsUsingJoinWorldBattle() )
	{
		m_pWorldBattleImage->SetImage( "BATTLE_SERVER_BUTTON" );
		m_pWorldBattleImage->SetVisibleSingle( TRUE );
		RegisterControl( m_pWorldBattleImage );
	}
	else if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
	{
		m_pWorldBattleImage->SetImage( "EXIT_BATTLE_SERVER_BUTTON" );
		m_pWorldBattleImage->SetVisibleSingle( TRUE );
		RegisterControl( m_pWorldBattleImage );
	}
	else
	{
		m_pWorldBattleImage->SetVisibleSingle( FALSE );
		RegisterControl( m_pWorldBattleImage );
	}
}

void CWorldBattleButton::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIGroup::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );
}

void CWorldBattleButton::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	CUIGroup::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case EMWORLD_BATTLE_BUTTON:
		if ( CHECK_MOUSE_IN ( dwMsg ) )
		{
			m_pInterface->SHOW_COMMON_LINEINFO(
				m_strTooltip.c_str(),
				NS_UITEXTCOLOR::WHITE,
				TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP );         

			if ( CHECK_LB_UP_LIKE ( dwMsg ) )
			{
				// 전장 입장 서버일 경우 전장 서버로 입장한다;
				if ( GLUseFeatures::GetInstance().IsUsingJoinWorldBattle() )
				{
					m_pInterface->DoModal(
						ID2GAMEINTEXT( "CONFIRM_JOIN_PVP_SERVER" ),
						UI::MODAL_QUESTION,
						UI::YESNO,
						UI::MODAL_CONFIRM_JOIN_PVP_SERVER );
				}
				// 전장 서버일 경우 전장서버에서 나간다;
				else if ( GLUseFeatures::GetInstance().IsUsingWorldBattle() )
				{
					if ( RANPARAM::bCheckAllDelItemsRebuyPage )
					{
						int nRebuyItemCount =
							m_pInterface->GetDialogueWindowRebuyItemCount();

						if ( nRebuyItemCount <= 0 )
						{
							m_pInterface->DoModal(
								ID2GAMEINTEXT( "MODAL_ESC_SERVER_STAGE" ),
								UI::MODAL_QUESTION,
								UI::YESNO,
								UI::MODAL_SERVER_STAGE );
						}
						else
						{
							std::string strCheckRebuyItems =
								sc::string::format( ID2GAMEINTEXT( "MODAL_ESC_SERVER_STAGE_CHECK_REBUY_ITEMS" ),
								nRebuyItemCount );

							m_pInterface->DoModal( 
								strCheckRebuyItems.c_str(),
								UI::MODAL_QUESTION,
								UI::YESNO,
								UI::MODAL_SERVER_STAGE );
						}
					}
					else
					{
						m_pInterface->DoModal(
							ID2GAMEINTEXT( "MODAL_ESC_SERVER_STAGE" ),
							UI::MODAL_QUESTION,
							UI::YESNO,
							UI::MODAL_SERVER_STAGE );
					}
				}
			}
		}
		break;
	}
}

MyWorldBattleButton::MyWorldBattleButton ( GLGaeaClient* pClient
										 , CInnerInterface* pInterface
										 , EngineDeviceMan* pEngineDevice )
										 : CWorldBattleButton( pClient
										 , pInterface
										 , pEngineDevice )
{

}

void MyWorldBattleButton::CreateUIWindowAndRegisterOwnership ( )
{
	CWorldBattleButton::Create( WORLD_BATTLE_JOIN_BUTTON, "WORLD_BATTLE_BUTTON", UI_FLAG_CENTER_X | UI_FLAG_BOTTOM );
	CWorldBattleButton::CreateSubControl();
	CWorldBattleButton::m_pInterface->UiRegisterControl( this );
	CWorldBattleButton::m_pInterface->UiShowGroupBottom( WORLD_BATTLE_JOIN_BUTTON );
}

bool MyWorldBattleButton::IsVisible ( )
{
	return CWorldBattleButton::IsVisible();
}

void MyWorldBattleButton::ToggleWorldBattleButton (bool bOn)
{
	CWorldBattleButton::SetVisibleSingle( bOn );
}
