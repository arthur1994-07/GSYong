#include "pch.h"
#include "../../../SigmaCore/Log/LogMan.h"
#include "../../../RanLogic/Notify/NotifyDefine.h"
#include "../DbManager.h"
#include "./DbActionGameNotify.h"


// ----------------------------------------------------------------------------
#include "../../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------


namespace db
{
    NotifyInsert::NotifyInsert(
        int ChaDbNum,
        gamelogic::notify::NOTIFY_TYPE Type,
        const sc::MGUID& Guid,
        const std::string& Data)
        : m_ChaDbNum(ChaDbNum)
        , m_Type(Type)
        , m_Guid(Guid)
        , m_Data(Data)
    {
    }

    NotifyInsert::~NotifyInsert()
    {
    }   

    int NotifyInsert::Execute(NetServer* pServer)
    {
        if (m_pSubDbManager)
        {
            return m_pSubDbManager->NotifyInsert(
                m_ChaDbNum,
                m_Type,
                m_Guid,
                m_Data);
        }
        else
        {
            sc::writeLogError(std::string("NotifyInsert use ado function"));
            return sc::db::DB_ERROR;
        }
    }

    NotifyDelete::NotifyDelete(const sc::MGUID& Guid)
        : m_Guid(Guid)
    {
    }

    NotifyDelete::~NotifyDelete()
    {
    }
    
    int NotifyDelete::Execute(NetServer* pServer)
    {
        if (m_pSubDbManager)
        {
            return m_pSubDbManager->NotifyDelete(m_Guid);
        }
        else
        {
            sc::writeLogError(std::string("NotifyDelete use ado function"));
            return sc::db::DB_ERROR;
        }
    }

} // namespace db