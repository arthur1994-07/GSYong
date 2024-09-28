#include "pch.h"
#include "NotifyClientFactory.h"

#include <RpcDce.h>

#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../../SigmaCore/Math/SeqUniqueGuid.h"

#include "../../RanLogic/Notify/NotifyDefine.h"

#include "NotifyClientFriendAdd.h"
#include "NotifyClientClubInvite.h"
#include "NotifyClientGroupChatInvite.h"
#include "NotifyClientPartyInvite.h"

NotifyFactory::NotifyFactory()
{
}

std::tr1::shared_ptr<INotifyClientBase> NotifyFactory::MakeNotifyClient( 
    GLGaeaClient* pGaeaClient,
    const sc::MGUID& Guid, 
    const __time64_t& Date, 
    const gamelogic::notify::NOTIFY_TYPE Type, 
    const std::string& JsonStr )
{
    json_spirit::mValue Value;
    if (!json_spirit::read(JsonStr, Value))
        return std::tr1::shared_ptr<INotifyClientBase> ((INotifyClientBase*) 0);

    const json_spirit::mObject& Object = Value.get_obj();
    int MessageType = 0;
    if (!sc::Json::find_value(Object, "M", MessageType))
    {
        return std::tr1::shared_ptr<INotifyClientBase> ((INotifyClientBase*) 0);
    }

    //! DB에서 읽어서 전달된 데이터는 Json 형식이다
    switch (Type)
    {
    case gamelogic::notify::FRIEND_ADD:
        {
            std::tr1::shared_ptr<INotifyClientBase> spNotifyClient(new NotifyClientFriendAdd(pGaeaClient, Guid, Date) );
            if( spNotifyClient->Parsing( Object ) )
                return spNotifyClient;
        }
        break;
    case gamelogic::notify::UNKNOWN:
    default:
        sc::writeLogError(sc::string::format("MakeNotify unknown type %1%", static_cast<int> (Type)));
        break;
    };

    return std::tr1::shared_ptr<INotifyClientBase> ((INotifyClientBase*) 0);
}

std::tr1::shared_ptr<INotifyClientBase> NotifyFactory::MakeNotifyClient( 
    GLGaeaClient* pGaeaClient,
    const gamelogic::notify::NOTIFY_TYPE Type,
    NET_MSG_GENERIC* nmg )
{

    sc::MGUID Guid;
    sc::SeqUniqueGUID::Instance()->getGUID( Guid );

    const __time64_t& Date = sc::time::GetCurrentTime();

    switch (Type)
    {
    case gamelogic::notify::FRIEND_ADD:
        {
            std::tr1::shared_ptr<INotifyClientBase> spNotifyClient(new NotifyClientFriendAdd(pGaeaClient, Guid, Date) );
            if( spNotifyClient->Parsing( nmg ) )
                return spNotifyClient;
        }
        break;

    case gamelogic::notify::GROUPCHAR_INVITE:
        {
            std::tr1::shared_ptr<INotifyClientBase> spNotifyClient(new NotifyClientGroupChatInvite(pGaeaClient, Guid, Date) );
            if( spNotifyClient->Parsing( nmg ) )
                return spNotifyClient;
        }
        break;
    case gamelogic::notify::CLUB_INVITE:
        {
            std::tr1::shared_ptr<INotifyClientBase> spNotifyClient(new NotifyClientClubInvite(pGaeaClient, Guid, Date) );
            if( spNotifyClient->Parsing( nmg ) )
                return spNotifyClient;
        }
        break;
    case gamelogic::notify::PARTY_INVITE:
        {
            std::tr1::shared_ptr<INotifyClientBase> spNotifyClient(new NotifyClientPartyInvite(pGaeaClient, Guid, Date) );
            if( spNotifyClient->Parsing( nmg ) )
                return spNotifyClient;
        }
        break;
    case gamelogic::notify::UNKNOWN:
    default:
        sc::writeLogError(sc::string::format("MakeNotify unknown type %1%", static_cast<int> (Type)));
        break;
    };

    return std::tr1::shared_ptr<INotifyClientBase> ((INotifyClientBase*) 0);
}
