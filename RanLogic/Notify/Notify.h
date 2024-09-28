#pragma once

#include <string>
#include <vector>
#include <boost/tr1/memory.hpp>
#include "../../SigmaCore/String/MinGuid.h"
#include "./NotifyDefine.h"

namespace gamelogic
{
    
class NotifyBase
{
public:
    NotifyBase(
        notify::NOTIFY_TYPE Type,
        const sc::MGUID& Guid,
        const std::string& JsonData,
        const __time64_t& Date);
    virtual ~NotifyBase();

protected:
    sc::MGUID           m_Guid;     //! 알림 고유번호
    notify::NOTIFY_TYPE m_Type;     //! 알림 타입    
    __time64_t          m_Date;     //! 등록 시간
    std::string         m_JsonData; //! Json data

public:
    //! 알림 설명
    virtual std::string Description() = 0;

    //! 알림 타입
    notify::NOTIFY_TYPE GetType() const { return m_Type; }

    //! 알림 고유번호
    sc::MGUID GetGuid() const { return m_Guid; }

    //! 알림 Json data
    std::string GetJsonData() { return m_JsonData; }
    const std::string& GetJsonData() const { return m_JsonData; }

    //! 알림 Json data
    void SetJsonData(const std::string& JsonStr);

    __time64_t GetDate() const { return m_Date; }
    std::string GetDateStr() const;
};

std::tr1::shared_ptr<NotifyBase> MakeNotify(
    notify::NOTIFY_TYPE Type,
    const sc::MGUID& Guid,
    const std::string& JsonData,
    const __time64_t& Date);

} // namespace gamelogic