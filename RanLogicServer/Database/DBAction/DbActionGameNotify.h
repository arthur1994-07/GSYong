#pragma once

#include "../../../SigmaCore/String/MinGuid.h"
#include "../../../RanLogic/Notify/NotifyDefine.h"
#include "DbAction.h"

namespace db
{
    class NotifyInsert : public DbAction
    {
    public:
        NotifyInsert(
            int ChaDbNum,
            gamelogic::notify::NOTIFY_TYPE Type,
            const sc::MGUID& Guid,
            const std::string& Data);
        virtual ~NotifyInsert();

    protected:
        int m_ChaDbNum;
        gamelogic::notify::NOTIFY_TYPE m_Type;
        sc::MGUID m_Guid;
        std::string m_Data;

    public:
        virtual int Execute(NetServer* pServer) override;
    };

    class NotifyDelete : public DbAction
    {
    public:
        NotifyDelete(const sc::MGUID& Guid);
        virtual ~NotifyDelete();

    protected:
        sc::MGUID m_Guid;

    public:
        virtual int Execute(NetServer* pServer) override;
    };

} // namespace db