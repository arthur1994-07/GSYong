#include "pch.h"
#include "../../SigmaCore/File/FileUtil.h"
#include "../../SigmaCore/Log//LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/Util/DateTime.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/Util/MinMessageBox.h"

#include "./ClubLogMan.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace club
{

LogManServer::LogManServer()
{
    InitializeCriticalSectionAndSpinCount(&m_Lock, sc::CRITICAL_SECTION_SPIN_COUNT);    
    MakeDb();
	prepareSQLStatement();
}

LogManServer::~LogManServer()
{
	finalizeSQLStatement();

    if (m_SQLite.IsOpen())
        m_SQLite.Close();
    DeleteCriticalSection(&m_Lock);
}

bool LogManServer::MakeDb()
{
    bool FileExist = false;
    std::string DbPath(sc::getAppPath());
    DbPath.append("\\cache\\");
    sc::file::createDirectory(DbPath);

    DbPath.append("ClubLog.db3");
    
    if (sc::file::isExist(DbPath))
        FileExist = true;

    if (SQLITE_OK != m_SQLite.Open(DbPath.c_str()))
    {
        std::string ErrMsg(
            sc::string::format(
                "MakeDb failed. %1%", DbPath));
        sc::ErrorMessageBox(ErrMsg, false);
        return false;
    }

    if (!FileExist)
    {
        std::string Query("\
    CREATE TABLE IF NOT EXISTS ClubLog( \
        LogNum INTEGER NOT NULL, \
        ClubDbNum INTEGER NOT NULL, \
        LogType INTEGER NOT NULL, \
        LogDate INTEGER NOT NULL, \
        LogText TEXT NOT NULL, \
        PRIMARY KEY(LogNum)); \
        CREATE INDEX IF NOT EXISTS Idx_ClubLog_ClubDbNum ON ClubLog (ClubDbNum); \
        CREATE INDEX IF NOT EXISTS Idx_ClubLog_LogType ON ClubLog (LogType); \
        CREATE INDEX IF NOT EXISTS Idx_ClubLog_LogDate ON ClubLog (LogDate);");
        
        m_SQLite.ExecuteSQL(Query);
    }

    return true;
}

bool LogManServer::Insert(DWORD ClubDbNum, club::EM_LOG_TYPE LogType, std::string& LogText)
{
	// 처리 속도 향상을 위해서 미리 컴파일한 스크립트 사용하도록 수정함
	return InsertNew( ClubDbNum, LogType, LogText );

    // 다른 thread 에서 접근하기 때문에 Lock 이 필요하다.
    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    bool bReturn = false;
    DWORD StartTime = GetTickCount();

    sc::string::replaceForDb(LogText);
    __time64_t CurrentTime = sc::time::GetCurrentTime();

    std::string Query(
        sc::string::format(
            "INSERT INTO ClubLog(ClubDbNum, LogType, LogDate, LogText) VALUES (%1%, %2%, %3%, '%4%');",
            ClubDbNum,
            LogType,
            CurrentTime,
            LogText));
    if (SQLITE_OK == m_SQLite.ExecuteSQL(Query))
    {
        bReturn = true;
    }
    else
    {
        sc::writeLogError(
            sc::string::format(
            "club::LogMan::Insert failed. %1%",
            Query));
        bReturn = false;
    }

    DWORD DurationTime = GetTickCount() - StartTime;
    if (DurationTime > 1000)
    {
        sc::writeLogInfo(
            sc::string::format(
                "LogManServer::Insert %1% msec", DurationTime));
    }

    return bReturn;
}

void LogManServer::Search(DWORD ClubDbNum, std::vector<LOG_DATA>& Result, int PageNum, int PageSize, club::EM_LOG_TYPE LogType)
{
    // 다른 thread 에서 접근하기 때문에 Lock 이 필요하다.
    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    DWORD StartTime = GetTickCount();

    int LimitRow = PageNum*PageSize;    
    std::string Query;
    if (LogType == club::LOG_ALL)
    {
        Query = sc::string::format(
            "SELECT LogNum, LogType, LogDate, LogText FROM ClubLog WHERE ClubDbNum=%1% ORDER BY LogNum DESC LIMIT %2%, %3%;",
            ClubDbNum,
            LimitRow,
            PageSize);
    }
    else
    {
        Query = sc::string::format(
            "SELECT LogNum, LogType, LogDate, LogText FROM ClubLog WHERE ClubDbNum=%1% AND LogType=%2% ORDER BY LogNum DESC LIMIT %3%, %4%;",
            ClubDbNum,
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

    DWORD DurationTime = GetTickCount() - StartTime;
    if (DurationTime > 1000)
    {
        sc::writeLogInfo(
            sc::string::format(
                "LogManServer::Search %1% msec", DurationTime));
    }
}

bool LogManServer::DeleteOneDayData(const __time64_t& DeleteDate)
{
    // 다른 thread 에서 접근하기 때문에 Lock 이 필요하다.
    sc::CriticalSectionOwnerLock OwnerLock(m_Lock);

    bool bReturn = false;
    DWORD StartTime = GetTickCount();

    __time64_t DeleteTime = DeleteDate;
    if (DeleteDate == 0)
    {
        // 1일 = 60 * 60 * 24 = 86400
        DeleteTime = sc::time::GetCurrentTime() - club::ClubLogDeleteTime;
    }

    std::string Query(
        sc::string::format(
            "DELETE FROM ClubLog WHERE LogDate<%1%",
            DeleteTime));
    if (SQLITE_OK == m_SQLite.ExecuteSQL(Query))
    {
        bReturn = true;
    }
    else
    {
        sc::writeLogError(
            sc::string::format(
                "club::LogMan::DeleteOneDayData failed. %1%",
                Query));
        bReturn = false;
    }

    DWORD DurationTime = GetTickCount() - StartTime;
    if (DurationTime > 1000)
    {
        sc::writeLogInfo(
            sc::string::format(
                "LogManServer::DeleteOneDayData %1% msec", DurationTime));
    }

    return bReturn;
}

bool LogManServer::InsertNew( DWORD ClubDbNum, club::EM_LOG_TYPE LogType, std::string& LogText )
{
	// 다른 thread 에서 접근하기 때문에 Lock 이 필요하다.
	sc::CriticalSectionOwnerLock OwnerLock( m_Lock );

	bool bReturn = false;
	DWORD StartTime = GetTickCount();

	__time64_t CurrentTime = sc::time::GetCurrentTime();
	sc::string::replaceForDb( LogText );
	std::string strLogTextUtf8 = sc::string::ansi_to_utf8( LogText );

	sqlite3_bind_int( m_stmtInsert, 1, ClubDbNum );
	sqlite3_bind_int( m_stmtInsert, 2, LogType );
	sqlite3_bind_int64( m_stmtInsert, 3, CurrentTime );
	sqlite3_bind_text( m_stmtInsert, 4, strLogTextUtf8.c_str(), strLogTextUtf8.length(), SQLITE_STATIC );
	
	int Result = sqlite3_step( m_stmtInsert );
	if ( SQLITE_DONE == Result )
	{
		bReturn = true;
	}
	else
	{
		sc::writeLogError( sc::string::format( "club::LogMan::Insert failed. %1%, errcode %2%", m_strInsertQuery, Result ) );
		bReturn = false;
	}
	
	sqlite3_reset( m_stmtInsert );

	DWORD DurationTime = GetTickCount() - StartTime;
	if ( DurationTime > 1000 )
	{
		sc::writeLogError( sc::string::format( "LogManServer::Insert %1% msec", DurationTime ) );
	}

	return bReturn;
}

void LogManServer::prepareSQLStatement()
{
	m_strInsertQuery = "INSERT INTO ClubLog( ClubDbNum, LogType, LogDate, LogText ) VALUES ( ?, ?, ?, ? );";
	sqlite3_prepare_v2( m_SQLite.GetPtr(), m_strInsertQuery.c_str(), -1, &m_stmtInsert, NULL );
}

void LogManServer::finalizeSQLStatement()
{
	sqlite3_finalize( m_stmtInsert );
}

} // namespace club