#include "pch.h"
#include <boost/thread.hpp>
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/File/FileUtil.h"
#include "../Database/DbManager.h"
#include "TxtToLogDbInserter.h"


#include "../../SigmaCore/DebugInclude.h"


TxtToLogDbInserter::TxtToLogDbInserter(
    db::IDbManager* pDbMan,
    db::IDbManager* pAdoMan,
    const std::string& DirectoryPath)
    : m_pDbMan(pDbMan)
    , m_pAdoMan(pAdoMan)
    , m_DirPath(DirectoryPath)
    , m_RunThread(false)
{
}

TxtToLogDbInserter::~TxtToLogDbInserter()
{
}

void TxtToLogDbInserter::AddLogFile(const std::string& FileName)
{
    std::string AddFileName(FileName);
    sc::string::lowerString(AddFileName);
    m_LogFile.push_back(AddFileName);
}

bool TxtToLogDbInserter::IsLogFile(const std::string& FileName)
{
    std::string LogFileName(FileName);
    sc::string::lowerString(LogFileName);
    for (size_t i=0; i<m_LogFile.size(); ++i)
    {
        if (LogFileName == m_LogFile[i])
            return true;
    }

    return false;
}

void TxtToLogDbInserter::Run()
{
    boost::thread DbInsertThread = boost::thread(boost::bind(&TxtToLogDbInserter::LogFileToDB, this));
}

void TxtToLogDbInserter::Stop()
{
    m_RunThread = false;
}

void TxtToLogDbInserter::LogFileToDB()
{
    m_RunThread = true;

    CFileFind finder;
    std::string strWildcard(m_DirPath);
    strWildcard += _T("\\*.sql");
    BOOL bWorking = finder.FindFile(strWildcard.c_str());

    std::vector<std::string> FileList;    

    while (bWorking)
    {
        bWorking = finder.FindNextFile();

        // skip . and .. files; otherwise, we'd
        // recur infinitely!
        if (finder.IsDots())
            continue;

        //	if it's a directory, recursively search it
        if (!finder.IsDirectory())
        {
            CString FileName = finder.GetFileName();
            FileName.MakeLower();
            CString strPATH = finder.GetFilePath();

            if (!IsLogFile(FileName.GetString()))
            {
                FileList.push_back(strPATH.GetString());                
            }
        }
    }

    for (size_t i=0; i<FileList.size(); ++i)
    {
        if (!m_RunThread)
            return;

        LogFileToDB(FileList[i]);
        sc::file::remove(FileList[i]);
    }

    m_RunThread = false;
}

void TxtToLogDbInserter::LogFileToDB(const std::string& FilePath)
{
    // Open
    FILE* pFile = fopen(FilePath.c_str(), "r");
    if (!pFile)
        return;

    // Read line
    while (true)
    {
        DWORD  dwBytesRead = 0;
        char   ReadBuffer[1024] = {0};
        if (fgets(ReadBuffer, 1024-1, pFile) == NULL)
        {
            fclose(pFile);
            return;
        }
        
        //ReadBuffer[dwBytesRead]='\0'; // NULL character
        // Insert to db
        if (m_pAdoMan)
        {
            m_pAdoMan->ExecuteSqlLogDb(ReadBuffer);
        }
        else if (m_pDbMan)
        {
            m_pDbMan->ExecuteSqlLogDb(ReadBuffer);
        }
        else
        {
            // There is no database manager
        }
    }

    fclose(pFile);
}