//TournamentMiniExpeditionWindow
#include "StdAfx.h"

#include "../../../EngineLib/GUInterface/GameTextControl.h"

#include "../../../RanLogic/Tournament/GLTournamentClientInfo.h"
#include "../../../RanLogic/Land/FactionManager/FactionManagerDefine.h"
#include "../../../RanLogic/Land/FactionManager/FactionManager.h"


#include "../../../RanLogicClient/Party/GLPartyManagerClient.h"
#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"

#include "../PartyRenewal/MiniWindow/MiniExpeditionInfoGroup/MiniExpeditionInfoGroup.h"
#include "../PartyRenewal/MiniWindow/MiniExpeditionWindow.h"
#include "TournamentMiniExpeditionWindow.h"
#include "TournamentMiniExpeditionInfoGroup.h"

DWORD TournamentMiniExpeditionWindow::TournamentTapID = MiniExpeditionWindow::EXPEDITION_TAP_5 +1;

TournamentMiniExpeditionWindow::TournamentMiniExpeditionWindow(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: MiniExpeditionWindow(pGaeaClient,pInterface, pEngineDevice), TOR_EXPEDITION_TAP_1(MiniExpeditionWindow::EXPEDITION_TAP_5 +1)
{
	m_bNotFound = true;
}

TournamentMiniExpeditionWindow::~TournamentMiniExpeditionWindow(void)
{
}

void TournamentMiniExpeditionWindow::EventSelectedTap(UIGUID controlID)
{
	if(static_cast<int>(TOR_EXPEDITION_TAP_1) <= controlID && 
		controlID <= static_cast<int>(TOR_EXPEDITION_TAP_1+4))
		m_dwTapPageIndex = controlID - TOR_EXPEDITION_TAP_1;

}

void TournamentMiniExpeditionWindow::TranslateUIMessage( UIGUID ControlID, DWORD dwMsg )
{
	EMTAPSELECTORRESULT emResult = EMTAPSELECTORRESULT_NONE;
	TapSelectorUIMessage( ControlID, dwMsg, emResult );

	CUIWindowEx::TranslateUIMessage( ControlID, dwMsg );
}

void TournamentMiniExpeditionWindow::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{	
	CUIWindowEx::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);

	if(m_isInit == false)
		return;

	MatchingSystem::GLTouranmentClient* glTour = m_pGaeaClient->GetTournamentClient();


	{
		Faction::FactionID _FactionIdInstance(Faction::EMFACTION_TEAM,m_Faction);
		const Faction::FactionMemberVector* FactionMember = 
			m_pGaeaClient->GetLandManClient()->GetFactionManager()->getFactionMembers(_FactionIdInstance);

		if(FactionMember == NULL)
		{
			return;
		}

		SetVisibleSingle(TRUE);

		m_pExpeditionInfoGroup->SetExpeditionSlot(FactionMember, m_dwTapPageIndex);
	}



	
}

void TournamentMiniExpeditionWindow::SetVisibleSingle(BOOL bVisible)
{
	CUIWindowEx::SetVisibleSingle(bVisible);
	if ( bVisible )
	{
		MatchingSystem::GLTouranmentClient* glTour = m_pGaeaClient->GetTournamentClient();

		int factionCounting = 0;
		Faction::FactionID _FactionIdInstance(Faction::EMFACTION_TEAM,m_Faction);
		const Faction::FactionMemberVector* FactionMember = 
			m_pGaeaClient->GetLandManClient()->GetFactionManager()->getFactionMembers(_FactionIdInstance);

		if(FactionMember == NULL)
		{
			return;
		}

		BOOST_FOREACH( Faction::GLActorID actorid, *FactionMember )
		{
			if(actorid.actorType == CROW_PC)
			{
				factionCounting++;
			}
		}
		
		if(factionCounting == 0)
			m_bNotFound = true;

		if(factionCounting > 7)
		{
			SetMiniWindowForExpedition();
		}
		else
		{
			SetMiniWindowForParty();
		}

		m_bNotFound = false;
	}
	else
	{
		m_pExpeditionInfoGroup->SetExpeditionSlot(NULL,0);
	}
}

void TournamentMiniExpeditionWindow::CreateSubControl(void)
{
	CString strExpeditionInfoTap[MAX_PARTYGROUP] = 
	{
		"MINI_EXPEDITION_INFO_TAP_1",
		"MINI_EXPEDITION_INFO_TAP_2",
		"MINI_EXPEDITION_INFO_TAP_3",
		"MINI_EXPEDITION_INFO_TAP_4",
		"MINI_EXPEDITION_INFO_TAP_5",
	};

	TOR_EXPEDITION_TAP_1 = TournamentMiniExpeditionWindow::TournamentTapID;
	for ( DWORD _i = 0; _i < MAX_PARTYGROUP; ++_i )
	{
		CTapSelectorEx::BUTTON* pExpeditionTap = new CTapSelectorEx::BUTTON(m_pEngineDevice);
		pExpeditionTap->CreateSub( this, strExpeditionInfoTap[_i].GetString (), UI_FLAG_DEFAULT, TournamentTapID );
		pExpeditionTap->CreateSubControl( (char*)ID2GAMEWORD("MINI_EXPEDITION_INFO_TAP", _i),
			"MINI_EXPEDITION_INFO_TAP_BASE","PARTY_TAP_ACTIVE",
			"MINI_EXPEDITION_INFO_TAP_BASE","PARTY_TAP_INACTIVE",
			"MINI_EXPEDITION_INFO_TAP_BASE");
		RegisterControl ( pExpeditionTap );
		RegisterTapButton( pExpeditionTap );
		TournamentMiniExpeditionWindow::TournamentTapID ++;
		m_pExpeditionTap[_i] = pExpeditionTap;
	}


	m_pExpeditionInfoGroup = new CTournamentMiniExpeditionInfoGroup(m_pGaeaClient, m_pInterface,m_pEngineDevice );
	m_pExpeditionInfoGroup->CreateSub(this, "MINI_EXPEDITION_INFO_GROUP_REGION");
	m_pExpeditionInfoGroup->CreateSubControl();
	RegisterControl(m_pExpeditionInfoGroup);	
}

void TournamentMiniExpeditionWindow::CreateUIWindowAndRegisterOwnership(WORD wFlag)
{
	//

	CreateBaseWindowLightGray( "MINI_EXPEDITION_WINDOW", (char*)ID2GAMEWORD("PARTY_RENEWAL_INFO_TAP", 1));
	CreateLineBox( "MINI_EXPEDITION_WINDOW_REGION", "PARTY_WINDOW_RENEWAL_TEXTURE_INFO" );
	CreateSubControl();
	SetAlignFlag( wFlag );
	//m_pInterface->UiRegisterControl(this, true);
	//m_pInterface->UiShowGroupFocus( MINI_EXPEDITION_WINDOW );
}
