#include "stdafx.h"
#include "MiniDumpEx.h"
#include "MiniDumpUtil.h"

#include "dbghelp.h"
#pragma comment(lib, "dbghelp.lib")

MiniDumpInitializer g_kMiniDumpInitializer;

LONG WINAPI MiniDumpWriter(PEXCEPTION_POINTERS pException);

MiniDumpInitializer::MiniDumpInitializer() : m_pOldFilter(NULL)
{
	if (m_pOldFilter == NULL)
	{
		m_pOldFilter = SetUnhandledExceptionFilter(MiniDumpWriter);
	}
}

MiniDumpInitializer::~MiniDumpInitializer()
{
	if (m_pOldFilter)
	{
		SetUnhandledExceptionFilter(m_pOldFilter);
	}
}

LONG WINAPI MiniDumpWriter(PEXCEPTION_POINTERS pException)
{
	// 스택이 오버 플로우 되면 스택이 깨진다.
	if (pException->ExceptionRecord->ExceptionCode != EXCEPTION_STACK_OVERFLOW)
	{
		CONTEXT stContext;
		EXCEPTION_RECORD stExRec;
		EXCEPTION_POINTERS stExpPtrs;

		if (pException == NULL)
		{
			SnapCurrentProcess(&stContext);

			// Zero out all the individual values.
			ZeroMemory ( &stExRec, sizeof(EXCEPTION_RECORD) );
			ZeroMemory ( &stExpPtrs, sizeof(EXCEPTION_POINTERS) );

			// warning C4312: 'type cast' : conversion from 'DWORD'
			// to 'PVOID' of greater size
			#pragma warning ( disable : 4312 )
			stExRec.ExceptionAddress = (PVOID)(stContext.Eip) ;
			#pragma warning ( default : 4312 )

			// Set the exception pointers.
			stExpPtrs.ContextRecord = &stContext;
			stExpPtrs.ExceptionRecord = &stExRec ;

			pException = &stExpPtrs;
		}

		MINIDUMP_EXCEPTION_INFORMATION stExInfo;
		PMINIDUMP_EXCEPTION_INFORMATION pExceptionParam;

		// Finally, set up the exception info structure.
		stExInfo.ThreadId = GetCurrentThreadId();
		stExInfo.ClientPointers = TRUE;
		stExInfo.ExceptionPointers = pException;
		pExceptionParam = &stExInfo;

		HANDLE hFile = CreateFile(_T("error.dmp"), GENERIC_WRITE,
			FILE_SHARE_READ, 0, CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);

		if (hFile != INVALID_HANDLE_VALUE)
		{
#ifdef _DEBUG
			MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile,
				MiniDumpWithFullMemory, pExceptionParam, NULL, NULL);
#else
			MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile,
				MiniDumpNormal, pExceptionParam, NULL, NULL);
#endif

			CloseHandle(hFile);
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}
