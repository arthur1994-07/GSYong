#include "StdAfx.h"
#include ".\Protection.h"
//#include "GlobalDefine.h"
//#include "DEBUG_NEW.h"


CProtection::CProtection(void)
{
	SecureZeroMemory(&CONTEXTDebug, sizeof(CONTEXT));
	SecureZeroMemory(&DR_7, sizeof(DebugReg7*));

	CONTEXTDebug.ContextFlags = CONTEXT_DEBUG_REGISTERS;
}

CProtection::~CProtection(void)
{
}


void CProtection::Init()
{
}

BOOL CProtection::Pesudo_IsDebuggerPresent()
{
	BOOL Retval = 0;
	__asm
	{
		push eax
		mov eax,dword ptr fs:[0x18]
		mov eax,dword ptr ds:[eax+0x30]
		movzx eax,byte ptr ds:[eax+0x2]
		mov Retval,eax
		pop eax
	}
	return Retval;
}

void CProtection::WindowsSyStemCompulsoryDown()
{
	DWORD dwVersion = GetVersion();
	if ( dwVersion < 0x80000000)// 윈도우 2000 이상일때 설정해줄 프리빌리지
	{
		HANDLE hToken;
		TOKEN_PRIVILEGES TokenPrivileges;

		OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &TokenPrivileges.Privileges[0].Luid);

		TokenPrivileges.PrivilegeCount = 1;
		TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, 0, (PTOKEN_PRIVILEGES)NULL, 0);
	}
	//==========================================================================
	//	ExitWindowsEx API를 사용하면 됩니다. 이 함수의 원형은 다음과 같습니다.
	//	BOOL ExitWindowsEx(UINT uFlags, DWORD dwReserved); 
	//	uFlags로 종료방법을 지정할 수 있습니다. 다음과 같은 값이 가능합니다.
	//	EWX_LOGOFF 현재 사용자를 로그오프한다. 
	//	EWX_POWEROFF 시스템을 종료하고 파워오프한다. 파워오프는 이를 지원하는 하드웨어에서만 가능하다. 
	//	EWX_REBOOT 시스템을 종료하고 시스템을 재시동한다. 
	//	EWX_SHUTDOWN 시스템을 종료한다. 
	//	EWX_FORCE WM_QUERYSESSION이나 WM_ENDQUERYSESSION을 보내지 않고 실행중인 모든 프로세스를 종료한다. 
	//	위의 네 가지 플래그들과 함께 사용할 수 있다. 
	//==========================================================================
	ExitWindowsEx(EWX_FORCE | EWX_SHUTDOWN | EWX_POWEROFF,0); 
}