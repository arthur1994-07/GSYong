#include "pch.h"
#include "./GLGaeaClient.h"
#include "./PVE/GLPVEClient.h"
#include "../RanLogic/Msg/PVEMsg.h"
#include "../RanLogic/Msg/GLContrlInstanceMsg.h"
#include "./Char/GLCharacter.h"
#include "./Pet/GLPetClient.h"
#include "./Pet/GLPetClient.h"
#include "./Land/GLLandManClient.h"

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLGaeaClient::ReqMapTable()
{
    if (m_pPVEClient)
    {
        m_pPVEClient->ReqMapTable();
    }
}

int GLGaeaClient::GetThisStage()
{
	if (m_pPVEClient)
	{
		return m_pPVEClient->GetThisStage();
	}
	
	return -1;
}

void GLGaeaClient::ReqPVEEntrance(const DWORD dwCharDbNum, const DWORD dwMapID, const DWORD dwGateID, const int nLevel)
{
    if (m_pPVEClient)
    {
        m_pPVEClient->ReqEntrance(dwCharDbNum, dwMapID, dwGateID, nLevel);
    }
}

void GLGaeaClient::ReqPVEEntranceConfirm(bool bOk)
{
    if (m_pPVEClient)
    {
        m_pPVEClient->ReqPVEEntranceConfirm(bOk);
    }
}

void GLGaeaClient::ReqPVERetry(bool bOk)
{
    if (m_pPVEClient)
    {
        m_pPVEClient->ReqPVERetry(bOk);
    }
}

void GLGaeaClient::ReqPVEExit()
{
    if (m_pPVEClient)
    {
        m_pPVEClient->ReqPVEExit();
    }
}

void GLGaeaClient::ReqPVEClearOfBelonging(DWORD dwCharDbNum, DWORD dwMapID, int nLevel)
{
    if (m_pPVEClient)
    {
        m_pPVEClient->ReqClearOfBelonging(dwCharDbNum, dwMapID, nLevel);
    }
}

bool GLGaeaClient::GetPVEEntranceMapInfo(TriggerSystem::SClientTriggerMapInfo*& info)
{
    if (m_pPVEClient)
    {
        return m_pPVEClient->GetEntranceMapInfo(info);
    }

    return false;
}

void GLGaeaClient::GetPVEStageResult(GLMSG::StageResult& results, int& nStage, bool& bLast, int& nTimer)
{
    if (m_pPVEClient)
    {
        m_pPVEClient->GetStageResult(results, nStage, bLast, nTimer);
    }
}

void GLGaeaClient::PVERebirth(DWORD dwGaeaID)
{
	if (m_pCharacter && 
		m_pCharacter->GetGaeaID() == dwGaeaID)
	{
		if (m_pPet->IsVALID())
		{
			m_pPet->Delete();
			m_pCharacter->m_bIsPetActive = TRUE;
		}

		if (m_pCharacter->VehicleState())
		{
			m_pCharacter->ReqSetVehicle(false);
			m_pCharacter->SetVehicle(false);
			m_pCharacter->m_bIsVehicleActive = TRUE;
		}

		D3DXVECTOR3 vPos = m_pCharacter->GetPositionActor();
		WORD wNowMP = static_cast<WORD>(m_pCharacter->GETMAXMP());
		WORD wNowSP = static_cast<WORD>(m_pCharacter->GETMAXSP());
		WORD wNowHP = static_cast<WORD>(m_pCharacter->GETMAXHP());

		m_pCharacter->SetActState(EM_ACT_DIE);

		m_pCharacter->ResetAction();
		m_pCharacter->InitializeActorMove(vPos);
		
//		GLWidgetScript::GetInstance().LuaCallFunc< void >( NSWIDGET_SCRIPT::g_strFunc_LargeMap_HideMoveIcon );

		//!! SF_TODO
		//m_pInterface->SetReviveSkill(false);

		m_pCharacter->Resurrect(wNowHP, wNowMP, wNowSP, vPos, false);
	}
	else
	{
		std::tr1::shared_ptr<GLCharClient> spChar = GetChar(dwGaeaID);

		if (spChar == 0)
		{
			return;
		}

		D3DXVECTOR3 vPos = spChar->GetPositionActor();
		WORD wNowMP = static_cast<WORD>(spChar->GETMAXMP());
		WORD wNowSP = static_cast<WORD>(spChar->GETMAXSP());
		WORD wNowHP = static_cast<WORD>(spChar->GETMAXHP());

		spChar->InitializeActorMove(vPos);
		spChar->Resurrect(wNowHP, wNowMP, wNowSP, vPos);
	}
}

void GLGaeaClient::SendTriggerMessage(TriggerSystem::SMessage* msg)
{
	if (m_pPVEClient)
	{
		m_pPVEClient->SendMessage(msg);
	}
}


/// 신규 인던;
void GLGaeaClient::ReqNewInstanceRetry ( bool bOk, InstanceSystem::InstanceMapID nInstanceID, SNATIVEID sKeyMapID )
{
	GLMSG::NET_RETRY_FACTION_CF sMsg( nInstanceID, sKeyMapID );
	sMsg.bOK = bOk;

	NETSENDTOFIELD( &sMsg );
	
}