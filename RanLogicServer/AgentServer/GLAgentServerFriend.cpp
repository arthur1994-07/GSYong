#include "pch.h"
#include "../../RanLogic/Msg/FriendMsg.h"
#include "../../RanLogic/GLUseFeatures.h"

#include "../CaptureTheField/CaptureTheFieldManager.h"
#include "../Database/DBAction/DbActionGameFriend.h"
#include "./GLAgentServer.h"

#include "../../SigmaCore/DebugInclude.h"

BOOL GLAgentServer::MsgFriendGroupNewCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLCharAG* pChar = GetChar(dwGaeaID);
    if (!pChar)
        return FALSE;

    GLMSG::NET_FRIEND_GROUP_NEW_CA* pPacket = (GLMSG::NET_FRIEND_GROUP_NEW_CA*) nmg;
    char GroupName[CHR_ID_LENGTH] = {0};
    StringCchCopy(GroupName, CHR_ID_LENGTH, pPacket->m_GroupName);
    pChar->AddFriendGroup(std::string(GroupName));

    return TRUE;
}

BOOL GLAgentServer::MsgFriendGroupDelCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLCharAG* pChar = GetChar(dwGaeaID);
    if (!pChar)
        return FALSE;

    GLMSG::NET_FRIEND_GROUP_DEL_CA* pPacket = (GLMSG::NET_FRIEND_GROUP_DEL_CA*) nmg;    
    pChar->DelFriendGroup(pPacket->m_GroupNum);

    return TRUE;
}

BOOL GLAgentServer::MsgFriendSetGroupCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLCharAG* pChar = GetChar(dwGaeaID);
    if (!pChar)
        return FALSE;

    GLMSG::NET_FRIEND_SET_GROUP_CA* pPacket = (GLMSG::NET_FRIEND_SET_GROUP_CA*) nmg;
    pChar->SetFriendGroup(pPacket->m_FriendDbNum, pPacket->m_GroupNum);

    return TRUE;
}

BOOL GLAgentServer::MsgFriendGroupNameChangeCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLCharAG* pChar = GetChar(dwGaeaID);
    if (!pChar)
        return FALSE;

    GLMSG::NET_FRIEND_GROUP_NAME_CHANGE_CA* pPacket = (GLMSG::NET_FRIEND_GROUP_NAME_CHANGE_CA*) nmg;
    char GroupName[CHR_ID_LENGTH] = {0};
    StringCchCopy(GroupName, CHR_ID_LENGTH, pPacket->m_GroupName);
    pChar->FriendGroupRename(pPacket->m_GroupNum, std::string(GroupName));

    return TRUE;
}

BOOL GLAgentServer::MsgFriendGroupReqCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLCharAG* pChar = GetChar(dwGaeaID);
    if (!pChar)
        return FALSE;

    //GLMSG::NET_FRIEND_GROUP_REQ_CA* pPacket = (GLMSG::NET_FRIEND_GROUP_REQ_CA*) nmg;
    pChar->MsgFriendGroupReq();
    return TRUE;
}

BOOL GLAgentServer::MsgFriendStateReqCA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD dwGaeaID)
{
    GLCharAG* pChar = GetChar(dwGaeaID);
    if (!pChar)
        return FALSE;

    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    GLMSG::NET_FRIEND_STATE_REQ_CA Data;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Data))
        return FALSE;
    
    GLCharAG* pMasterChar = GetCharByDbNum(Data.m_MainChar);
    if (pMasterChar)
    {
        // 알고싶은 캐릭터가 접속중이다
        GLMSG::NET_FRIEND_STATE_REQ_AC SendData(
            Data.m_MainChar,
            Data.m_SubChar,
            pMasterChar->GetStateFriend(Data.m_SubChar));
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        SENDTOCLIENT(ClientSlot, NET_MSG_FRIEND_STATE_REQ_AC, SendBuffer);
    }
    else
    {
        // DB 에 요청
        AddGameAdoJob(
            db::DbActionPtr(
                new db::FriendStateSelect(Data.m_MainChar, Data.m_SubChar, pChar->ChaDbNum())));
    }

    return TRUE;
}

BOOL GLAgentServer::MsgFriendStateReqDA(NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD dwGaeaID)
{
    if (dwGaeaID != GAEAID_NULL)
        return FALSE;

    GLMSG::NET_FRIEND_STATE_REQ_DA* pPacket = (GLMSG::NET_FRIEND_STATE_REQ_DA*) pMsg;

    GLCharAG* pChar = GetCharByDbNum(pPacket->m_ReqChaDbNum);
    if (!pChar)
        return FALSE;
    
    GLMSG::NET_FRIEND_STATE_REQ_AC SendData(
        pPacket->m_MainChar,
        pPacket->m_SubChar,
        pPacket->m_State);
    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);
    SENDTOCLIENT(pChar->ClientSlot(), NET_MSG_FRIEND_STATE_REQ_AC, SendBuffer);

    return TRUE;
}


// 친구이동 요청 처리
BOOL GLAgentServer::Msg2FriendReq ( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	if ( dwGaeaID != GAEAID_NULL )
		return FALSE;

	GLMSG::SNETPC_2_FRIEND_REQ* pNetMsg = (GLMSG::SNETPC_2_FRIEND_REQ*) nmg;
	GLMSG::SNETPC_2_FRIEND_FB NetMsgFB;

	GLCharAG* pMyChar = GetCharByDbNum(pNetMsg->dwChaDbNum);
	if (!pMyChar)
		return FALSE;

	std::tr1::shared_ptr<SFRIEND> sFRIEND = pMyChar->GetFriend(pNetMsg->szFRIEND_NAME);
	if (!sFRIEND)
	{
		NetMsgFB.emFB = EM2FRIEND_FB_FAIL;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	if ( sFRIEND->m_Online == false )
	{
		NetMsgFB.emFB = EM2FRIEND_FB_FRIEND_CONDITION;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	GLCharAG* pCHAR_FRIEND = GetCharByDbNum(sFRIEND->m_ChaDbNum);
	if (!pCHAR_FRIEND)
	{
		NetMsgFB.emFB = EM2FRIEND_FB_FRIEND_CONDITION;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

    if ( pMyChar->m_dwUserLvl < USER_USER_GM &&
        pCHAR_FRIEND->m_dwUserLvl >= USER_USER_GM )
    {
        // 일반 유저가 GM계정으로 이동 안됨.
        GLMSG::SNETPC_2_FRIEND_FB NetMsgFB;
        NetMsgFB.emFB = EM2FRIEND_FB_IMMOVABLE;
        SENDTOCLIENT( pMyChar->m_ClientSlot, &NetMsgFB );

        return FALSE;
    }

	// 상대가 나를 삭제했거나 차단한 상태이면 이동이 취소된다
	std::tr1::shared_ptr<SFRIEND> sFRIEND2 = pCHAR_FRIEND->GetFriend(pMyChar->m_szName);
	if (!sFRIEND2)
	{
		NetMsgFB.emFB = EM2FRIEND_FB_FAIL;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	if (sFRIEND2->IsBLOCK())
	{
		NetMsgFB.emFB = EM2FRIEND_FB_FRIEND_BLOCK;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	if (sFRIEND2->IsOFF())
	{
		NetMsgFB.emFB = EM2FRIEND_FB_FRIEND_OFF;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	if (pCHAR_FRIEND->m_dwCurFieldSvr==FIELDSERVER_MAX)
	{
		NetMsgFB.emFB = EM2FRIEND_FB_FRIEND_CONDITION;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	if (pCHAR_FRIEND->m_nChannel != pMyChar->m_nChannel)
	{
		NetMsgFB.emFB = EM2FRIEND_FB_FRIEND_CHANNEL;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

	if (pCHAR_FRIEND->GetCurrentMap().isSameID() == false )
	{
		NetMsgFB.emFB = EM2FRIEND_FB_FRIEND_INSTANTMAP;
		SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
		return FALSE;
	}

    // CTF 중이면 이동할 수 없다.
	// 이제 점령전 진행중에도 사냥터로 이용할 수 있게 되었다;
    /*if (GLUseFeatures::GetInstance().IsUsingCaptureTheField())
    {
        if (m_pCTFMan->GetPvPMapID() == pCHAR_FRIEND->GetCurrentMap().getGaeaMapID().Id() ||
            m_pCTFMan->GetPvEMapID() == pCHAR_FRIEND->GetCurrentMap().getGaeaMapID().Id() )
        {
            if (m_pCTFMan->IsStarted())
            {
                NetMsgFB.emFB = EM2FRIEND_FB_IMMOVABLE;
                SENDTOCLIENT(pMyChar->ClientSlot(), &NetMsgFB);
                return FALSE;
            }
        }
    }*/

	// 친구의 위치 점검 요청.
	GLMSG::SNETPC_2_FRIEND_CK NetMsgCK;
	NetMsgCK.dwCOMMAND_CHARID = pMyChar->m_CharDbNum;
	NetMsgCK.dwFRIEND_CHARID = pCHAR_FRIEND->m_CharDbNum;
	NetMsgCK.dwItemID = pNetMsg->dwItemID;
	NetMsgCK.wItemPosX = pNetMsg->wItemPosX;
	NetMsgCK.wItemPosY = pNetMsg->wItemPosY;
	SENDTOFIELDSVR(pCHAR_FRIEND->m_nChannel, pCHAR_FRIEND->m_dwCurFieldSvr, &NetMsgCK);

	return TRUE;
}
