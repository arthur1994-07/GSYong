#include "pch.h"
#include "./NotifyClientClubInvite.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Json/json_spirit_reader.h"
#include "../../SigmaCore/Json/MinJson.h"

#include "../../SigmaCore/DebugInclude.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/Msg/GLContrlClubMsg.h"
#include "../GLGaeaClient.h"

NotifyClientClubInvite::NotifyClientClubInvite(
    GLGaeaClient* pGaeaClient,
    const sc::MGUID& Guid, 
    const __time64_t& Date )
    : INotifyClientBase( pGaeaClient, Guid, Date, gamelogic::notify::CLUB_INVITE )
{
}

NotifyClientClubInvite::~NotifyClientClubInvite()
{
}

bool NotifyClientClubInvite::Parsing( NET_MSG_GENERIC* nmg )
{
    if ( !nmg )
        return false;

    GLMSG::NET_CLUB_MEMBER_REQ_ASK_AC* pNetMsg = (GLMSG::NET_CLUB_MEMBER_REQ_ASK_AC*) nmg;

    MasterID = pNetMsg->dwMaster;
    ClubName = stringformat( "%1%", pNetMsg->szClubName );
    MasterName = stringformat( "%1%", pNetMsg->szMasterName );

    return true;
}

std::string NotifyClientClubInvite::Description() const
{
    return stringformat( "%1%", GetClubName().c_str() );
}

std::string NotifyClientClubInvite::GetMessageBoxText() const
{
    return stringformat( ID2GAMEINTEXT("CLUB_JOIN_ASK"), GetClubName().c_str(), GetMasterName().c_str() );
}

void NotifyClientClubInvite::SendAccept()
{
    // 수락 패킷 전송
    m_pGaeaClient->ReqClubJoinAns( GetMasterID(), true );
}

void NotifyClientClubInvite::SendRefused()
{
    // 거절 패킷 전송
    m_pGaeaClient->ReqClubJoinAns( GetMasterID(), false );
}
