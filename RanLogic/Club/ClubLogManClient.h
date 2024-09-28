#ifndef _GLOGIC_CLIENT_CLUB_LOG_MAN_CLIENT_H_
#define _GLOGIC_CLIENT_CLUB_LOG_MAN_CLIENT_H_

#pragma once

#include <string>
#include <vector>
#include "../../SigmaCore/Database/Sqlite/SQLite.h"
#include "../../RanLogic/Club/GLClubDefine.h"

namespace club
{

class LogManClient
{
public:
    LogManClient();
    ~LogManClient();

protected:
    SQLite::Database m_SQLite;
    DWORD m_LastUpdateTime; //! ������ update �ð�

protected:
    bool MakeDb();

public:
    void Reset();
    bool Insert(__int64 LogNum, __time64_t LogDate, club::EM_LOG_TYPE LogType, std::string& LogText);
    void Search(std::vector<LOG_DATA>& Result, int PageNum, int PageSize, club::EM_LOG_TYPE LogType);

    //! ������ update �ð�
    DWORD GetLastUpdateTime() const { return m_LastUpdateTime; }
    void  SetLastUpdateTime(DWORD LastTime) { m_LastUpdateTime = LastTime; }
};

} // namespace club

#endif // _GLOGIC_CLIENT_CLUB_LOG_MAN_CLIENT_H_