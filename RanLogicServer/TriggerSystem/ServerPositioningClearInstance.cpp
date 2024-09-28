#include "pch.h"
#include "ServerPositioningClearInstance.h"
#include "ServerPositioningClearTrigger.h"
#include "ServerTriggerFinger.h"
#include "ServerExitMessage.h"
#include "ServerEntryMessage.h"
#include "ServerStageMessage.h"
#include "ServerPartyOutMessage.h"

#include "../Land/GLLandMan.h"
#include "../Character/GLChar.h"
#include "../../RanLogic/Msg/PVEMsg.h"

#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

CServerPositioningClearInstance::CServerPositioningClearInstance()
:   m_completed(false)
,	m_nNow(0)
,	m_count(0)
{

}

void CServerPositioningClearInstance::Start()
{
    GLLandMan* pLandMan = GetLandMan();

    if (pLandMan)
    {
        pLandMan->CollectPC(m_entrance);
    }

	Parsing();
    Update();
    CServerTriggerInstance::Start();
}

void CServerPositioningClearInstance::FrameMove(float fAccTime, float fElapsedTime)
{
    CServerTriggerInstance::FrameMove(fAccTime, fElapsedTime);

    bool bCheck = Check();
    if (bCheck)
    {
		m_completed = true;
		Update();

		SServerMissionCompletedMessage msg;
		msg.eType = MISSION_POSITININGCLEAR;
		SendMessage(&msg);

        Exit();
    }
}

void CServerPositioningClearInstance::EntryPC(SMessage* msg)
{
    SServerEntryMessage* entry = 0;

    if (msg->Cast(entry))
    {
        m_entrance.insert(entry->dwCharDbNum);
        Update(entry->dwCharDbNum);
    }
}

void CServerPositioningClearInstance::ExitPC(SMessage* msg)
{
    SServerExitMessage* exit = 0;

    if (msg->Cast(exit))
    {		
		// RM #871 ¼öÁ¤
		//m_completed = true;

		Update(exit->dwCharDbNum);

		CHARDBNUM_LIST_ITER iter = m_entrance.find( exit->dwCharDbNum );
		if ( iter != m_entrance.end() )
		{
			m_entrance.erase(exit->dwCharDbNum);
		}
    }
}

void CServerPositioningClearInstance::PartyOutPC ( SMessage* _pMsg )
{
	SServerPartyOutMessage* pPartyOut = 0;

	if ( _pMsg->Cast( pPartyOut ) )
	{
		CHARDBNUM_LIST_ITER iter = m_entrance.find( pPartyOut->dwCharDbNum );
		if ( iter != m_entrance.end() )
		{
			m_entrance.erase( pPartyOut->dwCharDbNum );
		}
	}
}

bool CServerPositioningClearInstance::Check()
{
    GLLandMan* pLandMan = GetLandMan();

    if (pLandMan == 0)
    {
        return false;
    }

    int nCount = 0;

    for (CHARDBNUM_LIST_ITER it = m_entrance.begin(); it != m_entrance.end(); ++it)
    {
        GLChar* pChar = pLandMan->GetCharByDbNum(*it);

        if (pChar && InMinMax(pChar->GetPosition()))
        {
            if (++nCount >= m_count)
            {
                m_nNow = nCount;
                return true;
            }
        }
    }

	if ( m_nNow != nCount )
	{
		m_nNow = nCount;

		Update();
	}
	
    return false;
}

bool CServerPositioningClearInstance::InMinMax(const D3DXVECTOR3& pos)
{
	GLLandMan* pLandMan = GetLandMan();

	if (pLandMan == 0)
	{
		return false;
	}

	const GLMapAxisInfo& sMapAxisInfo = pLandMan->GetMapAxisInfo();

	int nX = 0, nY = 0;

	sMapAxisInfo.Convert2MapPos(pos.x, pos.z, nX, nY);

    if (nX < floor(m_min.x) || nY < floor(m_min.y))
    {
        return false;
    }

    if (nX > floor(m_max.x) || nY > floor(m_max.y))
    {
        return false;
    }

    return true;
}

void CServerPositioningClearInstance::Update(DWORD dwCharDbNum)
{
    GLMSG::NET_PVE_POSITIONINGCLEAR_STATE_FC msg;
    msg.nX = (int)(m_minipos.x);
    msg.nY = (int)(m_minipos.y);
    msg.nNow = m_nNow;
    msg.nNeed = m_count;
	msg.bCompleted = m_completed;
    SendMsgClient(dwCharDbNum, NET_MSG_PVE_POSITIONINGCLEAR_STATE_FC, msg);
}

void CServerPositioningClearInstance::Parsing()
{
	const CServerPositioningClearTrigger* pTrigger = 0;

	if (CastTrigger(pTrigger))
	{
		m_count = pTrigger->GetCount();
		m_min = pTrigger->GetMin();
		m_max = pTrigger->GetMax();
		m_minipos.x = ((m_min.x + m_min.x) * 0.5f);
		m_minipos.y = ((m_max.y + m_max.y) * 0.5f);
	}
}
