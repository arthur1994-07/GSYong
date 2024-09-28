#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicLineBoxSmart.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/TriggerSystem/ClientMapInfoMessage.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../../../RanLogic/InstanceSystem/GLInstanceSystemDefine.h"
#include "../../../RanLogic/MatchSystem/MatchModel.h"

#include "TournamentUserInterface.h"
#include "../Competition/PVEEntranceConfirm.h"
#include "../Competition/PVERetry.h"
#include "../Competition/PVEEntranceState.h"
#include "../Competition/PVEBoard.h"
#include "../Competition/PVEIndicator.h"
#include "../Competition/PVEResult.h"
#include "../Competition/PVEPage.h"
#include "../Tournament/TournamentIndicator.h"
#include "../Tournament/TournamentEntranceConfirm.h"
#include "../Tournament/TournamentMulty.h"
#include "../Tournament/TournamentObserveWindow.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CTournamentUserInterface::CTournamentUserInterface(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
CUIGroup(pEngineDevice)
,	m_pGaeaClient(pGaeaClient)
,	m_pInterface(pInterface)
,   m_bVisibleStageTimeIndicator(false)
,   m_bVisibleStageIndicator(false)
,   m_bVisibleMonsterIndicator(false)
{

}

CTournamentUserInterface::~CTournamentUserInterface()
{

}

bool CTournamentUserInterface::CreateSubControl()
{
	return true;
}


void CTournamentUserInterface::Update( int x, int y, BYTE LB, BYTE MB, BYTE RB, int nScroll, float fElapsedTime, BOOL bFirstControl )
{



}

void CTournamentUserInterface::SetStageTimeIndicator(int nSec, bool bBlank, bool bShow)
{

}

void CTournamentUserInterface::SetStageIndicator(int nStage, int nEnd, int nRange, bool bShow)
{

}

void CTournamentUserInterface::SetMonsterIndicator(int nRem, int nAll, bool bShow)
{

}

void CTournamentUserInterface::SetPositioningIndicator(int nX, int nY, int nNow, int nNeed, bool bShow)
{

}

void CTournamentUserInterface::SetVisibleSingle(BOOL bVisible)
{
	if (bVisible == false)
	{
		if (m_bVisibleStageTimeIndicator)
		{
			SetStageTimeIndicator(0, false, bVisible);
		}

		if (m_bVisibleStageIndicator)
		{
			SetStageIndicator(0, 0, 0, bVisible);
		}

		if (m_bVisibleMonsterIndicator)
		{
			SetMonsterIndicator(0, 0, bVisible);
		}
	}

	CUIGroup::SetVisibleSingle(bVisible);
}

MyTournamentUserInterface::MyTournamentUserInterface(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
CTournamentUserInterface(pGaeaClient, pInterface, pEngineDevice)
{

}

void MyTournamentUserInterface::CreateUIWindowAndRegisterOwnership()
{
	Create(COMPETITION_TOURNAMENTENTUI, _T("COMPETITION_PVE_UI"), UI_FLAG_XSIZE | UI_FLAG_YSIZE);
	CreateSubControl();
	SetAlignFlag(UI_FLAG_CENTER_Y | UI_FLAG_RIGHT);
	m_pInterface->UiRegisterControl(this);
}

void MyTournamentUserInterface::ToggleShow(bool bShow)
{
	if (m_pInterface)
	{
		if (bShow)
		{
			if (m_bVisibleStageTimeIndicator || m_bVisibleStageIndicator || m_bVisibleMonsterIndicator )
			{
				m_pInterface->UiShowGroupTop(COMPETITION_TOURNAMENTENTUI);
			}
		}
		else
		{
			if (!m_bVisibleStageTimeIndicator && !m_bVisibleStageIndicator && !m_bVisibleMonsterIndicator )
			{
				m_pInterface->UiHideGroup(COMPETITION_TOURNAMENTENTUI);
			}
		}
	}
}

void MyTournamentUserInterface::SetEntranceConfirm(bool bShow,int waitentrance)
{
	m_pInterface->SetEntrance(bShow,waitentrance);
}

void MyTournamentUserInterface::SetEntranceConfirm(bool bShow)
{
	m_pInterface->SetEntrance(bShow);
}
void MyTournamentUserInterface::SetMonsterIndicator(int nCur, int nAll, bool bShow)
{
	CTournamentUserInterface::SetMonsterIndicator(nCur, nAll, bShow);
	
	ToggleShow(bShow);
}

void MyTournamentUserInterface::SetPositioningIndicator(int nX, int nY, int nNow, int nNeed, bool bShow)
{
	CTournamentUserInterface::SetPositioningIndicator(nX, nY, nNow, nNeed, bShow);
	ToggleShow(bShow);
}

void MyTournamentUserInterface::SetStageIndicator(int nSec, bool bBlank, bool bShow)
{
	m_fElapsedTime = (float)nSec;
	CTournamentUserInterface::SetStageTimeIndicator(nSec, bBlank, bShow);

	
	//
	ToggleShow(bShow);
}
void MyTournamentUserInterface::SetStageState(int nState, bool bShow)
{

	ToggleShow(bShow);
}
void MyTournamentUserInterface::AllClose()
{
	m_fElapsedTime = 0;
	CTournamentUserInterface::SetVisibleSingle(FALSE);
	CTournamentUserInterface::SetVisibleSingle(TRUE);
}

void MyTournamentUserInterface::SetCloseAll()
{
	AllClose();
}
