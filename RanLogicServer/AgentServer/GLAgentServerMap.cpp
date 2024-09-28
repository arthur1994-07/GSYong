#include "pch.h"

#include "./GLAgentServer.h"
#include "../../RanLogic/Msg/MapMsg.h"
#include "../../RanLogic/GLogicData.h"
#include "./AgentClientMan.h"
#include "../Party/GLPartyManagerAgent.h"

namespace
{
	const GLPartyAgent* const GetBoardCurrentCount(GLAgentServer* pAgentServer,GLCharAG* pChar,DWORD* pCount)
	{
		// 캐릭터가 제거될때 게시판에도 이미 없어져야했을존재다, 그래서 반드시 존재해야함
		GASSERT(pChar);
		
		const GLPartyID& PartyID = pChar->GetPartyID();
		if( PartyID.isValidParty() == false )
		{
			*pCount = 1;
			return NULL;
		}

		const GLPartyAgent* const pParty = pAgentServer->GetParty(PartyID);
		if ( pParty == NULL )
		{
			*pCount = 1;
			pChar->ResetPartyID();			
		}
		else
			*pCount = (int)pParty->getNPartyMember();

		return pParty;
	}

}

BOOL GLAgentServer::MsgMapSearchNameCF( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::NET_MAP_SEARCH_NAME_REQ_CF* pNetMsg = (GLMSG::NET_MAP_SEARCH_NAME_REQ_CF*)nmg;
	
	pNetMsg->m_szChaName[CHR_ID_LENGTH] = '\0';

	GLCharAG* pTarChar = GetCharByDbNum( GetChaDbNumByChaName( pNetMsg->m_szChaName) );
    if (!pTarChar)
        return FALSE;    

	GLMSG::NET_MAP_SEARCH_NAME_REQ_CA msg;
	strncpy_s(msg.m_szChaName, sizeof(msg.m_szChaName), pNetMsg->m_szChaName, sizeof(pNetMsg->m_szChaName));

	msg.m_SenderChaDBNum = pNetMsg->m_SenderChaDBNum;
	msg.m_mapID = pNetMsg->m_mapID;
	msg.m_bGM = pNetMsg->m_bGM;

	SENDTOCHANNEL(&msg, pTarChar->m_nChannel);

	return TRUE;
}


BOOL GLAgentServer::MsgMapSearchName( NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID )
{
	GLMSG::NET_MAP_SEARCH_NAME_REQ_CA* pNetMsg = (GLMSG::NET_MAP_SEARCH_NAME_REQ_CA*)nmg;
	
	pNetMsg->m_szChaName[CHR_ID_LENGTH] = '\0';

    //GLCharAG* pMyChar = GetChar(dwGaeaID);
	GLCharAG* pTarChar = GetCharByDbNum( GetChaDbNumByChaName( pNetMsg->m_szChaName) );
	GLCharAG* pMyChar = GetCharByDbNum( pNetMsg->m_SenderChaDBNum );
    if (!pMyChar || !pTarChar)
        return FALSE;    
	
    bool bSearchGM = false;
    if (pMyChar->UserType() >= USER_USER_GM)
        bSearchGM = true;


    const std::string findName = pNetMsg->m_szChaName;
    std::vector<std::pair<int, std::string> > charNameVec;
    //m_pCharMan->ChaNameSearch(pMyChar->m_wSchool, pNetMsg->m_mapID, findName, charNameVec, bSearchGM);
    m_pCharMan->ChaNameSearch(pTarChar, pNetMsg->m_mapID, findName, charNameVec, bSearchGM);
	
	GLMSG::NET_MAP_SEARCH_NAME_1_ACK_AC msg1;
	msg1.m_NameCount = (int) charNameVec.size();
	//SENDTOCLIENT(dwClientID, &msg1); // 기존에는 메세지가 유저에게서 왔지만 수정으로 인해서 필드를 거쳐서 오기때문에 사용 못함
	SENDTOCLIENT(pMyChar->ClientSlot(), &msg1);

	if (!charNameVec.empty())
	{
		for (size_t i=0; i < charNameVec.size(); ++i)
		{
			GLMSG::NET_MAP_SEARCH_NAME_2_ACK_AC msg2;
            msg2.m_Level     = charNameVec[i].first;
			msg2.m_ChaName = charNameVec[i].second;

            msgpack::sbuffer SendBuffer;
            msgpack::pack(SendBuffer, msg2);

			//SENDTOCLIENT(dwClientID, NET_MSG_MAP_SEARCH_NAME_2_ACK_AC, SendBuffer);
			SENDTOCLIENT(pMyChar->ClientSlot(), NET_MSG_MAP_SEARCH_NAME_2_ACK_AC, SendBuffer);
		}
	}

	return TRUE;
}


BOOL GLAgentServer::MsgMapSearchNameDetailInfoCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::NET_MAP_SEARCH_NAME_DETAIL_INFO_REQ_CA* pNetMsg = (GLMSG::NET_MAP_SEARCH_NAME_DETAIL_INFO_REQ_CA*)nmg;
	pNetMsg->m_szChaName[CHR_ID_LENGTH] = '\0';

	GLCharAG* pCharAg = m_pCharMan->GetCharByChaName(pNetMsg->m_szChaName);
    if (!pCharAg)
        return TRUE;

	GLMSG::NET_MAP_SEARCH_NAME_DETAIL_INFO_REQ_AF msg;
	strncpy_s(msg.m_szChaName, sizeof(msg.m_szChaName), pNetMsg->m_szChaName, sizeof(pNetMsg->m_szChaName));
	msg.m_AskerDBNum = GetChar(dwGaeaID)->m_CharDbNum;
	SENDTOCHANNEL(&msg, pCharAg->m_nChannel);

	return TRUE;
}

BOOL GLAgentServer::MsgMapSearchNameDetailInfoFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
    GLMSG::NET_MAP_SEARCH_NAME_DETAIL_INFO_ACK_FA RecvData;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), RecvData))
        return FALSE;
	
    GLCharAG* pChar = GetCharByDbNum(RecvData.m_AskerDBNum);
    if (!pChar)
        return TRUE;

	GLMSG::NET_MAP_SEARCH_NAME_DETAIL_INFO_ACK_AC SendData;
	SendData.m_Level    = RecvData.m_Level;
	SendData.m_ChaName  = RecvData.m_ChaName;
	SendData.m_Title    = RecvData.m_Title;
	SendData.m_wSchool  = RecvData.m_wSchool;
	SendData.m_emClass  = RecvData.m_emClass;
	SendData.m_isParty  = RecvData.m_isParty;
	SendData.m_ClubName = RecvData.m_ClubName;
	SendData.m_dwGaeaID = RecvData.m_dwGaeaID;

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, SendData);

	SENDTOCLIENT(pChar->ClientSlot(), NET_MSG_MAP_SEARCH_NAME_DETAIL_INFO_ACK_AC, SendBuffer);
	return TRUE;
}

BOOL GLAgentServer::MsgMapSearchNamePositionCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::NET_MAP_SEARCH_NAME_POSITION_REQ_CA* pNetMsg = (GLMSG::NET_MAP_SEARCH_NAME_POSITION_REQ_CA*)nmg;
	pNetMsg->m_szChaName[CHR_ID_LENGTH] = '\0';

	GLCharAG* pCharAg = m_pCharMan->GetCharByChaName(pNetMsg->m_szChaName);
    if (!pCharAg)
        return TRUE;

	GLMSG::NET_MAP_SEARCH_NAME_POSITION_REQ_AF msg;
	msg.m_AskerDBNum = GetChar(dwGaeaID)->m_CharDbNum;

	strncpy_s(msg.m_szChaName, sizeof(msg.m_szChaName), pNetMsg->m_szChaName, sizeof(msg.m_szChaName));
	SENDTOFIELDSVR(pCharAg->m_nChannel, pCharAg->m_dwCurFieldSvr, &msg);

	return TRUE;
}

BOOL GLAgentServer::MsgMapSearchNamePositionFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::NET_MAP_SEARCH_NAME_POSITION_ACK_FA* pNetMsg = (GLMSG::NET_MAP_SEARCH_NAME_POSITION_ACK_FA*)nmg;
	pNetMsg->m_szChaName[CHR_ID_LENGTH] = '\0';

	GLMSG::NET_MAP_SEARCH_NAME_POSITION_ACK_AC msg;
	msg.m_Level = pNetMsg->m_Level;
	strncpy_s( msg.m_szChaName, sizeof(msg.m_szChaName), pNetMsg->m_szChaName,sizeof(pNetMsg->m_szChaName) );
	msg.m_mapID = pNetMsg->m_mapID.getBaseMapID().Id();
	msg.m_vPosition = pNetMsg->m_vPosition;

	GLCharAG* pChar =GetCharByDbNum(pNetMsg->m_AskerDBNum);
	if( !pChar )
		return TRUE;

	SENDTOCLIENT( &msg,pChar->m_CharDbNum );

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

BOOL GLAgentServer::MsgMapCreatePartyInfoCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    NET_MSG_PACK* pPacket = (NET_MSG_PACK*) nmg;
	GLMSG::NET_MAP_CREATE_PARTY_BOARD_REQ_CA Data;
    if (!msgpack::BufferToObject(pPacket->DataBuffer(), pPacket->DataSize(), Data))
        return FALSE;

	GLCharAG* pChar = GetChar(dwGaeaID);
	if (!pChar)
		return TRUE;

	SPartyInfoBoardData insertData;
	insertData.m_MapID = Data.m_baseMapID;
	insertData.m_Title = Data.m_Title;
	insertData.m_MasterName = pChar->m_szName;

	PartyInfoBoardIndexName* indexName = &m_PartyInfoBoardTable.get<1>(); 
	PartyInfoBoardIndexName::iterator it = indexName->find(insertData.m_MasterName);
	if( it != indexName->end() )
	{
		indexName->replace(it,insertData);
	}
	else
	{
		indexName->insert(insertData);
	}

	MapRefreshPartyInfoHelper(insertData.m_MapID,dwClientID,dwGaeaID);

	return TRUE;
}

bool GLAgentServer::MapDeletePartyInfoHelper(const std::string& name)
{
	PartyInfoBoardIndexName* indexName = &m_PartyInfoBoardTable.get<1>(); 
	PartyInfoBoardIndexName::iterator it = indexName->find(name);

	if( it == indexName->end() )
		return false;

	indexName->erase(it);

	return true;
}


BOOL GLAgentServer::MsgMapDeletePartyInfoCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::NET_MAP_DELETE_PARTY_BOARD_REQ_CA* pNetMsg = (GLMSG::NET_MAP_DELETE_PARTY_BOARD_REQ_CA*)nmg;
	GLCharAG* pChar = GetChar(dwGaeaID);
	if (!pChar)
		return TRUE;
	
	if( MapDeletePartyInfoHelper(pChar->m_szName) )
		MapRefreshPartyInfoHelper(pNetMsg->m_forRefreshMapID,dwClientID,dwGaeaID);

	return TRUE;
}

void GLAgentServer::MapRefreshPartyInfoHelper(DWORD mapID,DWORD dwClientID, DWORD dwGaeaID)
{
	GLCharAG* pChar = GetChar(dwGaeaID);
	if (!pChar)
		return;

	PartyInfoBoardIndexName* indexName = &m_PartyInfoBoardTable.get<1>(); 
	bool isExistMyBoard = indexName->find(pChar->m_szName) != indexName->end();

	PartyInfoBoardIndexMapID* indexMapID = &m_PartyInfoBoardTable.get<0>();
	std::pair<PartyInfoBoardIndexMapID::iterator,PartyInfoBoardIndexMapID::iterator> ret = indexMapID->equal_range(mapID);

	GLMSG::NET_MAP_PARTY_INFO_LIST_BOARD_1_ACK_AC msg1;
	msg1.m_isExistMyBoard = isExistMyBoard;
	msg1.m_MapID = mapID;
	msg1.m_Count = (DWORD)std::distance(ret.first,ret.second);

	SENDTOCLIENT(dwClientID,&msg1);

	for (PartyInfoBoardIndexMapID::iterator it = ret.first; it != ret.second; ++it)
	{
		const SPartyInfoBoardData& data = *it;
		GLMSG::NET_MAP_PARTY_INFO_LIST_BOARD_2_ACK_AC msg2;
		GetBoardCurrentCount(this,GetChar(data.m_MasterName.c_str()),&msg2.m_CurrentPlayer);
		msg2.m_MaxPlayer = 8;
		msg2.m_ChaName   = data.m_MasterName;
		msg2.m_Title     = data.m_Title;

        msgpack::sbuffer SendBuffer;
        msgpack::pack(SendBuffer, msg2);

		SENDTOCLIENT(dwClientID, NET_MSG_MAP_PARTY_INFO_LIST_BOARD_2_ACK_AC, SendBuffer);
	}
}

BOOL GLAgentServer::MsgMapRefreshPartyInfoCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::NET_MAP_REFRESH_PARTY_BOARD_REQ_CA* pNetMsg = (GLMSG::NET_MAP_REFRESH_PARTY_BOARD_REQ_CA*)nmg;
	MapRefreshPartyInfoHelper(pNetMsg->m_MapID,dwClientID,dwGaeaID);
	return TRUE;
}

BOOL GLAgentServer::MsgMapPartyInfoDetailCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	GLMSG::NET_MAP_PARTY_INFO_DETAIL_REQ_CA* pNetMsg = (GLMSG::NET_MAP_PARTY_INFO_DETAIL_REQ_CA*)nmg;
	pNetMsg->m_szChaName[CHR_ID_LENGTH] = '\0';

	PartyInfoBoardIndexName* indexName = &m_PartyInfoBoardTable.get<1>(); 
	PartyInfoBoardIndexName::iterator board_it = indexName->find(pNetMsg->m_szChaName);
	if(board_it == indexName->end())
	{
		MapRefreshPartyInfoHelper(pNetMsg->m_MapID,dwClientID,dwGaeaID);
		return TRUE;
	}

	const SPartyInfoBoardData& data = *board_it;
	GLMSG::NET_MAP_PARTY_INFO_DETAIL_1_ACK_AC msg1;
	msg1.m_MapID = data.m_MapID;
	GLCharAG* pBoardChar = GetChar(data.m_MasterName);
	// 없으면 안된다 없다면 로그아웃됐을때 게시판정보를 삭제해줘야함
	GASSERT(pBoardChar);
    if (!pBoardChar)
        return FALSE;

	const GLPartyAgent* const pParty = GetBoardCurrentCount(this,pBoardChar,&msg1.m_Count);

	SENDTOCLIENT(dwClientID,&msg1);

	if (!pParty)
	{
		GLMSG::NET_MAP_PARTY_INFO_DETAIL_2_ACK_AC msg2;
		msg2.m_PartyMember.SetName(pBoardChar->m_szName);
		msg2.m_PartyMember.emClass  = pBoardChar->m_emClass;
		msg2.m_PartyMember.dwGaeaID = pBoardChar->m_dwGaeaID;
		msg2.m_PartyMember.wLevel = pBoardChar->m_wLevel;
		SENDTOCLIENT(dwClientID, &msg2);
	}
	else
	{
		for (size_t i=0; i < pParty->m_Member.size(); ++i)
		{
			GLCharAG* pPartyMemeberChar = GetChar( pParty->m_Member[i] );

			GLMSG::NET_MAP_PARTY_INFO_DETAIL_2_ACK_AC msg2;
			msg2.m_PartyMember.SetName(pPartyMemeberChar->m_szName);
			msg2.m_PartyMember.emClass  = pPartyMemeberChar->m_emClass;
			msg2.m_PartyMember.dwGaeaID = pPartyMemeberChar->m_dwGaeaID;
			msg2.m_PartyMember.wLevel = pPartyMemeberChar->m_wLevel;
			SENDTOCLIENT(dwClientID, &msg2);
		}
	}

	return TRUE;
}

BOOL GLAgentServer::MsgMapPartyMemberUpdateFA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
	if ( dwGaeaID != GAEAID_NULL )
		return FALSE;

	GLMSG::SNET_PARTY_MBR_UPDATE* pNetMsg = (GLMSG::SNET_PARTY_MBR_UPDATE*)nmg;
	const GLCharAG* const pCharAG = GetChar(pNetMsg->dwGaeaID);
	if ( !pCharAG )
		return FALSE;

	const DWORD dwFieldNum(GetFieldServer(pCharAG->GetCurrentMap().getBaseMapID()));

	const  GLPartyAgent* const pPartyAG = GetParty(pCharAG->GetPartyID());
	if ( pPartyAG == NULL )
		return FALSE;

	GLPartyAgent::MEMBER vecMember = pPartyAG->GetMember();
	GLPartyAgent::MEMBER_ITER endItr = vecMember.end();
	for ( GLPartyAgent::MEMBER_ITER itr = vecMember.begin(); itr != endItr; ++itr )
	{
		const GLCharAG* const pMember = GetChar(*itr);
		if ( !pMember )
			continue;

		const DWORD dwMemberFieldNum(GetFieldServer(pMember->GetCurrentMap().getBaseMapID()));
		if ( dwMemberFieldNum != dwFieldNum )
		{
			// SENDTOFIELDSVR(pMember->m_nChannel, pMember->m_dwCurFieldSvr, &nmg);
			SENDTOCLIENT(pMember->ClientSlot(), nmg);
		}
	}
	return TRUE;
}

void GLAgentServer::ConvertWorldToMap(const MapID& mapID, const D3DXVECTOR3& vPos, int& nPosX, int& nPosY)
{
    m_sMapList.ConvertWorldToMap(mapID.getBaseMapID(), vPos, nPosX, nPosY);
}

void GLAgentServer::ConvertMapToWorld(const MapID& mapID, int PosX, int PosY, D3DXVECTOR3& vPos)
{
    m_sMapList.ConvertMapToWorld(mapID.getBaseMapID(), PosX, PosY, vPos);
}
