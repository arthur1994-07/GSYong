#pragma once

#include "./Notify.h"

namespace gamelogic
{

class NotifyFriendAdd : public NotifyBase
{
public:
    NotifyFriendAdd(
        const sc::MGUID& Guid,
        const std::string& JsonData,
        const __time64_t& Date);
    virtual ~NotifyFriendAdd();

protected:

public:
    //! �˸� ����
    virtual std::string Description() override;
};

} // namespace gamelogic