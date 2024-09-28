#include "stdafx.h"
#include "./InnerInterface.h"

#include "../RanLogic/GLCompetition.h"

#include "./OldUI/Competition/CompetitionNotifyButton.h"
#include "./OldUI/Competition/CompetitionWindow.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

void CInnerInterface::CompetitionNotifyButtonUpdate()
{
    if ( m_pCompetitionNotifyButton )
    {
        m_pCompetitionNotifyButton->Update();
    }
}

bool CInnerInterface::CompetitionWindowIsOpen()
{
	if ( GLUseFeatures::GetInstance().IsUsingNewInstance() == false )
	{
		if ( m_pCompetitionWindow )
		{
			return m_pCompetitionWindow->IsOpen();
		}
	}
	else
	{	
		if ( m_pRnCompetitionWindow )
		{
			return m_pRnCompetitionWindow->IsOpen();
		}
	}

	return false;
}

void CInnerInterface::CompetitionWindowOpen()
{
	if( m_pGaeaClient && m_pGaeaClient->IsCharPrison() ) 
	{
		CompetitionWindowClose(); 
		m_pGaeaClient->PrintMsgText( NS_UITEXTCOLOR::DISABLE, ID2GAMEINTEXT( "UI_RECEIVE_PRISION_LIMIT", 0 ) );
		return;
	}

	if ( GLUseFeatures::GetInstance().IsUsingNewInstance() == false )
	{
		if ( m_pCompetitionWindow )
		{
			if ( m_pCompetitionNotifyButton )
				m_pCompetitionNotifyButton->ClearAlarm();
			m_pCompetitionWindow->Open();
		}
	}
	else
	{
		if ( m_pRnCompetitionWindow )
		{
			if ( m_pCompetitionNotifyButton )
				m_pCompetitionNotifyButton->ClearAlarm();
			m_pRnCompetitionWindow->Open();
		}
	}
}

void CInnerInterface::CompetitionWindowClose()
{
	if ( GLUseFeatures::GetInstance().IsUsingNewInstance() == false )
	{
		if ( m_pCompetitionWindow )
		{
			m_pCompetitionWindow->Close();
		}
	}
	else
	{
		if ( m_pRnCompetitionWindow )
		{
			m_pRnCompetitionWindow->Close();
		}
	}
}

void CInnerInterface::CompetitionCTFUpdate()
{
	if ( m_pCompetitionWindow && m_pCompetitionWindow->IsOpen() )
	{
		m_pCompetitionWindow->Update(CCompetitionWindow::COMPETITION_PAGE_CTF);
	}
}

const UIRECT& CInnerInterface::GetCompetitionWindowPos()
{
	if ( m_pCompetitionWindow )
	{
		return m_pCompetitionWindow->GetGlobalPos();
	}

	if ( m_pRnCompetitionWindow )
	{
		return m_pRnCompetitionWindow->GetGlobalPos();
	}

	static UIRECT rcRet(0.0f,0.0f,0.0f,0.0f);
	return rcRet;
}

const UIRECT& CInnerInterface::GetTournamentDashBoardWindowPos(UIGUID ControlID)
{
	switch(ControlID)
	{
	case TOURNAMENT_MORE_INFO:
		if ( m_pTournamentMoreInfoWindow )
		{
			return m_pTournamentMoreInfoWindow->GetGlobalPos();
		}
		break;
	case TOURNAMENT_DASHBOARD:
		if ( m_pTournamentDashBoard )
		{
			return m_pTournamentDashBoard->GetGlobalPos();
		}
		break;
	case TOURNAMENT_UI:
		break;
	case TOURNAMENT_TOTO:
		if ( m_pTournamentToToWindow )
		{
			return m_pTournamentToToWindow->GetGlobalPos();
		}
		break;
	}
	

	static UIRECT rcRet(0.0f,0.0f,0.0f,0.0f);
	return rcRet;
}

void CInnerInterface::OpenTournamentJoinWindow(bool _Open, int Time )
{
	if(m_pTournamentUserInterface)
	{
		m_pTournamentUserInterface->SetEntranceConfirm(_Open,Time);
	}
}

void CInnerInterface::OpenTournamentJoinWindow(bool _Open)
{
	if(m_pTournamentUserInterface)
	{
		m_pTournamentUserInterface->SetEntranceConfirm(_Open);
	}
}

void CInnerInterface::CompetitionTopRankingOpen( const PVP::ENUM_CTF_TYPE emType )
{
	if ( m_pCompetitionTopRanking )
	{
		m_pCompetitionTopRanking->Open( emType );
	}
}

void CInnerInterface::CompetitionTopRankingClose()
{
	if ( m_pCompetitionTopRanking )
	{
		m_pCompetitionTopRanking->Close();
	}
}

void CInnerInterface::CompetitionTopRankingSetTextTime( const CString strText )
{
	if ( m_pCompetitionTopRanking )
	{
		m_pCompetitionTopRanking->SetTextTime(strText);
	}
}

void CInnerInterface::CompetitionNotifyButtonSetLock(const bool _bLock)
{
	if ( m_pCompetitionNotifyButton )
		m_pCompetitionNotifyButton->SetLock(_bLock);
}

void CInnerInterface::CompetitionNotifyButtonClearAlarm()
{
	if ( m_pCompetitionNotifyButton )
		m_pCompetitionNotifyButton->ClearAlarm();
}

void CInnerInterface::CompetitionNotifyButtonSetAlarm(const DWORD _dwMatchingNum, const char* szText) 
{
	if ( m_pCompetitionNotifyButton )
		m_pCompetitionNotifyButton->SetAlarm(_dwMatchingNum, szText);
}

void CInnerInterface::CompetitionNotifyButtonSetTooltip(const DWORD _dwMatchingNum, const char* szText)
{
	if ( m_pCompetitionNotifyButton )
		m_pCompetitionNotifyButton->SetTooltip(_dwMatchingNum, szText);
}