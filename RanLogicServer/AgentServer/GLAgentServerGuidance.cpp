#include "../pch.h"
#include "../../RanLogic/Msg/GuidanceMsg.h"
#include "../Guidance/GuidanceAgent.h"
#include "./GLAgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void GLAgentServer::FrameMoveGuidance(float fElapsedTime)
{
    m_pGuidance->FrameMove(fElapsedTime);
}

GLGuidanceAgentMan* GLAgentServer::Guidance()
{
    return m_pGuidance;    
}

bool GLAgentServer::GuidanceIsBattle() const
{
    return m_pGuidance->IsBattle();
}

GLGuidance* GLAgentServer::GuidanceFindByClubID(DWORD ClubId)
{
    return m_pGuidance->FindByClubID(ClubId);
}

DWORD GLAgentServer::GuidanceId(DWORD ClubId) const
{
    return m_pGuidance->GetGuidID(ClubId);
}

BOOL GLAgentServer::MsgGuidanceMapInfoCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID)
{
    NET_GUIDANCE_MAP_INFO_CA* pNetMsg = (NET_GUIDANCE_MAP_INFO_CA*)pMsg;
    GLCharAG* pChar = GetCharByDbNum( pNetMsg->dwChaNum );
    if ( pChar )
    {
        m_pGuidance->SendGuidanceMapInfo(ClientSlot, pNetMsg->ClubDbNum);
        return TRUE;
    }

    return FALSE;
}

BOOL GLAgentServer::MsgGmGuidanceInfoCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID)
{
    GLCharAG* pChar = GetCharByGaeaID(GaeaID);
    if ( !pChar )
        return FALSE;

	if ( pChar->UserType() < USER_GM3 )
		return FALSE;

    m_pGuidance->SendInfo(ClientSlot);
    return TRUE;
}

BOOL GLAgentServer::MsgGmGuidanceChangeCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID)
{
    NET_GM_GUIDANCE_CHANGE_CA* pPacket = (NET_GM_GUIDANCE_CHANGE_CA*) pMsg;
    GLCharAG* pChar = GetCharByGaeaID(GaeaID);
    if (!pChar)
        return FALSE;
	
	if (pChar->UserType() < USER_GM3)
		return FALSE;

    m_pGuidance->ChangeGuidClub(pPacket->m_GuidanceId, pPacket->m_ClubDbNum);
    return TRUE;
}

BOOL GLAgentServer::MsgGmGuidanceBattleStartEndCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID)
{
	NET_GM_GUIDANCE_ON_OFF_CA* pPacket = (NET_GM_GUIDANCE_ON_OFF_CA*) pMsg;
	GLCharAG* pChar = GetCharByGaeaID(GaeaID);
	if (!pChar)
		return FALSE;

	if (pChar->UserType() < USER_GM3)
		return FALSE;

	if (pPacket->m_nFlag == NET_GM_GUIDANCE_ON_OFF_CA::START)
	{
		GLGuidance* pGuidande = m_pGuidance->Find(pPacket->m_GuidanceId);

		if (!pGuidande)
			return FALSE;

		pGuidande->DoBattleStart(this,0,-1);

		GLMSG::SNETPC_SERVER_CLUB_GUID_BATTLE_INFO NetMsgClubBattle;
		NetMsgClubBattle.bClubBattle = true;
		SENDTOALLCLIENT( &NetMsgClubBattle );

		m_pGuidance->SendGuidBattle_RemainTime_BRD();
	}
	else if (pPacket->m_nFlag == NET_GM_GUIDANCE_ON_OFF_CA::END)
	{
		GLGuidance* pGuidande = m_pGuidance->Find(pPacket->m_GuidanceId);

		if (!pGuidande)
			return FALSE;

		pGuidande->m_fNotifyRemainTimer = 601.0f;
		//선도전을 끝내기 위해 만료 시간을 넣어준다
		pGuidande->m_fBattleTimer = static_cast<float>(pGuidande->m_dwBattleTime);
	}
	else if (pPacket->m_nFlag == NET_GM_GUIDANCE_ON_OFF_CA::ALL_START)
	{
		std::vector<GLGuidance>& vecGuidance = m_pGuidance->GetVecGLGuidance();
		for (size_t i=0; i<vecGuidance.size(); ++i)
		{
			vecGuidance[i].DoBattleStart(this,0,-1);
		
			GLMSG::SNETPC_SERVER_CLUB_GUID_BATTLE_INFO NetMsgClubBattle;
			NetMsgClubBattle.bClubBattle = true;
			SENDTOALLCLIENT( &NetMsgClubBattle );
		}
		m_pGuidance->SendGuidBattle_RemainTime_BRD();
	}
	else if (pPacket->m_nFlag == NET_GM_GUIDANCE_ON_OFF_CA::ALL_END)
	{
		std::vector<GLGuidance>& vecGuidance = m_pGuidance->GetVecGLGuidance();
		for (size_t i=0; i<vecGuidance.size(); ++i)
		{
			vecGuidance[i].m_fNotifyRemainTimer = 601.0f;
			//선도전을 끝내기 위해 만료 시간을 넣어준다
			vecGuidance[i].m_fBattleTimer = static_cast<float>(vecGuidance[i].m_dwBattleTime);
		}
	}
	return TRUE;
}