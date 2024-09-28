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

// 20160720 현재 사용하지 않는다.
//! Invalid parameter 의 대부분의 에러는
//! vector size 가 3 인데 4 번째 원소를 참조한다던지 하는
//! 범위를 초과해서 억세스 하는 경우이다.
void MinInvalidParameterHandler( 
	const wchar_t* expression, 
	const wchar_t* function, 
	const wchar_t* file, 
	unsigned int line, 
	uintptr_t pReserved )
{
    AfxMessageBox( "1234" );
	MIN_ASSERT( 0 && _T("MinInvalidParameterHandler") );
	// 이 부분에서 invalid parameter 가 발생한 부분의
	// 함수명과 라인을 얻을 수 있다.
	// 하지만 BugTrap 을 사용하기 위해서 MIN_ASSERT 를 호출하고
	// 프로그램을 종료, 버그트랩을 띄운다.	
	/*
	wprintf(L"Invalid parameter detected in function %s."
		L" File: %s Line: %d\n", function, file, line);
	wprintf(L"Expression: %s\n", expression);
	*/
}

// 20160720 현재 사용하지 않는다.
//! 보안상의 이유로 vs2005 에서 checked iterators 가 활성화 되어있다.
//! http://msdn2.microsoft.com/en-us/library/aa985965(VS.80).aspx
//! http://msdn2.microsoft.com/en-us/library/a9yf33zb(VS.80).aspx
//! http://msdn2.microsoft.com/en-us/library/aa985974(VS.80).aspx
//! 따라서 잘못된 범위의 접근일때 _SCL_SECURE_VALIDATE_RANGE 매크로를 통해서
//! invalid_parameter 가 호출되고 Dr.Watson 이 실행되게 된다.
//! 우리는 Dr.Watson 이 실행되지 않고 BugTrap 이 실행되기를 원한다.
//! 따라서 _SCL_SECURE_VALIDATE_RANGE 를 무효화해야 한다.
//! 이를 위해서 우리가 제작한 _set_invalid_parameter_handler 를 설치하고
//! 이를 통해서 BugTrap 을 실행한다.
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

	// e-mail 보고를 받을 것인가?
	if ( true == bUseEmail )
	{
		BT_SetSupportEMail( BUG_TRAP::Email( Type ).c_str() );
	}
	
	// 스크린 캡쳐를 사용할 것이지 설정
	// 루아 longjump 예외가 버그트랩에 걸리지 않아서 BTF_INTERCEPTSUEF 플래그 추가, BT_InterceptSUEF 로 교체, BTF_INTERCEPTSUEF 설정으로는 동작하지 않음
	// BT_InterceptSUEF 는 동작함
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
	서버는 기본적으로 full-dump 를 남기도록 한다.(kr, gs만 full-dump)
	그외에는 기존 MiniDumpNormal 에서 MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory 옵션으로 덤프 옵션 변경
	GS, KR
	MiniDumpWithFullMemory
	save
	나머지는
	MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory
	save, summit
	클라이언트는 기존 그대로이고 MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory 이거만
	서버는 ui 없이 종료하고 배치파일로 재시작하도록 한다.
	서버 5종류(세션, 캐시, 필드, 에이전트, 로그인)만 bServer 값 true 로 넘어온다.
	버그트랩 동작옵션은 다음과 같고, 복수 선택을 할 수 없다.
	그래서 버그트랩 코드를 수정해서 사용한다. 세이브도 해야 하고, 전송도 해야 하는 상황이 있기 때문이다.
	버그트랩 버전이 올라간다면 여기도 함께 수정해야 한다.
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
		// 서버는 덤프 자동 저장 기능을 사용하기 때문에 저장 위치 지정 필수
		// 덤프 저장 위치
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

		// 서버만 버그트랩 덤프 저장 및 전송후에 동작할 동작 설정한다. restart.bat 파일을 실행해서 재시작시킨다.
		// 각 서버마다 restart.bat 필요
		BT_SetPostErrHandler( PostErrHandler, 0 );

		// 재시작 핸들러 때문에 서버에서만 new handler 등록
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

	// 버그트랩 BT_InstallSehFilter 위로 이 코드 올리면 버그트랩 동작하지 않는다. BT_InterceptSUEF 이 같은 동작을 하는 것이기 때문에
	// PreventSetUnhandledExceptionFilter 는 주석처리한다.
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
