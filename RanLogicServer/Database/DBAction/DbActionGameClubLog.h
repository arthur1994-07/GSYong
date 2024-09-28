#pragma once

#include "../../../RanLogic/Club/GLClubDefine.h"
#include "./DbAction.h"

namespace db
{
    class ClubLogInsert : public DbAction
    {
    public:
        ClubLogInsert(
            DWORD ClientSlot,
            DWORD ClubDbNum,
            club::EM_LOG_TYPE LogType,
            const std::string& LogMsg);
        virtual ~ClubLogInsert();

    protected:
        DWORD m_ClubDbNum;
        club::EM_LOG_TYPE m_LogType;
        std::string m_LogMsg;

    public:
        virtual int Execute(NetServer* pServer) override;        
    };

    class ClubLogDelete : public DbAction
    {
    public:
        ClubLogDelete(const __time64_t& DeleteTime);
        virtual ~ClubLogDelete();

    protected:
        __time64_t m_DeleteTime;

    public:
        virtual int Execute(NetServer* pServer) override;        
    };

    class ClubLogSearch : public DbAction
    {
    public:
        ClubLogSearch(
            DWORD ClientSlot,
            DWORD ChaDbNum,
            DWORD ClubDbNum,
            WORD Page,
            WORD PageSize,
            club::EM_LOG_TYPE LogType);
        virtual ~ClubLogSearch();

    protected:
        DWORD m_ChaDbNum;
        DWORD m_ClubDbNum;        
        WORD  m_Page;
        WORD  m_PageSize;
        club::EM_LOG_TYPE m_LogType;

    public:
        virtual int Execute(NetServer* pServer) override;        
    };

} // namespace db