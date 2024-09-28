#include "stdafx.h"
#include <strsafe.h>
#include <string>
//#include <fstream>
#include <iostream>
//#include <math.h>
#include <sstream>

#include <iosfwd>
#include "./gassert.h"
//#include "./DebugSet.h"
#include "./String/StringFormat.h"
#include "./DebugSet.h"


#include "./DebugInclude.h"


bool _gassert(bool bExp, const char *const szExp, const char *const szFile, const long nLine)
{
    if (!bExp)
    {
#ifdef NDEBUG
        {
            std::stringstream strText;
            strText << std::endl;
            strText << _T("ASSERT:Error was occoured! ");
            strText << _T("File:") << szFile << _T(" ");
            strText << _T("Line:") << nLine << _T(" ");
            strText << _T("Expression:") << szExp << _T(" is false.");
            sc::DebugSet::GetInstance()->LogToFileWithTime( strText.str().c_str() );			
        }
#else
        {
            std::stringstream strText;
            strText << std::endl;
            strText << "Assert!" << std::endl;
            strText << std::endl;
            strText << "File : " << szFile << std::endl;
            strText << "Line : " << nLine << std::endl;
            strText << std::endl;
            strText << std::endl;
            strText << "Expression : " << szExp << " is fail." << std::endl;
            strText << std::endl;
            strText << "Debug Now?" << std::endl;
            strText << std::endl;

            if ( IDYES == MessageBoxA( NULL, strText.str().c_str(), "ASSERT", MB_ICONEXCLAMATION | MB_YESNO ) )
            {
                return true;
            }
            else
            {
            }			
        }
#endif

        return false;
    }
    return false;
}

void smtm_PurecallHandler(void)
{
    _gassert( 0, "pure virtual function called.", __FILE__, __LINE__ );

    int* null_pointer = NULL;
    *null_pointer = 0;
}

void DoRTC_Initialization ()
{
#ifdef __MSVC_RUNTIME_CHECKS
#ifdef NOCRT
    _RTC_Initialize();
#else
    // This is only necessary when using a custom RTC handler function.
    // If using a debug CRT and a custom handler is *not* designated with
    // _RTC_SetErrorFunc, then failures will be reported via _CrtDbgReport.
    int Catch_RTC_Failure(int errType, const char *file, int line, const char *module, const char *format, ...);
    _RTC_SetErrorFunc(Catch_RTC_Failure);
#endif
#endif
    // Do any other initialization here
}

void DoRTC_Termination ()
{
    // Do any other termination work here
#if defined(__MSVC_RUNTIME_CHECKS) && defined(NOCRT)
    _RTC_Terminate();
#endif
}

#ifdef __MSVC_RUNTIME_CHECKS

#include <stdarg.h>
#include <malloc.h>

#ifdef NOCRT
// Use intrinsics, so we don't add CRT dependencies
#pragma intrinsic(_tcscat)
#pragma intrinsic(_tcscpy)

TCHAR* IntToString(int i)
{
    static TCHAR buf[15];
    bool neg = i < 0;
    int pos = 14;
    buf[14] = 0;
    do
    {
        buf[--pos] = i % 10 + '0';
        i /= 10;
    }
    while (i);

    if (neg)	buf[--pos] = '-';

    return &buf[pos];
}
#else // NOCRT is undefined
#include <stdio.h>
#endif

int Catch_RTC_Failure(int errType, const char *file, int line, const char *module, const char *format, ...)
{
    // Prevent re-entrancy ; isn't necessary here, but if this were a
    // multi-threading program, it would be.
    static long running = 0;
    while (InterlockedExchange(&running, 1))	Sleep(0);

    // First, get the RTC error number from the var-arg list...
    va_list vl;
    va_start(vl, format);
    _RTC_ErrorNumber rtc_errnum = va_arg ( vl, _RTC_ErrorNumber );
    va_end(vl);

    // Build a string buffer to display in a message box
    char buf[MAX_PATH] = {0};

#ifdef NOCRT
    const char *err = _RTC_GetErrDesc(rtc_errnum);
    StringCchCopyA( buf, MAX_PATH, err );
    if (line)
    {
        StringCchCatA(buf, MAX_PATH, "\nLine #" );
        StringCchCatA( buf, MAX_PATH, IntToString(line) );
    }
    if (file)
    {
        StringCchCatA( buf, MAX_PATH, "\nFile:" );
        StringCchCatA( buf, MAX_PATH, file );
    }
    if (module)
    {
        StringCchCatA( buf, MAX_PATH, "\nModule:" );
        StringCchCatA( buf, MAX_PATH, module );
    }
#else // NOCRT is undefined
    char buf2[1024];
    va_start(vl, format);
    StringCbVPrintfA( buf2, 1024, format, vl);
    StringCchPrintfA( buf, 1024, "RTC Failure #%d (user specified type %d) occurred:\n\nModule:\t%s\nFile:\t%s\nLine:\t%d\n\nFull Message:\n%s",
        rtc_errnum, errType, module ? module : "", file ? file : "", line, buf2 );
#endif

    running = 0;

    StringCchCatA( buf, MAX_PATH, "\n\nWould you like to break to the debugger?" );
    return (MessageBoxA(NULL, buf, "RTC Failed...", MB_YESNO) == IDYES) ? 1 : 0;
}

#ifdef NOCRT
extern "C" _RTC_error_fn _CRT_RTC_INIT(void *, void **, int , int , int )
{
    return &Catch_RTC_Failure;
}
#endif

#endif

enum { __emWATCH_NUM = (32), };
DWORD __dwWATCH_CUR = 0;
TCHAR __szWATCH_TAG[__emWATCH_NUM][256];

void _LOOPWATCH(const TCHAR* szTAG)
{
    //if ( __dwWATCH_CUR >= __emWATCH_NUM )	__dwWATCH_CUR = 0;

    //StringCchCopy( __szWATCH_TAG[__dwWATCH_CUR], szTAG );
    //__dwWATCH_CUR++;
    sc::DebugSet::GetInstance()->LogToFileWithTime(szTAG);
}

namespace
{
    bool g_tologfile = false;
};

void SETMESSAGEBOX ( bool bToLogFile )
{
    g_tologfile = bToLogFile;
}

int MESSAGEBOX( HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType )
{
    if ( g_tologfile )
    {
        return MessageBox( hWnd, lpText, lpCaption, uType );
    }

    if ( lpCaption && lpCaption[0]!=NULL )
    {
        sc::DebugSet::GetInstance()->LogToFileWithTime(
            sc::string::format("%1% : %2%", lpCaption, lpText));
    }
    else
    {
        sc::DebugSet::GetInstance()->LogToFileWithTime(
            sc::string::format("messagebox : %1%", lpText));
    }
    return IDYES;
}