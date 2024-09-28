#include "pch.h"
#include "ServerReadyInstance.h"
#include "ServerReadyTrigger.h"
#include "ServerTriggerFinger.h"
#include "ServerExitMessage.h"
#include "ServerEntryMessage.h"
#include "ServerPartyOutMessage.h"

#include "../Land/GLLandMan.h"
#include "../Character/GLChar.h"

#include "../../RanLogic/Msg/PVEMsg.h"
#include "../../RanLogic/FSM/GLCharMovementID.h"


#include "../SigmaCore/DebugInclude.h"

using namespace TriggerSystem;

void CServerReadyInstance::Start()
{
	const CServerReadyTrigger* pTrigger = 0;

	if (CastTrigger(pTrigger))
	{
		m_all = pTrigger->GetCount();
	}

    GLLandMan* pLandMan = GetLandMan();

    for (CHARDBNUM_LIST_ITER it = m_entrance.begin(); it != m_entrance.end(); ++it)
    {
        GLChar* pChar = pLandMan->GetCharByDbNum(*it);

        if( pChar == NULL )
            continue;

        pChar->SetAlive();
    }

	CServerTriggerInstance::Start();
}

void CServerReadyInstance::SetCur(DWORD dwChaDbNum)
{
	++m_cur;
    m_entrance.insert(dwChaDbNum);	
    
    m_bUpdate = true;
    
}

void CServerReadyInstance::RemoveCur(DWORD dwChaDbNum)
{
	CHARDBNUM_LIST_ITER iter = m_entrance.find( dwChaDbNum );
	if ( iter != m_entrance.end() )
	{
		SetMovement(dwChaDbNum, true);

		--m_cur;
		m_entrance.erase(dwChaDbNum);
		m_bUpdate = true;
	}
}

void CServerReadyInstance::FrameMove(float fAccTime, float fElapsedTime)
{
    CServerTriggerInstance::FrameMove(fAccTime, fElapsedTime);

    if (m_bUpdate)
    {
        Update();
        m_bUpdate = false;
    }
}

void CServerReadyInstance::Exit()
{
    Update();

    // ������ �Ұ��� ����
    for (CHARDBNUM_LIST_ITER it = m_entrance.begin(); it != m_entrance.end(); ++it)
    {
        SetMovement(*it, true);
    }

    CServerTriggerInstance::Exit();
}

void CServerReadyInstance::EntryPC(SMessage* msg)
{
    SServerEntryMessage* entry = 0;

    if (msg->Cast(entry))
    {
		SetMovement(entry->dwCharDbNum, false);
        SetCur(entry->dwCharDbNum);
    }
}

void CServerReadyInstance::ExitPC(SMessage* msg)
{
    SServerExitMessage* exit = 0;

    if (msg->Cast(exit))
    {
        RemoveCur(exit->dwCharDbNum);
    }
}

void CServerReadyInstance::PartyOutPC ( SMessage* _pMsg )
{
	SServerPartyOutMessage* pPartyOut = 0;

	if ( _pMsg->Cast( pPartyOut ) )
	{
		RemoveCur( pPartyOut->dwCharDbNum );
	}
}

void CServerReadyInstance::SetMovement(DWORD dwCharDbNum, bool bEnable)
{
    GLLandMan* pLandMan = GetLandMan();

    if (pLandMan)
    {
        GLChar* pChar = pLandMan->GetCharByDbNum(dwCharDbNum);

        if (pChar)
        {
            pChar->SetMovement(CHAR_MOVEMENT_STATE_INSTANTMAP_READY, bEnable);
        }
    }
}

void CServerReadyInstance::Update()
{
	/**
		����ð��� 0 �̸� ����� ���� �̴�.
		�̰��� �ʿ��� ������ ���� �߿� �ð� ����� ������ ������
		���̻� ���� ������ ������ �ʿ䰡 ���� �����̴�.

	 */
	if (GetLifeTime() == 0.f)
	{
		return;
	}

	CServerTriggerFinger* pFinger = 0;
	const CServerReadyTrigger* pTrigger = 0;

	//���������� �������� ���� �ΰ� �߰�
    //if (CastTrigger(pTrigger))
    //{
    //    float fCurTime = GetLifeTime() - GetTime();

    //    GLMSG::NET_PVE_ENTRANCE_STATE_FC msg;
    //    msg.nCur = m_cur;
    //    msg.nAll = m_all;
    //    msg.nTime = static_cast<int>(ceil(fCurTime));

    //    SendMsgClient(0, NET_MSG_PVE_ENTRANCE_STATE_FC, msg);
    //}
	if (CastTrigger(pTrigger) && CastFinger(pFinger))
	{
		float fCurTime = GetLifeTime() - GetTime();

		GLMSG::NET_PVE_ENTRANCE_STATE_FC msg;
		msg.nCur = m_cur;
		msg.nAll = m_all;
		msg.nTime = static_cast<int>(ceil(fCurTime));
		msg.nStage = pFinger->GetStage();

		SendMsgClient(0, NET_MSG_PVE_ENTRANCE_STATE_FC, msg);
	}
}
