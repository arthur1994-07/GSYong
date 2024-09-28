#include "../pch.h"
#include "../../RanLogic/Msg/CharacterMsg.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../../RanLogic/Msg/GLContrlMsg.h"

#include "../Cache/CacheUserLobby.h"
#include "../Cache/CacheRandomboxChance.h"
#include "../Character/GLCharCache.h"
#include "../Database/DBAction/DbActionGame.h"
#include "../Database/DbManager.h"
#include "../Net/NetCachePacket.h"
#include "../PrivateMarket/GLPrivateMarketSearchMan.h"

#include "./CacheServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

//! 캐릭터 게임 조인
void CacheServer::MsgGameJoinAH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    cp::NET_CHAR_JOIN_AH* pPacket = (cp::NET_CHAR_JOIN_AH*) pMsg;
    DWORD UserDbNum = pPacket->UserDbNum();
    DWORD ChaDbNum = pPacket->ChaDbNum();

    // 캐릭터 상태 온라인
    m_pDbManager->AddGameJob(
        db::DbActionPtr(
            new db::CharacterOnline(
                ChaDbNum)));

    // 캐릭터 체크
    CACHE_CHAR_ITER iter = m_CacheChar.find(ChaDbNum);
    if (iter != m_CacheChar.end())
    {
        // 이미 존재한다?
        sc::writeLogError(
            sc::string::format("MsgGameJoinAC User %1% Character %2% already exist", UserDbNum, ChaDbNum));
    }
    else
    {        
        m_CacheChar.insert(
            CACHE_CAHR_VALUE(ChaDbNum, spCharCache(new GLCharCache(UserDbNum, ChaDbNum))));
    }

    // 캐릭터 Point 를 가져온다. (PointShop)
    m_pDbManager->AddGameJob(
        db::DbActionPtr(
        new db::CharacterPointGet(
        ChaDbNum)));

    // Lobby User Info 제거, 더이상 Lobby 정보 유지할 필요가 없다.
    m_pUserLobbyMan->RemoveUser(UserDbNum);

 #ifndef _RELEASE
     sc::writeLogInfo(
         sc::string::format("MsgGameJoinAC User %1% Character %2% Online", UserDbNum, ChaDbNum));
 #endif

	//우편
	if (m_spPostCache)
		m_spPostCache->LoadPost(ChaDbNum);
}

//! DB->Cache Server:Character Point 정보
void CacheServer::MsgCharPointDH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_CHAR_POINT_DH* pPacket = (NET_CHAR_POINT_DH*) pMsg;
    int ChaDbNum = pPacket->ChaDbNum();
    CHARACTER_POINT Point = pPacket->Point();

    CACHE_CHAR_ITER iter = m_CacheChar.find(ChaDbNum);
    if (iter == m_CacheChar.end())
    {
        sc::writeLogError(
            sc::string::format(
            "MsgCharPointDh Character not found %1%", ChaDbNum));
        return;
    }

    spCharCache spChar = iter->second;
    spChar->PointSet(Point);

// #ifndef _RELEASE
//     sc::writeLogInfo(
//         sc::string::format(
//         "MsgCharPointDh Character %1% Point (%2%, %3%)",
//         ChaDbNum,
//         Point.Refundable(),
//         Point.NotRefundable()));
// #endif
}

spCharCache CacheServer::GetChar(DWORD ChaDbNum)
{
    CACHE_CHAR_ITER iter = m_CacheChar.find(ChaDbNum);
    if (iter != m_CacheChar.end())
    {
        return iter->second;
    }
    else
    {
        static spCharCache spChar;
        return spChar;
    }
}

bool CacheServer::IsOnline( DWORD dwChaDbNum )
{
	CACHE_CHAR_ITER iter = m_CacheChar.find(dwChaDbNum);
	if (iter != m_CacheChar.end())
		return true;

	return false;
}

bool CacheServer::IsIntegrationServer()
{
	return net::SERVER_INTEGRATION_CACHE == m_pConfig->GetServerType();
}

//! Field->Cache:Character Point 정보 요청
void CacheServer::MsgCharReqPointFH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    cp::NET_CHAR_REQ_POINT_FH* pPacket = (cp::NET_CHAR_REQ_POINT_FH*) pMsg;
    DWORD ChaDbNum = pPacket->ChaDbNum();
    spCharCache spChar = GetChar(ChaDbNum);
    if (spChar)
    {
        NET_CHAR_POINT_HF MsgHF(ChaDbNum, spChar->Point());
        SendClient(dwClient, &MsgHF);
    }
    else
    {
        // 이런 경우가 발생하면 안된다.
        // 반드시 캐릭터는 접속을 종료하기 전까지 Cache 서버에 있어야 한다.
        sc::writeLogError(
            sc::string::format(
                "MsgCharReqPointFH can't find Character. ChaDbNum %1%",
                ChaDbNum));

        // 캐릭터 정보가 없기 때문에 DB 에서 직접 가져올 수 밖에 없다.
        __int64 PointRefundable = 0;
        __int64 PointNotRefundable = 0;
        m_pDbManager->GetChaPoint(ChaDbNum, PointRefundable, PointNotRefundable);

        NET_CHAR_POINT_HF MsgHF(ChaDbNum, CHARACTER_POINT(PointRefundable, PointNotRefundable));
        SendClient(dwClient, &MsgHF);
    }
}

void CacheServer::MsgGameCloseAH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    cp::NET_CHAR_CLOSE_AH* pPacket = (cp::NET_CHAR_CLOSE_AH*) pMsg;
    DWORD UserDbNum = pPacket->UserDbNum();
    DWORD ChaDbNum = pPacket->ChaDbNum();

	// 캐릭터가 접속 종료한 경우 개인상점에서도 삭제한다.
	if ( m_pPrivateMarketSearch )
	{
		m_pPrivateMarketSearch->DeleteItem( ChaDbNum, /*임시로 0으로*/0 );
	}

    // 캐릭터 상태 오프라인 
    m_pDbManager->AddGameJob(
        db::DbActionPtr(
            new db::CharacterOffline(ChaDbNum)));

    // 캐릭터 제거
    CACHE_CHAR_ITER iter = m_CacheChar.find(ChaDbNum);
    if (iter != m_CacheChar.end())
    {
        m_CacheChar.erase(iter);        
    }
    else
    {
        // 이미 존재 하지 않는다?
        sc::writeLogError(
            sc::string::format("MsgGameCloseAC User %1% Character %2% already not exist", UserDbNum, ChaDbNum));        
    }

    // Lobby User Info 제거
    m_pUserLobbyMan->RemoveUser(UserDbNum);

#ifndef _RELEASE
    sc::writeLogInfo(
        sc::string::format("User %1% Character %2% OffLine", UserDbNum, ChaDbNum));
#endif

	if (m_spPostCache)
		m_spPostCache->UnLoadPost(ChaDbNum);
}

//! Agent->Cache:캐릭터 리스트 요청
void CacheServer::MsgChaListAH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_CHA_LIST_AH* pPacket = (NET_CHA_LIST_AH*) pMsg;
    /*
    m_pDbManager->AddGameJob(
        db::DbActionPtr(
            new db::CharacterBasicInfoAgent(
                pPacket->m_UserNum,
                pPacket->m_ServerGroup,
                pPacket->m_ClientSlot,
                dwClient,
                pPacket->m_Ip,
                pPacket->m_Port)));
    */
    // 미리 유저의 캐릭터 정보를 모두 가져와서 세팅해 둔다.
    m_pDbManager->AddGameJob(
        db::DbActionPtr(
        new db::CharacterBasicInfoAgentAll(
            pPacket->m_UserNum,
            pPacket->m_ServerGroup,
            pPacket->m_ClientSlot,
            dwClient,
            pPacket->m_Ip,
            pPacket->m_Port)));
}

//! Database->Cache:User Character List
void CacheServer::MsgChaBaInfoDH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_CHA_BBA_INFO_DH* pPacket = (NET_CHA_BBA_INFO_DH*) pMsg;
    NET_CHA_BBA_INFO_HA MsgHA(pPacket->m_AgentServerSlot, pPacket->m_uPort, std::string(pPacket->m_Ip));
    MsgHA.SetChaTotalNum(pPacket->m_ChaServerTotalNum);
    for (size_t i=0; i<MAX_ONESERVERCHAR_NUM; ++i)
        MsgHA.SetChaNum(i, pPacket->m_ChaDbNum[i]);
    
    SendClient(pPacket->m_CacheServerSlot, &MsgHA);
}

//! Agent->Cache:Character Lobby display info request
void CacheServer::MsgChaLobbyInfoAH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_LOBBY_CHAR_INFO_AH* pPacket = (NET_LOBBY_CHAR_INFO_AH*) pMsg;
    /*
    m_pDbManager->AddGameJob(
    db::DbActionPtr(
        new db::CharacterBasicInfo(
            pPacket->m_UserDbNum,
            pPacket->m_ChaDbNum, 
            pPacket->m_AgentSlot,
            dwClient, 
            pPacket->m_IpAddress,
            pPacket->m_Port)));
    */
    DWORD UserDbNum = pPacket->m_UserDbNum;
    DWORD ChaDbNum = pPacket->m_ChaDbNum;

    const spChacheUserLobby spUserInfo = m_pUserLobbyMan->GetUser(UserDbNum);
    if (!spUserInfo)
    {
        // 정보가 없다. NET_CHA_LIST_AH 요청을 미리했는가?
        return;
    }

    SCHARINFO_LOBBY ChaData;
    if (spUserInfo->GetChar(ChaDbNum, ChaData))
    {
        NET_LOBBY_CHAR_INFO_HA SendData(
            pPacket->m_UserDbNum,
            pPacket->m_AgentSlot,
            pPacket->m_IpAddress,
            pPacket->m_Port,
            ChaData);
        
        msgpack::sbuffer PackBuffer;
        msgpack::pack(PackBuffer, SendData);

        NET_MSG_PACK MsgHA;
        if (MsgHA.SetData(NET_MSG_LOBBY_CHAR_INFO_HA, PackBuffer))
            SendClient(spUserInfo->CacheSlot(), &MsgHA);
        else
            sc::writeLogError("CacheServer::MsgChaLobbyInfoAH");
    }
    else
    {
        // 신규 생성 캐릭터
        m_pDbManager->AddGameJob(
            db::DbActionPtr(
                new db::CharacterBasicInfoNew(
                    pPacket->m_UserDbNum,
                    pPacket->m_ChaDbNum, 
                    pPacket->m_AgentSlot,
                    dwClient, 
                    pPacket->m_IpAddress,
                    pPacket->m_Port)));
    }
}

void CacheServer::MsgChaLobbyInfoAHEnd(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_LOBBY_CHAR_INFO_AH_END* pPacket = (NET_LOBBY_CHAR_INFO_AH_END*) pMsg;

    DWORD UserDbNum = pPacket->m_UserDbNum;
    
    const spChacheUserLobby spUserInfo = m_pUserLobbyMan->GetUser(UserDbNum);
    if (!spUserInfo)
    {
        return;
    }

    NET_LOBBY_CHAR_INFO_HA_END MsgHA(
        pPacket->m_UserDbNum,
		pPacket->m_AgentSlot,
        pPacket->m_IpAddress,
        pPacket->m_Port
		);

    SendClient(spUserInfo->CacheSlot(), &MsgHA);    
}

//! DB->Cache:Character Lobby display info request
void CacheServer::MsgChaLobbyInfoDH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    //NET_LOBBY_CHAR_INFO_DH* pPacket = (NET_LOBBY_CHAR_INFO_DH*) pMsg;
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;

    NET_LOBBY_CHAR_INFO_DH MsgDH;
    if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), MsgDH))
    {
        NET_LOBBY_CHAR_INFO_HA MsgHA(
            MsgDH.m_UserDbNum,
            MsgDH.m_AgentSlot,
            MsgDH.m_IpAddress,
            MsgDH.m_Port,
            MsgDH.m_ChaData);

        msgpack::sbuffer PackBuffer;
        msgpack::pack(PackBuffer, MsgHA);

        NET_MSG_PACK MsgServer;
        if (MsgServer.SetData(NET_MSG_LOBBY_CHAR_INFO_HA, PackBuffer))
            SendClient(MsgDH.m_CacheSlot, &MsgServer);
        else
            sc::writeLogError("CacheServer::MsgChaLobbyInfoDH SetData");
    }
    else
    {
        sc::writeLogError("CacheServer::MsgChaLobbyInfoDH");
    }
}

//! DB->Cache:신규 캐릭터 Character Lobby display info request
void CacheServer::MsgChaLobbyNewInfoDH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    // NET_MSG_LOBBY_NEW_CHAR_INFO_DH
    // NET_LOBBY_CHAR_INFO_NEW_DH* pPacket = (NET_LOBBY_CHAR_INFO_NEW_DH*) pMsg;
    // NET_LOBBY_CHAR_INFO_DH2* pPacket = (NET_LOBBY_CHAR_INFO_DH2*) pMsg;
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_LOBBY_CHAR_INFO_NEW_DH MsgData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), MsgData))
    {
        sc::writeLogError("CacheServer::MsgChaLobbyNewInfoDH");
        return;
    }

    m_pUserLobbyMan->AddChar(MsgData.m_UserDbNum, MsgData.m_ChaData);

    NET_LOBBY_CHAR_INFO_HA SendData(
        MsgData.m_UserDbNum,
        MsgData.m_AgentSlot,
        MsgData.m_IpAddress,
        MsgData.m_Port,
        MsgData.m_ChaData);

    msgpack::sbuffer PackBuffer;
    msgpack::pack(PackBuffer, SendData);

    NET_MSG_PACK MsgAC;
    if (MsgAC.SetData(NET_MSG_LOBBY_CHAR_INFO_HA, PackBuffer))
        SendClient(MsgData.m_CacheSlot, &MsgAC);
    else
        sc::writeLogError("CacheServer::MsgChaLobbyNewInfoDH");
}

void CacheServer::MsgLobbyUserInfoDH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_LOBBY_USER_INFO_DH* pPacket = (NET_LOBBY_USER_INFO_DH*) pMsg;
    m_pUserLobbyMan->RemoveUser(pPacket->m_UserDbNum);
    m_pUserLobbyMan->AddUser(
        pPacket->m_UserDbNum,
        pPacket->m_CacheSlot,
        pPacket->m_AgentSlot,
        pPacket->m_IpAddress,        
        pPacket->m_Port);
}

void CacheServer::MsgLobbyCharInfoDH2(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    // NET_MSG_LOBBY_CHAR_INFO_DH2
    // NET_LOBBY_CHAR_INFO_DH2* pPacket = (NET_LOBBY_CHAR_INFO_DH2*) pMsg;
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    NET_LOBBY_CHAR_INFO_DH2 RecvData;
    if (msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        m_pUserLobbyMan->AddChar(RecvData.m_UserDbNum, RecvData.m_ChaData);
    else
        sc::writeLogError("CacheServer::MsgLobbyCharInfoDH2");
}

void CacheServer::MsgLobbyUserInfoEndDH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_LOBBY_USER_INFO_END_DH* pPacket = (NET_LOBBY_USER_INFO_END_DH*) pMsg;

    const spChacheUserLobby spUserInfo = m_pUserLobbyMan->GetUser(pPacket->m_UserDbNum);
    if (!spUserInfo)
        return;
    
    NET_CHA_BBA_INFO_HA MsgHA(
        spUserInfo->AgentClientSlot(),
        spUserInfo->Port(),
        spUserInfo->IpAddress());
    MsgHA.SetChaTotalNum(spUserInfo->TotalChaNum());

    for (size_t i=0; i<MAX_ONESERVERCHAR_NUM; ++i)
        MsgHA.SetChaNum(i, spUserInfo->GetCharDbNum(i));

    SendClient(spUserInfo->CacheSlot(), &MsgHA);    
}


void CacheServer::MsgGetChaExtraInfoFH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_GET_CHA_EXTRA_INFO_FH *pPacket = (GLMSG::SNET_GET_CHA_EXTRA_INFO_FH *) pMsg;

	 m_pDbManager->AddGameJob(
						db::DbActionPtr(new db::GetChaExtraInfo(dwClient, pPacket->dwChaNum)));
}


void CacheServer::MsgGetChaExtraInfoDH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_GET_CHA_EXTRA_INFO_DH *pPacket = (GLMSG::SNET_GET_CHA_EXTRA_INFO_DH *) pMsg;


	//
	//mjeon.capturethefield.tracing
	//
	sc::writeLogDebug(
        sc::string::format(
            "Char %1% ContributionPoint %2%",
            pPacket->dwChaNum,
            pPacket->nContributionPoint));


	GLMSG::SNET_GET_CHA_EXTRA_INFO_FH_FB msg;
	msg.dwChaNum			= pPacket->dwChaNum;
	msg.nContributionPoint	= pPacket->nContributionPoint;

	SendClient(dwClient, &msg);
}


void CacheServer::MsgSetChaExtraInfoFH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
	GLMSG::SNET_SET_CHA_EXTRA_INFO_FH *pPacket = (GLMSG::SNET_SET_CHA_EXTRA_INFO_FH *) pMsg;


	//
	//mjeon.capturethefield.tracing
	//
	sc::writeLogInfo(sc::string::format("(TRACE) Save ContributionPoint(%1%) for Char(%2%).", pPacket->nContributionPoint, pPacket->dwChaNum));


	m_pDbManager->AddGameJob(
						db::DbActionPtr(new db::SetChaExtraInfo(pPacket->dwChaNum, pPacket->nContributionPoint)));
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.15, 확률제어 데이터 추가
 * modified : 
 */
void CacheServer::MsgGmSetRandomboxChanceReqAH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_HA NetMsgFB;

    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_AH* pPacket = (GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_REQ_AH*)pMsg;

    if (m_pRandomboxMan->Add(pPacket->sBoxID, pPacket->sItemID, pPacket->nReqCount, pPacket->dwChaNum, pPacket->dwGMNum))
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_HA::REP_FLAG_SUCESS;
    }
    else
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_REGISTER_FB_HA::REP_FLAG_DUPLICATE_ERR;
    }

    NetMsgFB.dwClientSlot = pPacket->dwClientSlot;

    SendClient(dwClient, &NetMsgFB);

 #ifndef _RELEASE
    sc::writeLogInfo("MsgGmSetRandomboxChanceReqAH Success!!");
 #endif
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.15, 확률제어 데이터 List 요청
 * modified : 
 */
void CacheServer::MsgGmGetRandomboxChanceListAH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_HA NetMsgFB;

    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_AH* pPacket = (GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_REQ_AH*)pMsg;

    NetMsgFB.dwClientSlot = pPacket->dwClientSlot;

    std::vector<RANDOMBOX_CHANCE_CACHE_DATA> vRCCD;
    if (m_pRandomboxMan->List(vRCCD))
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_HA::REP_FLAG_SUCESS;

		for (int i = 0; i < (int)vRCCD.size(); i++)
		{	
            NetMsgFB.nIndex = vRCCD[i].UniqueIndex();
            NetMsgFB.sBoxID = vRCCD[i].BoxId();
            NetMsgFB.sItemID = vRCCD[i].ItemId();
            NetMsgFB.nReqCount = vRCCD[i].ReqCount();
            NetMsgFB.dwTargetChaNum = vRCCD[i].ChaNum();
            NetMsgFB.dwGMNum = vRCCD[i].GMNum();
            NetMsgFB.nOpenCount = vRCCD[i].OpenCount();

            SendClient(dwClient, &NetMsgFB);
		}   
    }
    else
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_LIST_FB_HA::REP_FLAG_NOTFOUND_ERR;

        SendClient(dwClient, &NetMsgFB);
    }

 #ifndef _RELEASE
    sc::writeLogInfo("MsgGmGetRandomboxChanceListAH Success");
 #endif
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.15, 확률제어 데이터 삭제 요청
 * modified : 
 */
void CacheServer::MsgGmDelRandomboxChanceAH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_HA NetMsgFB;

    GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_AH* pPacket = (GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_REQ_AH*)pMsg;

    if (m_pRandomboxMan->Delete(pPacket->nIndex))
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_HA::REP_FLAG_SUCESS;

        // 데이터 삭제시에도 로그를 남겨야 할까? -> 시간나면 처리하자
        /*
        m_pDbManager->AddLogAdoJob(db::DbActionPtr(new db::LogRandomboxChanceComplete(pNetMsg->sBoxID,
                                                                                      pNetMsg->sItemID,
                                                                                      pNetMsg->nReqCount,
                                                                                      pNetMsg->dwTargetChaNum,
                                                                                      pNetMsg->dwGMNum,
                                                                                      pNetMsg->nOpenCount,
                                                                                      pNetMsg->dwChaNum
                                                                                     )));
        */
    }
    else
    {
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GM_RANDOMBOX_CHANCE_DELETE_FB_HA::REP_FLAG_NOTFOUND_ERR;
    }

    NetMsgFB.dwClientSlot = pPacket->dwClientSlot;

    SendClient(dwClient, &NetMsgFB);

 #ifndef _RELEASE
    sc::writeLogInfo("MsgGmDelRandomboxChanceAH Success!!");
 #endif
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.15, Randombox open 처리
 * modified : 
 */
void CacheServer::MsgRandomboxOpenChanceReqFH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_FB_HF NetMsgFB;

    GLMSG::SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_REQ_FH* pPacket = (GLMSG::SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_REQ_FH*) pMsg;

    RANDOMBOX_CHANCE_CACHE_DATA rccd;
    INT32 nIndexAll = 0;
    INT32 nIndexCha = 0;
    if (m_pRandomboxMan->CheckRandomboxOpenChance(pPacket->sBoxID, pPacket->dwChaNum, rccd, nIndexAll, nIndexCha))
    {
 #ifndef _RELEASE
        sc::writeLogInfo("MsgRandomboxOpenChanceReqFH Winner check!!");
 #endif
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_FB_HF::REP_FLAG_WINNER;
    }
    else
    {
 #ifndef _RELEASE
        sc::writeLogInfo("MsgRandomboxOpenChanceReqFH Normal check!!");
 #endif
        NetMsgFB.wRepFlag = GLMSG::SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_FB_HF::REP_FLAG_NORMAL;
    }
    NetMsgFB.dwGaeaID = pPacket->dwGaeaID;
    NetMsgFB.dwChaNum = pPacket->dwChaNum;
    NetMsgFB.wPosX = pPacket->wPosX;
    NetMsgFB.wPosY = pPacket->wPosY;
    NetMsgFB.sBoxID = pPacket->sBoxID;
    NetMsgFB.sItemID = rccd.ItemId();
    NetMsgFB.nReqCount = rccd.ReqCount();
    NetMsgFB.dwTargetChaNum = rccd.ChaNum();
    NetMsgFB.dwGMNum = rccd.GMNum();
    NetMsgFB.nOpenCount = rccd.OpenCount();
    NetMsgFB.nIndexAll = nIndexAll;
    NetMsgFB.nIndexCha = nIndexCha;

    SendClient(dwClient, &NetMsgFB);

 #ifndef _RELEASE
    sc::writeLogInfo(sc::string::format("MsgRandomboxOpenChanceReqFH [%1%][%2%]", nIndexAll, nIndexCha));
 #endif
}

/*
 * redmine : #1161 Randombox 확률제어 시스템 처리
 * created : sckim, 2015.12.15, Randombox open 확률 데이터 완료 처리
 * modified : 
 */
void CacheServer::MsgRandomboxOpenChanceEndFH(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    GLMSG::SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_END_FH* pPacket = (GLMSG::SNET_MSG_GCTL_RANDOMBOX_OPEN_CHANCE_END_FH*)pMsg;

    m_pRandomboxMan->EndRandomboxOpenChanceDelete(pPacket->bComplete, pPacket->nIndexAll, pPacket->nIndexCha);

 #ifndef _RELEASE
    sc::writeLogInfo(sc::string::format("MsgRandomboxOpenChanceEndFH [%1%][%2%]", pPacket->nIndexAll, pPacket->nIndexCha));
 #endif
}


void CacheServer::MsgLogMoneyFH ( DWORD _dwClient, NET_MSG_GENERIC* _pMsg )
{
	if ( m_pMoneyMan )
		m_pMoneyMan->OnMessage( _dwClient, static_cast< GLMSG::NET_LOG_MESSAGE_HEADER* >( _pMsg ) );
}


void CacheServer::AdoTest1()
{
    SCHARINFO_LOBBY ChaData;
    m_pDbManager->GetChaBInfo(99989, 39350, &ChaData);
}

void CacheServer::AdoTest2()
{
    std::vector<SCHARINFO_LOBBY> vCharInfo;
    m_pDbManager->GetChaBInfoAll(99989, 1, vCharInfo);
}

