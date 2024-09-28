#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../../../RanLogic/Tournament/GLTournamentClientInfo.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Party/GLPartyClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../../RanLogicClient/Land/GLLandManClient.h"



#include "../PartyRenewal/MainWindow/ExpeditionInfoTap/ExpeditionInfoPartySlot/ExpeditionInfoGroup.h"
#include "../PartyRenewal/MainWindow/ExpeditionInfoTap/ExpeditionInfoPartySlot/ExpeditionInfoMemberSlot/ExpeditionInfoMemberSlot.h"
#include "../PartyRenewal/MainWindow/ExpeditionInfoTap/ExpeditionInfoPartySlot/ExpeditionInfoGroup.h"
#include "../PartyRenewal/StateWindow/MemberStateGroup/MemberStateGroup.h"
#include "../PartyRenewal/StateWindow/MemberStateGroup/MemberStateSlot/MemberStateSlot.h"

#include "../../InnerInterface.h"
#include "TournamentMiniExpeditionInfoGroup.h"
#include "../Party/MiniPartyWindow.h"
#include "../Party/MiniPartySlot.h"
#include "../Party/PartyBuffInfo.h"
#include "TournamentExpeditionInfoGroup.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CTournamentExpeditionInfoGroup::CTournamentExpeditionInfoGroup(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: MemberStateGroup(pGaeaClient,pInterface,pEngineDevice),m_Faction(-1),m_pGaeaClient(pGaeaClient)
{

	
}

void CTournamentExpeditionInfoGroup::Update(int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl)
{
	CUIGroupHelper::Update(x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl);

	m_vMousePosition = D3DXVECTOR2(float(x), float(y));
	for ( DWORD _i = MAXPARTY; _i; --_i )
		m_pMemberSlot[_i - 1]->RESET();

	Faction::FactionID _FactionIdInstance(Faction::EMFACTION_TEAM,m_Faction);
	const Faction::FactionMemberVector* FactionMember = 
		m_pGaeaClient->GetLandManClient()->GetFactionManager()->getFactionMembers(_FactionIdInstance);

	if ( FactionMember == NULL )
		return;

	int factionCounting = 0;

	BOOST_FOREACH(Faction::GLActorID actorid, *FactionMember )
	{
		if(factionCounting > 7)
			return;

		if(actorid.actorType == EMCROW(CROW_PC))
		{
			if( m_pGaeaClient->GetCharacter()->CharDbNum() == actorid.actorID_Num)
			{
				GLCharacter* spChar = m_pGaeaClient->GetCharacter();

				if(spChar == NULL)
					return;

				m_pMemberSlot[factionCounting]->SetMemberStateSlot( spChar );
				factionCounting++;
			}
			else
			{
				std::tr1::shared_ptr<GLCharClient> pPartyClient = m_pGaeaClient->GetCharByDBnum(actorid.actorID_Num);

				if(pPartyClient == NULL)
					return;

				m_pMemberSlot[factionCounting]->SetMemberStateSlot( pPartyClient );
				factionCounting++;
			}
		}
	}
}

void CTournamentExpeditionInfoGroup::SetFaction( DWORD _factionid )
{
	m_Faction = _factionid;
	MatchingSystem::GLTouranmentClient* glTour = m_pGaeaClient->GetTournamentClient();

	Faction::FactionID _FactionIdInstance(Faction::EMFACTION_TEAM,m_Faction);
	const Faction::FactionMemberVector* FactionMember = 
		m_pGaeaClient->GetLandManClient()->GetFactionManager()->getFactionMembers(_FactionIdInstance);

	if(FactionMember == NULL)
	{
		return;
	}

	SetVisibleSingle(TRUE);
	
}

void CTournamentExpeditionInfoGroup::TranslateUIMessage(UIGUID ControlID, DWORD dwMsg)
{
	switch ( ControlID )
	{
	case MEMBER_STATE_SLOT0:
	case MEMBER_STATE_SLOT1:
	case MEMBER_STATE_SLOT2:
	case MEMBER_STATE_SLOT3:
	case MEMBER_STATE_SLOT4:
	case MEMBER_STATE_SLOT5:
	case MEMBER_STATE_SLOT6:
	case MEMBER_STATE_SLOT7:
		{
			const DWORD dwSlotIndex = DWORD(ControlID - MEMBER_STATE_SLOT0);
			if ( CHECK_MOUSEIN_RBUPLIKE(dwMsg) )
			{
				Faction::FactionID _FactionIdInstance(Faction::EMFACTION_TEAM,m_Faction);
				const Faction::FactionMemberVector* FactionMember = 
					m_pGaeaClient->GetLandManClient()->GetFactionManager()->getFactionMembers(_FactionIdInstance);

				if ( FactionMember == NULL )
					return;

				int factionCounting = 0;

				
				MatchingSystem::GLTouranmentClient* glTour = m_pGaeaClient->GetTournamentClient();

				if(glTour->GetTournamentViewType() != MatchingSystem::TOURNAMENT_OBSERVER)
					return;
				

				BOOST_FOREACH(Faction::GLActorID actorid, *FactionMember )
				{
					if(factionCounting >= 7)
						return;

					if(actorid.actorType == EMCROW(CROW_PC))
					{
						if(factionCounting == dwSlotIndex)
						{
							if( m_pGaeaClient->GetCharacter()->CharDbNum() == actorid.actorID_Num)
							{
								GLCharacter* spChar = m_pGaeaClient->GetCharacter();
								//GLCharClient* pPartyClient;
								STARGETID sTarID(CROW_PC, spChar->GetGaeaID(), spChar->GetPosition());
								m_pGaeaClient->SetChaseCam(sTarID,true);

							}
							else
							{
								std::tr1::shared_ptr<GLCharClient> pPartyClient = m_pGaeaClient->GetCharByDBnum(actorid.actorID_Num);
								//GLCharClient* pPartyClient;
								STARGETID sTarID(CROW_PC, pPartyClient->GetGaeaID(), pPartyClient->GetPosition());
								m_pGaeaClient->SetChaseCam(sTarID,true);
							}
						}
							
					}
					factionCounting++;
				}

				
				//관전 드가자.
			}
		}
	}	
}
