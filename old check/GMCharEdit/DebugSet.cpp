#include "stdafx.h"
#include <dbghelp.h>
#include <strsafe.h>
#include "./gassert.h"
#include "./Util/SystemInfo.h"
//#include "./File/FileSystemUtil.h"
//#include "./File/SubPath.h"
//#include "./String/StringUtil.h"
#include "./File/FileUtil.h"
#include "./String/StringFormat.h"
#include "./DebugSet.h"

#include "./DebugInclude.h"

namespace sc
{
static DebugSet* m_pDebugSet = NULL;

DebugSet* DebugSet::GetInstance()
{
    if (!m_pDebugSet)
        m_pDebugSet = new DebugSet;
    return m_pDebugSet;
}

void DebugSet::ReleaseInstance()
{
    if (m_pDebugSet)
    {
        delete m_pDebugSet;
        m_pDebugSet = NULL;
    }
}

DebugSet::DebugSet()
    : m_hFile( INVALID_HANDLE_VALUE )
    , m_bLogFileFinalOpen( false )
    , m_bLogWrite( false )
    , m_wFlushCount( 0 )
    , m_LogSplitCount( 0 )
{
    // overflow
    //::memset(m_szFullDir, 0, sizeof(wchar_t)*MAX_PATH);
    ::memset(m_szFullDir, 0, sizeof(char)*MAX_PATH);
}

DebugSet::~DebugSet()
{
    if (INVALID_HANDLE_VALUE != m_hFile)
    {
        FlushFileBuffers(m_hFile);
        SAFE_CLOSE_HANDLE(m_hFile);
    }
}

// Game 은 sc::file::deleteFiles(strAppPath); 를 시작시 호출해서 이전 정보를 지운다.
// 계속 쌓일 수 있는 문제 때문에 이렇게 했지 싶다.
// Tool 은 그렇게 하지 않는다.
// 
void DebugSet::MakeLogFile(const std::string& FileName)
{
    THIS_AUTO_RECURSIVE_LOCK;

    if (INVALID_HANDLE_VALUE != m_hFile)
    {
        FlushFileBuffers(m_hFile);
        SAFE_CLOSE_HANDLE(m_hFile);
    }

    // Use time structure to build a customized time string.
    if (FileName.empty() )
    {
        time_t ltime;
        struct tm today;
        time(&ltime);
        localtime_s( &today, &ltime );
        char szFileName[MAX_PATH] = {0};
        _tcsftime(szFileName, MAX_PATH, "Logic_%y%m%d_%H%M%S.txt", &today);
        m_strLogName = szFileName;
        m_strLogFile = sc::string::format("%1%%2%", m_szFullDir, m_strLogName);
    }
    else
    {
        m_strLogName = FileName;
        m_strLogFile = sc::string::format("%1%%2%", m_szFullDir, FileName);
    }

    if ( !sc::file::autoRename(m_strLogFile) )
	{
		return;
	}

    m_hFile = ::CreateFile(
        m_strLogFile.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL );
    if ( m_hFile == INVALID_HANDLE_VALUE )
    {
        SAFE_CLOSE_HANDLE(m_hFile);
        return;
    }

    //unsigned char szUnicodeMark[2];
    DWORD dwBytesWritten=0;
    //szUnicodeMark[0] = 0xFF;
    //szUnicodeMark[1] = 0xFE;
    //::WriteFile( m_hFile, szUnicodeMark, 2, &dwBytesWritten, NULL );

    std::string strHead1( "----------------------------------------------- \r\n" );
    std::string strHead2( "Game Error Report \r\n" );
    std::string strHead3( "----------------------------------------------- \r\n" );	

    //::WriteFile( m_hFile, strHead1.c_str(), (DWORD) (sizeof(wchar_t)*strHead1.size()), &dwBytesWritten, NULL );
    //::WriteFile( m_hFile, strHead2.c_str(), (DWORD) (sizeof(wchar_t)*strHead2.size()), &dwBytesWritten, NULL );
    //::WriteFile( m_hFile, strHead3.c_str(), (DWORD) (sizeof(wchar_t)*strHead3.size()), &dwBytesWritten, NULL );

    //std::string strTime = sc::string::format("[%1%]Start \n", m_strSTARTTIME.c_str() );
    //::WriteFile( m_hFile, strTime.c_str(), (DWORD) (sizeof(wchar_t)*strTime.size()), &dwBytesWritten, NULL );

	::WriteFile( m_hFile, strHead1.c_str(), (DWORD) (sizeof(char)*strHead1.size()), &dwBytesWritten, NULL );
	::WriteFile( m_hFile, strHead2.c_str(), (DWORD) (sizeof(char)*strHead2.size()), &dwBytesWritten, NULL );
	::WriteFile( m_hFile, strHead3.c_str(), (DWORD) (sizeof(char)*strHead3.size()), &dwBytesWritten, NULL );

	std::string strTime = sc::string::format("[%1%]Start \r\n", m_strSTARTTIME.c_str() );
	::WriteFile( m_hFile, strTime.c_str(), (DWORD) (sizeof(char)*strTime.size()), &dwBytesWritten, NULL );

    m_bLogWrite = true;
}

void DebugSet::FlushLogFile()
{
    THIS_AUTO_RECURSIVE_LOCK;

    ++m_wFlushCount;
    if ( m_wFlushCount > LOG_FLUSH_COUNT )
    {
        ::FlushFileBuffers( m_hFile );
        m_wFlushCount=0;
    }

    ++m_LogSplitCount;
    if ( m_LogSplitCount > LOG_SPLIT_COUNT )
    {
		std::string strLogName;
        MakeLogFile( strLogName );
        m_LogSplitCount=0;
        m_wFlushCount=0;
    }
}

std::string DebugSet::GetCurTime()
{
    CTime Time( CTime::GetCurrentTime() );

    CString strTime;
    strTime.Format(
        "%d-%02d-%02d %02d:%02d:%02d",
        Time.GetYear(),
        Time.GetMonth(),
        Time.GetDay(),
        Time.GetHour(),
        Time.GetMinute(),
        Time.GetSecond());

    return std::string(strTime.GetString());
}

void DebugSet::OutputDebugString( const char* szFile, DWORD dwLine, const char* szStr, bool bMessageBox )
{
    if (!szStr)
        return;

    char szMsg[MAX_PATH] = {0};
    StringCchCopyA(szMsg, MAX_PATH, szStr);
    LogToFileWithTime(szStr);
#ifdef _DEBUG
    Trace( szFile, dwLine, szStr );
    if (true == bMessageBox)
        ::MessageBox(NULL, szStr, "ERROR", MB_OK);
#endif
}

void DebugSet::OutputDebugString(const char* szStr, bool bMessageBox )
{
    if (!szStr)
        return;

    char szMsg[MAX_PATH] = {0};
    StringCchCopyA(szMsg, MAX_PATH, szStr);
    LogToFileWithTime(szStr);
#ifdef _DEBUG
    if (bMessageBox)
        ::MessageBox(NULL, szStr, "ERROR", MB_OK);	
#endif
}

void DebugSet::Trace(const char* szFile, DWORD dwLine, const char* strMsg)
{
#ifdef _DEBUG
#ifdef _UNICODE
    char szMsg[MAX_PATH]={0};
    CORE_COMMON::StringUtil::unicodeToAnsi(
        strMsg,
        szMsg,
        MAX_PATH);
    TRACE(
        "%s(%d) : %s\r\n",
        szFile,
        dwLine,
        szMsg );
#else
    TRACE(
        "%s(%d) : %s\r\n",
        szFile,
        dwLine,
        strMsg );
#endif // _UNICODE
#endif // _DEBUG
}


void DebugSet::LogToFileWithTime(const char* szFormat)
{
    if (szFormat)
        LogToFileWithTime(std::string(szFormat));
}

void DebugSet::LogToFileWithTime(const std::string& strFormat)
{
    if (!strFormat.empty())
        LogToFileWithTime("", strFormat);
}

void DebugSet::LogToFileWithTime(const std::string& strFileName, const std::string& strFormat)
{
    std::string strLog = sc::string::format("[%1%]%2%", GetCurTime(), strFormat);
    LogToFile(strFileName,strLog);
}

void DebugSet::LogToFile(const char* szFormat)
{	
    if (szFormat )
        LogToFile(std::string(szFormat));
}

void DebugSet::LogToFile(const std::string& strFormat)
{
    if (!strFormat.empty())
        LogToFile("", strFormat);
}

void DebugSet::LogToFile(const std::string& strFileName, const char* szFormat)
{
    if (szFormat)
        LogToFile(strFileName, std::string(szFormat));
}

void DebugSet::LogToFile(const std::string& strFileName, const std::string& strFormat)
{
    THIS_AUTO_RECURSIVE_LOCK;

    if ( INVALID_HANDLE_VALUE == m_hFile )
        MakeLogFile( strFileName );

	// MakeLogFile 에서 실패 했을 수도 있다.
	if ( INVALID_HANDLE_VALUE == m_hFile )
		return;

    std::string Message(strFormat);
    Message.append("\r\n");

    DWORD dwWritten;
    ::WriteFile( m_hFile, Message.c_str(), (DWORD) (sizeof(char)*Message.length()), &dwWritten, NULL );

    FlushLogFile();
}

void DebugSet::LogToFileWithFuncName( const std::string& functionName, int line , const std::string& strFormat )
{
    std::string strLog = sc::string::format(" ( %1%: Line:%2% ) =>%3%",functionName,line,strFormat);
    LogToFile(strLog);
}

void DebugSet::DeleteFile(const std::string& strFileName)
{
    THIS_AUTO_RECURSIVE_LOCK;
    // 절대 경로 붙이기
    char szFullPathFileName[MAX_PATH] = {0};
    ::StringCchCopyA( szFullPathFileName, MAX_PATH, m_szFullDir );
    ::StringCchCatA( szFullPathFileName, MAX_PATH, strFileName.c_str() );
    sc::file::remove( szFullPathFileName );
}

void DebugSet::OpenErrorLog()
{
    if (INVALID_HANDLE_VALUE != m_hFile)
    {
        SAFE_CLOSE_HANDLE(m_hFile);
    }

    CString strPROCESS;
    strPROCESS.Format( _T( "notepad %s" ), m_strLogName.c_str() );

    STARTUPINFO si;
    ::memset( &si, 0, sizeof( STARTUPINFO ) );
    si.cb = sizeof( STARTUPINFO );
    si.dwFlags = 0;

    PROCESS_INFORMATION pi;
    ::CreateProcess(
        NULL,
        strPROCESS.GetBuffer(),
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        m_szFullDir,
        &si,
        &pi );
    strPROCESS.ReleaseBuffer();
}

HRESULT	DebugSet::OneTimeSceneInit(const char* szAppPath, bool bLogFileFinalOpen, bool bServer)
{
    FinalCleanup();

    std::string AppPath = sc::getAppPath();

    //if (true == bServer)
    //{    
        AppPath.append("\\log\\");
        StringCchCopyA(m_szFullDir, MAX_PATH, AppPath.c_str());
        sc::file::createDirectory(m_szFullDir);
    //}
    //else
    //{        
    //    AppPath.append("//log");
    //    StringCchCopyA(m_szFullDir, MAX_PATH, AppPath.c_str());
    //    sc::file::createDirectory(m_szFullDir);
    //}

    m_bLogFileFinalOpen = bLogFileFinalOpen;

    m_strSTARTTIME = GetCurTime();	

    m_bLogWrite = false;

    return S_OK;
}

HRESULT DebugSet::FinalCleanup()
{
    if ( m_bLogWrite && m_bLogFileFinalOpen )
        OpenErrorLog();

    return S_OK;
}

} // namespace sc