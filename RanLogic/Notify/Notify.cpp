#include "pch.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/Util/DateTime.h"

#include "./NotifyFriendAdd.h"
#include "./Notify.h"


#include "../../SigmaCore/DebugInclude.h"


namespace gamelogic
{

NotifyBase::NotifyBase(
    notify::NOTIFY_TYPE Type,
    const sc::MGUID& Guid,
    const std::string& JsonData,
    const __time64_t& Date)
    : m_Type(Type)
    , m_Guid(Guid)
    , m_JsonData(JsonData)
    , m_Date(Date)
{
}

NotifyBase::~NotifyBase()
{
}

std::string NotifyBase::GetDateStr() const
{
    return sc::time::DateTimeFormat(m_Date, true, false);
}

std::tr1::shared_ptr<NotifyBase> MakeNotify(
    notify::NOTIFY_TYPE Type,
    const sc::MGUID& Guid,
    const std::string& JsonData,
    const __time64_t& Date)
{
    switch (Type)
    {
    case notify::FRIEND_ADD:
        return std::tr1::shared_ptr<NotifyBase> (new NotifyFriendAdd(Guid, JsonData, Date));
        break;

    default:
        sc::writeLogError(sc::string::format("MakeNotify unknown type %1%", static_cast<int> (Type)));
        break;
    };

    return std::tr1::shared_ptr<NotifyBase> ((NotifyBase*) 0);
}

} // namespace gamelogic