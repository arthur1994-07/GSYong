#include "pch.h"
#include "./GLAgentServer.h"

#include "../PVE/PVESystem.h"
#include "../PVE/PVEIndun.h"
#include "../Party/GLPartyManagerAgent.h"
#include "../InstanceSystem/LogicAgent/InstanceSystemAgent.h"
#include "../Move/MoveManagerServer.h"

#include "Database/DBAction/DbActionLogInstantDungeon.h"

#include "../RanLogicServer/TriggerSystem/ServerRetryMessage.h"
#include "../RanLogicServer/TriggerSystem/ServerTriggerSystem.h"

#include "../../RanLogic/Msg/PVEMsg.h"
#include "../../RanLogic/Msg/GLContrlInDunMsg.h"
#include "../../SigmaCore/DebugInclude.h"

using namespace PVESystem;

BOOL GLAgentServer::MsgPVEReqMapTableCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLMSG::NET_PVE_MAPTABLE_REQ_CA NetMsg;

    if (CastNetMsg(nmg, NetMsg) == false)
    {
        return FALSE;
    }

    GLCharAG* pChar = GetChar(dwGaeaID);

    if (pChar == 0 || dwGaeaID != NetMsg.dwGaeaID)
    {
        return FALSE;
    }

    DWORD dwCharDbNum = pChar->ChaDbNum();

    GLMSG::NET_PVE_MAPTABLE_ACK_AC AckMsg;

    if (PVESystem::CollectMapTable(dwCharDbNum, AckMsg.tables, AckMsg.table))
    {
		msgpack::sbuffer buf;
		msgpack::pack(buf, AckMsg);
		SENDTOCLIENT(dwClientID, NET_MSG_PVE_MAPTABLE_ACK_AC, buf);
        return TRUE;
    }

    return FALSE;
}

BOOL GLAgentServer::MsgPVEReqEntranceCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLMSG::NET_PVE_ENTRANCE_REQ_CA NetMsg;

    if (CastNetMsg(nmg, NetMsg) == false)
    {
		sc::writeLogError(sc::string::format("[PVE ENTERENCE LOG] MsgCast fail - GaeaID %1%", dwGaeaID));
        return FALSE;
    }

    GLCharAG* pChar = GetChar(dwGaeaID);

    if (pChar == 0)
    {
		sc::writeLogError(sc::string::format("[PVE ENTERENCE LOG] GetChar fail - GaeaID %1%", dwGaeaID));
        return FALSE;
    }

	DWORD dwCharDbNum = pChar->ChaDbNum();

	/**
		소속된 인던이 있다면 소속 인던으로 입장 시킨다.

	 */
	CPVEInDun* pIndun = PVESystem::FindInstantDungeon(dwCharDbNum);

	if (pIndun && pIndun->IsRunning())
	{
		/**
			클라에서 보내온 입장 정보와
			실제 입장 정보가 같아야 입장 시킨다.

		 */
		if (pIndun->GetID() != NetMsg.sInDunID)
		{
			sc::writeLogError(sc::string::format("[PVE ENTERENCE LOG] Indun is already exist and IndunID is not match - ChaDBNum %1%, RequestedIndunID %2%, CreatedIndunID %3%", dwCharDbNum, NetMsg.sInDunID.dwMapID, pIndun->GetID().dwMapID));
			sc::writeLogError(sc::string::format("[PVE ENTERENCE LOG] OriChaNum %1%, TarChaNum %2%, OriLev %3%, TarLev %4%, OriNum %5%, TarNum %6%",  NetMsg.sInDunID.dwCharDbNum, pIndun->GetID().dwCharDbNum,  NetMsg.sInDunID.nLevel,  pIndun->GetID().nLevel,  NetMsg.sInDunID.nNum,  pIndun->GetID().nNum));
			return FALSE;
		}

		pIndun->Entrance(dwCharDbNum);
	}
	else
	{
		/**
			소속된 인던이 없다면 새로 소속 인던을 만든다.

		 */
		GLPartyAgent* pParty = 0;

		if (m_pPartyManager)
		{
			pParty = m_pPartyManager->GetParty(pChar->GetPartyID());
		}

		if (pParty == 0)
		{
			sc::writeLogError(sc::string::format("[PVE ENTERENCE] Indun is not exist but it is not Party - ChaDBNum %1%", dwCharDbNum));
			return FALSE;
		}

		pIndun = PVESystem::FindInstantDungeon(NetMsg.sInDunID);

		if (pIndun == 0)
		{
			sc::writeLogError(sc::string::format("[PVE ENTERENCE] Indun is not exist but IndunID is not vaild - ChaDBNum %1%, RequestedIndunID %2%", dwCharDbNum, NetMsg.sInDunID.dwMapID));
			return FALSE;
		}

		if (pIndun->GetInfo().gateid != NetMsg.dwGateID)
		{
			sc::writeLogError(sc::string::format("[PVE ENTERENCE] Indun is not exist but GateID is not match - ChaDBNum %1%, RequestedGateID %2%, CreatedGateID %3%", dwCharDbNum, NetMsg.dwGateID, pIndun->GetInfo().gateid));
			return FALSE;
		}

		pIndun->SetServer(this);
		pIndun->Entrance(pParty);
	}

    return TRUE;
}

BOOL GLAgentServer::MsgPVEReqEntranceConfirmFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLMSG::NET_PVE_ENTRANCE_CONFIRM_REQ NetMsg;

    if (CastNetMsg(nmg, NetMsg) == false)
    {
		sc::writeLogError(sc::string::format("[PVE ENTERENCE CONFIRM] MsgCast fail - GaeaID %1%", dwGaeaID));
        return FALSE;
    }

    if( NetMsg.dwChaDbNum == 0 )
	{
		sc::writeLogError(sc::string::format("[PVE ENTERENCE CONFIRM] Requested ChaDBNum is null - GaeaID %1%", dwGaeaID));
        return FALSE;
	}

    GLCharAG* pChar = GetCharByDbNum( NetMsg.dwChaDbNum );

    if (pChar == 0 )
	{
		sc::writeLogError(sc::string::format("[PVE ENTERENCE CONFIRM] Can not find char - GaeaID %1%, ChaDBNum %2%", dwGaeaID, NetMsg.dwChaDbNum));
        return FALSE;
	}

    CPVEInDun* pIndun = PVESystem::FindInstantDungeon(NetMsg.sInDunID);

    if (pIndun == 0)
    {
		sc::writeLogError(sc::string::format("[PVE ENTERENCE CONFIRM] Indun is not exist but IndunID is not vaild - ChaDBNum %1%, RequestedIndunID %2%", NetMsg.dwChaDbNum, NetMsg.sInDunID.dwMapID));
        return FALSE;
    }

	DWORD dwCharDbNum = pChar->ChaDbNum();

    if (NetMsg.byOk == GLMSG::ENTRANCE_CONFIRM_REQ_TRUE)
    {
        pIndun->PassMember(dwCharDbNum);
		sc::writeLogInfo(sc::string::format("[PVE ENTERENCE CONFIRM] ENTRANCE_CONFIRM_REQ_TRUE - ChaDBNum %1%", dwCharDbNum));
    }
    else if(NetMsg.byOk == GLMSG::ENTRANCE_CONFIRM_REQ_IMPOSSIBLE)
    {
        pIndun->Impossible(dwCharDbNum);
		sc::writeLogInfo(sc::string::format("[PVE ENTERENCE CONFIRM] ENTRANCE_CONFIRM_REQ_IMPOSSIBLE - ChaDBNum %1%", dwCharDbNum));
    }
    else
    {
        pIndun->Cancel(dwCharDbNum);
		sc::writeLogInfo(sc::string::format("[PVE ENTERENCE CONFIRM] CANCEL - ChaDBNum %1%", dwCharDbNum));
    }

    return TRUE;
}

BOOL GLAgentServer::MsgPVEReqClearOfBelongingCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLMSG::NET_PVE_CLEAROFBELONGING_REQ_CA NetMsg;

    if (CastNetMsg(nmg, NetMsg) == false)
    {
		sc::writeLogError(std::string("[PVEInDun] MsgPVEReqClearOfBelongingCA - Msg Casting Error"));
        return FALSE;
    }

    GLCharAG* pChar = GetChar(dwGaeaID);

    if (pChar == 0)
    {
		sc::writeLogError(std::string("[PVEInDun] MsgPVEReqClearOfBelongingCA - Not Find Char"));
        return FALSE;
    }

	DWORD dwCharDbNum = pChar->ChaDbNum();

    CPVEInDun* pIndun = PVESystem::FindInstantDungeon(dwCharDbNum);

    if (pIndun == 0)
    {
		sc::writeLogError(sc::string::format("[PVEInDun] MsgPVEReqClearOfBelongingCA - Not Find Char - ClientID : %1%, GaeaID : %2%, ChaDBNum : %3%, InDunID : %4%", dwClientID, dwGaeaID, dwCharDbNum, NetMsg.sInDunID.dwMapID));
        return FALSE;
    }

	if (pIndun->GetID() != NetMsg.sInDunID)
	{
		sc::writeLogError(sc::string::format("[PVEInDun] MsgPVEReqClearOfBelongingCA - Not Find InDun - ClientID : %1%, InDunID : %2%", dwClientID, NetMsg.sInDunID.dwMapID));
		return FALSE;
	}

	if (pIndun->ClearOfBelonging(dwCharDbNum))
	{
		GLMSG::NET_PVE_CLEAROFBELONGING_ACK_AC AnsMsg;
		AnsMsg.sInDunID = pIndun->GetID();
		msgpack::sbuffer buf;
		msgpack::pack(buf, AnsMsg);
		SENDTOCLIENT(pChar->ClientSlot(), NET_MSG_PVE_CLEAROFBELONGING_ACK_AC, buf);
	}
	else
	{
		sc::writeLogError(sc::string::format("[PVEInDun] MsgPVEReqClearOfBelongingCA - ClearOfBelonging Error - ClientID : %1%, CharDbNum : %2%, InDunID : %3%", dwClientID, dwCharDbNum, NetMsg.sInDunID.dwMapID));
	}

    return TRUE;
}

BOOL GLAgentServer::MsgPVEAckCreateInstantMapFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLMSG::NET_PVE_CREATE_INSTANT_MAP_ACK_FA NetMsg;

    if (CastNetMsg(nmg, NetMsg) == false)
    {
		sc::writeLogError(std::string("[PVEInDun] MsgPVEAckCreateInstantMapFA - Msg Casting Error"));
        return FALSE;
    }

    CPVEInDun* pIndun = PVESystem::FindInstantDungeon(NetMsg.sInDunID);

    if (pIndun == 0)
    {
        return FALSE;
    }

    bool vIsCreate = pIndun->PostCreateInstanceMap(NetMsg.dwInstanceMapID, NetMsg.bResult);

    // pIndun->m_preid = 0
    // 최초 생성시에는 0이다.
    if( vIsCreate && pIndun->GetPreEntranceMapID() == 0)
    {

        const GLPartyAgent* const pPartyAgent = GetParty( pIndun->GetPartyID() );

        if (pPartyAgent == 0)
        {
            sc::writeLogError( "Agent InstanceLog Fail - Not Exist Party!");
            return TRUE;
        }

        const GLPartyAgent::MEMBER& real = pPartyAgent->GetMember();

        for (GLPartyAgent::MEMBER_CITER it = real.begin(); it != real.end(); ++it)
        {
            GLCharAG* pChar = GetChar(*it);
            if( pChar )
            {
                MAP_TABLE tbl = PVESystem::GetMapTable( pIndun->GetID() );
                const MAP_INFO& mapInfo = pIndun->GetInfo();

                int nEntryCount = tbl.nInCount;
                int nRemnantCout = mapInfo.dailylimit - tbl.nInCount; 

                AddLogAdoJob(
                    db::DbActionPtr(
                    new db::LogInstantInfiniteStaircaseStart(
                    pIndun->GetGUID(),
                    real.size(),
                    pChar->ChaDbNum(),
                    pChar->GetChaLevel(),
                    nEntryCount,
                    nRemnantCout,
					mapInfo.mapid )));
            }
        }
    }

	return TRUE;
}

BOOL GLAgentServer::MsgPVEAckEntranceInstantMapFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLMSG::NET_PVE_ENTRANCE_INSTANT_MAP_ACK_FA NetMsg;

    if (CastNetMsg(nmg, NetMsg) == false)
    {
		sc::writeLogError(std::string("[PVEInDun] MsgPVEAckEntranceInstantMapFA - Msg Casting Error"));
        return FALSE;
    }

    GLCharAG* pChar = GetCharByDbNum(NetMsg.dwCharDbNum);

    if (pChar == 0)
    {
		sc::writeLogError(sc::string::format("[PVEInDun] MsgPVEAckEntranceInstantMapFA - Not Find Char - ClientID : %1%, InDun ID : %2%, ChaDBNum : %3%", dwClientID, NetMsg.sInDunID.dwMapID, NetMsg.dwCharDbNum));
        return FALSE;
    }

    CPVEInDun* pIndun = PVESystem::FindInstantDungeon(NetMsg.sInDunID);

    if (pIndun == 0)
    {
		sc::writeLogError(sc::string::format("[PVEInDun] MsgPVEAckEntranceInstantMapFA - Not Find InstantDungeon - ClientID : %1%, InDun ID : %2%, ChaDBNum : %3%", dwClientID, NetMsg.sInDunID.dwMapID, NetMsg.dwCharDbNum));
        return FALSE;
    }

    pIndun->PostEntranceInstanceMap(pChar->ChaDbNum(), NetMsg.bResult);

    return TRUE;
}

BOOL GLAgentServer::MsgPVEReqMoveFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLMSG::NET_PVE_MOVE_REQ_FA NetMsg;

    if (CastNetMsg(nmg, NetMsg) == false)
    {
		sc::writeLogError(std::string("[PVEInDun] MsgPVEReqMoveFA - Msg Casting Error"));
        return FALSE;
    }

    CPVEInDun* pIndun = PVESystem::FindInstantDungeon(NetMsg.sInDunID);

    if (pIndun == 0)
    {
		sc::writeLogError(sc::string::format("[PVEInDun] MsgPVEReqMoveFA - Not Find InstantDungeon - ClientID : %1%, InDun ID : %2%", dwClientID, NetMsg.sInDunID.dwMapID));
        return FALSE;
    }

	if (pIndun->GetStage() >= NetMsg.nStage &&
		NetMsg.bIsItem )
	{
		sc::writeLogError(sc::string::format("[PVEInDun] MsgPVEReqMoveFA - Stage not match - ClientID : %1%, InDun ID : %2%, InDun Stage : %3%, Client Stage : %4%", dwClientID, NetMsg.sInDunID.dwMapID, pIndun->GetStage(), NetMsg.nStage));
		return FALSE;
	}

	/**
		인던 상태가 완료 중이어야
		계속 스테이지를 진행 할 수 있다.

	 */
	if (pIndun->IsRunning())
	{
		pIndun->CreateInstanceMap(NetMsg.dwToMapID, NetMsg.dwToGateID, NetMsg.nStage);

		if (NetMsg.bIsItem)
		{
			GLCharAG* pChar = GetCharByDbNum(NetMsg.dwCharDbNum);
			if(pChar)
			{
				GLMSG::SNETPC_REQ_INVEN_STAGE_PASS_AF Msg;
				Msg.wPosX = NetMsg.wPosX;
				Msg.wPosY = NetMsg.wPosY;
				Msg.dwCharDbNum = NetMsg.dwCharDbNum;
				msgpack::sbuffer buf;
				msgpack::pack(buf, Msg);

				SENDTOFIELDSVR(pChar->m_nChannel, pChar->m_dwCurFieldSvr, NET_MSG_GCTRL_REQ_INVEN_STAGE_PASS_AF, buf);
			}
		}
	}

    return TRUE;
}

BOOL GLAgentServer::MsgPVEReqRetryFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLMSG::NET_PVE_RETRY_REQ_FA NetMsg;

    if (CastNetMsg(nmg, NetMsg) == false)
    {
		sc::writeLogError(std::string("[PVEInDun] MsgPVEReqRetryFA - Msg Casting Error"));
        return FALSE;
    }

    CPVEInDun* pIndun = PVESystem::FindInstantDungeon(NetMsg.sInDunID);

    if (pIndun == 0)
    {
		sc::writeLogError(sc::string::format("[PVEInDun] MsgPVEReqRetryFA - Not Find InstantDungeon - ClientID : %1%, InDun ID : %2%", dwClientID, NetMsg.sInDunID.dwMapID));
        return FALSE;
    }

    pIndun->Retry();

    return TRUE;
}

BOOL GLAgentServer::MsgPVEReqReturnFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLMSG::NET_PVE_RETURN_REQ_FA NetMsg;

    if (CastNetMsg(nmg, NetMsg) == false)
    {
		sc::writeLogError(std::string("[PVEInDun] MsgPVEReqReturnFA - Msg Casting Error"));
        return FALSE;
    }

    CPVEInDun* pIndun = PVESystem::FindInstantDungeon(NetMsg.sInDunID);

    if (pIndun == 0)
    {
		sc::writeLogError(sc::string::format("[PVEInDun] MsgPVEReqReturnFA - Not Find InstantDungeon - ClientID : %1%, InDun ID : %2%", dwClientID, NetMsg.sInDunID.dwMapID));
        return FALSE;
    }

	pIndun->RestoreAll(NetMsg.nWhy);

    return TRUE;
}

BOOL GLAgentServer::MsgPVEStageResultFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    GLMSG::NET_PVE_STAGE_RESULT_FA_OR_AF NetMsg;

    if (CastNetMsg(nmg, NetMsg) == false)
    {
		sc::writeLogError(std::string("[PVEInDun] MsgPVEStageResultFA - Msg Casting Error"));
        return FALSE;
    }

    CPVEInDun* pIndun = PVESystem::FindInstantDungeon(NetMsg.sInDunID);

    if (pIndun == 0)
    {
		sc::writeLogError(sc::string::format("[PVEInDun] MsgPVEStageResultFA - Not Find InstantDungeon - ClientID : %1%, InDun ID : %2%, GaeaID : %3%", dwClientID, NetMsg.sInDunID.dwMapID, dwGaeaID));
        return FALSE;
    }

    pIndun->SetStageResult(NetMsg.results);

    return TRUE;
}

BOOL GLAgentServer::MsgPVEReqReloadTriggerCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::NET_PVE_GM_RELOAD_TRIGGER_REQ_CA_OR_AF NetMsg;

	if (CastNetMsg(nmg, NetMsg) == false)
	{
		sc::writeLogError(std::string("[PVEInDun] MsgPVEReqReloadTriggerCA - Msg Casting Error"));
		return FALSE;
	}

	GLCharAG* pChar = GetChar(dwGaeaID);

	if (pChar == 0 || 
		pChar->ChaDbNum() != NetMsg.dwCharDbNum || 
		pChar->UserType() < USER_GM3)
	{
		return FALSE;
	}	

	GLMSG::NET_PVE_GM_RELOAD_TRIGGER_ACK_AC ResMsg;

	TriggerSystem::DestroyServerTriggerSystem();
	ResMsg.bResult = TriggerSystem::InitServerTriggerSystem();

	PVESystem::DestroyPVEIndun();
	PVESystem::DestroyPVECache();
	PVESystem::InitPVECache();

	msgpack::sbuffer buf;
	msgpack::pack(buf, ResMsg);
	SENDTOCLIENT(pChar->ClientSlot(), NET_MSG_PVE_GM_RELOAD_TRIGGER_ACK_AC, buf);

	if (ResMsg.bResult)
	{
		SENDTOALLCHANNEL(nmg);	
	}

	return TRUE;
}

BOOL GLAgentServer::MsgPVEAckReloadTriggerFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::NET_PVE_GM_RELOAD_TRIGGER_ACK_FA NetMsg;

	if (CastNetMsg(nmg, NetMsg) == false)
	{
		sc::writeLogError(std::string("[PVEInDun] MsgPVEAckReloadTriggerFA - Msg Casting Error"));
		return FALSE;
	}

	GLCharAG* pChar = GetCharByDbNum(NetMsg.dwCharDbNum);

	if (pChar == 0 || 
		pChar->UserType() < USER_GM3)
	{
		return FALSE;
	}

	GLMSG::NET_PVE_GM_RELOAD_TRIGGER_ACK_AC ResMsg;
	ResMsg.bResult = NetMsg.bResult;
	ResMsg.bAgent = false;
	ResMsg.dwFieldSvr = NetMsg.dwFieldSvr;
	msgpack::sbuffer buf;
	msgpack::pack(buf, ResMsg);
	SENDTOCLIENT(pChar->ClientSlot(), NET_MSG_PVE_GM_RELOAD_TRIGGER_ACK_AC, buf);

	return TRUE;
}

const DWORD GLAgentServer::CreatePVEInstantMap(SNATIVEID baseMapID, DWORD dwGaeaID, const GLPartyID& PartyID, const DWORD dwFieldChannel, const DWORD dwFieldID)
{
    const InstanceSystem::InstanceMapID _instanceMapID(m_pInstanceManager->getInstanceMapID());
    if ( _instanceMapID == GAEAID_NULL )
        return GAEAID_NULL;

	SMAPNODE* pMapNode(m_sMapList.FindMapNode(baseMapID));
	if ( pMapNode == NULL )
		return GAEAID_NULL;

	GLAGLandManager* pBaseLandMan	= GetLandManager(baseMapID);
	if ( pBaseLandMan == NULL )
		return GAEAID_NULL;    

    GLAGLandManager* pTargetLandMan	= new GLAGLandManager(*pBaseLandMan, _instanceMapID, dwFieldChannel, dwFieldID);

    m_vecInstantMapId.push_back(_instanceMapID);

    m_pInstanceManager->popInstanceMapID();

    InsertMap(pTargetLandMan);

    return _instanceMapID;
}

BOOL GLAgentServer::ChangeMap(DWORD dwGaeaID, DWORD dwMapID, DWORD dwGateID, const SPVEInDunID indunID)
{
    GLCharAG* pChar = GetChar(dwGaeaID);

    if (pChar == 0)
    {
        return FALSE;
    }
	
    GLMSG::NET_PVE_ENTRANCE_INSTANT_MAP_REQ_AF NetMsg;
    NetMsg.dwCharDbNum = pChar->ChaDbNum();
    NetMsg.dwMapID = dwMapID;
    NetMsg.dwGateID = dwGateID;
    NetMsg.sInDunID = indunID;
    msgpack::sbuffer buf;
    msgpack::pack(buf, NetMsg);

    // Agent 서버에서 갖고 있는 케릭터 맵정보 변경
    SetCurrentField(pChar, pChar->m_dwCurFieldSvr, MapID(indunID.dwMapID, dwMapID));

    // 필드 서버 변경 필요 없음. 현재 필드서버로 인던 맵 생성을 요청
    SENDTOFIELDSVR(pChar->m_nChannel, pChar->m_dwCurFieldSvr, NET_MSG_PVE_ENTRANCE_INSTANT_MAP_REQ_AF, buf);

    return TRUE;
}

BOOL GLAgentServer::ChangeFieldSvr(DWORD dwNextFieldSvr, DWORD dwGaeaID, const DWORD dwBaseMapID, const DWORD dwGaeaMapID, DWORD dwGateID)
{
	GLCharAG* pChar = GetChar( dwGaeaID );
	if ( NULL == pChar )
		return FALSE;

	MoveSystem::SMOVE_INFO_AGENT sTempMoveInfo;
	sTempMoveInfo.emMoveType = MoveSystem::EMMOVE_TYPE_TRIGGERSYSTEM;
	sTempMoveInfo.dwChaDBNum = pChar->ChaDbNum();
	sTempMoveInfo.sMapID = MapID( dwBaseMapID, dwGaeaMapID );
	sTempMoveInfo.dwGateID = dwGateID;
	sTempMoveInfo.vPosition = D3DXVECTOR3( FLT_MAX, FLT_MAX, FLT_MAX );
	sTempMoveInfo.dwFieldNum = dwNextFieldSvr;
	sTempMoveInfo.dwChannelNum = pChar->GetChannel();

	if ( false == MoveSystem::CMoveManagerServer::Instance()->MoveMap( sTempMoveInfo ) )
	{
		return FALSE;
	}

	return TRUE;
    

    /*if (pChar == 0 || IsFieldOnline(pChar->m_nChannel, dwNextFieldSvr) == false)
    {
        return FALSE;
    }

    DWORD dwClientSlot = pChar->ClientSlot();
    DWORD dwChaDbNum = pChar->ChaDbNum();
    DWORD dwSvrGroupNum = m_pMsgServer->GetServerGroup();

    // 필드 서버 변경시 클라이언트 정보 변경 사항 반영
    ReActionMoveFieldSvr(dwClientSlot, dwGaeaID);

    // 현재 필드 서버에 자신이 떠나갈 예정임을 통보.
    GLMSG::SNETPC_FIELDSVR_OUT NetMsg;
    NetMsg.dwChaNum = dwChaDbNum;
    NetMsg.dwSvrGroupNum = dwSvrGroupNum;
    NetMsg.emType = EMJOINTYPE_PVEINDUN;
	NetMsg.targetMapID = SNATIVEID(dwGaeaMapID);
    SENDTOFIELD(dwClientSlot, &NetMsg);

    // 이동할 맵 ID 저장.
    pChar->SetNextFieldSvr(MapID(dwBaseMapID, dwGaeaMapID), dwGateID, D3DXVECTOR3(FLT_MAX, FLT_MAX, FLT_MAX), dwNextFieldSvr);

    return TRUE;*/
}


BOOL GLAgentServer::MsgPVEReqEnterNumCharge( NET_MSG_GENERIC* pMsg, DWORD ClientSlot, DWORD GaeaID )
{
	GLMSG::SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_FA NetMsg;

	if (CastNetMsg(pMsg, NetMsg) == false)
	{
		sc::writeLogError(std::string("[PVEInDun] MsgPVEReqEnterNumCharge - Msg Casting Error"));
		return FALSE;
	}

	GLCharAG* pChar = GetCharByDbNum(NetMsg.dwCharDbNum);
	if ( NULL == pChar )
		return FALSE;

	SNATIVEID sMapID;
	sMapID.dwID = NetMsg.dwMapID;

	// Sub ID 가 1 일경우 Matching 입장횟수로 처리한다;
	// 후에 트리거 시스템이 제거되면 제거한다;
	if ( 1 == sMapID.Sid() )
	{
		// 사용에 실패할 경우;
		if ( false == MatchingSystem::CMatchingJoinManager::Instance()
			->ChargeJoinCount_DBNum(
			static_cast< MatchingSystem::MatchingBaseID >( sMapID.Mid() ),
			NetMsg.dwCharDbNum ) )
		{
			// 사용 실패;
			return TRUE;
		}

		GLMSG::SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_AF Msg;
		Msg.wPosX = NetMsg.wPosX;
		Msg.wPosY = NetMsg.wPosY;
		Msg.dwCharDbNum = NetMsg.dwCharDbNum;
		Msg.bOk = true;
		msgpack::sbuffer buf;
		msgpack::pack(buf, Msg);

		SENDTOFIELDSVR(pChar->m_nChannel, pChar->m_dwCurFieldSvr, NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_AF, buf);

		return TRUE;
	}

	bool bOk = false;
	MAP_TABLE tbl;
	PVESystem::GetMapTable(tbl, NetMsg.dwCharDbNum, NetMsg.dwMapID, NetMsg.nLevel);

	if ( tbl.nInCount > 0)
	{
		tbl.nInCount--;
		PVESystem::SetMapTable(tbl);
		bOk = true;

		GLMSG::NET_PVE_MAPTABLE_ACK_AC AckMsg;

		if (PVESystem::CollectMapTable(NetMsg.dwCharDbNum, AckMsg.tables, AckMsg.table))
		{
			msgpack::sbuffer buf;
			msgpack::pack(buf, AckMsg);
			SENDTOCLIENT(pChar->ClientSlot(), NET_MSG_PVE_MAPTABLE_ACK_AC, buf);
		}
	}


	GLMSG::SNETPC_REQ_INVEN_ENTER_NUM_CHARGE_AF Msg;
	Msg.wPosX = NetMsg.wPosX;
	Msg.wPosY = NetMsg.wPosY;
	Msg.dwCharDbNum = NetMsg.dwCharDbNum;
	Msg.bOk = bOk;
	msgpack::sbuffer buf;
	msgpack::pack(buf, Msg);

	SENDTOFIELDSVR(pChar->m_nChannel, pChar->m_dwCurFieldSvr, NET_MSG_GCTRL_INVEN_ENTER_NUM_CHARGE_AF, buf);


	return TRUE;
}
