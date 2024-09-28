#include "StdAfx.h"

#include "../../../../EngineLib/DeviceMan.h"
#include "../../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../../EngineLib/GUInterface/BasicButton.h"
#include "../../../../EngineLib/DxTools/DxFontMan.h"

#include "../../../../RanLogic/GLUseFeatures.h"
#include "../../../../RanLogicClient/GLGaeaClient.h"

#include "../../../InnerInterface.h"
#include "./PartyInfoTap/PartyInfoTap.h"
#include "./ExpeditionInfoTap/ExpeditionInfoTap.h"
#include "PartyWindowRenewal.h"


// ----------------------------------------------------------------------------
#include "../../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

PartyWindowRenewal::PartyWindowRenewal(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: CUIWindowEx(pInterface, pEngineDevice)
, m_pGaeaClient(pGaeaClient)
, m_pCurrentTapWindow(NULL)
{
}

PartyWindowRenewal::~PartyWindowRenewal()
{
}

void PartyWindowRenewal::CreateSubControl()
{
	CreateLineBox( "PARTY_WINDOW_RENEWAL_REGION", "PARTY_WINDOW_RENEWAL_TEXTURE_INFO" );

	m_pPartyInfoTapWindow = new PartyInfoTap(m_pGaeaClient, m_pInterface, m_pEngineDevice);	
	m_pPartyInfoTapWindow->CreateSub(this,"PARTY_RENEWAL_INFO_TAP_WINDOW", UI_FLAG_DEFAULT, PARTY_RENEWAL_INFO_TAP_WINDOW);
	m_pPartyInfoTapWindow->CreateSubControl();
	m_pPartyInfoTapWindow->SetVisibleSingle(FALSE);
	RegisterControl(m_pPartyInfoTapWindow);

	if ( GLUseFeatures::GetInstance().IsUsingExpeditionWindowRenewal() == true )
	{
		m_pExpeditionInfoTapWindow = new ExpeditionInfoTap(m_pGaeaClient, m_pInterface, m_pEngineDevice);	
		m_pExpeditionInfoTapWindow->CreateSub(this,"EXPEDITION_INFO_TAP_WINDOW", UI_FLAG_DEFAULT, EXPEDITION_RENEWAL_INFO_TAP_WINDOW);
		m_pExpeditionInfoTapWindow->CreateSubControl();
		m_pExpeditionInfoTapWindow->SetVisibleSingle(FALSE);
		RegisterControl(m_pExpeditionInfoTapWindow);
	}
	else
		m_pExpeditionInfoTapWindow = NULL;

	CTapSelectorEx::BUTTON* pPartyTap = new CTapSelectorEx::BUTTON(m_pEngineDevice);
	pPartyTap->CreateSub( this, "PARTY_RENEWAL_INFO_TAP", UI_FLAG_DEFAULT, PARTY_RENEWAL_INFO_TAP );
	pPartyTap->CreateSubControl( (char*)ID2GAMEWORD("PARTY_RENEWAL_INFO_TAP", 0),
		"PARTY_TAP_BASE","PARTY_TAP_ACTIVE",
		"PARTY_TAP_BASE","PARTY_TAP_INACTIVE",
		"PARTY_TAP_BASE");

	RegisterControl ( pPartyTap );
	RegisterTapButton( pPartyTap );

	if ( GLUseFeatures::GetInstance().IsUsingExpeditionWindowRenewal() == true )
	{
		CTapSelectorEx::BUTTON* pRaidTap = new CTapSelectorEx::BUTTON(m_pEngineDevice);
		pRaidTap->CreateSub( this, "EXPEDITION_INFO_TAP", UI_FLAG_DEFAULT, EXPEDITION_RENEWAL_INFO_TAP );
		pRaidTap->CreateSubControl( (char*)ID2GAMEWORD("PARTY_RENEWAL_INFO_TAP", 1),
			"PARTY_TAP_BASE","PARTY_TAP_ACTIVE",
			"PARTY_TAP_BASE","PARTY_TAP_INACTIVE",
			"PARTY_TAP_BASE");

		RegisterControl ( pRaidTap );
		RegisterTapButton( pRaidTap );
	}	

	CreateFlipButton( "PARTY_WINDOW_HELP_BUTTON", "INVENTORY_WINDOW_HELP_BUTTON_OVER", PARTY_INFO_HELP_BUTTON, CBasicButton::MOUSEIN_FLIP );
}

void PartyWindowRenewal::SetPartyOption(const SPartyOption& PartyOption)
{
	m_pPartyInfoTapWindow->SetPartyOption(PartyOption);
	if ( m_pExpeditionInfoTapWindow )
		m_pExpeditionInfoTapWindow->SetPartyOption(PartyOption);
}

void PartyWindowRenewal::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	CUIWindowEx::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

}

void PartyWindowRenewal::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
	TapSelectorUIMessage( ControlID, dwMsg, emResult );
	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );

	switch ( ControlID )
	{
	case PARTY_INFO_HELP_BUTTON:
		if( CHECK_MOUSE_IN( dwMsg ) )
		{
			m_pInterface->MultiLineCommentUtil(
				ID2GAMEINTEXT("PARTY_RENEWAL_HELP"),
				m_pEngineDevice->LoadFont( _DEFAULT_FONT, 9, _DEFAULT_FONT_FLAG ),
				800.0f,
				TEXT_ALIGN_LEFT | TEXT_ALIGN_TOP,
				NS_UITEXTCOLOR::LIGHTSKYBLUE );
		}		
		break;
	}
}

void PartyWindowRenewal::SetVisibleSingle(BOOL bVisible)
{
	CUIWindowEx::SetVisibleSingle(bVisible);	
	if ( m_pCurrentTapWindow )
		m_pCurrentTapWindow->SetVisibleSingle(bVisible);		
}

void PartyWindowRenewal::EventSelectedTap(UIGUID controlID)
{
	if ( m_pCurrentTapWindow )
		m_pCurrentTapWindow->SetVisibleSingle(FALSE);

	switch ( controlID )
	{
	case PARTY_RENEWAL_INFO_TAP:
		m_pCurrentTapWindow = m_pPartyInfoTapWindow;	
		break;
	case EXPEDITION_RENEWAL_INFO_TAP:
		if ( GLUseFeatures::GetInstance().IsUsingExpeditionWindowRenewal() == true )
			m_pCurrentTapWindow = m_pExpeditionInfoTapWindow;
		else
			m_pCurrentTapWindow = m_pPartyInfoTapWindow;
		break;
	default:
		return;
	}
	m_pCurrentTapWindow->SetVisibleSingle(TRUE);
}


MyPartyWindowRenewal::MyPartyWindowRenewal( GLGaeaClient* pGaeaCleint, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice )
: PartyWindowRenewal( pGaeaCleint, pInterface, pEngineDevice )
{
}

void MyPartyWindowRenewal::CreateUIWindowAndRegisterOwnership()
{
    Create( PARTY_WINDOW_RENEWAL, "BASIC_WINDOW", UI_FLAG_XSIZE | UI_FLAG_YSIZE );
    CreateBaseWindowLightGray( "PARTY_WINDOW_RENEWAL", (char*)ID2GAMEWORD("PARTY_NAME_STATIC") );		
    CreateSubControl();
    SetAlignFlag( UI_FLAG_CENTER_Y | UI_FLAG_RIGHT );
    m_pInterface->UiRegisterControl( this, true );
    m_pInterface->UiShowGroupFocus( PARTY_WINDOW_RENEWAL );
}

void MyPartyWindowRenewal::SetPartyOption(const SPartyOption& PartyOption)
{
	PartyWindowRenewal::SetPartyOption(PartyOption);
}