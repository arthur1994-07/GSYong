#include "pch.h"
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "../../RanLogic/Msg/LoginMsg.h"
#include "../../RanLogic/Msg/ServerMsg.h"
#include "../Client/NetMatchClientMan.h"
#include "./MatchServer.h"


// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


void MatchServer::RecvMsgProcess()
{
    m_pRecvMsgManager->MsgQueueFlip();

    MSG_LIST* pMsg = m_pRecvMsgManager->GetMsg( NULL );
    while (pMsg)
    {
        MsgProcess(pMsg);
        pMsg = m_pRecvMsgManager->GetMsg( pMsg );
    }
}

void MatchServer::MsgProcess(MSG_LIST* pMsg)
{
    if (!pMsg)
        return;

    DWORD dwClient = pMsg->dwClient;
    NET_MSG_GENERIC* nmg = (NET_MSG_GENERIC*) pMsg->Buffer;
    
    if (dwClient<0 || dwClient >= (DWORD)m_nMaxClient)
    {
        WRITE_LOG_ERROR( _T("MatchServer::MsgProcess() - ClientSlot is out of range") );
    }
    else
    {
        EMNET_MSG emType = EMNET_MSG(nmg->nType);
        if (emType < 0 || emType >= NET_MSG_GCTRL_MAX )
        {
            sc::writeLogError(
                stringformat(
                    "MessageProcess Out of range or illegal message:%1%>=%2%",
                    emType,
                    NET_MSG_GCTRL_MAX));
        }
        else
        {
            if (m_MsgFunc[emType].Msgfunc)
            {
                (this->*m_MsgFunc[emType].Msgfunc)(dwClient, nmg);
            }
        }
    }
}

int MatchServer::InsertMsg(int nClient, void* pMsg)
{
    if (pMsg)
	{
		NET_MSG_GENERIC* pPacket = (NET_MSG_GENERIC*) pMsg;
		m_pRecvMsgManager->MsgQueueInsert(nClient, pPacket, pPacket->Size());
	}
	return 0;
}

void MatchServer::initPacketFunc()
{
	m_MsgFunc[NET_MSG_HEARTBEAT_SERVER_ANS].Msgfunc	= &MatchServer::MsgHeartbeatAns;
    m_MsgFunc[NET_MSG_HEARTBEAT_SERVER_REQ].Msgfunc	= &MatchServer::MsgHeartbeatReq;
	m_MsgFunc[NET_MSG_I_AM_AGENT].Msgfunc			= &MatchServer::MsgIamAgent;
	m_MsgFunc[NET_MSG_NEW_INSTANCE].Msgfunc			= &MatchServer::MsgNewInstance;
	
	m_MsgFunc[NET_MSG_MATCH_REQUEST_AM].Msgfunc		= &MatchServer::MsgMatchRequestAM;
	m_MsgFunc[NET_MSG_QUIT_ROOM_IM].Msgfunc	= &MatchServer::MsgUnMatchRequestIM;
	m_MsgFunc[NET_MSG_MATCH_CANCEL_CAM].Msgfunc		= &MatchServer::MsgMatchCancelCAM;

	
	m_MsgFunc[NET_MSG_TRANSFER_CHAR].Msgfunc		= &MatchServer::MsgTransferChar;
	m_MsgFunc[NET_MSG_TRANSFER_CHAR_DONE_FA].Msgfunc= &MatchServer::MsgTransferCharDone;
	m_MsgFunc[NET_MSG_TRANSFERRED_CHAR_JOIN_AF].Msgfunc= &MatchServer::MsgTransferCharJoin;
	m_MsgFunc[MET_MSG_JOIN_INSTANCESVR_INFO].Msgfunc= &MatchServer::MsgJoinInstanceInfo;
	m_MsgFunc[NET_MSG_CREATE_INSTANCE_MI_FB].Msgfunc= &MatchServer::MsgCreateInstanceFB;
	m_MsgFunc[NET_MSG_DELETE_INSTANCE_MI_FB].Msgfunc= &MatchServer::MsgDeleteInstanceFB;

	

	//Relay Handler: Agent -> Match -> Instance
	m_MsgFunc[NET_MSG_TRANSFER_REQUEST_AF].Msgfunc	= &MatchServer::MsgRelayToInstance;
	m_MsgFunc[NET_MSG_GCTRL_REQ_LANDIN].Msgfunc		= &MatchServer::MsgRelayToInstance;
	m_MsgFunc[NET_MSG_GCTRL_REQ_READY].Msgfunc		= &MatchServer::MsgRelayToInstance;	
	m_MsgFunc[NET_MSG_MOVEMAP_AF].Msgfunc			= &MatchServer::MsgRelayToInstance;
	m_MsgFunc[NET_MSG_GCTRL_ACTSTATE].Msgfunc		= &MatchServer::MsgRelayToInstance;
	m_MsgFunc[NET_MSG_TRANSFER_BACK_MI].Msgfunc	= &MatchServer::MsgRelayToInstance;

	


	//Relay Hander: Instance -> Match -> Agent
	m_MsgFunc[NET_MSG_FIELDSVR_OUT_FB].Msgfunc= &MatchServer::MsgRelayToAgent;
	m_MsgFunc[NET_MSG_GCTRL_DROP_OUT_FORCED].Msgfunc= &MatchServer::MsgRelayToAgent;
}

void MatchServer::MsgHeartbeatReq(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    NET_HEARTBEAT_SERVER_ANS Msg;
    SendClient(dwClient, &Msg);
}

void MatchServer::MsgHeartbeatAns(DWORD dwClient, NET_MSG_GENERIC* pMsg)
{
    if (m_pClientManager)
        m_pClientManager->SetHeartBeat(dwClient);
}


// int NetMatchClientMan::SetAcceptedClient(DWORD dwClient, SOCKET sSocket, net::SLOT_TYPE slotType) 에서 처리
//   이 때, SvrGroupNum은 이미 config XML에 등록된 내용과 비교해서 알 수 있다.

void MatchServer::MsgIamAgent(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
	NET_I_AM_AGENT *msg = (NET_I_AM_AGENT *)nmg;

	if (msg->MagicNum != NET_I_AM_AGENT::MAGIC_NUM)
    {
        sc::writeLogError(
            stringformat(
                "NET_I_AM_AGENT invalid magic num %1%", msg->MagicNum));
        return;
    }

    unsigned int SCrcValue = msg->CrcValue;
    unsigned int TCrcValue = sc::string::getCrc32(msg, msg->Size() - sizeof(unsigned int));
    if (SCrcValue != TCrcValue)
    {
        sc::writeLogError(
            stringformat(
                "NET_I_AM_AGENT invalid crc %1%/%2%", SCrcValue, TCrcValue));
        return;
    }

	DWORD dwSvrGroupNum = msg->dwSvrGroupNum;

	if (m_pClientManager)
	{
		GASSERT(dwSvrGroupNum < MAXNUM_OF_AGENTSERVER_TO_MATCH);
		m_pClientManager->SetSlotAgent(dwSvrGroupNum, dwClient);

		sc::writeLogInfo(stringformat("AgentServer(%1%) has been connected to the slot(%2%).", dwSvrGroupNum, dwClient));
	}
	else
	{
		sc::writeLogError(std::string("MatchServer::MsgIamAgent() - ClientManager is NULL."));
	}	
}

void MatchServer::MsgNewInstance(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
	GASSERT(m_pClientManager);
	
	NET_NEW_INSTANCE* msg = (NET_NEW_INSTANCE*)nmg;
	
    if (msg->MagicNum != NET_NEW_INSTANCE::MAGIC_NUM)
    {
        sc::writeLogError(
            stringformat(
                "NET_NEW_INSTANCE invalid magic num %1%", msg->MagicNum));
        return;
    }

    unsigned int SCrcValue = msg->CrcValue;
    unsigned int TCrcValue = sc::string::getCrc32(msg, msg->Size() - sizeof(unsigned int));
    if (SCrcValue != TCrcValue)
    {
        sc::writeLogError(
            stringformat(
                "NET_NEW_INSTANCE invalid crc %1%/%2%", SCrcValue, TCrcValue));
        return;
    }

	DWORD dwInstanceSvrNum = msg->dwSvrNum;

	if (m_pClientManager)
	{
		GASSERT(dwInstanceSvrNum < MAXNUM_OF_INSTANCESERVER_TO_MATCH);
		m_pClientManager->SetSlotInstance(dwInstanceSvrNum, dwClient);

		m_InstanceServerSlots[dwInstanceSvrNum] = dwClient;

		m_pMatchSystem->NewInstanceServer(dwInstanceSvrNum);

		sc::writeLogInfo(stringformat("New InstanceServer(%1%) has been connected to the slot(%2%).", msg->dwSvrNum, dwClient));
	}
	else
	{
		sc::writeLogError(std::string("MatchServer::MsgNewInstance() - ClientManager is NULL."));
	}
}

void MatchServer::MsgCreateInstanceFB(DWORD dwClientID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_CREATE_INSTANCE_MI_FB *pmsg = (GLMSG::SNET_CREATE_INSTANCE_MI_FB *)nmg;

	switch(pmsg->status)
	{
	case INSTANCE_CREATE_DONE:
		{
			sc::writeLogInfo(stringformat("Created a real instance(%1%)", pmsg->dwKey));
			
			DWORD dwSvrNum = m_pMatchSystem->GetInstanceServerNum(pmsg->dwKey);
			m_pMatchSystem->StateInstanceConstruction(dwSvrNum);

			m_pMatchSystem->TransferToInstance(pmsg->dwKey);
		}
		break;

	case INSTANCE_CREATE_ALREADY_EXIST:
		{
			sc::writeLogError(stringformat("MsgCreateInstanceFB - Instance(%1%) already exists", (UINT)pmsg->dwKey));

			m_pMatchSystem->FreeRoom(pmsg->dwKey);
		}
		break;

	case INSTANCE_CREATE_WRONG_MAPID:
		{		
			sc::writeLogError(stringformat("MsgCreateInstanceFB - InstanceMap(%1%) is wrong", (UINT)pmsg->dwMapID));
			
			m_pMatchSystem->FreeRoom(pmsg->dwKey);
		}
		break;

	default:
		{
			sc::writeLogError(stringformat("MsgCreateInstanceFB - Wrong feedback code(%1%)", (UINT)pmsg->status));
			
			m_pMatchSystem->FreeRoom(pmsg->dwKey);
		}
	}
}

void MatchServer::MsgDeleteInstanceFB(DWORD dwClientID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_DELETE_INSTANCE_MI_FB *pmsg = (GLMSG::SNET_DELETE_INSTANCE_MI_FB *)nmg;

	switch(pmsg->status)
	{
	case INSTANCE_DELETE_DONE:
		{
			sc::writeLogInfo(stringformat("Deleted a real instance(%1%)", pmsg->dwKey));
		}
		break;

	case INSTANCE_DELETE_NOT_EXIST:
		{
			sc::writeLogError(stringformat("MsgDeleteInstanceFB - Instance(%1%) already exists", (UINT)pmsg->dwKey));
		}
		break;

	case INSTANCE_DELETE_WRONG_MAPID:
		{
			sc::writeLogError(stringformat("MsgDeleteInstanceFB - InstanceMap(%1%) is wrong", (UINT)pmsg->dwMapID));
		}
		break;

	default:
		{
			sc::writeLogError(stringformat("MsgDeleteInstanceFB - Wrong feedback code(%1%)", (UINT)pmsg->status));
		}
	}
}

void MatchServer::MsgMatchRequestAM(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
	GLMSG::NET_MATCH_REQUEST_AM *msg = (GLMSG::NET_MATCH_REQUEST_AM *)nmg;

	MATCH_REQUEST *pRequest = new MATCH_REQUEST();

	if (pRequest)	
	{
		for (DWORD i=0; i<msg->dwPartySize; i++)
		{
			INSTANCE_PLAYER player(msg->dwSvrGroupNum, msg->arrParty[i].dwChaNum);
			player.dwClass = msg->arrParty[i].dwClass;
			player.dwLevel = msg->arrParty[i].dwLevel;

			pRequest->AddMember(player);
		}

		pRequest->SetType(msg->bUnmatch ? REQUEST_UNMATCH : REQUEST_MATCH);//(REQUEST_MATCH);
		pRequest->SetMapID(msg->dwMapID);

		Request(pRequest);	//matching queue에 삽입
	}
}


void MatchServer::MsgMatchCancelCAM(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
	GLMSG::NET_MATCH_CANCEL_CAM *msg = (GLMSG::NET_MATCH_CANCEL_CAM *)nmg;
	
	UNIQUEID id(msg->dwSvrGroupNum, msg->dwChaNum);

	m_pMatchSystem->Cancel(id.dw64id);
}


void MatchServer::MsgUnMatchRequestIM(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
	//
	//InstanceServer에서 해당 캐릭터의 DropOutPC()에 의해 이 메시지가 발송
	//
	GLMSG::NET_QUIT_ROOM_IM *msg = (GLMSG::NET_QUIT_ROOM_IM *)nmg;

	INSTANCE_PLAYER who(msg->dwSvrGroupNum, msg->dwChaNum);

	m_pMatchSystem->FinishUnmatch(msg->dwKey, who);	//Quit
}

//mjeon.instance.transfer
//AM
void MatchServer::MsgTransferChar(DWORD dwClientID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_TRANSFER_CHAR *pmsg = (GLMSG::SNET_TRANSFER_CHAR *)nmg;	

	sc::writeLogDebug(stringformat("Received TransferChar(%1%/%2%)", pmsg->wSeq, pmsg->wLastSeq));
	
	if (pmsg->IsToInstance())
	{
		//DWORD dwInstanceSvrNum = m_pMatchSystem->GetInstanceServerNum(pmsg->dwSvrGroupNum, pmsg->dwChaNum);

		SendInstance(pmsg->dwInstanceServerNum, nmg);	//FieldServer -> InstanceServer
	}
	else
	{
		SendAgent(pmsg->dwSvrGroupNum, pmsg);			//InstanceServer ->	FieldServer
	}
}

void MatchServer::MsgTransferCharDone(DWORD dwClientID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_TRANSFER_CHAR_DONE_FA *pmsg = (GLMSG::SNET_TRANSFER_CHAR_DONE_FA *)nmg;

	SendAgent(pmsg->dwSvrGroupNum, nmg);
}

void MatchServer::MsgTransferCharJoin(DWORD dwClientID, NET_MSG_GENERIC* nmg)
{
	GLMSG::SNET_TRANSFERRED_CHAR_JOIN_AF *pmsg = (GLMSG::SNET_TRANSFERRED_CHAR_JOIN_AF *)nmg;

	DWORD dwInstanceSvrNum = m_pMatchSystem->GetInstanceServerNum(pmsg->dwKey);

	SendInstance(dwInstanceSvrNum, pmsg);
}

void MatchServer::MsgJoinInstanceInfo(DWORD dwClient, NET_MSG_GENERIC* nmg)
{
	//
	//해당 InstanceServer의 접속 정보를 Agent를 경유해서 Client로 전달한다.
	//
	NET_MSG_JOIN_INSTANCESVR_INFO *pmsg = (NET_MSG_JOIN_INSTANCESVR_INFO *)nmg;

	//
	//여기서 해당 캐릭터(UniqueID)가 속한 Instance(Key)를 mapping해둔다.
	//
	m_pMatchSystem->SetKey(INSTANCE_PLAYER(pmsg->dwSvrGroupNum, pmsg->dwChaNum), pmsg->dwKey);

	SendAgent(pmsg->dwSvrGroupNum, nmg);
}

void MatchServer::MsgRelayToInstance(DWORD dwClientID, NET_MSG_GENERIC* nmg)
{
	GASSERT(m_pClientManager->IsAgentSlot(dwClientID));
	//
	//반드시 UniqueKey 여야 한다!
	//
	NET_MSG_UNIQUEKEY *nmu = static_cast<NET_MSG_UNIQUEKEY *>(nmg);
	
	DWORD dwInstanceSvrNum = m_pMatchSystem->GetInstanceServerNum(nmu->dwSvrGroupNum, nmu->dwChaNum);

	SendInstance(dwInstanceSvrNum, nmg);
}

void MatchServer::MsgRelayToAgent(DWORD dwClientID, NET_MSG_GENERIC* nmg)
{
	GASSERT(m_pClientManager->IsInstanceSlot(dwClientID));
	//
	//반드시 UniqueKey 여야 한다!
	//
	NET_MSG_UNIQUEKEY *nmu = static_cast<NET_MSG_UNIQUEKEY *>(nmg);

	SendAgent(nmu->dwSvrGroupNum, nmg);
}
