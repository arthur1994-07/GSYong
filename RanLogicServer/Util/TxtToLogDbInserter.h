#ifndef _LOGIC_SERVER_TXT_TO_LOG_DB_INSERTER_H_
#define _LOGIC_SERVER_TXT_TO_LOG_DB_INSERTER_H_

#pragma once

#include <string>
#include <vector>

namespace db
{
    class IDbManager;
}

class TxtToLogDbInserter
{
public:
    TxtToLogDbInserter(
        db::IDbManager* pDbMan,
        db::IDbManager* pAdoMan,
        const std::string& DirectoryPath);
    ~TxtToLogDbInserter();

protected:
    std::vector<std::string> m_LogFile;
    db::IDbManager* m_pDbMan;
    db::IDbManager* m_pAdoMan;
    std::string m_DirPath;
    volatile bool m_RunThread;

protected:
    void LogFileToDB();
    void LogFileToDB(const std::string& FilePath);
    bool IsLogFile(const std::string& FileName);

public:
    void AddLogFile(const std::string& FileName);
    void Run();
    void Stop();
    bool IsRunning() const { return m_RunThread; }
};

#endif // _LOGIC_SERVER_TXT_TO_LOG_DB_INSERTER_H_