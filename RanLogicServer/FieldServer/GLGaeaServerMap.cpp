#include "pch.h"
#include "../../SigmaCore/gassert.h"
#include "../../RanLogic/GLogicData.h"
#include "../../RanLogic/Msg/MapMsg.h"

#include "../Club/GLClubField.h"
#include "../Club/GLClubManField.h"
#include "./CharManField.h"
#include "./GLGaeaServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL GLGaeaServer::MsgMapSearchNameDetailInfo( DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg )
{
	GLMSG::NET_MAP_SEARCH_NAME_DETAIL_INFO_REQ_AF* pNetMsg = (GLMSG::NET_MAP_SEARCH_NAME_DETAIL_INFO_REQ_AF*) nmg;	
	pNetMsg->m_szChaName[CHR_ID_LENGTH] = '\0';

	PGLCHAR pChar = m_pCharMan->GetChaByName(pNetMsg->m_szChaName);
    if (!pChar)
        return TRUE;

	GLMSG::NET_MAP_SEARCH_NAME_DETAIL_INFO_ACK_FA msg;
	msg.m_Level   = pChar->GETLEVEL();
	msg.m_ChaName = pNetMsg->m_szChaName;	
	msg.m_Title   = pChar->m_pTitleMan->GetCurrentTitle();
	msg.m_wSchool = pChar->m_wSchool;
	msg.m_emClass = pChar->m_emClass;
	msg.m_isParty = pChar->isValidParty();
	std::tr1::shared_ptr<GLClubField> spClubField = pChar->GetMyClub();
    if (spClubField)
        msg.m_ClubName = spClubField->Name();
    msg.m_dwGaeaID = pChar->GetGaeaID();
    msg.m_AskerDBNum = pNetMsg->m_AskerDBNum;

    msgpack::sbuffer SendBuffer;
    msgpack::pack(SendBuffer, msg);
    SENDTOAGENT(NET_MSG_MAP_SEARCH_NAME_DETAIL_INFO_ACK_FA, SendBuffer);
    return TRUE;
}

BOOL GLGaeaServer::MsgMapSearchNamePosition(DWORD dwClientID, DWORD dwGaeaID, NET_MSG_GENERIC* nmg)
{
	GLMSG::NET_MAP_SEARCH_NAME_POSITION_REQ_AF* pNetMsg = (GLMSG::NET_MAP_SEARCH_NAME_POSITION_REQ_AF*)nmg;
	pNetMsg->m_szChaName[CHR_ID_LENGTH] = '\0';

	PGLCHAR pChar = m_pCharMan->GetChaByName(pNetMsg->m_szChaName);
    if (!pChar)
		return TRUE;	

	GLMSG::NET_MAP_SEARCH_NAME_POSITION_ACK_FA msg;
	msg.m_Level = pChar->GETLEVEL();
	strncpy_s( msg.m_szChaName,sizeof(msg.m_szChaName),pNetMsg->m_szChaName,sizeof(pNetMsg->m_szChaName) );
	msg.m_mapID = pChar->GetCurrentMap();
	msg.m_vPosition = pChar->GetPosition();
	msg.m_AskerDBNum = pNetMsg->m_AskerDBNum;
	SENDTOAGENT(&msg);

	return TRUE;
}

