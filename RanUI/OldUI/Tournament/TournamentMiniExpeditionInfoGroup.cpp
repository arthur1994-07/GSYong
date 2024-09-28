#include "StdAfx.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"

#include "../../../RanLogic/Tournament/GLTournamentClientInfo.h"
#include "../../../RanLogic/Land/FactionManager/FactionManagerDefine.h"
#include "../../../RanLogic/Land/FactionManager/FactionManager.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Party/GLPartyClient.h"
#include "../../../RanLogicClient/Char/GLCharacter.h"
#include "../../../RanLogicClient/Char/GLCharClient.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"

#include "../../InnerInterface.h"

#include "../Party/MiniPartyWindow.h"
#include "../Party/MiniPartySlot.h"
#include "../Party/PartyBuffInfo.h"
#include "../PartyRenewal/MiniWindow/MiniExpeditionInfoGroup/MiniExpeditionInfoGroup.h"

#include "TournamentMiniExpeditionInfoGroup.h"
#include "TournamentExpeditionInfoPartySlot.h"
// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CTournamentMiniExpeditionInfoGroup::CTournamentMiniExpeditionInfoGroup(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice)
: MiniExpeditionInfoGroup(pEngineDevice,pInterface,pGaeaClient )
{
}

CTournamentMiniExpeditionInfoGroup::~CTournamentMiniExpeditionInfoGroup ()
{
}

void CTournamentMiniExpeditionInfoGroup::CreateSubControl(void)
{
	MiniExpeditionInfoGroup::CreateSubControl();

	CString strRaidInfoPartySlot[MAXPARTY] = 
	{
		"EXPEDITION_INFO_PARTY_SLOT0",
		"EXPEDITION_INFO_PARTY_SLOT1",
		"EXPEDITION_INFO_PARTY_SLOT2",
		"EXPEDITION_INFO_PARTY_SLOT3",
		"EXPEDITION_INFO_PARTY_SLOT4",
		"EXPEDITION_INFO_PARTY_SLOT5",
		"EXPEDITION_INFO_PARTY_SLOT6",
		"EXPEDITION_INFO_PARTY_SLOT7",
	};


	for ( unsigned int _i = 0; _i < MAXPARTY; _i++ )
	{
		m_pTournamentExpeditionInfoMemberSlot[_i] = new TournamentExpeditionInfoPartySlot(m_pEngineDevice, _i,m_pGaeaClient);
		m_pTournamentExpeditionInfoMemberSlot[_i]->CreateSub( this, strRaidInfoPartySlot[_i].GetString (), UI_FLAG_DEFAULT, TOURNAMENT_INFO_PARTY_SLOT0 + _i );
		m_pTournamentExpeditionInfoMemberSlot[_i]->CreateSubControl();
		RegisterControl(m_pTournamentExpeditionInfoMemberSlot[_i]);
		m_pTournamentExpeditionInfoMemberSlot[_i]->SetVisibleSingle(FALSE);
		m_pTournamentExpeditionInfoMemberSlot[_i]->RESET();
	}

	//m_pInterface->AddNonCloseUI( GetWndID() );



}	


void CTournamentMiniExpeditionInfoGroup::Update ( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{
	const UIRECT& rcGlobalPos = GetGlobalPos();
	m_pPartyBuffInfo->SetLocalPos(D3DXVECTOR2(x + 25 - rcGlobalPos.left, y + 25 - rcGlobalPos.top));
	m_pPartyBuffInfo->SetVisibleSingle(FALSE);

	CUIGroupHelper::Update( x, y, LB, MB, RB, nScroll, fElapsedTime, bFirstControl );

}




void CTournamentMiniExpeditionInfoGroup::SetExpeditionSlot( const Faction::FactionMemberVector* pTeamInfo, DWORD dwPartyIndex )
{


	for ( DWORD i = MAXPARTY; i; --i )		
		m_pTournamentExpeditionInfoMemberSlot[i - 1]->SetVisibleSingle(FALSE);	

	//슬롯이 NULL이면 안되지!
	if ( pTeamInfo == NULL )
		return;

	for ( DWORD i = 0; i < MAXPARTY; ++i )
	{
		m_pTournamentExpeditionInfoMemberSlot[i]->RESET();
	}

	int factionCounting = 0;
	BOOST_FOREACH(Faction::GLActorID actorid, *pTeamInfo )
	{
		if(factionCounting > 7)
			return;

		if(actorid.actorType == CROW_PC)
		{
			m_pTournamentExpeditionInfoMemberSlot[factionCounting]->SetInformation(actorid.actorID_Num);
		}
		factionCounting++;
	}
	SetVisibleSingle(TRUE);
}

void CTournamentMiniExpeditionInfoGroup::TranslateUIMessage ( UIGUID ControlID, DWORD dwMsg )
{
	switch ( ControlID )
	{
	case TOURNAMENT_INFO_PARTY_SLOT0:
	case TOURNAMENT_INFO_PARTY_SLOT1:
	case TOURNAMENT_INFO_PARTY_SLOT2:
	case TOURNAMENT_INFO_PARTY_SLOT3:
	case TOURNAMENT_INFO_PARTY_SLOT4:
	case TOURNAMENT_INFO_PARTY_SLOT5:
	case TOURNAMENT_INFO_PARTY_SLOT6:
	case TOURNAMENT_INFO_PARTY_SLOT7:
		{
			const DWORD dwSlotIndex = DWORD(ControlID - TOURNAMENT_INFO_PARTY_SLOT0);


			DWORD factionSize = 0;

			MatchingSystem::GLTouranmentClient* glTour = m_pGaeaClient->GetTournamentClient();
			// 게임 정보를 찾는다.
			MatchingSystem::TournamentGameInfo* pTeamInfo = glTour->GetNowTeamInfo();
			DWORD TeamSize  = 0;


			if(pTeamInfo == NULL)
			{
				return;
			}

			// 가져온 팀정보로 다시 검색을 때린다.
			std::vector<MatchingSystem::TournamentTeamInfo*>::iterator _biter = pTeamInfo->Team.begin();
			MatchingSystem::TournamentTeamInfo* pTeamGameInfo = NULL;

			for ( ;_biter != pTeamInfo->Team.end(); _biter++)
			{
				MatchingSystem::TournamentTeamInfo* pTeamGameInfoT = NULL;
				pTeamGameInfoT = *_biter;

				// 해당 팀정보 가져온다!
				if(pTeamGameInfo->TeamFactionID == m_dwFaction)
				{
					pTeamGameInfo = pTeamGameInfoT;
					break;
				}
			}

			if(pTeamGameInfo == NULL)
				break;

			DWORD CountStart = m_dwPartyIndex * MAXPARTY;

			if(pTeamGameInfo->TeamGaeaID.size() <= CountStart)
				break;

			DWORD pParty = pTeamGameInfo->TeamGaeaID[m_dwPartyIndex];

			if ( CHECK_MOUSE_IN(dwMsg) )
			{
				m_dwSelectedSlot = dwSlotIndex;
				m_pPartyBuffInfo->SetPartyID(dwSlotIndex, m_dwPartyIndex);
				m_pPartyBuffInfo->SetVisibleSingle(TRUE);
			}

			if ( CHECK_MOUSEIN_RBUPLIKE(dwMsg) && (m_dwPartyIndex != DEFSLOT_NULL) )
			{
				STARGETID sTargetID(CROW_PC, pParty);
				m_pGaeaClient->GetCharacter()->ReqSkillReaction(sTargetID);
			}
		}
		break;
	}
}