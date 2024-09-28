#include "stdafx.h"

#include "../RanLogic/GLUseFeatures.h"

#include "./OldUI/Display/TargetInfoDisplay.h"
#include "./InnerInterface.h"
#include "./OldUI/Tournament/TournamentIndicator.h"
#include "./OldUI/Tournament/TournamentEntranceConfirm.h"
#include "./OldUI/Tournament/TournamentMulty.h"
#include "./OldUI/Tournament/TournamentObserveWindow.h"
//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

void CInnerInterface::SetEntrance(bool bShow,int waitentrance)
{
	if ( !GLUseFeatures::GetInstance().IsUsingTournament() )
		return;

	if ( !m_pEntranceConfirm )
		return;

	m_pEntranceConfirm->SetMapName("MATCHING_CONTENTS_NAME");
	m_pEntranceConfirm->SetTimer(waitentrance);

	if (bShow)
	{
		m_pEntranceConfirm->Open();
	}
	else
	{
		m_pEntranceConfirm->Close();
	}

}

void CInnerInterface::SetEntrance(bool bShow)
{
	if ( !GLUseFeatures::GetInstance().IsUsingTournament() )
		return;

	if ( !m_pEntranceConfirm )
		return;

	m_pEntranceConfirm->SetMapName("MATCHING_CONTENTS_NAME");
	m_pEntranceConfirm->SetTimer(10);

	if (bShow)
	{
		m_pEntranceConfirm->Open();
	}
	else
	{
		m_pEntranceConfirm->Close();
	}

}
int CInnerInterface::SetInstancePopUp(MatchingSystem::InstancePopUpReceipt* receipt)
{
	if ( !GLUseFeatures::GetInstance().IsUsingTournament() )
		return 0;

	if ( !receipt )
		return 0;

	if ( !m_pTournamentObserveWindow )
		return 0;

	if(receipt->isShow)
		UiShowGroupFocus(COMPETITION_TOURNAMENT_OBSERVE);
	else
		UiHideGroup(COMPETITION_TOURNAMENT_OBSERVE);

	m_pTournamentObserveWindow->SetText("OBSERVE_TIMER_TEXT");
	m_pTournamentObserveWindow->SetTimer(receipt->dwTime, receipt->isRevers);
	return 0;
}

void CInnerInterface::SetBigGroupWindowRight(DWORD _factionid)
{	
	if ( !GLUseFeatures::GetInstance().IsUsingTournament() )
		return;

	if ( !m_pBigGroupWindowRight )
		return;

	m_pBigGroupWindowRight->SetFaction(_factionid);
	m_pBigGroupWindowRight->SetVisibleSingle(TRUE);
	UiShowGroupFocus(COMPETITION_TOURNAMENTENT_BIG_RIGHT);
}
void CInnerInterface::SetBigGroupWindowLeft(DWORD _factionid)
{
	if ( !GLUseFeatures::GetInstance().IsUsingTournament() )
		return;

	if ( !m_pBigGroupWindowLeft )
		return;

	m_pBigGroupWindowLeft->SetFaction(_factionid);
	m_pBigGroupWindowLeft->SetVisibleSingle(TRUE);
	UiShowGroupFocus(COMPETITION_TOURNAMENTENT_BIG_LEFT);
}

void CInnerInterface::SetMiniGroupWindowRight(DWORD _factionid)
{
	if ( !GLUseFeatures::GetInstance().IsUsingTournament() )
		return;

	if ( !m_pBigGroupWindowRight )
		return;

	m_pMiniGroupWindowRight->SetFaction(_factionid);
	m_pMiniGroupWindowRight->SetVisibleSingle(TRUE);
	UiShowGroupFocus(COMPETITION_TOURNAMENTENT_MINI_RIGHT);
}

void CInnerInterface::SetMiniGroupWindowLeft(DWORD _factionid)
{
	if ( !GLUseFeatures::GetInstance().IsUsingTournament() )
		return;

	if ( !m_pMiniGroupWindowLeft )
		return;

	m_pMiniGroupWindowLeft->SetFaction(_factionid);
	m_pMiniGroupWindowLeft->SetVisibleSingle(TRUE);
	UiShowGroupFocus(COMPETITION_TOURNAMENTENT_MINI_LEFT);
}

void CInnerInterface::HideAllTournamentWindow()
{
	if ( !GLUseFeatures::GetInstance().IsUsingTournament() )
		return;

	if ( !m_pBigGroupWindowRight )
		return;

	if ( !m_pMiniGroupWindowRight )
		return;

	if ( !m_pTournamentIndicator )
		return;

	m_pBigGroupWindowRight->SetVisibleSingle(FALSE);
	m_pBigGroupWindowLeft->SetVisibleSingle(FALSE);
	m_pMiniGroupWindowRight->SetVisibleSingle(FALSE);
	m_pMiniGroupWindowLeft->SetVisibleSingle(FALSE);
	m_pTournamentIndicator->SetVisibleSingle(FALSE);
	UiHideGroup(COMPETITION_TOURNAMENT_OBSERVE);

}

int CInnerInterface::GetIndicatorTime()
{
	if ( !GLUseFeatures::GetInstance().IsUsingTournament() )
		return 0;

	if (m_pTournamentIndicator)
	{
		return m_pTournamentIndicator->GetTime();
	}
	return 0;
}

int CInnerInterface::SetIndicatorTime(int nSec, bool bBlank, bool bShow)
{
	if ( !GLUseFeatures::GetInstance().IsUsingTournament() )
		return 0;

	if (m_pTournamentIndicator)
	{
		m_pTournamentIndicator->SetTime(nSec,bBlank,bShow);
	}
	return 0;
}

void CInnerInterface::SetStageTimeIndicator( int nSec, bool bBlank, bool bShow )
{
	if ( !GLUseFeatures::GetInstance().IsUsingTournament() )
		return;

	if (m_pTournamentIndicator)
	{
		return m_pTournamentIndicator->SetTime(nSec,bBlank,bShow);
	}

	//UiShowGroupFocus( RNCDM_CLUB_INFO );
}


void CInnerInterface::SetTournamentIndicatorType( DWORD ObserverType , DWORD factionid1, DWORD factionid2)
{
	if ( !GLUseFeatures::GetInstance().IsUsingTournament() )
		return;

	MatchingSystem::GLTouranmentClient*  pclient = 
		m_pGaeaClient->GetTournamentClient();
	if ( !pclient )
		return;

	switch(ObserverType)
	{
	case MatchingSystem::TOURNAMENT_UI_ONE_BY_ONE:

		UiShowGroupFocus(COMPETITION_TOURNAMENTENT_INDICATOR);
		//2의 의미는 게임에서 편이 2개라고 표현하는거
		if ( m_pTournamentIndicator )
			m_pTournamentIndicator->SetOneByOne();
		break;

	case MatchingSystem::TOURNAMENT_UI_N_BY_N:

		UiShowGroupFocus(COMPETITION_TOURNAMENTENT_INDICATOR);

		if(pclient->GetTournamentViewType() == MatchingSystem::TOURNAMENT_OBSERVER)
		{
			SetMiniGroupWindowLeft(factionid1);
			SetMiniGroupWindowRight(factionid2);
		}

		break;

    case MatchingSystem::TOURNAMENT_UI_TEAM_VS_TEAM:
        {
            pclient->m_dwObserveType = ObserverType;

			if ( m_pTournamentIndicator )
				m_pTournamentIndicator->SetTeamVSTeam();
            UiShowGroupFocus(COMPETITION_TOURNAMENTENT_INDICATOR);
        }
        break;

	case MatchingSystem::TOURNAMENT_UI_LEAGE_SCORE:
		UiShowGroupFocus(COMPETITION_TOURNAMENT_MULTY);
		break;

	default:
		break;
	}


	//UiShowGroupFocus( RNCDM_CLUB_INFO );
}

void CInnerInterface::SetTournamentIndicatorInfo( const NET_MSG_GENERIC* const _pMessage )
{
	if ( !GLUseFeatures::GetInstance().IsUsingTournament() )
		return;

	if ( !m_pTournamentIndicator )
		return;

    m_pTournamentIndicator->SetTeamVsTeamInfo(_pMessage);
}