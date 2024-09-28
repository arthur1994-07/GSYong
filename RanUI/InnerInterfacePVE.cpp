#include "stdafx.h"
#include "./InnerInterface.h"

#include "./OldUI/Competition/CompetitionWindow.h"
#include "./OldUI/Competition/PVEUserInterface.h"

//-----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
//-----------------------------------------------------------------------------

void CInnerInterface::UpdatePVEMapTable()
{
    if (m_pCompetitionWindow)
    {
        m_pCompetitionWindow->Update(CCompetitionWindow::COMPETITION_PAGE_PVE);
    }

	if (m_pRnCompetitionWindow)
	{
		m_pRnCompetitionWindow->UpDateWdilBuinding();
	}
}

void CInnerInterface::SetPVEEntranceConfirm(bool bShow)
{
    if (m_pPVEUserInterface)
    {
        m_pPVEUserInterface->SetEntranceConfirm(bShow);
    }
}

void CInnerInterface::SetPVEEntranceState(int nCur, int nAll, int nSec, bool bShow)
{
    if (m_pPVEUserInterface)
    {
        m_pPVEUserInterface->SetEntranceState(nCur, nAll, nSec, bShow);
    }
}

void CInnerInterface::SetPVEStage(int nStage, bool bShow)
{
    if (m_pPVEUserInterface)
    {
        m_pPVEUserInterface->SetStage(nStage, bShow);
    }
}

void CInnerInterface::SetPVEMessage(const TCHAR* pText, bool bShow)
{
    if (m_pPVEUserInterface)
    {
        m_pPVEUserInterface->SetMessage(pText, bShow);
    }
}

void CInnerInterface::SetPVEMonsterIndicator(int nCur, int nAll, bool bShow)
{
    if (m_pPVEUserInterface)
    {
        m_pPVEUserInterface->SetMonsterIndicator(nCur, nAll, bShow);
    }
}

void CInnerInterface::SetPVEPositioningIndicator(int nX, int nY, int nNow, int nNeed,  bool bShow)
{
    if (m_pPVEUserInterface)
    {
        m_pPVEUserInterface->SetPositioningIndicator(nX, nY, nNow, nNeed, bShow);
    }
}

void CInnerInterface::SetPVETimeIndicator( int nSec, bool bShow )
{
	if (m_pPVEUserInterface)
	{
		m_pPVEUserInterface->SetTimeIndicator( nSec, bShow );
	}
}

void CInnerInterface::SetPVEStageIndicator(int nStage, int nEnd, int nRange, int nSec, bool bBlank, bool bShow)
{
    if (m_pPVEUserInterface)
    {
        m_pPVEUserInterface->SetStageIndicator(nStage, nEnd, nRange, nSec, bBlank, bShow);
    }
}

void CInnerInterface::SetPVEStageResult()
{
    if (m_pPVEUserInterface)
    {
        m_pPVEUserInterface->SetStageResult();
    }
}

void CInnerInterface::SetPVEStageState(int nState, bool bShow)
{
    if (m_pPVEUserInterface)
    {
        m_pPVEUserInterface->SetStageState(nState, bShow);
    }
}

void CInnerInterface::SetPVERetry(LONGLONG nMoney, int nTime, bool bShow)
{
    if (m_pPVEUserInterface)
    {
        m_pPVEUserInterface->SetRetry(nMoney, nTime, bShow);
    }
}

void CInnerInterface::SetNewInstanceRetry ( LONGLONG nMoney, int nTime, bool bShow, InstanceSystem::InstanceMapID nInstanceID, SNATIVEID sKeyMapID )
{
	if (m_pPVEUserInterface)
	{
		m_pPVEUserInterface->SetNewInstanceRetry(nMoney, nTime, bShow, nInstanceID, sKeyMapID);
	}
}