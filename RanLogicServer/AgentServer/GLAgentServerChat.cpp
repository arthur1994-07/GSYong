#include "pch.h"
#include "../../EngineLib/DxTools/DebugSet.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../Club/GLClubManAgent.h"
#include "../Club/GLClubAgent.h"
#include "../Party/GLPartyManagerAgent.h"
#include "./AgentClientMan.h"
#include "./GLAgentServer.h"

#include "../../RanLogic/Prison/LimitPrison.h"
#include "../../SigmaCore/DebugInclude.h"


BOOL GLAgentServer::ChatMsgLinkProc(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    if ( dwGaeaID != GAEAID_NULL )
        return FALSE;

    GLMSG::NET_CHAT_LINK_FB* pNetMsg = (GLMSG::NET_CHAT_LINK_FB*) nmg;

	switch(pNetMsg->emType)
	{
		case CHAT_TYPE_PRIVATE :
		case CHAT_TYPE_MASTER  :
		case CHAT_TYPE_PARTY   :
		case CHAT_TYPE_GUILD   :
		case CHAT_TYPE_ALLIANCE:
		case CHAT_TYPE_PARTY_RECRUIT:
			{
				GLCharAG* pTarChar = m_pCharMan->GetCharByChaName(std::string(pNetMsg->szName));

				if( pTarChar ) {
					const SMAPNODE* pNode = FindMapNode( pTarChar->GetCurrentMap().getBaseMapID() );

					if( pNode && PRISONLIMIT.IsLimitStatus(pNode, pTarChar->m_dwUserLvl, PML_CHATTING ))
					{
						GLCharAG* pChar = GetCharByDbNum(pNetMsg->dwChaNum);
						
						if( pChar ) {

							GLMSG::SNET_GCHAT_ERRORCODE msg;
							msg.dwKey = pChar->ChaDbNum();
							msg.code  = CHAT_CODE_RECEIVE_PRISON;
							SENDTOCLIENT(pChar->ClientSlot(), &msg);
						}

						return FALSE;
					}
				}
			}
			break;

	}

    switch (pNetMsg->emType)
    {
    case CHAT_TYPE_GLOBAL:
	case CHAT_TYPE_GLOBAL_GM:
        ChatGlobal(nmg);
        break;
    case CHAT_TYPE_NORMAL: // 일반메시지
        //	들어올일이 없다.
        break;
    case CHAT_TYPE_PRIVATE: // 개인메시지
	case CHAT_TYPE_PRIVATE_GM:
        ChatPrivate(nmg);
        break;
	case CHAT_TYPE_MASTER:
    case CHAT_TYPE_PARTY:	
	case CHAT_TYPE_PARTY_GM:
        ChatParty(nmg);
        break;
    case CHAT_TYPE_GUILD : // 길드메시지
	case CHAT_TYPE_GUILD_GM:
        ChatGuild(nmg);
        break;
    case CHAT_TYPE_ALLIANCE:
	case CHAT_TYPE_ALLIANCE_GM:
        ChatAlliance(nmg);
        break;
    case CHAT_TYPE_PARTY_RECRUIT:
	case CHAT_TYPE_PARTY_RECRUIT_GM:
        ChatPartyRecruit(nmg);
        break;
    case CHAT_TYPE_LOUDSPEAKER:
        //	들어올일이 없다.
        break;	
    default:
        break;
    }
    return TRUE;
}

void GLAgentServer::ChatGlobal(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_CHAT_LINK_FB* pPacket = (GLMSG::NET_CHAT_LINK_FB*) pMsg;

    //GLCharAG* pChar = GetChar ( dwGaeaID );
    GLCharAG* pChar = GetCharByDbNum(pPacket->dwChaNum);
    if (!pChar)
        return;
    if (pChar->m_dwUserLvl < USER_USER_GM)
        return;

	// 팩션채팅 적용하지 않는다;
	/*if ( pPacket->bFactionChatting )
		SENDTOFIELD( pChar->ClientSlot(), pPacket );
	else*/
	{
		NET_CHAT_CTRL_FB NetChatFB;
		NetChatFB.emType = CHAT_TYPE_GLOBAL;
		NetChatFB.SetChatMsg(pPacket->szChatMsg);

		SENDTOALLCLIENT(&NetChatFB);
	}
}

void GLAgentServer::ChatPrivate(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_CHAT_LINK_FB* pPacket = (GLMSG::NET_CHAT_LINK_FB*) pMsg;

    //GLCharAG* pChar = GetChar ( dwGaeaID );
    GLCharAG* pChar = GetCharByDbNum(pPacket->dwChaNum);
    if (!pChar)
        return;
    if (pChar->IsCHATBLOCK())
        return;

    GLCharAG* pTarChar = m_pCharMan->GetCharByChaName(std::string(pPacket->szName));
    if (!pTarChar)
    {
        // 귓속말의 대상이 없을때 FB 메시지
        GLMSG::SNETPC_CHAT_PRIVATE_FAIL NetMsgFail(EMCHAT_PRIVATE_FAIL_NOTFOUND);
        NetMsgFail.SetName(pPacket->szName);
        SENDTOCLIENT(pChar->ClientSlot(), &NetMsgFail);
        return;
    }

    // 귓속말 무시상태인지 검사한다
    if (pTarChar->m_bWhisperIgnore && pChar->m_dwUserLvl < USER_USER_GM)
    {
        // 상대방이 GM이라면 상담 불가 메세지를 보내고 실제로 전송한다.
        if (USER_USER_GM <= pTarChar->m_dwUserLvl)
        {
            GLMSG::SNETPC_CHAT_PRIVATE_FAIL NetMsgFail(EMCHAT_PRIVATE_FAIL_GM_IGNORE);
            SENDTOCLIENT(pChar->ClientSlot(), &NetMsgFail);
        }
        else
        {
            // 유저도 귓속말 무시기능 개발시 구현한다
            return;
        }
    }

    //	상대방에게 자기 자신이 귓속말 블럭되어 있는지 검사.
    int dwFR_FLAG = pTarChar->GetStateFriend(pChar->m_szName);
    if (dwFR_FLAG == EMFRIEND_BLOCK && pChar->m_dwUserLvl > USER_USER_GM)
        dwFR_FLAG = EMFRIEND_OFF;

    if (dwFR_FLAG == EMFRIEND_BLOCK)
    {
        // 해당 사람에게 블럭 되었을때.
        // 귓속말의 대상이 없을때 FB 메시지.
        GLMSG::SNETPC_CHAT_PRIVATE_FAIL NetMsgFail(EMCHAT_PRIVATE_FAIL_BLOCK);
        NetMsgFail.SetName(pPacket->szName);
        SENDTOCLIENT(pChar->ClientSlot(), &NetMsgFail);
    }
    else
    {
		if ( pPacket->bFactionChatting )
		{
			SENDTOFIELD( pTarChar->ClientSlot(), pPacket );
		}
		else
		{
			pPacket->SetName(pChar->m_szName);

			SENDTOCLIENT(pTarChar->ClientSlot(), pPacket);
		}

		pPacket->SetName(pChar->m_szName);

        // 자신의 메시지를 보냄
        SendToGMMsgLink(pChar, pPacket);
        // 상대방의 메시지를 보냄
        SendToGMMsgLink(pTarChar, pPacket);

        //mjeon.attendance
        GLMSG::SNET_ATTENDANCE_WHISPER_DONE_AF msgAttendance;
        msgAttendance.dwChaNum = pChar->ChaDbNum();
        SENDTOFIELD(pChar->ClientSlot(), &msgAttendance);

		/*if ( pChar->m_bTracingUser )
		{
			CDebugSet::ToTracingFile(
				pChar->m_szUserName,
				"@@Private Send@@, [%s][%s]->[%s][%s], %s", 
				pChar->m_szUserName,
				pChar->m_szName,
				pTarChar->m_szUserName,
				pTarChar->m_szName,
				pPacket->szChatMsg);
		}

		if ( pTarChar->m_bTracingUser )
		{
			CDebugSet::ToTracingFile(
				pTarChar->m_szUserName,
				"@@Private Recv@@, [%s][%s]<-[%s][%s], %s", 
				pTarChar->m_szUserName,
				pTarChar->m_szName,
				pChar->m_szUserName,
				pChar->m_szName,
				pPacket->szChatMsg);
		}*/
    }
}

void GLAgentServer::ChatParty(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_CHAT_LINK_FB* pPacket = (GLMSG::NET_CHAT_LINK_FB*) pMsg;

    GLCharAG* pChar = GetCharByDbNum(pPacket->dwChaNum);
    if ( pChar == NULL )
        return;

    if ( pChar->IsCHATBLOCK() == true )
        return;
	
	const GLPartyID& PartyID = pChar->GetPartyID();
	if ( PartyID.isValidParty() == false )
		return;

	if ( pPacket->bFactionChatting )
		SENDTOFIELD( pChar->ClientSlot(), pPacket );

	if ( PartyID.isValidExpedition() )
	{
		const GLExpeditionAgent* const pExpedition = GetExpedition(PartyID);
		if ( pExpedition == NULL )
		{
			m_pPartyManager->SendErrorMsg(PartyID, pChar->GaeaId(), EMPARTY_ERROR_NOEXPEDITION, "ChatParty()");
			pChar->ResetPartyID();
			return;
		}

		for ( DWORD _i = MAX_PARTYGROUP; _i; --_i )
		{
			const GLPartyID& ExpeditionID = pExpedition->GetPartyID(_i - 1);
			const GLPartyAgent* const pParty = GetParty(ExpeditionID);
			if ( pParty == NULL )
			{
				m_pPartyManager->SendErrorMsg(PartyID, pChar->GaeaId(), EMPARTY_ERROR_NOEXPEDITION, "ChatParty()");
				m_pPartyManager->DeleteExpedition(PartyID);
				return;
			}

			GLPartyAgent::MEMBER_CITER iter = pParty->m_Member.begin();
			GLPartyAgent::MEMBER_CITER iter_end = pParty->m_Member.end();

			for ( ; iter!=iter_end; ++iter)
			{
				const DWORD dwMemberGaeaID = *iter;
				GLCharAG* pMemChar = GetChar(dwMemberGaeaID);
				if ( pMemChar == NULL )
				{
					m_pPartyManager->removePartyMember(ExpeditionID, dwMemberGaeaID);
					continue;
				}

				if (pMemChar->ClientSlot() != pChar->ClientSlot() &&
					false == pPacket->bFactionChatting )
					SENDTOCLIENT(pMemChar->ClientSlot(), pPacket);

				SendToGMMsgLink(pMemChar, pPacket);

				/*if (pMemChar->m_bTracingUser)
				{
					if (pMemChar->ClientSlot() == pChar->ClientSlot())
					{
						CDebugSet::ToTracingFile(
							pMemChar->m_szUserName,
							"##Party Send##, [%s][%s], %s", 
							pMemChar->m_szUserName,
							pMemChar->m_szName,
							pPacket->szChatMsg);
					}
					else
					{
						CDebugSet::ToTracingFile(
							pMemChar->m_szUserName,
							"##Party Recv##, [%s][%s]->[%s][%s], %s", 
							pChar->m_szUserName,
							pChar->m_szName,
							pMemChar->m_szUserName,
							pMemChar->m_szName,
							pPacket->szChatMsg);
					}
				}*/
			}
		}
		
	}
	else
	{
		const GLPartyAgent* const pParty = GetParty(PartyID);
		if (pParty == NULL )
		{
			m_pPartyManager->SendErrorMsg(PartyID, pChar->GaeaId(), EMPARTY_ERROR_NOEXPEDITION, "ChatParty()");
			pChar->ResetPartyID();
			return;
		}

		GLPartyAgent::MEMBER_CITER iter = pParty->m_Member.begin();
		GLPartyAgent::MEMBER_CITER iter_end = pParty->m_Member.end();
		 
		for ( ; iter!=iter_end; ++iter)
		{
			const DWORD dwMemberGaeaID = *iter;
			GLCharAG* const pMemChar = GetChar(dwMemberGaeaID);
			if ( pMemChar == NULL )
			{
				m_pPartyManager->removePartyMember(PartyID, dwMemberGaeaID);				
				continue;
			}
			if (pMemChar->ClientSlot() != pChar->ClientSlot() &&
				false == pPacket->bFactionChatting )
				SENDTOCLIENT(pMemChar->ClientSlot(), pPacket);

			SendToGMMsgLink(pMemChar, pPacket);

			/*if (pMemChar->m_bTracingUser)
			{
				if (pMemChar->ClientSlot() == pChar->ClientSlot())
				{
					CDebugSet::ToTracingFile(
						pMemChar->m_szUserName,
						"##Party Send##, [%s][%s], %s", 
						pMemChar->m_szUserName,
						pMemChar->m_szName,
						pPacket->szChatMsg);
				}
				else
				{
					CDebugSet::ToTracingFile(
						pMemChar->m_szUserName,
						"##Party Recv##, [%s][%s]->[%s][%s], %s", 
						pChar->m_szUserName,
						pChar->m_szName,
						pMemChar->m_szUserName,
						pMemChar->m_szName,
						pPacket->szChatMsg);
				}
			}*/
		}
    }
}

void GLAgentServer::ChatGuild(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_CHAT_LINK_FB* pPacket = (GLMSG::NET_CHAT_LINK_FB*) pMsg;
    
    GLCharAG* pChar = GetCharByDbNum(pPacket->dwChaNum);
    if (!pChar)
        return;
    if (pChar->m_ClubDbNum == CLUB_NULL)
        return;
    if (pChar->IsCHATBLOCK())
        return;

    std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pClubMan->GetClub(pChar->m_ClubDbNum);
    if (!pCLUB)
        return;

    pPacket->SetName(pChar->GetChaName());

	if ( pPacket->bFactionChatting )
		SENDTOFIELD( pChar->ClientSlot(), pPacket );

    CLUBMEMBERS& Members = pCLUB->GetMembers();
    CLUBMEMBERS_ITER pos = Members.begin();
    CLUBMEMBERS_ITER end = Members.end();
    for ( ; pos!=end; ++pos)
    {
        GLCharAG* pMEMBER = GetCharByDbNum((*pos).first);
        if (!pMEMBER)
            continue;

		if ( false == pPacket->bFactionChatting )
			SENDTOCLIENT(pMEMBER->ClientSlot(), pPacket);

        // 상대방의 메시지를 보냄
        SendToGMMsgLink(pMEMBER, pPacket);


		/*if (pMEMBER->m_bTracingUser)
		{
			if (pMEMBER->ClientSlot() == pChar->ClientSlot()) 
			{
				CDebugSet::ToTracingFile(
					pChar->m_szUserName,
					"%%Guild Send%%, [%s][%s], %s",
					pChar->m_szUserName, 
					pChar->m_szName,
					pPacket->szChatMsg);
			}
			else
			{
				CDebugSet::ToTracingFile(
					pMEMBER->m_szUserName,
					"%%Guild Recv%%, [%s][%s]->[%s][%s], %s", 
					pChar->m_szUserName, 
					pChar->m_szName,
					pMEMBER->m_szUserName,
					pMEMBER->m_szName,
					pPacket->szChatMsg);
			}
		}*/
    }
}

void GLAgentServer::ChatAlliance(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_CHAT_LINK_FB* pPacket = (GLMSG::NET_CHAT_LINK_FB*) pMsg;

    GLCharAG* pChar = GetCharByDbNum(pPacket->dwChaNum);
    if (!pChar)
        return;
    if (pChar->m_ClubDbNum == CLUB_NULL)
        return;
    if (pChar->IsCHATBLOCK())
        return;

    std::tr1::shared_ptr<GLClubAgent> pCLUB = m_pClubMan->GetClub(pChar->m_ClubDbNum);
    if (!pCLUB)
        return;
    if (pCLUB->m_dwAlliance == CLUB_NULL)
        return;

    std::tr1::shared_ptr<GLClubAgent> pCLUB_CHIEF = m_pClubMan->GetClub(pCLUB->m_dwAlliance);
    if (!pCLUB_CHIEF)
        return;

    pPacket->SetName(pChar->GetChaName());

	if ( pPacket->bFactionChatting )
		SENDTOFIELD( pChar->ClientSlot(), pPacket );

    CLUB_ALLIANCE_ITER pos_alliance = pCLUB_CHIEF->m_setAlliance.begin();
    CLUB_ALLIANCE_ITER end_alliance = pCLUB_CHIEF->m_setAlliance.end();
    for ( ; pos_alliance!=end_alliance; ++pos_alliance)
    {
        const GLCLUBALLIANCE &sALLIANCE = pos_alliance->second;
        std::tr1::shared_ptr<GLClubAgent> pCLUB_INDIAN = m_pClubMan->GetClub(sALLIANCE.m_dwID);
        if (!pCLUB_INDIAN)
            continue;

        CLUBMEMBERS& Members = pCLUB_INDIAN->GetMembers();
        CLUBMEMBERS_ITER pos = Members.begin();
        CLUBMEMBERS_ITER end = Members.end();
        for ( ; pos!=end; ++pos )
        {
            GLCharAG* pMEMBER = GetCharByDbNum((*pos).first);
            if (!pMEMBER)
                continue;

			if ( false == pPacket->bFactionChatting )
				SENDTOCLIENT(pMEMBER->ClientSlot(), pPacket);

			/*if (pMEMBER->m_bTracingUser)
			{
				if (pMEMBER->ClientSlot() == pChar->ClientSlot())
				{
					CDebugSet::ToTracingFile(
						pChar->m_szUserName,
						"**Alliance Send**, [%s][%s], %s",
						pChar->m_szUserName, 
						pChar->m_szName,
						pPacket->szChatMsg);
				}
				else
				{
					CDebugSet::ToTracingFile(
						pMEMBER->m_szUserName,
						"**Alliance Recv**, [%s][%s]->[%s][%s], %s", 
						pChar->m_szUserName,
						pChar->m_szName,
						pMEMBER->m_szUserName,
						pMEMBER->m_szName,
						pPacket->szChatMsg);
				}
			}*/
        }
    }
}

void GLAgentServer::ChatPartyRecruit(NET_MSG_GENERIC* pMsg)
{
    GLMSG::NET_CHAT_LINK_FB* pPacket = (GLMSG::NET_CHAT_LINK_FB*) pMsg;

    //if ( !GLCONST_CHAR::bCHAT_EXTEND ) return FALSE;

    //GLCharAG* pChar = GetChar(dwGaeaID);
    GLCharAG* pChar = GetCharByDbNum(pPacket->dwChaNum);
    if (!pChar)
        return;
    if (pChar->IsCHATBLOCK())
        return;

    GLMSG::NET_CHAT_PARTY_RECRUIT_FB_AF NetMsgFB(pChar->ChaDbNum());

    MAP_PARTY_RECRUIT_TIME_ITER	iter = m_mapPartyRecruitTime.find(pChar->m_CharDbNum);

    __time64_t curTime = CTime::GetCurrentTime().GetTime();
    __time64_t partyTime;

    if (iter != m_mapPartyRecruitTime.end())
    {				
        partyTime = iter->second;

        //	갱신을 1분 마다 하기 때문에 실제로 시간이 지났을 경우 삭제하고 정상 처리 한다.
        if (curTime > partyTime) 
        {
            m_mapPartyRecruitTime.erase(iter);
        }
        else
        {
            NetMsgFB.emFB = EMCHAT_PARTY_RECRUIT_TIME;
            NetMsgFB.fPartyRecruitTime = (float)(partyTime - curTime);
            NetMsgFB.Crc32 = sc::string::getCrc32(&NetMsgFB, sizeof(GLMSG::NET_CHAT_PARTY_RECRUIT_FB_AF)-sizeof(unsigned int));
            SENDTOCHANNEL(&NetMsgFB, pChar->GetChannel());
            return;
        }
    }

    CTimeSpan cTimeSpan(0,0,0,(int)GLCONST_CHAR::fCHAT_PARTY_RECRUIT_TIME);

    curTime += cTimeSpan.GetTotalSeconds();

    m_mapPartyRecruitTime.insert(std::make_pair(pChar->m_CharDbNum, curTime));

    NetMsgFB.emFB = EMCHAT_PARTY_RECRUIT_OK;
    NetMsgFB.Crc32 = sc::string::getCrc32(&NetMsgFB, sizeof(GLMSG::NET_CHAT_PARTY_RECRUIT_FB_AF)-sizeof(unsigned int));
    SENDTOCHANNEL(&NetMsgFB, pChar->GetChannel());

    pPacket->SetName(pChar->GetChaName()); // 캐릭터 이름 해킹 방지 2012-07-23 jgkim
    SENDTOALLCLIENT(pPacket, pChar->GetChannel());

    // Club Log
    // [%1%]님이 [%2%]에서 파티모집을 시작했습니다. [%3%].
    DWORD ClubNum = pChar->GetClubDbNum();
    if (ClubNum != CLUB_NULL)
    {
        std::string ClubLog(
            stringformat(
                ID2GAMEINTEXT("CLUB_LOG_25"),
                pChar->GetChaName(),            
                GetMapNameStr(pChar->GetCurrentMap().getBaseMapID()),
                pPacket->szChatMsg));
        SENDCLUBLOG(ClubNum, club::LOG_PARTY_RECRUIT, ClubLog);
    }
}
