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
    sc::MGUID           m_Guid;     //! �˸� ������ȣ
    notify::NOTIFY_TYPE m_Type;     //! �˸� Ÿ��    
    __time64_t          m_Date;     //! ��� �ð�
    std::string         m_JsonData; //! Json data

public:
    //! �˸� ����
    virtual std::string Description() = 0;

    //! �˸� Ÿ��
    notify::NOTIFY_TYPE GetType() const { return m_Type; }

    //! �˸� ������ȣ
    sc::MGUID GetGuid() const { return m_Guid; }

    //! �˸� Json data
    std::string GetJsonData() { return m_JsonData; }
    const std::string& GetJsonData() const { return m_JsonData; }

    //! �˸� Json data
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