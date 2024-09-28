#pragma once

#include <boost/tr1/memory.hpp>

#include "../../RanLogic/Notify/NotifyDefine.h"
#include "../../SigmaCore/String/MinGuid.h"

class INotifyClientBase;
struct NET_MSG_GENERIC;
class GLGaeaClient;

class NotifyFactory
{
public:
    NotifyFactory();
    
    std::tr1::shared_ptr<INotifyClientBase> MakeNotifyClient(
        GLGaeaClient* pGaeaClient,
        const sc::MGUID& Guid, 
        const __time64_t& Date, 
        const gamelogic::notify::NOTIFY_TYPE Type,
        const std::string& Json );

    std::tr1::shared_ptr<INotifyClientBase> MakeNotifyClient( 
        GLGaeaClient* pGaeaClient,
        const gamelogic::notify::NOTIFY_TYPE Type,
        NET_MSG_GENERIC* nmg );
};
