#include "stdafx.h"
#include <DbgHelp.h>
#include <fstream>
#include "../../SigmaCore/gassert.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../BugTrap/BugTrap.h"
#include "./EHStackWalker.h"
#include "./MinBugTrap.h"
#include "../EngineLib/common/BugTrapStr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void __cdecl OutOfMemoryHandler()
{
	RaiseException( STATUS_NO_MEMORY, 0, 0, NULL );
}

void WINAPI PostErrHandler( INT_PTR );

using namespace BUG_TRAP;

// 20160720 ���� ������� �ʴ´�.
//! Invalid parameter �� ��κ��� ������
//! vector size �� 3 �ε� 4 ��° ���Ҹ� �����Ѵٴ��� �ϴ�
//! ������ �ʰ��ؼ� �＼�� �ϴ� ����̴�.
void MinInvalidParameterHandler( 
	const wchar_t* expression, 
	const wchar_t* function, 
	const wchar_t* file, 
	unsigned int line, 
	uintptr_t pReserved )
{
    AfxMessageBox( "1234" );
	MIN_ASSERT( 0 && _T("MinInvalidParameterHandler") );
	// �� �κп��� invalid parameter �� �߻��� �κ���
	// �Լ����� ������ ���� �� �ִ�.
	// ������ BugTrap �� ����ϱ� ���ؼ� MIN_ASSERT �� ȣ���ϰ�
	// ���α׷��� ����, ����Ʈ���� ����.	
	/*
	wprintf(L"Invalid parameter detected in function %s."
		L" File: %s Line: %d\n", function, file, line);
	wprintf(L"Expression: %s\n", expression);
	*/
}

// 20160720 ���� ������� �ʴ´�.
//! ���Ȼ��� ������ vs2005 ���� checked iterators �� Ȱ��ȭ �Ǿ��ִ�.
//! http://msdn2.microsoft.com/en-us/library/aa985965(VS.80).aspx
//! http://msdn2.microsoft.com/en-us/library/a9yf33zb(VS.80).aspx
//! http://msdn2.microsoft.com/en-us/library/aa985974(VS.80).aspx
//! ���� �߸��� ������ �����϶� _SCL_SECURE_VALIDATE_RANGE ��ũ�θ� ���ؼ�
//! invalid_parameter �� ȣ��ǰ� Dr.Watson �� ����ǰ� �ȴ�.
//! �츮�� Dr.Watson �� ������� �ʰ� BugTrap �� ����Ǳ⸦ ���Ѵ�.
//! ���� _SCL_SECURE_VALIDATE_RANGE �� ��ȿȭ�ؾ� �Ѵ�.
//! �̸� ���ؼ� �츮�� ������ _set_invalid_parameter_handler �� ��ġ�ϰ�
//! �̸� ���ؼ� BugTrap �� �����Ѵ�.
//! 2007-11-09 Jgkim
void SetMinInvalidParameterHandler()
{
	_invalid_parameter_handler oldHandler, newHandler;
	newHandler = MinInvalidParameterHandler;
	oldHandler = _set_invalid_parameter_handler( newHandler );

	// Disable the message box for assertions.
	_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_WNDW | _CRTDBG_MODE_DEBUG );
}

LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter( LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter )
{
    return NULL;
}

BOOL PreventSetUnhandledExceptionFilter()
{
    HMODULE hKernel32 = LoadLibrary( _T( "kernel32.dll" ) );
    if ( hKernel32 == NULL )
		return FALSE;
    void* pOrgEntry = GetProcAddress( hKernel32, "SetUnhandledExceptionFilter" );
    if ( pOrgEntry == NULL )
		return FALSE;
    unsigned char newJump[ 100 ];
    DWORD dwOrgEntryAddr = ( DWORD ) pOrgEntry;
    dwOrgEntryAddr += 5; // add 5 for 5 op-codes for jmp far
    void* pNewFunc = &MyDummySetUnhandledExceptionFilter;
    DWORD dwNewEntryAddr = ( DWORD ) pNewFunc;
    DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;

    newJump[ 0 ] = 0xE9;  // JMP absolute
    memcpy( &newJump[ 1 ], &dwRelativeAddr, sizeof( pNewFunc ) );
    SIZE_T bytesWritten;
    BOOL bRet = WriteProcessMemory( GetCurrentProcess(), 
		pOrgEntry, newJump, sizeof( pNewFunc ) + 1, &bytesWritten );
    return bRet;
}

void BUG_TRAP::BugTrapInstall( EMSERVICE_PROVIDER Type, const std::string& strAppName, bool bUseEmail, bool bUseScreenCapture, bool bServer )
{
#ifndef _DEBUG
	/*SetMinInvalidParameterHandler();*/

	BT_InterceptSUEF( NULL, TRUE );

	// Setup exception handler
	BT_InstallSehFilter();

	std::string strApp = strAppName;
	strApp += BUG_TRAP::Country( Type );
	
	BT_SetAppName( strApp.c_str() );

	// e-mail ������ ���� ���ΰ�?
	if ( true == bUseEmail )
	{
		BT_SetSupportEMail( BUG_TRAP::Email( Type ).c_str() );
	}
	
	// ��ũ�� ĸ�ĸ� ����� ������ ����
	// ��� longjump ���ܰ� ����Ʈ���� �ɸ��� �ʾƼ� BTF_INTERCEPTSUEF �÷��� �߰�, BT_InterceptSUEF �� ��ü, BTF_INTERCEPTSUEF �������δ� �������� ����
	// BT_InterceptSUEF �� ������
	if ( true == bUseScreenCapture )
	{
		BT_SetFlags( BTF_DETAILEDMODE | BTF_EDITMAIL | BTF_ATTACHREPORT | BTF_SCREENCAPTURE );
	}
	else
	{
		BT_SetFlags( BTF_DETAILEDMODE | BTF_EDITMAIL | BTF_ATTACHREPORT );
	}

	/*BT_InterceptSUEF( GetModuleHandle( NULL ), TRUE );*/

	/*
	������ �⺻������ full-dump �� ���⵵�� �Ѵ�.(kr, gs�� full-dump)
	�׿ܿ��� ���� MiniDumpNormal ���� MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory �ɼ����� ���� �ɼ� ����
	GS, KR
	MiniDumpWithFullMemory
	save
	��������
	MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory
	save, summit
	Ŭ���̾�Ʈ�� ���� �״���̰� MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory �̰Ÿ�
	������ ui ���� �����ϰ� ��ġ���Ϸ� ������ϵ��� �Ѵ�.
	���� 5����(����, ĳ��, �ʵ�, ������Ʈ, �α���)�� bServer �� true �� �Ѿ�´�.
	����Ʈ�� ���ۿɼ��� ������ ����, ���� ������ �� �� ����.
	�׷��� ����Ʈ�� �ڵ带 �����ؼ� ����Ѵ�. ���̺굵 �ؾ� �ϰ�, ���۵� �ؾ� �ϴ� ��Ȳ�� �ֱ� �����̴�.
	����Ʈ�� ������ �ö󰣴ٸ� ���⵵ �Բ� �����ؾ� �Ѵ�.
	// Display BugTrap dialog to allow user selecting desirable
	// option. This is the default option.
	BTA_SHOWUI       = 1,

	// Automatically save error report to file.
	// Use BT_SetReportFilePath() to specify report path.
	BTA_SAVEREPORT   = 2,

	// Automatically send error report by e-mail.
	BTA_MAILREPORT   = 3,

	// Automatically send bug report to support server.
	BTA_SENDREPORT   = 4
	*/
	if ( bServer )
	{
		// ������ ���� �ڵ� ���� ����� ����ϱ� ������ ���� ��ġ ���� �ʼ�
		// ���� ���� ��ġ
		TSTRING DumpPath = sc::getAppPath();
		DumpPath += "\\dump";
		BT_SetReportFilePath( DumpPath.c_str() );

		switch ( Type )
		{
		case SP_KOREA:
		case SP_GS:
			{
				BT_SetActivityType( BTA_SENDREPORT );
				BT_SetDumpType( MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory );
				/*
				BT_SetActivityType( BTA_SAVEREPORT );
				BT_SetDumpType( MiniDumpWithFullMemory );
				*/
			}
			break;
		default:
			{
				BT_SetActivityType( BTA_SENDREPORT );
				BT_SetDumpType( MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory );
			}
			break;
		}

		// ������ ����Ʈ�� ���� ���� �� �����Ŀ� ������ ���� �����Ѵ�. restart.bat ������ �����ؼ� ����۽�Ų��.
		// �� �������� restart.bat �ʿ�
		BT_SetPostErrHandler( PostErrHandler, 0 );

		// ����� �ڵ鷯 ������ ���������� new handler ���
		std::set_new_handler( New_OutOfMemory_Handler );
	}
	else
	{
		BT_SetActivityType( BTA_SHOWUI );
		BT_SetDumpType( MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory );
	}

	BT_SetSupportURL( BUG_TRAP::SupportUrl( Type ).c_str() );

	// = BugTrapServer ===========================================
	BT_SetSupportServer( BUG_TRAP::ServerAddress( Type ).c_str(), BUG_TRAP::Port( Type ) );

	/*SetUnhandledExceptionFilter( MyUnhandledExceptionFilter );*/

	// ����Ʈ�� BT_InstallSehFilter ���� �� �ڵ� �ø��� ����Ʈ�� �������� �ʴ´�. BT_InterceptSUEF �� ���� ������ �ϴ� ���̱� ������
	// PreventSetUnhandledExceptionFilter �� �ּ�ó���Ѵ�.
    /*BOOL bRet = PreventSetUnhandledExceptionFilter();*/
#endif
}

void WINAPI PostErrHandler( INT_PTR )
{
	TCHAR szCmdApp[512];
	if( GetSystemDirectory( szCmdApp, MAX_PATH ) != 0 )
	{
		StringCchCat( szCmdApp, MAX_PATH, TEXT("\\cmd.exe"));
		TSTRING strPath( szCmdApp );
		strPath.append( " /k " );
		strPath.append( sc::getAppPath() );
		strPath.append( "\\restart.bat" );
		WinExec( strPath.c_str(), SW_SHOWNORMAL );
	}
}

void New_OutOfMemory_Handler()
{
	DWORD dwCurrentThreadId = GetCurrentThreadId();
	AddOutofMemoryLogHeader( dwCurrentThreadId );
	HANDLE hCurrentThread = OpenThread( THREAD_GET_CONTEXT, FALSE, dwCurrentThreadId );
	HANDLE hThread = ( HANDLE )_beginthreadex( 0, NULL, CreateOutofMemoryLog, &hCurrentThread, 0, NULL );
	WaitForSingleObject( hThread, INFINITE );
	CloseHandle( hThread );
	PostErrHandler( NULL );
	exit( 0 );
}

void AddOutofMemoryLogHeader( DWORD dwCurrentThreadId )
{
	wprintf( L"Out of Memory %d\n", dwCurrentThreadId );
	TCHAR szCurrentPath[MAX_PATH] = { 0, };
	GetOutofMemoryLogPath( szCurrentPath );

	std::wofstream outFile( szCurrentPath, std::ios::app );
	if ( !outFile.good() )
	{
		return;
	}

	SYSTEMTIME sysTime;
	GetLocalTime( &sysTime );

	outFile << "OccurTime " << sysTime.wYear << '/'<< sysTime.wMonth << '/'
		<< sysTime.wDay << ' ' << sysTime.wHour << ':' << sysTime.wMinute 
		<< ':' << sysTime.wSecond << std::endl;

	outFile << "Out Of Memory. Thread ID : " << dwCurrentThreadId << std::endl;
}

void GetOutofMemoryLogPath( TCHAR* pPathName )
{
	TCHAR szCurDirectory[MAX_PATH] = { 0, };
	TCHAR* pEnd = NULL;
	GetModuleFileName( NULL, szCurDirectory, MAX_PATH );
	pEnd = _tcsrchr( szCurDirectory, _T( '\\' ) );

	if ( NULL == pEnd )
	{
		return;
	}

	*pEnd = _T( '\0' );
	pEnd = pEnd + 1;

	_tcscpy_s( pPathName, MAX_PATH-1, szCurDirectory );
	_tcscat_s( pPathName,  MAX_PATH-1, TEXT( "\\OutofMemoryLog.txt" ) );
}

unsigned __stdcall CreateOutofMemoryLog( void* pArguments )
{
	HANDLE hThread = ( *( HANDLE* ) pArguments );

	CEHStackWalker EHStackWalker;
	EHStackWalker.SetLogFilePath();
	EHStackWalker.ShowCallstack( hThread );
	_endthreadex( 0 );
	return 0;
}