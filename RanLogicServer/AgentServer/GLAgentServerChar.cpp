#include "pch.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/GLogicDataMan.h"
#include "../../RanLogic/Msg/GLContrlPcMsg.h"
#include "../../RanLogic/Msg/CharacterMsg.h"
#include "../../RanLogic/Msg/FriendMsg.h"
#include "../../RanLogic/Character/CharNameCacheDefine.h"

#include "../Character/CharNameCache.h"
#include "../Club/GLClubManAgent.h"
#include "../Club/GLClubAgent.h"
#include "../Database/DBAction/DbActionGame.h"
#include "./GLAgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL GLAgentServer::MsgLevelUpFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLMSG::NET_CHAR_LEVEL_UP_FA* pPacket = (GLMSG::NET_CHAR_LEVEL_UP_FA*) nmg;
    if (dwGaeaID != GAEAID_NULL || pPacket->m_MagicNum != GLMSG::NET_CHAR_LEVEL_UP_FA::MAGIC_NUM)
        return FALSE;
    DWORD ChaDbNum = pPacket->m_ChaDbNum;
    WORD ChaLevel = pPacket->m_ChaLevel;
    GLCharAG* pChar = GetCharByDbNum(ChaDbNum);
    if (!pChar)
        return FALSE;
    
    pChar->SetChaLevel(ChaLevel);
    DWORD ClubDbNum = pChar->GetClubDbNum();   

    std::set<DWORD> NotifyList;
    std::tr1::shared_ptr<GLClubAgent> spClub = m_pClubMan->GetClub(ClubDbNum);
    if (spClub)
    {
        spClub->SetMemberState(ChaDbNum, ChaLevel);
        spClub->GetMemberDbNumList(NotifyList);
    
        if (GLogicData::GetInstance().IsClubLogLevelUp(ChaLevel))
        {
            // Club log
            //[%1%]님이 %2%레벨을 달성했습니다.
            std::string ClubLog(
                sc::string::format(
                    ID2GAMEINTEXT("CLUB_LOG_30"),
                    pChar->GetChaName(),
                    ChaLevel));
            SENDCLUBLOG(ClubDbNum, club::LOG_MEMBER_LEVEL_UP, ClubLog);
        }
    }

    pChar->GetValidFriend(NotifyList);

    // 레벨업 알림
    GLMSG::NET_OTHER_CHAR_LEVEL_CHANGE_AC SendData(ChaDbNum, ChaLevel);
    
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);

    for (std::set<DWORD>::iterator iter=NotifyList.begin(); iter!=NotifyList.end(); ++iter)
    {
        GLCharAG* pChar = GetCharByDbNum(*iter);
        if (pChar)
            SENDTOCLIENT(pChar->ClientSlot(), NET_MSG_OTHER_CHAR_LEVEL_CHANGE_AC, SendBuffer);
    }

    return TRUE;
}

BOOL GLAgentServer::MsgChaDbNumReqCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    CHA_DB_NUM_REQ_CA RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;

    sc::string::trim(RecvData.m_ChaName);
    
    // Cache 에서 찾아본다.
    std::tr1::shared_ptr<CHAR_BASE_CACHE_DATA> spChaData = GetCharCacheData(RecvData.m_ChaName);
    if (spChaData)
    {
        // 존재한다면 바로 응답
        CHA_DB_NUM_REQ_AC SendData(
            RecvData.m_ReqSeq,
            spChaData->ChaDbNum(),
            spChaData->UserDbNum(),
            spChaData->UserType(),
            spChaData->ChaName());
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        SENDTOCLIENT(ClientSlot, NET_MSG_CHAR_DB_NUM_REQ_AC, SendBuffer);
    }
    else
    {
        // 존재하지 않는다면 DB 에서 조회해야 한다.
        AddGameAdoJob(
            db::DbActionPtr(
                new db::GetinfofromChaDb(ClientSlot, RecvData.m_ReqSeq, RecvData.m_ChaName, charnamechache::REQUEST_CLIENT)));
    }

    return TRUE;
}

BOOL GLAgentServer::MsgCharDbNumReqDA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID)
{
    if (GaeaID != GAEAID_NULL || ClientSlot != DBACTION_CLIENT_ID)
        return FALSE;

    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    CHA_DB_NUM_REQ_DA Data;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Data))
        return FALSE;

    if (Data.m_ChaDbNum != 0)
		ChaNameDbNumAdd(Data.m_ChaName, Data.m_ChaDbNum, Data.m_UserDbNum, static_cast<EMUSERTYPE> (Data.m_UserType), Data.m_iClass, Data.m_iLv);
        //ChaNameDbNumAdd(Data.m_ChaName, Data.m_ChaDbNum, Data.m_UserDbNum, static_cast<EMUSERTYPE> (Data.m_UserType));

    switch ( Data.m_ReqType )
    {
    case charnamechache::REQUEST_CLIENT:
        {
            // 요청한 클라이언트에 응답
            CHA_DB_NUM_REQ_AC SendData(
                Data.m_ReqSeq,
                Data.m_ChaDbNum,
                Data.m_UserDbNum,
                Data.m_UserType,
                Data.m_ChaName);
            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, SendData);
            SENDTOCLIENT(Data.m_ClientSlot, NET_MSG_CHAR_DB_NUM_REQ_AC, SendBuffer);
        }
        break;
    case charnamechache::REQUEST_SERVER_REQFIREND:
        if (Data.m_ChaDbNum != 0 && Data.m_ReqCharName.empty() != TRUE)
            FriendOfflineRequest( Data.m_ReqCharName, Data.m_ChaName );
        break;
    case charnamechache::REQUEST_SERVER_ADDFIREND:
        {
            GLCharAG* pChar = GetChar( Data.m_ReqCharName );
            if( pChar )
                if( Data.m_ChaDbNum != 0 )
                    pChar->AddFriendAndGroup(Data.m_ChaName.c_str());
        }
        break;
    case charnamechache::REQUEST_SERVER_BLOCKFIREND:
        {
            GLCharAG* pChar = GetChar( Data.m_ReqCharName );
            if( pChar )
                if( Data.m_ChaDbNum != 0 )
                    pChar->BlockFriend(Data.m_ChaName.c_str());
        }

        break;
    }


 
    return TRUE;
}