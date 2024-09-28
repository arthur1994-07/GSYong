#include "pch.h"
#include "../RanLogic/Msg/PartyMsg.h"
#include "../RanLogic/GLUseFeatures.h"

#include "./Party/GLPartyManagerClient.h"
#include "./Party/GLPartyTenderManagerClient.h"
#include "./Party/PartyCacheClient.h"
#include "./GLGaeaClient.h"

#include "../SigmaCore/DebugInclude.h"

std::tr1::shared_ptr<PARTY_CACHE_CLIENT> GLGaeaClient::GetOtherPartyInfo(const GLPartyID& PartyID)
{
    std::tr1::shared_ptr<PARTY_CACHE_CLIENT> spData = m_pPartyCache->GetData(PartyID);
    if (!spData)
        PartyInfoReq(PartyID);
    return spData;
}

void GLGaeaClient::PartyInfoReq(const GLPartyID& PartyID)
{
    if (!m_pPartyCache->HaveReqList(PartyID))
    {   
        party::NET_PARTY_INFO_REQ_CA SendData(PartyID);
        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, SendData);
        NETSENDTOAGENT(NET_MSG_PARTY_INFO_REQ_CA, SendBuffer);

        m_pPartyCache->AddReqList(PartyID);
    }
}

void GLGaeaClient::MsgPartyInfoAC(NET_MSG_GENERIC* pMsg)
{
    // NET_MSG_PARTY_INFO_REQ_AC
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) pMsg;
    party::NET_PARTY_INFO_REQ_AC RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return;

    m_pPartyCache->Add(RecvData);

    // Party Á¤º¸ µµÂø
    /*
    PrintConsoleText("--------------------------------");
    PrintConsoleText(sc::string::format("Party id : %1%", RecvData.m_PartyID));
    for (size_t i=0; i<RecvData.m_Member.size(); ++i)
    {
        PrintConsoleText(
            sc::string::format(
                "Member : %1% %2% %3% %4%",
                RecvData.m_Member[i].m_ChaName,
                RecvData.m_Member[i].m_ChaDbNum,
                RecvData.m_Member[i].m_ChaClass,
                RecvData.m_Member[i].m_ChaLevel));
    }
    PrintConsoleText("--------------------------------");
    */
}

void GLGaeaClient::MsgParty(NET_MSG_GENERIC* pMsg)
{
	m_pPartyManagerClient->MsgProcess(pMsg);
}

void GLGaeaClient::MsgPartyTender(NET_MSG_GENERIC* pMsg)
{
	m_pPartyTenderManagerClient->MsgProcess(pMsg);
}



