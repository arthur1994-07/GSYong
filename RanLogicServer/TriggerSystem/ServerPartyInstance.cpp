#include "pch.h"
#include "ServerPartyInstance.h"
#include "ServerTriggerFinger.h"
#include "ServerPartyTrigger.h"
#include "ServerEntryMessage.h"
#include "ServerExitMessage.h"
#include "ServerStageMessage.h"
#include "ServerPartyOutMessage.h"

#include "../Land/GLLandMan.h"
#include "../Character/GLChar.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

CServerPartyInstance::CServerPartyInstance()
:	m_bAutoOut(false)
{

}

void CServerPartyInstance::Start()
{
	GLLandMan* pLandMan = GetLandMan();

	if (pLandMan)
	{
		pLandMan->CollectPC(m_entrance);
	}

	const CServerPartyTrigger* pTrigger = 0;

	if (CastTrigger(pTrigger))
	{
		m_bAutoOut = pTrigger->GetAutoOut();
	}

	CServerTriggerInstance::Start();
}

void CServerPartyInstance::EntryPC(SMessage* msg)
{
	SServerEntryMessage* entry = 0;

	if (msg->Cast(entry))
	{
		m_entrance.insert(entry->dwCharDbNum);

		sc::writeLogInfo ( sc::string::format( "[ Trigger System ] [ CServerPartyInstance::EntryPC %1% ]", entry->dwCharDbNum ) );
	}
}

void CServerPartyInstance::ExitPC(SMessage* msg)
{
	GLLandMan* pLandMan = GetLandMan();
	if ( NULL == pLandMan)
	{
		return;
	}

	SServerExitMessage* exit = 0;

	if (msg->Cast(exit))
	{
		ExitPartyMember( exit->dwCharDbNum );

		sc::writeLogInfo( sc::string::format(
			"[ Trigger System ] [ CServerPartyInstance::ExitPC %1% ]",
			exit->dwCharDbNum ) );
	}
}

void CServerPartyInstance::PartyOutPC ( SMessage* _pMsg )
{
	GLLandMan* pLandMan = GetLandMan();
	if ( NULL == pLandMan)
	{
		return;
	}

	SServerPartyOutMessage* pPartyOut = 0;

	if ( _pMsg->Cast( pPartyOut ) )
	{
		GLChar* pChar = pLandMan->GetCharByDbNum( pPartyOut->dwCharDbNum );
		if ( NULL == pChar )
		{
			return;
		}

		// 죽어 있다면 살려서 보내야 한다;
		if ( pChar->IsDie() )
		{
			pChar->SetAlive_PVE();
		}

		// 이동시킨다;
		pLandMan->Teleport( pPartyOut->dwCharDbNum );

		// 삭제한다;
		ExitPartyMember( pPartyOut->dwCharDbNum );

		if ( m_entrance.size() <= 1 )
		{
			GLMSG::NET_PVE_RETURN_REQ_FA sNetMsg;
			sNetMsg.sInDunID = pLandMan->GetPVEIndunID();
			sNetMsg.nWhy = GLMSG::STAGE_STATE_GAMEROVER_FAILED;
			SendMsgAgent( NET_MSG_PVE_RETURN_REQ_FA, sNetMsg );

			sc::writeLogInfo( sc::string::format(
				"[ Trigger System ] [ CServerPartyInstance::PartyOutPC %1%, Request to return PVE Instance Dungeon. ]",
				pPartyOut->dwCharDbNum ) );
		}

		sc::writeLogInfo( sc::string::format(
			"[ Trigger System ] [ CServerPartyInstance::PartyOutPC %1% ]",
			pPartyOut->dwCharDbNum ) );
	}
}

void CServerPartyInstance::FrameMove(float fAccTime, float fElapsedTime)
{
	CServerTriggerInstance::FrameMove(fAccTime, fElapsedTime);
}

const bool CServerPartyInstance::ExitPartyMember ( const DWORD _dwChaNum )
{
	GLLandMan* pLandMan = GetLandMan();
	if ( NULL == pLandMan )
	{
		return false;
	}

	CHARDBNUM_LIST_ITER iter = m_entrance.find( _dwChaNum );
	if ( iter != m_entrance.end() )
	{
		m_entrance.erase( _dwChaNum );

		sc::writeLogInfo( sc::string::format(
			"[ Trigger System ] [ CServerPartyInstance::ExitPartyMember %1%, Member Count %2% ]",
			_dwChaNum,
			m_entrance.size() ) );

		return true;
	}

	return false;
}