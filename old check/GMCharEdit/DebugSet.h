#ifndef	_CDEBUGSET_H_
#define	_CDEBUGSET_H_

#pragma once

#include <string>
#include "./Util/Lock.h"
#include "./String/StringFormat.h"

#define ToViewP(arg) (ToView(PSTREAM(arg)))

#ifndef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(P) { if(P) { CloseHandle(P); P = INVALID_HANDLE_VALUE; } }
#endif 

namespace sc
{
//     namespace LOGFILE
//     {
//         const std::wstring LoadSet      = L"LoadSet.log";
//         const std::wstring UseSoundList = L"UseSoundList.txt";
//     };

    class DebugSet : public RecursiveLock
    {
    protected:
        enum
        {
            LOG_FLUSH_COUNT=100,
            LOG_SPLIT_COUNT=5000
        };

    protected:
        HANDLE m_hFile;
        char m_szFullDir[MAX_PATH];
        bool m_bLogFileFinalOpen;
        bool m_bLogWrite;

        //CTime	m_LogTime;
        std::string m_strLogFile; //! Full Path
        std::string m_strLogName;
        std::string m_strSTARTTIME;

        WORD m_wFlushCount;
        WORD m_LogSplitCount;

    protected:
        DebugSet();	
        virtual ~DebugSet();

    public:
        static DebugSet* GetInstance();
        static void ReleaseInstance();

    protected:	
        void MakeLogFile(const std::string& FileName);
        void FlushLogFile();

    public:
        std::string GetCurTime();

        //! File O / Debug â O (Debug Bulild �϶���)
        void OutputDebugString(const char* szFile, DWORD dwLine, const char* szStr, bool bMessageBox=false);

        //! File O / MessageBox O (Debug Bulild �϶���)	
        void OutputDebugString(const char* szStr, bool bMessageBox=false);

        void Trace(const char* szFile, DWORD dwLine, const char* strMsg);

        //! ���Ϸ� �Ѹ�
        //! ���� O / �ð� O
        void LogToFileWithTime(const char* szFormat );
        void LogToFileWithTime(const std::string& strFormat );

        //! ���� O / �ð� X
        void LogToFile(const char* szFormat );
        void LogToFile(const std::string& strFormat );


	private:
		//! ���� O / �ð� X
		void LogToFile(const std::string& strFileName, const char* szFormat );
		void LogToFile(const std::string& strFileName, const std::string& strFormat );	

		//! ���� O / �ð� O
		void LogToFileWithTime(const std::string& strFileName, const std::string& strFormat);


	public:
        void LogToFileWithFuncName(const std::string& functionName, int line , const std::string& strFormat);

        void DeleteFile(const std::string& strFileName);

        //! ����� �����α� ������ notepad �� �̿��ؼ� open �Ѵ�.
        void OpenErrorLog();

        //! ����������� ���Ϸ� ����� ��, �����θ� �����ؾ� �Ѵ�.
        HRESULT OneTimeSceneInit(const char* pszFullDir, bool bLogFileFinalOpen=false, bool bServer=false);

        HRESULT FinalCleanup();

    public:
        inline void setFileFinalOpen(bool bOpen = true) { m_bLogFileFinalOpen = bOpen; }
        inline void setLogWrite(bool bWrite = true) { m_bLogWrite = bWrite; }
        inline const char* GetPath() const { return m_szFullDir; }

    }; // class DebugSet
    
} // End of namespace CORE_COMMON

// #ifdef _DEBUG
// #define ASTrace(str) CORE_COMMON::getDebugSet()->OutputDebugString( __FILE__, (DWORD)__LINE__, str, false )
// #else
// #define ASTrace(str)
// #endif

#endif // _CDEBUGSET_H_