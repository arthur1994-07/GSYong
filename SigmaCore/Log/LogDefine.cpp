#include "stdafx.h"
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 ) 
#include "./LogDefine.h"

#include "../DebugInclude.h"

namespace sc
{

//! Example:const wchar_t* pStr = getLogStrCode( LOG_DEBUG );
#ifdef UNICODE
const wchar_t* getLogStrCode(EM_LOG_LEVEL emLevel)
{
	return g_LogLevelStrTable[emLevel];
}
#else
const char* getLogStrCode(EM_LOG_LEVEL emLevel)
{
    return g_LogLevelStrTable[emLevel];
}
#endif

void initLogStrCode(void)
{
#ifdef UNICODE
	StringCchCopyW(g_LogLevelStrTable[0], 10, L"[DEBUG]");
	StringCchCopyW(g_LogLevelStrTable[1], 10, L"[WARN ]");
	StringCchCopyW(g_LogLevelStrTable[2], 10, L"[INFO ]");
	StringCchCopyW(g_LogLevelStrTable[3], 10, L"[ERROR]");	
	StringCchCopyW(g_LogLevelStrTable[4], 10, L"[FATAL]");
#else
    StringCchCopyA(g_LogLevelStrTable[0], 10, "[DEBUG]");
    StringCchCopyA(g_LogLevelStrTable[1], 10, "[WARN ]");
    StringCchCopyA(g_LogLevelStrTable[2], 10, "[INFO ]");
    StringCchCopyA(g_LogLevelStrTable[3], 10, "[ERROR]");	
    StringCchCopyA(g_LogLevelStrTable[4], 10, "[FATAL]");
#endif
}

} // namespace sc