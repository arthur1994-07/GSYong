#include "pch.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "../../Enginelib/Common/profile.h"
#include "../../RanLogic/Msg/LoginMsg.h"
#include "../../RanLogic/Msg/ServerMsg.h"

#include "../FieldServer/GLGaeaServer.h"
#include "../Database/DbManager.h"
#include "../Client/NetInstanceClientMan.h"
#include "./InstanceServer.h"

#include "../TriggerManager/GLTriggerManagerFD.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

void InstanceServer::initPacketFunc()
{
	m_MsgFunc[NET_MSG_HEARTBEAT_SERVER_REQ].Msgfunc	= &InstanceServer::MsgHeartbeatServerAnswer;
	m_MsgFunc[NET_MSG_HEARTBEAT_CLIENT_ANS].Msgfunc	= &InstanceServer::MsgHeartbeatClientAns;
	
	m_MsgFunc[NET_MSG_TRANSFER_CHAR].Msgfunc		= &InstanceServer::MsgTransferChar;

	m_MsgFunc[NET_MSG_CLIENT_VERSION_CS].Msgfunc	= &InstanceServer::MsgVersion;
	m_MsgFunc[MET_MSG_GAME_JOIN_FIELDSVR].Msgfunc	= &InstanceServer::MsgGameJoinChar;
	m_MsgFunc[NET_MSG_FIELD_REQ_JOIN].Msgfunc		= &InstanceServer::MsgFieldReqJoin;
	m_MsgFunc[NET_MSG_JOIN_FIELD_IDENTITY].Msgfunc	= &InstanceServer::MsgJoinInfoFromClient;

	m_MsgFunc[NET_MSG_CREATE_INSTANCE_MI].Msgfunc	= &InstanceServer::MsgCreateInstanceMI;
	m_MsgFunc[NET_MSG_DELETE_INSTANCE_MI].Msgfunc	= &InstanceServer::MsgDeleteInstanceMI;
}

int InstanceServer::InsertMsg(int nClient, void* pMsg)
{
    if (pMsg)
	{
		NET_MSG_GENERIC* pPacket = (NET_MSG_GENERIC*) pMsg;
		m_pRecvMsgManager->MsgQueueInsert(nClient, pPacket, pPacket->Size());
	}
	return 0;
}


void InstanceServer::MsgProcess( MSG_LIST* pMsg )
{
	if (!pMsg)
		return;

	NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*)(pMsg->Buffer);

	if (!nmg)
		return;

	if (nmg->Size() > NET_DATA_BUFSIZE)
		return;

	DWORD dwClient = pMsg->dwClient;
	DWORD dwGaeaID = GAEAID_NULL;


	if (dwClient >= (DWORD)m_pClientManager->GetMaxClient())
	{
		//GASSERT(0&&"m_nMaxClient43");

		return;
	}	

	if (dwClient == m_SessionServerSlot)
    {
        SessionMsgProcess(pMsg);
		return;
    }
    else if (dwClient == DBACTION_CLIENT_ID)
    {
        if(m_pGaeaServer->MsgProcess(nmg, DBACTION_CLIENT_ID, GAEAID_NULL))
			return;
    }
	else
	{
		if (m_pClientManager->IsClientSlot(dwClient))
		{
			//클라이언트에서 보낸 메시지에 대해서만 ClientManager의 GaeaID가 유효하다.

			dwGaeaID = m_pClientManager->GetGaeaID(dwClient);
		}
		/*
#ifdef _RELEASED
		else	//from MatchServer
		{
			// 여기를 거치는 경우는 각 MsgHandler로 전달되는 GaeaID가 유효한 값이 아니다!!
			if (!m_pGaeaServer->TraceFilter(nmg->nType))
			{
				sc::writeLogDebug(sc::string::format("(Slot:%1%)Isn't ChaNum required for the msg(%2%) ?", dwClient, nmg->nType));
			}
		}
#endif
		*/
	}


	//
	// 2.여기부터 MatchServer나 Client로부터 온 메시지들을 처리한다.
	//
	
    // MsgCountCheck
    if (dwClient < (DWORD) m_nMaxClient) 
    {
        if(m_pClientManager->IsClientSlot(dwClient) && m_pClientManager->IncPacketCount(dwClient, nmg->nType))
        {
            if (!IsReserveCloseClient(dwClient)) 
            {
                m_pClientManager->WritePacketLog(dwClient);
                ReserveCloseClient(dwClient);
            }
        }
    }

	EMNET_MSG emType = nmg->nType;
	if (emType < 0 || emType >= NET_MSG_GCTRL_MAX)
	{
		sc::writeLogError(
            sc::string::format(
                "MsgProcess Out of range or illegal message %1%", emType));
        CloseClient(dwClient);
	}
	else
	{
		if (NULL == m_MsgFunc[emType].Msgfunc)
		{
			// Insert to message queue
			if ( nmg->nType > NET_MSG_GCTRL )
			{
				m_pGaeaServer->MsgProcess(nmg, dwClient, dwGaeaID);
			}
		}
		else
		{
			(this->*m_MsgFunc[emType].Msgfunc)(dwClient, nmg);
		}
	}
}

void InstanceServer::MsgIamMatch(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
	/*
    NET_I_AM_AGENT* pPacket = (NET_I_AM_AGENT*) nmg;
    if (pPacket->MagicNum != NET_I_AM_AGENT::MAGIC_NUM)
    {
        sc::writeLogError(
            sc::string::format(
                "NET_I_AM_AGENT invalid magic num %1%", pPacket->MagicNum));
        return;
    }

    unsigned int SCrcValue = pPacket->CrcValue;
    unsigned int TCrcValue = sc::string::getCrc32(pPacket, pPacket->Size() - sizeof(unsigned int));
    if (SCrcValue != TCrcValue)
    {
        sc::writeLogError(
            sc::string::format(
                "NET_I_AM_AGENT invalid crc %1%/%2%", SCrcValue, TCrcValue));
        return;
    }

    sc::writeLogInfo(
        sc::string::format(
            "NET_I_AM_AGENT Agent slot %1%", dwClient));
	
	m_dwAgentSlot = dwClient;

	GASSERT((m_pClientManager->GetSlotAgent() == m_dwAgentSlot)&&"AgentSlot is weird!");

	//m_pClientManager->SetSlotAgent(dwClient);
	m_pGaeaServer->SetAgentSlot(dwClient);
	*/
}

//mjeon.instance.transfer
//AM
void InstanceServer::MsgTransferChar(DWORD dwClientID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_TRANSFER_CHAR *pmsg = (GLMSG::SNET_TRANSFER_CHAR *)nmg;

	m_pGaeaServer->TransferChar(dwClientID, GAEAID_NULL, nmg);	
}

void InstanceServer::MsgHeartbeatClientAns(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
	m_pClientManager->SetHeartBeat( dwClient );
}

void InstanceServer::MsgHeartbeatServerAnswer(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
	NET_HEARTBEAT_SERVER_ANS msg;
	//SendAgent( dwClient, &msg );
	SendMatch(&msg);
}

// Client->Field : Join Msg
void InstanceServer::MsgJoinInfoFromClient(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
	/*
		mjeon.AF
		 Client가 해당 Field로 접속한 이후, 최초로 보내는 인증 메시지이다.
		 그러므로, 이 곳에서 ClientID에 대해 GaeaID 및 ChaNum이 setup 된다.
	*/
	
	NET_GAME_JOIN_FIELD_IDENTITY_CF* pmsg = (NET_GAME_JOIN_FIELD_IDENTITY_CF*)nmg;

	if (!pmsg)
		return;

	//mjeon.instance.transfer
	GASSERT(pmsg->emType == EMJOINTYPE_TRANSFER);
	if (pmsg->emType != EMJOINTYPE_TRANSFER)
		return;

	GASSERT(m_pClientManager->IsClientSlot(dwClient)&&"MsgJoinInfoFromClient() - MUST be ClientSlot!!");

	m_pClientManager->SetCryptKey(dwClient, pmsg->ck);
	m_pClientManager->SetGaeaID(dwClient, pmsg->dwGaeaID);

	GLChar* pChar = m_pGaeaServer->GetChar(pmsg->dwGaeaID);
	if (!pChar)
	{
		GLMSG::SNET_DROP_OUT_FORCED NetMsg_OUT;
		NetMsg_OUT.dwSvrGroupNum = pmsg->dwSvrGroupNum;
		NetMsg_OUT.dwChaNum		 = pmsg->dwChaNum;

		SendMatch(&NetMsg_OUT);
		return;
	}

	pChar->SetClientSlot(dwClient);

	m_pClientManager->SetUserID(dwClient, pChar->m_szUID);
	
	m_pClientManager->SetUserDbNum(dwClient, pChar->GetUserID());
	m_pClientManager->SetChaNum(dwClient, pChar->CharDbNum());	//mjeon.AF

	//게이트 이동 확인 메시지
	GLMSG::SNETREQ_GATEOUT_FB NetMsgGateOutFB;
	NetMsgGateOutFB.dwChaNum = pChar->CharDbNum();
	NetMsgGateOutFB.emFB = EMCHAR_GATEOUT_OK;
	NetMsgGateOutFB.sMapID = pChar->GetCurrentMap();
	NetMsgGateOutFB.vPos = pChar->GetPosition();
	
	SendClient(dwClient, &NetMsgGateOutFB);	//Client에 direct로 전송
}

void InstanceServer::MsgCreateInstanceMI(DWORD dwClient, NET_MSG_GENERIC* nmg)
{	
	GASSERT(m_MatchServerSlot == dwClient);
	GASSERT(m_pClientManager->IsClientSlot(dwClient) != TRUE);

	if (dwClient != m_MatchServerSlot)
	{
		sc::writeLogError(sc::string::format("InstanceServer::MsgCreateInstanceMI - Slot(%1%) MUST be MatchSlot(%2%)!!", dwClient, m_MatchServerSlot));
		return;
	}
	
//	m_pGaeaServer->MsgCreateInstanceMI(dwClient, GAEAID_NULL, nmg);
}

void InstanceServer::MsgDeleteInstanceMI(DWORD dwClient, NET_MSG_GENERIC* nmg)
{	
	GASSERT(m_MatchServerSlot == dwClient);
	GASSERT(m_pClientManager->IsClientSlot(dwClient) != TRUE);

	if (dwClient != m_MatchServerSlot)
	{
		sc::writeLogError(sc::string::format("InstanceServer::MsgDeleteInstanceMI - Slot(%1%) MUST be MatchSlot(%2%)!!", dwClient, m_MatchServerSlot));
		return;
	}
	
//	m_pGaeaServer->MsgDeleteInstanceMI(dwClient, GAEAID_NULL, nmg);
}

void InstanceServer::MsgSndCryptKey(DWORD dwClient)
{
	CRYPT_KEY ck = m_pClientManager->GetNewCryptKey(); // Get crypt key
			
	NET_CRYPT_KEY nck;	
	nck.nType   = NET_MSG_SND_CRYT_KEY;
	nck.ck			= ck;
	SendClient(dwClient, &nck);
	m_pClientManager->SetCryptKey(dwClient, ck); // Set crypt key
}

// Filed->Client
// 일반적 채팅 메시지 전송
void InstanceServer::MsgSndChatNormal(DWORD dwClient, const char* szName, const char* szMsg)
{
    if (!szMsg)
        return;

	GLMSG::NET_CHAT_LINK_FB nc;
	GLChar* pChar = m_pGaeaServer->GetCharByDbNum(m_pGaeaServer->GetChaDbNumByChaName(szName));
    nc.SetData(NET_MSG_CHAT_LINK_FB, CHAT_TYPE_NORMAL, szName, szMsg);
	if ( pChar != NULL )
	{
		if ( pChar->UserLevel() >= USER_USER_GM )
			nc.emType = static_cast<EMNET_MSG_CHAT>(nc.emType | CHAT_ADD_TYPE_FOPR_GM);
	}
	SendClient(dwClient, &nc);
}


/*
// Agent->Field 에서 오는 로그인 성공정보
void InstanceServer::MsgLoginInfoAgent(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
    if (nmg == NULL)
		return;

	//AF
	NET_LOGIN_FEEDBACK_DATA3* pPacket = (NET_LOGIN_FEEDBACK_DATA3*)nmg;
	
	m_pClientManager->SetUserDbNum  (dwClient, pPacket->nUserNum);
	m_pClientManager->SetUserID   (dwClient, pPacket->szUserid);
	m_pClientManager->SetLoginTime(dwClient);
}
*/

// Agent->Field : 캐릭터 접속.
void InstanceServer::MsgGameJoinChar (DWORD dwClient, NET_MSG_GENERIC* nmg)
{	
	if (nmg == NULL)
		return;

	int nRetCode	= 0;
	DWORD dwItemNum = 0;

	//이 시점에 클라이언트와 필드사이의 세션은 아직 생성되지 않았다.

	//AF
	NET_GAME_JOIN_FIELDSVR* pMsg = (NET_GAME_JOIN_FIELDSVR*)nmg;	

	m_pAdoManager->AddGameJob(db::DbActionPtr(
								new db::CharacterInfoAndJoinField(m_nServerGroup, pMsg, new SCHARDATA2, 0.0f))
								,m_pAdoManager);
}

//DF
void InstanceServer::MsgFieldReqJoin(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
	GLMSG::NET_CHAR_JOIN_DF* pMsg = (GLMSG::NET_CHAR_JOIN_DF*)nmg;
	
	if (pMsg == NULL)
		return;

	SCHARDATA2* pCharData(pMsg->m_pCharData);

	if ( pCharData == NULL )
	{
		sc::writeLogError(std::string("InstanceServer::MsgFieldReqJoin() - pCharData is NULL."));		
		return;
	}	
	
	GLMSG::NET_CHAR_JOIN_DF::SINFO &sINFO = pMsg->m_sINFO;

	EMIP_BONUS_CLASS emIPBonus	= sINFO.m_emIPBonus;

	if (m_ServiceProvider == SP_VIETNAM)
	{
		if( sINFO.m_sVietnamSystem.lastOffLineTime != 0 )
		{
			CTime crtTime	   = CTime::GetCurrentTime();
			CTime lastOffTime  = sINFO.m_sVietnamSystem.lastOffLineTime;
			// 오프라인한 날짜가 다르면 초기화 한다.
			bool bInitialize = FALSE; // 테스트로 무조건 초기화 한다.

			// 년도, 달, 날짜가 다르면 초기화 한다.
			if( lastOffTime.GetYear() != crtTime.GetYear() ||
				lastOffTime.GetMonth() != crtTime.GetMonth() ||
				lastOffTime.GetDay() != crtTime.GetDay() )
			{
				sc::writeLogError(sc::string::format("=-=-=-=-=- INFO:Vietnam Init =-=-=-=-=- gameTime %d", sINFO.m_sVietnamSystem.gameTime) );
				sINFO.m_sVietnamSystem.gameTime   = 0;
				sINFO.m_sVietnamSystem.loginTime  = crtTime.GetTime();
			}
		}

		pCharData->m_sVietnamSystem = sINFO.m_sVietnamSystem;
	}


#ifdef CH_PARAM_USEGAIN
	pCharData->m_sChinaTime	 = sINFO.m_sChinaTime;
#endif

	//if (m_ServiceProvider == SP_OFFICE_TEST || m_ServiceProvider == SP_TAIWAN || m_ServiceProvider == SP_HONGKONG || m_ServiceProvider == SP_THAILAND)
	//{
		if( CheckTracingUser( pCharData->m_szUID, pCharData->GetUserID() ) != -1 )
		{
			pCharData->m_bTracingUser   = FALSE;
		}
	//}
	//else
	//{
	//	pCharData->m_bTracingUser   = FALSE;
	//}

	//
	//mjeon.AF
	//  AF/DF 메시지의 경우에는 m_pClientManager로부터 유효한 정보를 획득할 수 없다.
	//  그러므로 메시지를 통해 관련 정보들을 전달한다.	
	//	
	DWORD dwGaeaID		= pMsg->m_dwGaeaID;
	DWORD dwChaNum		= pCharData->m_CharDbNum;

	//
	//이 시점에는 아직 Client <-> FieldServer 사이에 세션이 맺어지지 않았다. 그러므로 유효한 ClientID가 존재하지 않는다.
	//
	GLChar *pGLChar = m_pGaeaServer->CreateCharacter(
        pCharData, 
        /*dwClientID*/dwChaNum,
        dwGaeaID,
		pMsg->m_PartyID,
        pMsg->m_Country,
		pMsg->m_dwCountryID,
        FALSE,
        sINFO.m_sStartMap,
        sINFO.m_dwStartGate,
        sINFO.m_vStartPos,
        sINFO.m_emType,
        sINFO.m_sEventTime.loginTime,
        emIPBonus);
	
	if (!pGLChar)
	{
		sc::writeLogError(std::string("InstanceServer::MsgFieldReqJoin() - Initialized character instance failed"));
		delete pCharData;
		return;
	}

	if (pGLChar->m_wLevel == 1 && pGLChar->m_vSavePos.x == 0.0f && pGLChar->m_vSavePos.z == 0.0f)
	{
		if (pGLChar->GetInvenMoney() != 0)
		{			
			sc::writeLogError(
                sc::string::format(
							"#### Money is not correct. Name %1% Money %2% ####",
							pGLChar->m_szName,
							pGLChar->GetInvenMoney()));
			pGLChar->SetInvenMoney(0);
		}
	}
	
	GLMSG::SNETPC_ACTSTATE NetMsgInner(sINFO.m_dwActState);
	pGLChar->MsgActState(&NetMsgInner);

	// 주/보조 무기 설정
	pGLChar->m_bUseArmSub = sINFO.m_bUseArmSub;

	GLLandMan* pLand(pGLChar->GetLandMan());
	const MapID& newMapID(pLand == NULL ? sINFO.m_sStartMap : pLand->GetMapID());

	// Agent 에 케릭터의 접속 성공을 알림.(Agent 가 Client 에게 새로운 Field 서버에 접속하게 요청.)	
	NET_GAME_JOIN_FIELDSVR_FB MsgJoinFB(
        sINFO.m_emType,
        m_dwInstanceServerNum,
        dwGaeaID,
        dwChaNum,
        newMapID,
        pGLChar->GetPosition());

	//MsgJoinFB.dwSlotFieldAgent = dwClientID;	
	SendAgent(&MsgJoinFB);

	delete pCharData;
}

void InstanceServer::MsgSndChatGlobal(const char* szChatMsg)
{
    if (szChatMsg)
    {
	    DWORD dwSndBytes = 0;
	    sc::writeLogInfo(std::string(szChatMsg));
	    GLMSG::NET_CHAT_LINK_FB ncf;
	    ncf.SetData(CHAT_TYPE_GLOBAL, szChatMsg);
	    SendAllClient(&ncf);
    }
}


void InstanceServer::MsgVersion(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
	if (nmg)
    {	    
	    NET_CLIENT_VERSION_SC ncv(m_LauncherVersion, m_PatchVersion);
	    SendClient(dwClient, &ncv);
    }
}

int InstanceServer::ExecuteCommand(char* strCmd)
{
    if (!strCmd)
        return NET_ERROR;

    std::string strCommand(strCmd);
    std::vector<std::string> SplitVec;
    boost::split(SplitVec, strCommand, boost::algorithm::is_any_of(" "));
    if (SplitVec.empty())
        return NET_ERROR;
	
	if (SplitVec[0] == "print_ip")
	{
		// SERVER_UTIL::CAddressChecker::GetInstance()->Save();
	}			
	else if (SplitVec[0] == "chat")
	{
        // 서버 채팅 메시지, global		
		if (SplitVec.size() > 1)
			MsgSndChatGlobal(SplitVec[1].c_str());
	}
	else if (SplitVec[0] == "profile_on")
	{
        // PROFILE 사용.
		g_bProfile = TRUE;
		sc::writeLogInfo(std::string("PROFILE ON"));
	}	
	else if (SplitVec[0] == "profile_off")
	{
        // PROFILE 사용안함.
		g_bProfile = FALSE;
		sc::writeLogInfo(std::string("PROFILE OFF"));
	}
    else
    {
    }
	return 0;
}
