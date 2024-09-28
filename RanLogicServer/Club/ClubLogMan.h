#ifndef _LOGIC_SERVER_CLUB_LOG_MAN_H_
#define _LOGIC_SERVER_CLUB_LOG_MAN_H_

#pragma once

#include <string>
#include <vector>
#include "../../SigmaCore/Database/Sqlite/SQLite.h"
#include "../../RanLogic/Club/GLClubDefine.h"

namespace club
{

class LogManServer
{
public: 
    LogManServer();
    ~LogManServer();

protected:
    SQLite::Database m_SQLite;
    CRITICAL_SECTION m_Lock;
	sqlite3_stmt* m_stmtInsert;
	std::string m_strInsertQuery;

public:
    bool MakeDb();
    bool Insert(DWORD ClubDbNum, club::EM_LOG_TYPE LogType, std::string& LogText);
    void Search(DWORD ClubDbNum, std::vector<LOG_DATA>& Result, int PageNum, int PageSize, club::EM_LOG_TYPE LogType);
    bool DeleteOneDayData(const __time64_t& DeleteDate);
	bool InsertNew( DWORD ClubDbNum, club::EM_LOG_TYPE LogType, std::string& LogText );

protected:
	void prepareSQLStatement();
	void finalizeSQLStatement();
};

} // namespace club
#endif // _LOGIC_SERVER_CLUB_LOG_MAN_H_