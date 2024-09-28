#include "pch.h"
#include "./NotifyClientFriendAdd.h"

#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Json/json_spirit_reader.h"
#include "../../SigmaCore/Json/MinJson.h"

#include "../../SigmaCore/DebugInclude.h"
#include "../../EngineLib/GUInterface/GameTextControl.h"

#include "../../RanLogic/Msg/GLContrlFriendMsg.h"

#include "../GLGaeaClient.h"
#include "../Friend/GLFriendClient.h"

NotifyClientFriendAdd::NotifyClientFriendAdd(
    GLGaeaClient* pGaeaClient,
    const sc::MGUID& Guid, 
    const __time64_t& Date )
    : INotifyClientBase( pGaeaClient, Guid, Date, gamelogic::notify::FRIEND_ADD )
{
}

NotifyClientFriendAdd::~NotifyClientFriendAdd()
{
}

bool NotifyClientFriendAdd::Parsing( NET_MSG_GENERIC* nmg )
{
	GLMSG::SNETPC_REQ_FRIENDADD_LURE* pNetMsg = (GLMSG::SNETPC_REQ_FRIENDADD_LURE *) nmg;

    if( pNetMsg != NULL )
    {
        m_strFriendName = stringformat( "%1%", pNetMsg->szREQ_CHAR );
        return true;
    }

    return false;
}

bool NotifyClientFriendAdd::Parsing( const json_spirit::mObject& Object )
{

    if( !sc::Json::find_value(Object, "1", m_strFriendName ) )
    {
        //PrintMsgText(NS_UITEXTCOLOR::RED, std::string("Json message type 'M' not exist"));
        return false;
    }

    return true;
}

std::string NotifyClientFriendAdd::Description() const
{
    return m_strFriendName;
}

std::string NotifyClientFriendAdd::GetMessageBoxText() const
{
    return sc::string::format( ID2GAMEINTEXT( "ADD_FRIEND_REQ" ), m_strFriendName.c_str() );
}

void NotifyClientFriendAdd::SendAccept()
{
    // 수락 패킷 전송
    if ( m_strFriendName.empty() )
    {
        GASSERT( 0 && "이름이 등록되지 않았습니다." );
        return;
    }

    m_pGaeaClient->GetFriendClient()->ReqFriendLureAns( m_strFriendName.c_str(), true, m_Guid );
}

void NotifyClientFriendAdd::SendRefused()
{
    // 거절 패킷 전송
    if ( m_strFriendName.empty() )
    {
        GASSERT( 0 && "이름이 등록되지 않았습니다." );
        return;
    }

    m_pGaeaClient->GetFriendClient()->ReqFriendLureAns( m_strFriendName.c_str(), false, m_Guid );
}