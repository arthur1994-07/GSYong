#include "pch.h"
#include "./GLGaeaServer.h"
#include "../../RanLogic/Msg/PVEMsg.h"
#include "../../RanLogic/Msg/TraceMsg.h"
#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "../../RanLogicServer/Party/GLPartyField.h"
#include "../../RanLogicServer/TriggerSystem/ServerTriggerSystem.h"
#include "../../RanLogicServer/TriggerSystem/ServerEntryMessage.h"
#include "../../RanLogicServer/TriggerSystem/ServerPortalMessage.h"
#include "../../RanLogicServer/TriggerSystem/ServerMapInfoMessage.h"
#include "../../RanLogicServer/TriggerSystem/ServerRetryMessage.h"
#include "../../RanLogicServer/TriggerSystem/ServerReturnMessage.h"
#include "../../RanLogicServer/TriggerSystem/ServerRewardMessage.h"
#include "../../RanLogicServer/TriggerSystem/ServerStageMessage.h"
#include "../../RanLogicServer/TriggerSystem/ServerFindTriggerMessage.h"
#include "../../RanLogicServer/TriggerSystem/ServerPortalOpenTrigger.h"

#include "../../RanLogicServer/Move/MoveManagerField.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL GLGaeaServer::MsgReqPVECreateInstantMapAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (dwClientID != GetAgentSlot())
    {
        return FALSE;
    }

    GLMSG::NET_PVE_CREATE_INSTANT_MAP_REQ_AF ReqMsg;

    if (CastNetMsg(nmg, ReqMsg) == false)
    {
        return FALSE;
    }

	SPVEENTRYINSTMAPINFO instPVEInfo;
    {
        instPVEInfo.PartyID = ReqMsg.sPartyID;
        instPVEInfo.sBaseMapID = ReqMsg.dwBaseMapID;
        instPVEInfo.instanceMapID = ReqMsg.dwInstanceMapID;
        instPVEInfo.sInDunID = ReqMsg.sInDunID;
        instPVEInfo.nStage = ReqMsg.nStage;
        instPVEInfo.strGuid = ReqMsg.strGuid;
    }

	m_quePVEInstantMapToCreate.push(instPVEInfo);

    return TRUE;
}

BOOL GLGaeaServer::MsgReqPVEEntranceInstantMapAF ( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* pMsg )
{
    if ( dwClientID != GetAgentSlot() )
    {
        return FALSE;
    }

    GLMSG::NET_PVE_ENTRANCE_INSTANT_MAP_REQ_AF sReqMsg;
    if ( CastNetMsg( pMsg, sReqMsg ) == false )
    {
        return FALSE;
    }

    GLChar* pChar = GetCharByDbNum( sReqMsg.dwCharDbNum );
    if ( NULL == pChar )
    {
        return FALSE;
    }

	/// 여기서의 Map ID 는 Instance ID 이다;
	/// 혼동하지 말자!;
	GLLandMan* pLand = GetLand( SNATIVEID( sReqMsg.dwMapID ) );
	if ( NULL == pLand )
	{
		return FALSE;
	}

	/// 이동정보를 세팅한다;
	MoveSystem::SMOVE_INFO sMoveInfo;
	sMoveInfo.dwChaDBNum = sReqMsg.dwCharDbNum;
	sMoveInfo.sMapID = pLand->GetMapID();
	sMoveInfo.dwGateID = sReqMsg.dwGateID;

	/// 이동한다;
	bool bResult = MoveSystem::CMoveManagerField::Instance()->MoveMap( sMoveInfo );

    //BOOL bResult = EnterLand(ReqMsg.dwCharDbNum, SNATIVEID(ReqMsg.dwMapID), ReqMsg.dwGateID, -1);

	// 인던인 경우 성공을 에이전트 서버에 알림;
	/*if ( bResult && ( pChar != NULL ) && ( pLand->GetInstantType() == SMAPNODE::EMINSTANCE_OLD_INSTANCE ) )
	{
		GLMSG::SNET_CREATE_INSTANT_MAP_FB NetMsgFB;	//FA
		NetMsgFB.dwChaNum = pChar->CharDbNum();
		NetMsgFB.emFB = EMCHAR_CREATE_INSTANT_MAP_OK;
		NetMsgFB.bCullByObjectMap = pLand->IsCullByObjectMap();
		NetMsgFB.mapID = pLand->GetMapID();
		NetMsgFB.vPos = pChar->GetPosition();

		SENDTOAGENT( &NetMsgFB );
	}*/

    GLMSG::NET_PVE_ENTRANCE_INSTANT_MAP_ACK_FA sNetMsg;
    sNetMsg.dwCharDbNum = sReqMsg.dwCharDbNum;
    sNetMsg.sInDunID = sReqMsg.sInDunID;
    sNetMsg.bResult = bResult;

    msgpack::sbuffer buf;
    msgpack::pack( buf, sNetMsg );
    SENDTOAGENT( NET_MSG_PVE_ENTRANCE_INSTANT_MAP_ACK_FA, buf );

    return TRUE;
}

BOOL GLGaeaServer::MsgPVEReqPortalCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::NET_PVE_PORTAL_REQ_CF ReqMsg;

    if (CastNetMsg(nmg, ReqMsg) == false)
    {
        return FALSE;
    }

    GLChar* pChar = GetChar(dwGaeaID);

    if (pChar == 0)
    {
        return FALSE;
    }

    GLLandMan* pLandMan = pChar->GetLandMan();

    if (pLandMan == 0)
    {
        return FALSE;
    }

    DxLandGateMan* pGateMan = pLandMan->GetLandGateMan();

    if (pGateMan == 0)
    {
        return FALSE;
    }

    if (pLandMan->GetBaseMapID().dwID != ReqMsg.dwFromMapID)
    {
        return FALSE;
    }

    DxLandGate* pGate = pGateMan->FindLandGate(ReqMsg.dwFromGateID);

    if (pGate)
    {
        TriggerSystem::SServerPortalMessage msg;
        msg.dwFromMapID = ReqMsg.dwFromMapID;
        msg.dwFromGateID = ReqMsg.dwFromGateID;
        msg.dwToMapID = ReqMsg.dwToMapID;
        msg.dwToGateID = ReqMsg.dwToGateID;
		pLandMan->PostTriggerMessage( TriggerSystem::SpMessage(new TriggerSystem::SServerPortalMessage(msg)) );
    }

    return TRUE;
}

BOOL GLGaeaServer::MsgPVEReqReturnAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (dwClientID != GetAgentSlot())
    {
        return FALSE;
    }

    GLMSG::NET_PVE_RETURN_REQ_AF ReqMsg;

    if (CastNetMsg(nmg, ReqMsg) == false)
    {
        return FALSE;
    }

    GLChar* pChar = GetCharByDbNum(ReqMsg.dwCharDbNum);

	if (pChar == 0)
    {
		return FALSE;
    }

    GLLandMan* pLandMan = pChar->GetLandMan();

    if (pLandMan == 0)
    {
        return FALSE;
    }

    // 리턴을 알린다.
    TriggerSystem::SServerReturnMessage msg;
    msg.nWhy = ReqMsg.nWhy;
    msg.dwCharDbNum = pChar->CharDbNum();
	pLandMan->PostTriggerMessage( TriggerSystem::SpMessage(new TriggerSystem::SServerReturnMessage(msg)) );

    return TRUE;
}

BOOL GLGaeaServer::MsgPVEReqRetryCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::NET_PVE_RETRY_REQ_CF ReqMsg;

    if (CastNetMsg(nmg, ReqMsg) == false)
    {
        return FALSE;
    }

    GLChar* pChar = GetChar(dwGaeaID);

    if (pChar == 0)
    {
        return FALSE;
    }

    GLLandMan* pLandMan = pChar->GetLandMan();

    if (pLandMan == 0)
    {
        return FALSE;
    }

    TriggerSystem::SServerRetryAnsMessage msg;
    msg.dwCharDbNum = pChar->CharDbNum();
    msg.bOk = ReqMsg.bOk;
	pLandMan->PostTriggerMessage( TriggerSystem::SpMessage(new TriggerSystem::SServerRetryAnsMessage(msg)) );

    return TRUE;
}

BOOL GLGaeaServer::MsgPVEReqSavePosAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (dwClientID != GetAgentSlot())
    {
        return FALSE;
    }

    GLMSG::NET_PVE_SAVEPOS_REQ_AF ReqMsg;

    if (CastNetMsg(nmg, ReqMsg) == false)
    {
        return FALSE;
    }

    GLChar* pChar = GetCharByDbNum(ReqMsg.dwCharDbNum);

    if (pChar == 0)
    {
        return FALSE;
    }

    pChar->SavePosition();

    return TRUE;
}

BOOL GLGaeaServer::MsgPVEStageResultAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    if (dwClientID != GetAgentSlot())
    {
        return FALSE;
    }

    GLMSG::NET_PVE_STAGE_RESULT_FA_OR_AF NetMsg;

    if (CastNetMsg(nmg, NetMsg) == false)
    {
        return FALSE;
    }

    GLLandMan* pLandMan = GetLand(NetMsg.sNativeInDunID);

    if (pLandMan == 0)
    {
        return FALSE;
    }

    TriggerSystem::SServerRewardAddMessage msg;
    msg.results = NetMsg.results;
	pLandMan->PostTriggerMessage( TriggerSystem::SpMessage(new TriggerSystem::SServerRewardAddMessage(msg)) );

    return TRUE;
}

BOOL GLGaeaServer::MsgPVEReqExitCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::NET_PVE_EXIT_REQ_CF NetMsg;

	if (CastNetMsg(nmg, NetMsg) == false)
	{
		return FALSE;
	}

	GLChar* pChar = GetChar(dwGaeaID);

	if (pChar == 0)
	{
		return FALSE;
	}

	if (pChar->CharDbNum() != NetMsg.dwCharDbNum)
	{
		return FALSE;
	}

	GLLandMan* pLandMan = pChar->GetLandMan();

	if (pLandMan == 0)
	{
		return FALSE;
	}

	TriggerSystem::SServerStageExitMessage msg;	
	msg.dwCharDbNum = pChar->CharDbNum();
	pLandMan->PostTriggerMessage( TriggerSystem::SpMessage(new TriggerSystem::SServerStageExitMessage(msg)) );

	return TRUE;
}

BOOL GLGaeaServer::MsgPVEReqReloadTriggerAF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	if (dwClientID != GetAgentSlot())
	{
		return FALSE;
	}

	GLMSG::NET_PVE_GM_RELOAD_TRIGGER_REQ_CA_OR_AF NetMsg;

	if (CastNetMsg(nmg, NetMsg) == false)
	{
		return FALSE;
	}

    size_t tSize = m_landManagerMap.size();
    for( size_t i=0; i<tSize; ++i )
    {
        GLLandManager* const pLandManager(m_landManagerMap[i].pLandManager);
        if (pLandManager)
        {
            TriggerSystem::SServerReturnAllMessage msg;
            pLandManager->SendTriggerMessage(&msg);
            pLandManager->RemoveTriggerFinger();
        }
    }

	TriggerSystem::DestroyServerTriggerSystem();

	GLMSG::NET_PVE_GM_RELOAD_TRIGGER_ACK_FA ResMsg;
	ResMsg.dwCharDbNum = NetMsg.dwCharDbNum;
	ResMsg.bResult = TriggerSystem::InitServerTriggerSystem();
	ResMsg.dwFieldSvr = GetFieldSvrID();
	msgpack::sbuffer buf;
	msgpack::pack(buf, ResMsg);
	SENDTOAGENT(NET_MSG_PVE_GM_RELOAD_TRIGGER_ACK_FA, buf);

	return TRUE;
}

BOOL GLGaeaServer::MsgPVEReqGoStageCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::NET_PVE_GM_GOSTAGE_CF NetMsg;

	if (CastNetMsg(nmg, NetMsg) == false)
	{
		return FALSE;
	}

	GLChar* pChar = GetChar(dwGaeaID);

	if ((pChar == 0 || 
		 pChar->CharDbNum() != NetMsg.dwCharDbNum || 
		 pChar->UserLevel() < USER_USER_GM ||
		 NetMsg.nStage <= 0) && 
		 !NetMsg.bIsItem )
	{
			return FALSE;
	}

	GLLandMan* pLandMan = pChar->GetLandMan();

	if (pLandMan == 0)
	{
		return FALSE;
	}

    GLPartyField* pParty = pChar->GetMyParty();
    if( pParty )
        pParty->SetMemberRevival();

	DWORD dwMapID = 0;
	DWORD dwGateID = 0;
	const SPVEInDunID id = pLandMan->GetPVEIndunID();

    
	if (NetMsg.nStage > 1)
	{
		TriggerSystem::SServerFindStageTriggerMessage msg;
		msg.id = id.dwMapID;
		msg.level = id.nLevel;
		msg.stage = NetMsg.nStage - 1;
		TriggerSystem::SendMessage(&msg);

		if (msg.trigger == 0)
		{
			return FALSE;
		}

		TriggerSystem::SServerFindPortalOpenTriggerMessage fmsg;
		msg.trigger->SendMessage(&fmsg);

		if (fmsg.trigger == 0)
		{
			return FALSE;
		}

		const TriggerSystem::CServerPortalOpenTrigger* potri = 0;

		if (fmsg.trigger->CastTrigger(potri) == false)
		{
			return FALSE;
		}

		dwMapID = potri->GetToMapID().dwID;
		dwGateID = potri->GetToGateID();
	}
	else
	{
		TriggerSystem::SServerMapInfoMessage msg;
		msg.mapinfo.mapid = id.dwMapID;
		TriggerSystem::SendMessage(&msg);
		dwMapID = msg.mapinfo.mapid.dwID;
		dwGateID = msg.mapinfo.gateid;
	}

	if (SNATIVEID(false) == dwMapID)
	{
		return FALSE;
	}

	GLMSG::NET_PVE_MOVE_REQ_FA NetMsgFld;
	NetMsgFld.sInDunID = id;
	NetMsgFld.dwToMapID = dwMapID;
	NetMsgFld.dwToGateID = dwGateID;
	NetMsgFld.nStage = NetMsg.nStage;
	NetMsgFld.bIsItem = NetMsg.bIsItem;
	msgpack::sbuffer buf;
	msgpack::pack(buf, NetMsgFld);
	SENDTOAGENT(NET_MSG_PVE_MOVE_REQ_FA, buf);

	return TRUE;
}

BOOL GLGaeaServer::MsgPVEReqEntranceConfirmCF(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
    GLMSG::NET_PVE_ENTRANCE_CONFIRM_REQ NetMsg;

    if( CastNetMsg(nmg, NetMsg) == false )
        return FALSE;

    GLChar* pMyChar = GetChar( dwGaeaID );

    if( pMyChar == 0 )
        return FALSE;

    //NET_PVE_ENTRANCE_CONFIRM_REQ
    // 받은 패킷으로 현재 필드에서 진입 가능한지 여부를 체크한다.
    const GLLandMan* pLandMan = pMyChar->GetLandMan();
    NetMsg.dwChaDbNum = pMyChar->CharDbNum();

    if( pLandMan->IsInstantMap())
    {
        NetMsg.byOk = GLMSG::ENTRANCE_CONFIRM_REQ_IMPOSSIBLE;
        
    }

    msgpack::sbuffer PackBuffer;
    msgpack::pack(PackBuffer, NetMsg);
    SENDTOAGENT(NET_MSG_PVE_ENTRANCE_CONFIRM_REQ, PackBuffer);

    return TRUE;
}


BOOL GLGaeaServer::MsgReqInvenStagePass( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqInvenStagePass( nmg );
	return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenStagePassOk( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_INVEN_STAGE_PASS_AF NetMsg;

	if (CastNetMsg(nmg, NetMsg) == false)
	{
		return FALSE;
	}

	GLChar* pChar = GetCharByDbNum ( NetMsg.dwCharDbNum );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqInvenStagePassOk( nmg );
	return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenEnterNumCharge( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	if ( dwGaeaID == GAEAID_NULL )
		return FALSE;

	GLChar* pChar = GetChar ( dwGaeaID );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqInvenEnterNumCharge( nmg );
	return TRUE;    
}

BOOL GLGaeaServer::MsgReqInvenEnterNumChargeOk( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_AF NetMsg;

	if (CastNetMsg(nmg, NetMsg) == false)
	{
		return FALSE;
	}

	GLChar* pChar = GetCharByDbNum ( NetMsg.dwCharDbNum );
	if ( !pChar )
		return FALSE;
	pChar->MsgReqInvenEnterNumChargeOk( nmg );
	return TRUE;    
}
