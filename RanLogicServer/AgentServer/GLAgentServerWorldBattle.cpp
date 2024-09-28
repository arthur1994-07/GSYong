#include "pch.h"
#include "../../RanLogic/Msg/WorldBattleMsg.h"

#include "../Club/GLClubManAgent.h"
#include "../Club/GLClubAgent.h"
#include "./GLAgentServer.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

BOOL GLAgentServer::MsgClubCountryCountCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    //wb::NET_CLUB_COUNTRY_INFO_CA* pPacket = (wb::NET_CLUB_COUNTRY_INFO_CA*) nmg;
    // 국가, 클럽숫자
    const std::vector<wb::COUNTRY_CLUB_COUNT_INFO>& vCountryClub = m_pClubMan->GetCountry();

    wb::NET_CLUB_COUNTRY_COUNT_START_AC MsgStart;
    SENDTOCLIENT(dwClientID, &MsgStart);

    for (size_t i=0; i<vCountryClub.size(); ++i)
    {
        wb::NET_CLUB_COUNTRY_COUNT_AC MsgData(vCountryClub[i]);
        SENDTOCLIENT(dwClientID, &MsgData);
    }

    wb::NET_CLUB_COUNTRY_COUNT_END_AC MsgEnd;
    SENDTOCLIENT(dwClientID, &MsgEnd);

    return TRUE;
}

BOOL GLAgentServer::MsgClubCountryCA(NET_MSG_GENERIC* nmg, DWORD dwClientID, DWORD dwGaeaID)
{
    wb::NET_CLUB_COUNTRY_CA* pPacket = (wb::NET_CLUB_COUNTRY_CA*) nmg;    
    m_pClubMan->SendCountryClubInfo(dwClientID, pPacket->m_Country);
    return TRUE;
}