#include "pch.h"
#include "./NotifyFriendAdd.h"


#include "../../SigmaCore/DebugInclude.h"


namespace gamelogic
{

    
NotifyFriendAdd::NotifyFriendAdd(
    const sc::MGUID& Guid,
    const std::string& JsonData,
    const __time64_t& Date)
    : NotifyBase(gamelogic::notify::FRIEND_ADD, Guid, JsonData, Date)
{
}

NotifyFriendAdd::~NotifyFriendAdd()
{
}

std::string NotifyFriendAdd::Description()
{
    return std::string("no description");
}

} // namespace gamelogic