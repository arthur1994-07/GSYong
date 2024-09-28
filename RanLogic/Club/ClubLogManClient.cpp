#include "pch.h"
#include "../../SigmaCore/Util/MinMessageBox.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/String/StringFormat.h"

#include "./ClubLogManClient.h"


#include "../../SigmaCore/DebugInclude.h"

namespace club
{

LogManClient::LogManClient()
{
    MakeDb();
    // 최초 한번은 호출해 주기 위해서 현재시간보다 뒷 시간을 세팅한다.
    m_LastUpdateTime = GetTickCount()-60000;
}

LogManClient::~LogManClient()
{
    if (m_SQLite.IsOpen())
        m_SQLite.Close();
}

bool LogManClient::MakeDb()
{    
    // memory db 로 관리한다.
    if (SQLITE_OK != m_SQLite.Open(":memory:"))
    {
        sc::ErrorMessageBox(std::string("LogManClient::MakeDb Open failed"), false);
        return false;
    }
    
    std::string Query("\
CREATE TABLE IF NOT EXISTS ClubLog( \
LogNum INTEGER NOT NULL, \
LogType INTEGER NOT NULL, \
LogDate INTEGER NOT NULL, \
LogText TEXT NOT NULL, \
PRIMARY KEY(LogNum)); \
CREATE INDEX Idx_ClubLog_LogType ON ClubLog (LogType); \
CREATE INDEX Idx_ClubLog_LogDate ON ClubLog (LogDate);");

    if (SQLITE_OK != m_SQLite.ExecuteSQL(Query))
    {
        sc::ErrorMessageBox(std::string("LogManClient::MakeDb Create Table failed"), false);
        return false;
    }
    else
    {
        return true;
    }
}

void LogManClient::Reset()
{
    std::string Query("DELETE FROM ClubLog;");
    m_SQLite.ExecuteSQL(Query);
}

bool LogManClient::Insert(__int64 LogNum, __time64_t LogDate, club::EM_LOG_TYPE LogType, std::string& LogText)
{
    sc::string::replaceForDb(LogText);

    std::string Query(
        sc::string::format(
        "INSERT INTO ClubLog(LogNum, LogType, LogDate, LogText) VALUES (%1%, %2%, %3%, '%4%');",
        LogNum,
        LogType,
        LogDate,
        LogText));
    if (SQLITE_OK == m_SQLite.ExecuteSQL(Query))
    {
        return true;
    }
    else
    {
        /*
        sc::writeLogError(
            sc::string::format(
                "LogManClient::Insert failed. %1%",
                Query));
        */
        return false;
    }
}

void LogManClient::Search(std::vector<LOG_DATA>& Result, int PageNum, int PageSize, club::EM_LOG_TYPE LogType)
{
    int LimitRow = PageNum*PageSize;    
    std::string Query;
    if (LogType == club::LOG_ALL)
    {
        Query = sc::string::format(
            "SELECT LogNum, LogType, LogDate, LogText FROM ClubLog ORDER BY LogDate DESC LIMIT %1%, %2%;",            
            LimitRow,
            PageSize);
    }
    else
    {
        Query = sc::string::format(
            "SELECT LogNum, LogType, LogDate, LogText FROM ClubLog WHERE LogType=%1% ORDER BY LogDate DESC LIMIT %2%, %3%;",            
            LogType,
            LimitRow,
            PageSize);
    }

    SQLite::TablePtr pTable(m_SQLite.QuerySQL2(Query));
    if (pTable.m_pTable)
    {
        SQLite::Table& rTable = pTable();
        int nRowCount = rTable.GetRowCount();
        if (nRowCount <= 0)
            return;

        for (int i=0; i<nRowCount; ++i)
        {
            __int64 LogNum      = rTable.GetValue<__int64> (0);
            int LogType         = rTable.GetValue<int> (1);
            __time64_t LogDate  = rTable.GetValue<__time64_t> (2);
            std::string LogText = rTable.GetValue<std::string> (3);

            LOG_DATA LogData(LogNum, static_cast<club::EM_LOG_TYPE> (LogType), LogDate, LogText);
            Result.push_back(LogData);
            rTable.GoNext();
        }
    }
}

} // namespace club