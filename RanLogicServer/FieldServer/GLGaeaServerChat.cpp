#include "pch.h"

#include "../Club/GLClubManField.h"
#include "../Club/GLClubField.h"
#include "../Database/DBAction/DbActionLog.h"

#include "./CharManField.h"

#include "./GLGaeaServer.h"

#include "../../SigmaCore/DebugInclude.h"



BOOL GLGaeaServer::ChatMsgLinkProc(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    //GASSERT(dwGaeaID != GAEAID_NULL);
    if (dwGaeaID == GAEAID_NULL)
        return FALSE;

    // NET_MSG_CHAT_LINK
    // GLMSG::NET_CHAT_LINK* pNetMsg = (GLMSG::NET_CHAT_LINK*) nmg;
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    GLMSG::NET_CHAT_LINK RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    if (m_bEmptyMsg)
        return TRUE;

    if (RecvData.sLinkDataBasic.size() > CHAT_LINK_MAX)
        return FALSE;

    GLChar* pChar = GetChar(dwGaeaID);
    if (!pChar)
        return FALSE;

    GLMSG::NET_CHAT_LINK_FB NetChatFB;
    NetChatFB.dwChaNum = pChar->CharDbNum();

    //	링크 정보를 셋팅한다.
    {
        // 링크 정보를 찾는다.
        BYTE LinkData[CHAT_LINK_DATA_SIZE];

        int nCnt = 0;
        for (size_t i=0; i<RecvData.sLinkDataBasic.size(); ++i)
        {
            if (!RecvData.sLinkDataBasic[i].VALID())
                continue;			

            if (RecvData.sLinkDataBasic[i].sLinkType.wMType == ITEM_LINK &&
                RecvData.sLinkDataBasic[i].sLinkType.wSType == ITEM_LINK_CLUB_STORAGE)
            {
                // club storage 링크는 agent 로 data 가 옮겨짐에 따라 어떻게 구현할지 고민해 보아야 한다.
                sc::writeInfo(std::string("ITEM_LINK ITEM_LINK_CLUB_STORAGE not support."));
                continue;
            }

            DWORD dwSize = pChar->GetChatLinkData(RecvData.sLinkDataBasic[i], LinkData, CHAT_LINK_DATA_SIZE);

            if (dwSize > 0)
            {
                if (NetChatFB.ADD(LinkData, dwSize)) 
                {
                    NetChatFB.sLinkDataRecv[nCnt].sLinkType = RecvData.sLinkDataBasic[i].sLinkType;
					NetChatFB.sLinkDataRecv[nCnt].sLinkPos = RecvData.sLinkDataBasic[i].sLinkPos;
                    NetChatFB.sLinkDataRecv[nCnt].dwSize = dwSize;
                    nCnt++;
                }				
            }
        }
    }

	// GM이 채팅한 경우에는 이름을 다른색으로 표시하기 위해서 구분한다.
	// 기존 채팅 Type에 CHAT_ADD_TYPE_FOPR_GM ( 100 )을 추가해준다.
	WORD nAddValue = 0;
    if( pChar->UserLevel() >= USER_USER_GM ) 
	{
		nAddValue = (WORD)CHAT_ADD_TYPE_FOPR_GM;

		std::string strSendName = sc::string::format("%1%", pChar->GetName());
		// 파일에 GM 채팅 로그를 기록 하는 부분을 DB로 변경
		//sc::writeLogInfo(sc::string::format("[GM CHAT LOG] Type : %1%, Name : %2%, Msg : %3%", RecvData.m_ChatType, RecvData.m_ChaName, RecvData.m_ChatMsg));
		AddLogAdoJob(db::DbActionPtr(new db::LogGMChat(RecvData.m_ChatType,  pChar->UserLevel(), strSendName, RecvData.m_ChaName.size() != 0 ? RecvData.m_ChaName : std::string("System or Unknown"), RecvData.m_ChatMsg )));
	}

    switch (RecvData.m_ChatType)
    {
    case CHAT_TYPE_GLOBAL:
        {
            //GLChar* pChar = GetChar(dwGaeaID);
            //if (!pChar || pChar->UserLevel() < USER_GM3)
            //    return FALSE;
			if( nAddValue == 0 ) // GM 이면 nAddValue가 0이 아님. 중복체크하지 않기위해
				return FALSE;

			GLLandManager* pLandManager = pChar->GetParentLandManager();
			if ( false == pLandManager )
				return FALSE;

			bool bActiveFactionChatting = pLandManager->IsActiveFactionChatting();

            NetChatFB.SetData(
                static_cast<EMNET_MSG_CHAT> (RecvData.m_ChatType + nAddValue),
                RecvData.m_ChaName.c_str(),
                RecvData.m_ChatMsg.c_str());
			NetChatFB.bFactionChatting = bActiveFactionChatting;

			SENDTOAGENT(&NetChatFB);	//pChar->ClientSlot()
        }
        break;
    case CHAT_TYPE_NORMAL:	// 일반메시지
        {
            if ( pChar->IsCHATBLOCK() )
            {
                return FALSE;				
            }

			GLLandManager* pLandManager = pChar->GetParentLandManager();
			if ( false == pLandManager )
				return FALSE;

			bool bActiveFactionChatting = pLandManager->IsActiveFactionChatting();

            NetChatFB.emType = static_cast<EMNET_MSG_CHAT> (RecvData.m_ChatType + nAddValue);
            NetChatFB.SetName(pChar->m_szName);
            NetChatFB.SetChatMsg(RecvData.m_ChatMsg.c_str());

			if ( false == bActiveFactionChatting )
				pChar->SendMsgViewAround(&NetChatFB);
			else
			{	
				Faction::ManagerField* const pFactionManager =
					pLandManager->GetFactionManager();

				if ( pFactionManager )
				{
					const Faction::FactionID* pFactionID =
						pFactionManager->getFactionID( pChar->GetActorID() );

					if ( pFactionID )
					{
						pFactionManager->SendToAllyFactionViewAround(
							pChar->GetActorID(), *pFactionID, &NetChatFB, true );
					}
				}
			}
        }
        break;
    case CHAT_TYPE_PRIVATE : // 개인메시지
        {
            if ( pChar->IsCHATBLOCK() )
            {
                return FALSE;
            }

			GLLandManager* pLandManager = pChar->GetParentLandManager();
			if ( false == pLandManager )
				return FALSE;

			bool bActiveFactionChatting = pLandManager->IsActiveFactionChatting();

            NetChatFB.emType = static_cast<EMNET_MSG_CHAT> (RecvData.m_ChatType + nAddValue);
            NetChatFB.SetName(RecvData.m_ChaName.c_str());
            NetChatFB.SetChatMsg(RecvData.m_ChatMsg.c_str());
			NetChatFB.bFactionChatting = bActiveFactionChatting;

			SENDTOAGENT(&NetChatFB);	//pChar->ClientSlot()
        }
        break;


	case CHAT_TYPE_MASTER: // 원정대장 메시지
    case CHAT_TYPE_PARTY : // 파티메시지
        {
            if (!pChar->isValidParty())
                return FALSE;
			
			if ( pChar->IsCHATBLOCK() )
                return FALSE;

			GLLandManager* pLandManager = pChar->GetParentLandManager();
			if ( false == pLandManager )
				return FALSE;

			bool bActiveFactionChatting = pLandManager->IsActiveFactionChatting();

            NetChatFB.emType = static_cast<EMNET_MSG_CHAT> (RecvData.m_ChatType + nAddValue);
            NetChatFB.SetName(pChar->m_szName);
            NetChatFB.SetChatMsg(RecvData.m_ChatMsg.c_str());
			NetChatFB.bFactionChatting = bActiveFactionChatting;

			SENDTOAGENT(&NetChatFB);	//pChar->ClientSlot()
        }
        break;

    case CHAT_TYPE_GUILD : // 길드메시지
        {
            if ( pChar->m_ClubDbNum==CLUB_NULL )
                return FALSE;

            if ( pChar->IsCHATBLOCK() )
            {
                return FALSE;
            }	

			GLLandManager* pLandManager = pChar->GetParentLandManager();
			if ( false == pLandManager )
				return FALSE;

			bool bActiveFactionChatting = pLandManager->IsActiveFactionChatting();

            NetChatFB.emType = static_cast<EMNET_MSG_CHAT> (RecvData.m_ChatType + nAddValue);
            NetChatFB.SetName(pChar->m_szName);
            NetChatFB.SetChatMsg(RecvData.m_ChatMsg.c_str());
			NetChatFB.bFactionChatting = bActiveFactionChatting;

			SENDTOAGENT(&NetChatFB);	//pChar->ClientSlot()
        }
        break;
    case CHAT_TYPE_ALLIANCE : // 길드메시지
        {
            if ( pChar->m_ClubDbNum==CLUB_NULL )
                return FALSE;

            if ( pChar->IsCHATBLOCK() )
            {
                return FALSE;
            }

			GLLandManager* pLandManager = pChar->GetParentLandManager();
			if ( false == pLandManager )
				return FALSE;

			bool bActiveFactionChatting = pLandManager->IsActiveFactionChatting();

            std::tr1::shared_ptr<GLClubField> pCLUB = m_pClubMan->GetClub ( pChar->m_ClubDbNum );
            if ( !pCLUB )
                return FALSE;
            if ( pCLUB->m_dwAlliance==CLUB_NULL )
                return FALSE;

            NetChatFB.emType = static_cast<EMNET_MSG_CHAT> (RecvData.m_ChatType + nAddValue);
            NetChatFB.SetName(pChar->m_szName);
            NetChatFB.SetChatMsg(RecvData.m_ChatMsg.c_str());
			NetChatFB.bFactionChatting = bActiveFactionChatting;

			SENDTOAGENT(&NetChatFB);	//pChar->ClientSlot()
        }
        break;
    case CHAT_TYPE_AREA : // 지역 메시지
        {
            if (!GLCONST_CHAR::bCHAT_EXTEND)
                return FALSE;

			GLLandManager* pLandManager = pChar->GetParentLandManager();
			if ( false == pLandManager )
				return FALSE;

			GLMSG::SNETPC_CHAT_AREA_FB NetMsgFB;
			if ( /*pLandManager->isOldInstantMap() ||*/
				pLandManager->isNewInstantMap() )
			{
				NetMsgFB.emFB = EMCHAT_AREA_UNAPPLY;
				SENDTOCLIENT(pChar->ClientSlot(), &NetMsgFB);
				return FALSE;
			}

			bool bActiveFactionChatting = pLandManager->IsActiveFactionChatting();

			GLLandMan* const pLand(pChar->GetLandMan());
			if ( pLand == NULL )
				return E_FAIL;

            DWORD dwCharge = (DWORD) GLCONST_CHAR::dwCHAT_AREA_CHARGE;
            if (pChar->GetInvenMoney() < dwCharge)
            {
                NetMsgFB.emFB = EMCHAT_AREA_NOMONEY;
                SENDTOCLIENT(pChar->ClientSlot(), &NetMsgFB);
                return FALSE;
            }

            if (pChar->IsCHATBLOCK())
            {
                return FALSE;
            }

            if (dwCharge > 0)
            {
                pChar->CheckMoneyUpdate( pChar->GetInvenMoney(), dwCharge, FALSE, "CHAT_AREA" );
                pChar->SubtractInvenMoneyServer(dwCharge, SUBTRACT_INVEN_MONEY_AREA_CHAT_COST);
                pChar->SendInvenMoneyUpdate();
                NetMsgFB.emFB = EMCHAT_AREA_OK;
                SENDTOCLIENT(pChar->ClientSlot(), &NetMsgFB);
            }

            NetChatFB.emType = static_cast<EMNET_MSG_CHAT> (RecvData.m_ChatType + nAddValue);
            NetChatFB.SetName(pChar->m_szName);
            NetChatFB.SetChatMsg(RecvData.m_ChatMsg.c_str());
			NetChatFB.bFactionChatting = bActiveFactionChatting;

			if (pLand->IsInstantMap()) 
			{
				//	같은 인던에 있는 유저에게 모두 보냅니다.
				SENDTOCLIENT_ONMAP_INSTANT(pLand->GetBaseMapID().dwID, &NetChatFB);
			}
			else
			{
				SENDTOCLIENT_ONMAP(pLand->GetBaseMapID().dwID, &NetChatFB);
			}
        }
        break;
    case CHAT_TYPE_PARTY_RECRUIT : // 파티모집 메시지
        {
            if (!GLCONST_CHAR::bCHAT_EXTEND)
                return FALSE;

			GLLandManager* pLandManager = pChar->GetParentLandManager();
			if ( false == pLandManager )
				return FALSE;

			if ( /*pLandManager->isOldInstantMap() ||*/
				pLandManager->isNewInstantMap() )
			{
				GLMSG::SNETPC_CHAT_PARTY_RECRUIT_FB NetMsgFB;
				NetMsgFB.emFB = EMCHAT_PARTY_RECRUIT_UNAPPLY;
				SENDTOCLIENT(pChar->ClientSlot(), &NetMsgFB);

				return FALSE;
			}

			bool bActiveFactionChatting = pLandManager->IsActiveFactionChatting();

            DWORD dwCharge = (DWORD)GLCONST_CHAR::dwCHAT_PARTY_RECRUIT_CHARGE;

            if (pChar->GetInvenMoney() < dwCharge)
            {
                GLMSG::SNETPC_CHAT_PARTY_RECRUIT_FB NetMsgFB;
                NetMsgFB.emFB = EMCHAT_PARTY_RECRUIT_NOMONEY;
                SENDTOCLIENT(pChar->ClientSlot(), &NetMsgFB);
                return FALSE;
            }

            if (pChar->IsCHATBLOCK())
            {
                return FALSE;
            }

            if (dwCharge > 0)
            {
                pChar->CheckMoneyUpdate(pChar->GetInvenMoney(), dwCharge, FALSE, "CHAT_PARTY_RECRUIT");
                pChar->SubtractInvenMoneyServer(dwCharge, SUBTRACT_INVEN_MONEY_PARTY_RECRUIT_CHAT_COST);
                pChar->SendInvenMoneyUpdate();
            }

            NetChatFB.emType = static_cast<EMNET_MSG_CHAT> (RecvData.m_ChatType + nAddValue);
            NetChatFB.SetName(pChar->m_szName);
            NetChatFB.SetChatMsg(RecvData.m_ChatMsg.c_str());

			if ( false == bActiveFactionChatting )
				SENDTOAGENT(&NetChatFB);	//pChar->ClientSlot()
            // Club Log
        }
        break;
    case CHAT_TYPE_LOUDSPEAKER : // 확성기 메시지
        //	 따로 처리합니다. 
        break;
    case CHAT_TYPE_CTRL_GLOBAL : // 시스템 메시지
        break;
    case CHAT_TYPE_CTRL_GLOBAL2 : // 시스템 메시지
        break;
	case CHAT_TYPE_FACTION:
		{
			if ( pChar->IsCHATBLOCK() )
			{
				GLMSG::SNETPC_CHAT_FACTION_FB sNetMsgFB;
				sNetMsgFB.emFB = EMCHAT_FACTION_BLOCK;
				SENDTOCLIENT( pChar->ClientSlot(), &sNetMsgFB );

				return FALSE;				
			}

			GLLandManager* pLandManager = pChar->GetParentLandManager();
			if ( false == pLandManager )
			{
				GLMSG::SNETPC_CHAT_FACTION_FB sNetMsgFB;
				sNetMsgFB.emFB = EMCHAT_FACTION_FAIL;
				SENDTOCLIENT( pChar->ClientSlot(), &sNetMsgFB );

				return FALSE;
			}

			bool bActiveFactionChatting = pLandManager->IsActiveFactionChatting();

			NetChatFB.emType = static_cast< EMNET_MSG_CHAT >(
				RecvData.m_ChatType + nAddValue );
			NetChatFB.SetName( pChar->m_szName );
			NetChatFB.SetChatMsg( RecvData.m_ChatMsg.c_str() );

			if ( false == bActiveFactionChatting )
			{
				GLMSG::SNETPC_CHAT_FACTION_FB sNetMsgFB;
				sNetMsgFB.emFB = EMCHAT_FACTION_UNAPPLY;
				SENDTOCLIENT( pChar->ClientSlot(), &sNetMsgFB );

				return FALSE;
			}
			else
			{	
				Faction::ManagerField* const pFactionManager =
					pLandManager->GetFactionManager();

				if ( pFactionManager )
				{
					const Faction::FactionID* pFactionID =
						pFactionManager->getFactionID( pChar->GetActorID() );

					if ( pFactionID )
					{
						pFactionManager->sendMessageToAllyFaction(
							*pFactionID, &NetChatFB );
					}
				}
			}
		}
		break;
    default:
        break;
    }

    return TRUE;
}

BOOL GLGaeaServer::ChatMsgLinkAFProc ( DWORD dwClientID
									 , DWORD dwGaeaID
									 , NET_MSG_GENERIC* nmg )
{
	GLMSG::NET_CHAT_LINK_FB* pNetMsg =
		static_cast< GLMSG::NET_CHAT_LINK_FB* >( nmg );

	GLChar* pChar = GetCharByDbNum( pNetMsg->dwChaNum );
	if ( false == pChar )
		return FALSE;

	GLLandManager* pLandManager = pChar->GetParentLandManager();
	if ( false == pLandManager )
		return FALSE;

	Faction::ManagerField* const pFactionManager =
		pLandManager->GetFactionManager();
	if ( false == pFactionManager )
		return FALSE;

	switch (pNetMsg->emType)
	{
	case CHAT_TYPE_GLOBAL:
	case CHAT_TYPE_GLOBAL_GM:
		{
			const Faction::FactionID* pFactionID =
				pFactionManager->getFactionID( pChar->GetActorID() );

			if ( pFactionID )
			{
				pFactionManager->SendToAllyFactionExcludeMe( pChar->GetActorID(), *pFactionID, pNetMsg );
			}	
		}
		//sc::writeLogInfo ( "[ Chatting Log ] [ Global Faction Chatting ]" );
		break;

	case CHAT_TYPE_ALLIANCE:
	case CHAT_TYPE_ALLIANCE_GM:
		{
			const Faction::FactionID* pFactionID =
				pFactionManager->getFactionID( pChar->GetActorID() );

			if ( pFactionID )
			{
				pFactionManager->sendMessageToAllyFaction( *pFactionID, pNetMsg );
			}	
		}
		//sc::writeLogInfo ( "[ Chatting Log ] [ Alliance Faction Chatting ]" );
		break;

	// 들어올 일이 없다;
	case CHAT_TYPE_NORMAL:
		break;

	case CHAT_TYPE_PRIVATE:
	case CHAT_TYPE_PRIVATE_GM:
		{
			const Faction::FactionID* pFactionID =
				pFactionManager->getFactionID( pChar->GetActorID() );

			GLChar* pTargetChar = m_pCharMan->GetChaByName(
				std::string( pNetMsg->szName ) );
			if ( false == pTargetChar )
				return FALSE;

			if ( pFactionID )
			{
				pNetMsg->SetName( pChar->m_szName );

				pFactionManager->SendToAllyFactionPrivate(
					pChar->GetActorID(),
					pTargetChar->GetActorID(),
					*pFactionID, pNetMsg );
			}	
		}
		//sc::writeLogInfo ( "[ Chatting Log ] [ Private Faction Chatting ]" );
		break;

	case CHAT_TYPE_MASTER:
	case CHAT_TYPE_PARTY:
	case CHAT_TYPE_MASTER_GM:
	case CHAT_TYPE_PARTY_GM:
		{
			const Faction::FactionID* pFactionID =
				pFactionManager->getFactionID( pChar->GetActorID() );

			if ( pFactionID )
			{
				pFactionManager->SendToAllyFactionParty(
					pChar->GetActorID(), *pFactionID, pNetMsg, true );
			}	
		}
		//sc::writeLogInfo ( "[ Chatting Log ] [ Party Faction Chatting ]" );
		break;

	case CHAT_TYPE_GUILD:
	case CHAT_TYPE_GUILD_GM:
		{
			const Faction::FactionID* pFactionID =
				pFactionManager->getFactionID( pChar->GetActorID() );

			if ( pFactionID )
			{
				pFactionManager->SendToAllyFactionClub(
					pChar->GetActorID(), *pFactionID, pNetMsg );
			}	
		}
		//sc::writeLogInfo ( "[ Chatting Log ] [ Club Faction Chatting ]" );
		break;

	// 들어올 일이 없다;
	case CHAT_TYPE_PARTY_RECRUIT:
	case CHAT_TYPE_LOUDSPEAKER:
		break;
	default:
		break;
	}

	return TRUE;
}