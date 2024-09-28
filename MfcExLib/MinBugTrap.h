#ifndef _MIN_BUG_TRAP_H_
#define _MIN_BUG_TRAP_H_

#pragma once

#include <string>
#include <Process.h>
#include "../EngineLib/ServiceProviderDefine.h"

namespace BUG_TRAP
{
	//! BugTrap 용 Exception handler 를 설치한다.
	//! strAppName : Application Name
	//! bUseEmail : e-mail 리포팅을 받을 것인가?
	//! bUserScreenCapture : Screen Capture 를 첨부해서 받을 것인가?
	void BugTrapInstall(
        EMSERVICE_PROVIDER Type,
        const std::string& strAppName,
        bool bUseEmail = true,
        bool bUseScreenCapture = true, 
		bool bServer = false );

}; // End of namespace CORE_COMMON

unsigned __stdcall CreateOutofMemoryLog( void* pArguments );
void New_OutOfMemory_Handler();
void AddOutofMemoryLogHeader( DWORD dwCurrentThreadId );
void GetOutofMemoryLogPath( TCHAR* pPathName );

#endif // _MIN_BUG_TRAP_H_