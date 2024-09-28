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
	if ( dwVersion < 0x80000000)// ������ 2000 �̻��϶� �������� ����������
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
	//	ExitWindowsEx API�� ����ϸ� �˴ϴ�. �� �Լ��� ������ ������ �����ϴ�.
	//	BOOL ExitWindowsEx(UINT uFlags, DWORD dwReserved); 
	//	uFlags�� �������� ������ �� �ֽ��ϴ�. ������ ���� ���� �����մϴ�.
	//	EWX_LOGOFF ���� ����ڸ� �α׿����Ѵ�. 
	//	EWX_POWEROFF �ý����� �����ϰ� �Ŀ������Ѵ�. �Ŀ������� �̸� �����ϴ� �ϵ������� �����ϴ�. 
	//	EWX_REBOOT �ý����� �����ϰ� �ý����� ��õ��Ѵ�. 
	//	EWX_SHUTDOWN �ý����� �����Ѵ�. 
	//	EWX_FORCE WM_QUERYSESSION�̳� WM_ENDQUERYSESSION�� ������ �ʰ� �������� ��� ���μ����� �����Ѵ�. 
	//	���� �� ���� �÷��׵�� �Բ� ����� �� �ִ�. 
	//==========================================================================
	ExitWindowsEx(EWX_FORCE | EWX_SHUTDOWN | EWX_POWEROFF,0); 
}