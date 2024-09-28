#include "pch.h"
#include "ServerRecognizeInstance.h"
#include "ServerRecognizeTrigger.h"
#include "ServerEntryMessage.h"

#include "../Land/GLLandMan.h"
#include "../Character/GLChar.h"
#include "../../RanLogic/Msg/PVEMsg.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

CServerRecognizeInstance::CServerRecognizeInstance()
:	m_time(0.f)
{

}

void CServerRecognizeInstance::Start()
{
	const CServerRecognizeTrigger* pTrigger = 0;

	if (CastTrigger(pTrigger))
	{
		m_time = pTrigger->GetRecognizeTime();
	}

    CServerTriggerInstance::Start();
}

void CServerRecognizeInstance::EntryPC(SMessage* msg)
{
    SServerEntryMessage* entry = 0;

    if (msg->Cast(entry))
    {
		GLLandMan* pLandMan = GetLandMan();

		if (pLandMan)
		{
			GLChar* pChar = pLandMan->GetCharByDbNum(entry->dwCharDbNum);

			if (pChar)
			{				
				pChar->SetMoveDelay(m_time);
			}
		}
    }
}
