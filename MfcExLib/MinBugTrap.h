#ifndef _MIN_BUG_TRAP_H_
#define _MIN_BUG_TRAP_H_

#pragma once

#include <string>
#include <Process.h>
#include "../EngineLib/ServiceProviderDefine.h"

namespace BUG_TRAP
{
	//! BugTrap �� Exception handler �� ��ġ�Ѵ�.
	//! strAppName : Application Name
	//! bUseEmail : e-mail �������� ���� ���ΰ�?
	//! bUserScreenCapture : Screen Capture �� ÷���ؼ� ���� ���ΰ�?
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