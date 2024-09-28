#include "StdAfx.h"

#include "../../../EngineLib/DeviceMan.h"
#include "../../../EngineLib/DxTools/DxFontMan.h"
#include "../../../EngineLib/GUInterface/BasicTextBox.h"
#include "../../../EngineLib/GUInterface/SwapImage.h"
#include "../../../EngineLib/GUInterface/BasicProgressBar.h"
#include "../../../EngineLib/GUInterface/UITextControl.h"
#include "../../../EngineLib/GUInterface/GameTextControl.h"
#include "../../../EngineLib/GUInterface/BasicTextButton.h"

#include "../../../RanLogicClient/GLGaeaClient.h"
#include "../../../RanLogicClient/Tournament/GLTournamentClient.h"
#include "../Competition/PVEEntranceConfirm.h"
#include "TournamentEntranceConfirm.h"
#include "../Competition/UIProgressTimer.h"

// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CTournamentEntranceConfirm::CTournamentEntranceConfirm(GLGaeaClient* pGaeaClient, CInnerInterface* pInterface, EngineDeviceMan* pEngineDevice) :
	CPVEEntranceConfirm(pGaeaClient,pInterface,pEngineDevice)
{
	
}

CTournamentEntranceConfirm::~CTournamentEntranceConfirm()
{
}

void CTournamentEntranceConfirm::SetMapName(const TCHAR* pMapName)
{
	if (m_pText)
	{
		TCHAR temp[256] = {0};
		_stprintf_s(temp, 256, ID2GAMEINTEXT("COMPETITION_PVE_ENREANCE_TEXT", 0), ID2GAMEWORD(pMapName, 0));
		m_pText->SetText(temp, NS_UITEXTCOLOR::WHITE);
	}
}

void CTournamentEntranceConfirm::OnOk()
{
    if (m_pGaeaClient)
    {
        MatchingSystem::GLTouranmentClient* m_client = m_pGaeaClient->GetTournamentClient();
	    if(m_client != NULL)
	    {
		   m_client->AskJoinAnswer(true);
		}

		if (m_pInterface)
			m_pInterface->OpenTournamentJoinWindow(false);
    }

    Close();
}

void CTournamentEntranceConfirm::OnCancel()
{
    if (m_pGaeaClient)
    {
        MatchingSystem::GLTouranmentClient* m_client = m_pGaeaClient->GetTournamentClient();
		if(m_client != NULL)
		{
			m_client->AskJoinAnswer(false);
		}
		if (m_pInterface)
			m_pInterface->OpenTournamentJoinWindow(false);
    }

    Close();
}


void CTournamentEntranceConfirm::Close()
{
	if (m_pInterface)
	{
		//SetVisibleSingle(FALSE);
		m_pInterface->UiHideGroup(COMPETITION_TOURNAMENTENTRANCECONFIRM);
	}
}

void CTournamentEntranceConfirm::Open()
{
	if (m_pInterface)
	{
		//SetVisibleSingle(TRUE);
		m_pInterface->UiShowGroupFocus(COMPETITION_TOURNAMENTENTRANCECONFIRM);
	}
}
