#include "stdafx.h"
#include <mmsystem.h>
#include <intrin.h>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "../String/StringFormat.h"
#include "../String/StringUtil.h"
#include "../Log/LogMan.h"
#include "./SystemInfo.h"

#include "../DebugInclude.h"

namespace sc
{

DWORD getProcessorNumber(void)
{
    SYSTEM_INFO SystemInfo;	
    GetSystemInfo(&SystemInfo);	
    return SystemInfo.dwNumberOfProcessors;
}

DWORD getProcessorType(void)
{
    SYSTEM_INFO SystemInfo;	
    GetSystemInfo(&SystemInfo);	
    return SystemInfo.dwProcessorType; 
}

WORD getProcessorFamilyCode(void)
{
    SYSTEM_INFO SystemInfo;	
    GetSystemInfo(&SystemInfo);	
    return SystemInfo.wProcessorLevel;
}

WORD getProcessorModelNumber(void)
{
    SYSTEM_INFO SystemInfo;	
    GetSystemInfo(&SystemInfo);	
    return HIBYTE(SystemInfo.wProcessorRevision);
}

WORD getProcessorSteppingID(void)
{
    SYSTEM_INFO SystemInfo;	
    GetSystemInfo(&SystemInfo);		
    return LOBYTE(SystemInfo.wProcessorRevision);
}

// Get CPU name, Support only INTEL CPU
std::string getProcessorName()
{
    SYSTEM_INFO SystemInfo;	
    GetSystemInfo(&SystemInfo);

    std::string strProcessor;

    std::string strTemp;

    //switch (SystemInfo.wProcessorLevel)
    //{
    //case 6 : // Family code 0110 
    //    strTemp = "Pentium III processor";
    //    // Pentium Celeron or Pentium III
    //    switch (HIBYTE(SystemInfo.wProcessorRevision))
    //    {
    //    case 3: // 0011
    //        // Intel Pentium II OverDrive processor
    //        strTemp = "Intel Pentium II OverDrive processor";
    //        break;
    //    case 6: // 0110
    //        // Intel Celeron processor, model 6
    //        strTemp = "Intel Celeron processor, model 6";
    //        break;
    //    case 7: // 0111
    //        // Pentium III processor, model 7, and Pentium III Xeon processor, model 7
    //        strTemp = "Pentium III processor model 7";
    //        break;
    //    case 8: // 1000
    //        // Pentium III processor, model 8, Pentium III Xeon processor, model 8, and intel Celeron processor, model 8
    //        strTemp = "Pentium III processor, model 8";
    //        break;
    //    case 10: // 1010
    //        // Pentium III Xeon processor, model A
    //        strTemp = "Pentium III Xeon processor, model A";
    //        break;
    //    case 11: // 1011
    //        // Pentium III processor, model B
    //        strTemp = "Pentium III processor, model B";
    //        break;		
    //    case 15:			
    //        strTemp = "Intel Core2 processor";
    //        break;
    //    case 23:
    //        strTemp = "Intel Core2 Quad processor";
    //        break;
    //    default:
    //        strTemp = "Unknown Processor";
    //        break;
    //    }
    //    break;
    //case 15: // Family code 1111
    //    // Pentium 4 processor, Mobile Intel Pentium 4 processor, 
    //    strTemp = "Pentium 4 processor";
    //    break;
    //default:
    //    strTemp = "Unknown Processor";
    //    break;
    //}

    {
        // Get extended ids.
        int CPUInfo[4] = {-1};
        __cpuid(CPUInfo, 0x80000000);
        unsigned int nExIds = CPUInfo[0];

        // Get the information associated with each extended ID.
        char CPUBrandString[0x40] = { 0 };
        for( unsigned int i=0x80000000; i<=nExIds; ++i)
        {
            __cpuid(CPUInfo, i);

            // Interpret CPU brand string and cache information.
            if  (i == 0x80000002)
            {
                memcpy( CPUBrandString,
                    CPUInfo,
                    sizeof(CPUInfo));
            }
            else if( i == 0x80000003 )
            {
                memcpy( CPUBrandString + 16,
                    CPUInfo,
                    sizeof(CPUInfo));
            }
            else if( i == 0x80000004 )
            {
                memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
            }
        }

        strTemp = std::string(CPUBrandString);
        string::trim(strTemp);
    }

    strProcessor = string::format(
        "%1% Level(%2%) Revision(%3%)",
        strTemp,
        SystemInfo.wProcessorLevel,
        (WORD) HIBYTE(SystemInfo.wProcessorRevision));
    return strProcessor;
}

EM_MS_OS_TYPE getOSType()
{
    EM_MS_OS_TYPE OsType;
    getOSName(OsType);
    return OsType;
}

std::string getOSName()
{
    EM_MS_OS_TYPE Type = getOSType();
    return getOSName(Type);
}

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);

// 참고
// http://msdn2.microsoft.com/en-us/library/ms724429.aspx
// http://msdn.microsoft.com/en-us/library/ms724833(VS.85).aspx
std::string getOSName(EM_MS_OS_TYPE& OsType)
{
    std::string strOsName("UnKnown");
    OSVERSIONINFOEX OSInfo;
    SYSTEM_INFO si;
    PGNSI pGNSI;	

    ZeroMemory(&OSInfo, sizeof(OSInfo));
    ZeroMemory(&si, sizeof(SYSTEM_INFO));

    // Set size
    OSInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);	

    if (GetVersionEx((OSVERSIONINFO*) &OSInfo) == FALSE)
    {
        OSInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        if (!GetVersionEx((OSVERSIONINFO*) &OSInfo))
            return strOsName;
    }	

    // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
    pGNSI = (PGNSI) GetProcAddress(
        GetModuleHandle(TEXT("kernel32.dl")),
        "GetNativeSystemInfo");
    if ( NULL != pGNSI )
        pGNSI(&si);
    else
        GetSystemInfo(&si);

    // Windows 8.1                         6.3 6.3 OSVERIONINFOEX.wProductType == VER_NT_WORKSTATION 
    // Windows 8                           6.2 6 2 OSVERIONINFOEX.wProductType == VER_NT_WORKSTATION 
    // Windows Server 2012                 6.2 6 2 OSVERIONINFOEX.wProductType != VER_NT_WORKSTATION 
    // Windows 7                           6.1 6 1 OSVERIONINFOEX.wProductType == VER_NT_WORKSTATION 
    // Windows Server 2008 R2              6.1 6 1 OSVERIONINFOEX.wProductType != VER_NT_WORKSTATION 
    // Windows Server 2008                 6.0 6 0 OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION 
    // Windows Vista                       6.0 6 0 OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION 
    // Windows Server 2003 R2              5.2 5 2 GetSystemMetrics(SM_SERVERR2) != 0 
    // Windows Home Server                 5.2 5 2 OSVERSIONINFOEX.wSuiteMask == VER_SUITE_WH_SERVER 
    // Windows Server 2003                 5.2 5 2 GetSystemMetrics(SM_SERVERR2) == 0 
    // Windows XP Professional x64 Edition 5.2 5 2 (OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION) && (SYSTEM_INFO.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64) 
    // Windows XP                          5.1 5 1 Not applicable 
    // Windows 2000                        5.0 5 0 Not applicable 
    switch ( OSInfo.dwPlatformId )
    {
    case VER_PLATFORM_WIN32_NT:
        if ( OSInfo.dwMajorVersion == 6 && OSInfo.dwMinorVersion == 3 && OSInfo.wProductType == VER_NT_WORKSTATION ) {
            strOsName = "Microsoft Windows 8.1";
            OsType = MS_OS_WIN8;
        }

        if ( OSInfo.dwMajorVersion == 6 && OSInfo.dwMinorVersion == 2 && OSInfo.wProductType == VER_NT_WORKSTATION ) {
            strOsName = "Microsoft Windows 8";
            OsType = MS_OS_WIN8;
        }

        if ( OSInfo.dwMajorVersion == 6 && OSInfo.dwMinorVersion == 2 && OSInfo.wProductType != VER_NT_WORKSTATION ) {
            strOsName = "Microsoft Windows Server 2012";
            OsType = MS_OS_WIN2012;
        }

        if ( OSInfo.dwMajorVersion == 6 && OSInfo.dwMinorVersion == 1 && OSInfo.wProductType == VER_NT_WORKSTATION ) {
            strOsName = "Microsoft Windows 7";
            OsType = MS_OS_WIN7;
        }

        if ( OSInfo.dwMajorVersion == 6 && OSInfo.dwMinorVersion == 1 && OSInfo.wProductType != VER_NT_WORKSTATION ) {
            strOsName = "Microsoft Windows Server 2008 R2";
            OsType = MS_OS_WIN2008_R2;
        }

        if ( OSInfo.dwMajorVersion == 6 && OSInfo.dwMinorVersion == 0 && OSInfo.wProductType == VER_NT_WORKSTATION ) {
            strOsName = "Microsoft Windows Vista";
            OsType = MS_OS_WIN_VISTA;
        }

        if ( OSInfo.dwMajorVersion == 6 && OSInfo.dwMinorVersion == 0 && OSInfo.wProductType != VER_NT_WORKSTATION ) {
            strOsName = "Microsoft Windows Server 2008";
            OsType = MS_OS_WIN2008;
        }

        if ((OSInfo.dwMajorVersion == 5) && (OSInfo.dwMinorVersion == 2))
        {
            // SM_SERVERR2 89
            if ( GetSystemMetrics(89) )
            {
                strOsName = "Microsoft Windows 2003 R2";
                OsType = MS_OS_WIN2003_R2;
            }
            else if ( OSInfo.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
            {
                strOsName = "Microsoft Windows XP Professional x64 Edition ";
                OsType = MS_OS_WINXP_PRO_X64;
            }
            else
            {
                strOsName = "Microsoft Windows 2003";
                OsType = MS_OS_WIN2003;
            }
        }

        if ( OSInfo.dwMajorVersion == 5 && OSInfo.dwMinorVersion == 1 ) {
            strOsName = "Microsoft Windows XP";
            OsType = MS_OS_WINXP;
        }

        if ( OSInfo.dwMajorVersion == 5 && OSInfo.dwMinorVersion == 0 ) {
            strOsName = "Microsoft Windows 2000";
            OsType = MS_OS_WIN2000;
        }

        if ( OSInfo.dwMajorVersion <= 4 ) {
            strOsName = "Microsoft Windows NT";
            OsType = MS_OS_WINNT;
        }

        break;

    case VER_PLATFORM_WIN32_WINDOWS:
        if ( OSInfo.dwMajorVersion == 4 && !OSInfo.dwMinorVersion )
        {		
            if ( OSInfo.szCSDVersion[1] == 'C' ) {
                strOsName = "Microsoft Windows 95 OSR2";
                OsType = MS_OS_WIN95_OSR2;
            } else {
                strOsName = "Microsoft Windows 95";
                OsType = MS_OS_WIN95;
            }
        }
        if ( OSInfo.dwMajorVersion == 4 && OSInfo.dwMinorVersion == 10 )
        {
            if ( OSInfo.szCSDVersion[1] == 'A' ) {
                strOsName = "Microsoft Windows 98 SE";
                OsType = MS_OS_WIN98_SE;
            } else {
                strOsName = "Microsoft Windows 98";
                OsType = MS_OS_WIN98;
            }
        }
        if ( OSInfo.dwMajorVersion == 4 && OSInfo.dwMinorVersion == 90 ) {
            strOsName = "Microsoft Windows ME";
            OsType = MS_OS_WINME;
        }
        break;

    case VER_PLATFORM_WIN32s:
        strOsName = "Microsoft Win32s";
        OsType = MS_OS_WIN32;
        break;
    }

    return strOsName;
}

DWORD getCpuSpeed(void)
{
    const int MAX_TRIES = 10;
    DWORD dwSpeed = 0;
    DWORD dwTestValue = UINT_MAX;
    int   nNumTimes = 0;

    DWORD dwStartingPriority = GetPriorityClass(GetCurrentProcess());
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    //--------------------------------------------------------
    // Find the first two similarities up to ten times.
    // This should be a maximum of five seconds.
    // 
    while ((dwSpeed != dwTestValue) && (nNumTimes < MAX_TRIES))
    {
        dwTestValue = dwSpeed;
        dwSpeed = calculateCpuSpeed();
        nNumTimes++;
    }	
    SetPriorityClass(GetCurrentProcess(), dwStartingPriority);
    return dwSpeed;
}

// Stolen from AMD
DWORD determineTimeStamp(void)
{
#ifdef WIN64
    return GetTickCount();
#else
    DWORD dwTickVal;
    __asm
    {
        _emit 0Fh
            _emit 31h
            mov   dwTickVal, eax
    }
    return (dwTickVal);
#endif
}

DWORD calculateCpuSpeed()
{
    int   nTimeStart = 0;
    int   nTimeStop = 0;
    DWORD dwStartTicks = 0;
    DWORD dwEndTicks = 0;
    DWORD dwTotalTicks = 0;
    DWORD dwCpuSpeed = 0;

    nTimeStart = timeGetTime();
    for (;;)
    {
        nTimeStop = timeGetTime();
        if ((nTimeStop - nTimeStart) > 1)
        {
            dwStartTicks = determineTimeStamp();
            break;
        }
    }
    nTimeStart = nTimeStop;
    for (;;)
    {
        nTimeStop = timeGetTime();
        if ((nTimeStop - nTimeStart) > 500)    // one-half second
        {
            dwEndTicks = determineTimeStamp();
            break;
        }
    }
    dwTotalTicks = dwEndTicks - dwStartTicks;
    dwCpuSpeed = dwTotalTicks / 500000;
    return (dwCpuSpeed);
}

// Retrieves the current directory for the current process
// DWORD nBufferLength : size of directory buffer
// LPTSTR lpBuffer : directory buffer
int getCurrentDirectory(DWORD nBufferLength, LPTSTR lpBuffer)
{
    if (::GetCurrentDirectory(nBufferLength, lpBuffer) == 0)
        return -1;
    return 0;
}

//! ex> c:\\temp 만 돌려줌 뒤에 \\ 없음
//! 중간에 공백이 있을때 인수가 있을때 c:\\temp\\a b\\c.exe a b c 동작해야 한다.
std::string getAppPath()
{

	TCHAR path[_MAX_PATH];

	GetModuleFileName(NULL, path, sizeof path);



	CString strPath = path;

	int i = strPath.ReverseFind('\\');//실행 파일 이름을 지우기 위해서 왼쪽에 있는 '/'를 찾는다.

	strPath = strPath.Left(i);//뒤에 있는 현재 실행 파일 이름을 지운다.

	return std::string(strPath.GetString());



    CString strFullPath;
    CString strCommandLine = GetCommandLine();

    if (!strCommandLine.IsEmpty())
    {
        std::string CommandLine(strCommandLine.GetString());
        std::vector<std::string> SplitVector;
        boost::split(SplitVector, CommandLine, boost::algorithm::is_any_of(" "));
        size_t FindPos=0;
        for (size_t i=0; i<SplitVector.size(); ++i)
        {
            std::transform(SplitVector[i].begin(), SplitVector[i].end(), SplitVector[i].begin(), towlower);
            if (std::string::npos != SplitVector[i].find(".exe"))
            {
                FindPos = i;
                break;
            }
        }

        strCommandLine = "";
        for (size_t i=0; i<=FindPos; ++i)
        {
            strCommandLine.Append(SplitVector[i].c_str());
            strCommandLine.Append(" ");
        }

        DWORD dwFind = strCommandLine.ReverseFind('\\');
        if (dwFind != -1)
        {
            strFullPath = strCommandLine.Left(dwFind);

            if (!strFullPath.IsEmpty())
                if (strFullPath.GetAt(0) == '"')
                    strFullPath = strFullPath.Right(strFullPath.GetLength() - 1);
        }
    }

    if (strFullPath.GetLength() < 4)
    {
        char szTest[MAX_PATH] = {0};
        GetCurrentDirectoryA(MAX_PATH, szTest);
        std::string strTest(szTest);
        return strTest;
    }
    else
    {
        return std::string(strFullPath.GetString());
    }
}

//! ex> c:\\temp 만 돌려줌 뒤에 \\ 없음
//! 중간에 공백이 있을때 인수가 있을때 c:\\temp\\a b\\c.exe a b c 동작해야 한다.
std::wstring getAppPathW()
{
    CStringW strFullPath;
    CStringW strCommandLine = GetCommandLineW();

    if (!strCommandLine.IsEmpty())
    {
        std::wstring CommandLine(strCommandLine.GetString());
        std::vector<std::wstring> SplitVector;
        boost::split(SplitVector, CommandLine, boost::algorithm::is_any_of(L" "));
        size_t FindPos=0;
        for (size_t i=0; i<SplitVector.size(); ++i)
        {
            std::transform(SplitVector[i].begin(), SplitVector[i].end(), SplitVector[i].begin(), towlower);
            if (std::wstring::npos != SplitVector[i].find(L".exe"))
            {
                FindPos = i;
                break;
            }
        }

        strCommandLine = L"";
        for (size_t i=0; i<=FindPos; ++i)
        {
            strCommandLine.Append(SplitVector[i].c_str());
            strCommandLine.Append(L" ");
        }

        DWORD dwFind = strCommandLine.ReverseFind(L'\\');
        if (dwFind != -1)
        {
            strFullPath = strCommandLine.Left(dwFind);

            if (!strFullPath.IsEmpty())
                if (strFullPath.GetAt(0) == '"')
                    strFullPath = strFullPath.Right(strFullPath.GetLength() - 1);
        }
    }

    if (strFullPath.GetLength() < 4)
    {
        wchar_t szTest[MAX_PATH] = {0};
        GetCurrentDirectoryW(MAX_PATH, szTest);
        std::wstring strTest(szTest);
        return strTest;
    }
    else
    {
        return std::wstring(strFullPath.GetString());
    }
}

MEMORYSTATUSEX getMemoryStatus()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);
    return statex;
}

DWORD getMemoryUsagePercent()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);
    return statex.dwMemoryLoad;
}

DWORDLONG getMemoryPhysicalTotal()
{
    MEMORYSTATUSEX statex;	
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);
    return statex.ullTotalPhys; 
}

DWORDLONG getMemoryPhysicalAvail()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);
    return statex.ullAvailPhys; 
}

DWORDLONG getMemoryPhysicalUse()
{
    MEMORYSTATUSEX statex;	
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);
    return (statex.ullTotalPhys - statex.ullAvailPhys);
}

DWORDLONG getMemoryVirtualTotal()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);
    return statex.ullTotalVirtual;	
}

DWORDLONG getMemoryVirtualAvail()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);
    return statex.ullAvailVirtual;
}

DWORDLONG getMemoryVirtualUse()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);
    return (statex.ullTotalVirtual - statex.ullAvailVirtual);
}

void getProcessMemoryInfo( PROCESS_MEMORY_COUNTERS& MemoryCounter )
{	
    HANDLE hProcess = GetCurrentProcess();
    GetProcessMemoryInfo( hProcess, &MemoryCounter, sizeof(MemoryCounter) );	
}

DWORD getMemoryWorkingSet()
{	
    PROCESS_MEMORY_COUNTERS MemoryCounter;
    getProcessMemoryInfo( MemoryCounter );
    return (DWORD) ( MemoryCounter.WorkingSetSize/(1024*1024) );
}

DWORD getMemoryWorkingSetKB()
{
	PROCESS_MEMORY_COUNTERS MemoryCounter;
	getProcessMemoryInfo( MemoryCounter );
	return (DWORD)(MemoryCounter.WorkingSetSize / 1024);
}

//void GetMemoryUsage()
//{
//	MEMORYSTATUSEX statex;
//	int nDIV = 1024;
//	wchar_t *divisor = "K");
//	// Handle the width of the field in which to print numbers this way to
//	// make changes easier. The asterisk in the print format specifier
//	// "%*I64d" takes an int from the argument list, and uses it to pad 
//	// and right-justify the number being formatted.
//	int nWIDTH = 7;
//
//	statex.dwLength = sizeof (statex);
//
//	GlobalMemoryStatusEx (&statex);
//
//	printf ("%ld percent of memory is in use.\n"),
//			statex.dwMemoryLoad);
//	printf ("There are %*I64d total %sbytes of physical memory.\n"),
//			nWIDTH, statex.ullTotalPhys/nDIV, divisor);
//	printf ("There are %*I64d free %sbytes of physical memory.\n"),
//			nWIDTH, statex.ullAvailPhys/nDIV, divisor);
//	printf ("There are %*I64d total %sbytes of paging file.\n"),
//			nWIDTH, statex.ullTotalPageFile/nDIV, divisor);
//	printf ("There are %*I64d free %sbytes of paging file.\n"),
//			nWIDTH, statex.ullAvailPageFile/nDIV, divisor);
//	printf ("There are %*I64x total %sbytes of virtual memory.\n"),
//			nWIDTH, statex.ullTotalVirtual/nDIV, divisor);
//	printf ("There are %*I64x free %sbytes of virtual memory.\n"),
//			nWIDTH, statex.ullAvailVirtual/nDIV, divisor);
//	// Show the amount of extended memory available.
//	printf ("There are %*I64x free %sbytes of extended memory.\n"),
//			nWIDTH, statex.ullAvailExtendedVirtual/nDIV, divisor);
//}
BOOL systemDown(int nMethod)
{
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&os);
    if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {			
        if (os.dwMajorVersion == 4) // Microsoft Windows NT
        {
        }				
        if ((os.dwMajorVersion == 5) && (!os.dwMinorVersion)) // Microsoft Windows 2000
        {
        }				
        if ((os.dwMajorVersion == 5) && (os.dwMinorVersion == 1)) // Microsoft Windows XP
        {
        }				
    }
    else if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) // 95, 98, ME
    {
    }
    else
    {
    }
    return ExitWindowsEx(nMethod ,EWX_FORCE);
}

BOOL systemLogoff()
{
    return systemDown(EWX_LOGOFF);
}

BOOL systemShutdown()
{
    return systemDown(EWX_SHUTDOWN);		
}

BOOL systemReboot()
{	
    return systemDown(EWX_REBOOT);
}

BOOL systemPoweroff()
{
    return systemDown(EWX_POWEROFF);
}

//! Low-fragmentation Heap (LFH) enable.
//! http://msdn.microsoft.com/en-us/library/aa366750(v=vs.85).aspx
//! +------------------------+-----------+
//! |     OS                 | 지원여부 |
//! +------------------------+-----------+
//! | WindowsXP SP2 .Net 3.0 |     X     |
//! +------------------------+-----------+
//! | Windows Vista          |     O     |
//! +------------------------+-----------+
//! | Windows 2000           |     ?     |
//! +------------------------+-----------+
//! | Windows 2003           |     ?     |
//! +------------------------+-----------+
//! | Windows Longhorn       |     ?     |
//! +------------------------+-----------+
void setLowFragmentationHeap()
{
	const ULONG HEAP_STANDARD = 0;
	const ULONG HEAP_LAL = 1;
	const ULONG HEAP_LFH = 2;

	HANDLE hHeap[1025];
	UINT uEA = GetProcessHeaps( 1024, hHeap );

	// query before heap type
	for ( UINT i=0; i<uEA; ++i )
	{
		BOOL bResult;
		ULONG HeapInformation;

		bResult = HeapQueryInformation( hHeap[i], HeapCompatibilityInformation, &HeapInformation, sizeof( HeapInformation ), NULL );
		if ( bResult == FALSE )
		{
			sc::writeLogWarn( sc::string::format( "Failed to retrieve heap features with i: %1% error: %2%.", i, GetLastError() ) );
			continue;
		}

		//
		// Print results of the query.
		//
 		sc::writeLogInfo( sc::string::format( "HeapCompatibilityInformation is %1% %2%.", i, HeapInformation ) );
 		switch( HeapInformation )
 		{
 		case HEAP_STANDARD:
 			sc::writeLogInfo( "This heap is a standard heap." );
 			break;
 		case HEAP_LAL:
 			sc::writeLogInfo( "This heap supports look-aside lists." );
 			break;
 		case HEAP_LFH:
 			sc::writeLogInfo( "This heap has the low-fragmentation heap enabled." );
 			break;
 		default:
 			sc::writeLogInfo( "Unrecognized HeapInformation reported for the default process heap." );
 			break;
 		}

		ULONG HeapFragValue = 2;
		if ( HeapSetInformation( hHeap[i], HeapCompatibilityInformation, &HeapFragValue, sizeof( HeapFragValue ) ) )
		{
			sc::writeLogInfo( sc::string::format( "Low Fragmentation Heap enabled. i: %1%",  i ) );
		}
		else
		{
			sc::writeLogWarn( sc::string::format( "Low Fragmentation Heap failed. i: %1%, error: %2%",  i, GetLastError() ) );
		}
	}
}

//! 메모리를 청소한다.
//! 시스템이 가능한 만큼 큰 메모리를 할당하고
//! 지워준다.
void cleanupMemory()
{
    // 	if (true == cleanup512M()) return;
    // 	if (true == cleanup256M()) return;
    // 	if (true == cleanup128M()) return;
}

bool cleanup512M()
{
    try
    {
        BYTE* pData = new BYTE[1024*1024*512];
        ::memset( pData, 0, 100 );
        delete[] pData;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool cleanup256M()
{
    try
    {
        BYTE* pData = new BYTE[1024*1024*256];
        ::memset( pData, 0, 100 );
        delete[] pData;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool cleanup128M()
{
    try
    {
        BYTE* pData = new BYTE[1024*1024*128];
        ::memset( pData, 0, 100 );
        delete[] pData;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

void heapCompact()
{
	HANDLE hHeap[1025] = { 0,  };
	UINT uEA = GetProcessHeaps( 1024, hHeap );
	size_t nReturnedTotal = 0;
	for ( UINT i=0; i<uEA; ++i )
	{
		if ( NULL == hHeap[i] )
		{
			continue;
		}

		size_t nReturned = HeapCompact( hHeap[i], 0 );
		nReturnedTotal += nReturned;
		if ( 0 == nReturned )
		{
			DWORD dwError = GetLastError();
			if ( NO_ERROR == dwError )
			{
				sc::writeLogError( "HeapCompact absolutely no space available in the heap." );
			}
			else
			{
				sc::writeLogWarn( sc::string::format( "HeapCompact returned error %1%", dwError ) );
			}
		}
	}

	sc::writeLogInfo( sc::string::format( "HeapCompact returned %1% bytes", nReturnedTotal ) );
}


std::string getFileVersion()
{
	TCHAR szImagePath[MAX_PATH];

	GetModuleFileNameEx(GetCurrentProcess(), NULL, 
		szImagePath, (sizeof(szImagePath)/sizeof(TCHAR)));

	std::string strResult("0.0.0.0");
	std::string strFilePath = szImagePath;

	DWORD dwSize    = 0;
	DWORD dwFVISize = GetFileVersionInfoSize( strFilePath.c_str() , &dwSize );

	UINT uLen = 0;
	LPBYTE pByte = new BYTE[dwFVISize];
	VS_FIXEDFILEINFO* pvffi = NULL; 

	if( GetFileVersionInfo(strFilePath.c_str(), NULL, dwFVISize, pByte) == FALSE 
		|| VerQueryValue(pByte, "\\", (LPVOID*)&pvffi, &uLen) == FALSE ) 
	{ 
		goto _getFileVersion_end;
		//return strResult;
	} 

	if ( !pvffi )
	{
		goto _getFileVersion_end;
		//return strResult;
	}

	DWORD dwFileVersionMS = pvffi->dwFileVersionMS;
	DWORD dwFileVersionLS = pvffi->dwFileVersionLS;

	strResult = sc::string::format("%1%.%2%.%3%.%4%", 
		HIWORD(dwFileVersionMS), 
		LOWORD(dwFileVersionMS),
		HIWORD(dwFileVersionLS),
		LOWORD(dwFileVersionLS)
	);

_getFileVersion_end:

	SAFE_DELETE_ARRAY(pByte);

	return strResult;
}


CSystemInfo::CSystemInfo()
{
}
CSystemInfo::~CSystemInfo()
{
}

DWORD CSystemInfo::GetProcessorNumber(void)
{
	SYSTEM_INFO SystemInfo;	
	GetSystemInfo(&SystemInfo);	
	return SystemInfo.dwNumberOfProcessors;
}

DWORD CSystemInfo::GetProcessorType(void)
{
	SYSTEM_INFO SystemInfo;	
	GetSystemInfo(&SystemInfo);	
	return SystemInfo.dwProcessorType; 
}

WORD CSystemInfo::GetProcessorFamilyCode(void)
{
	SYSTEM_INFO SystemInfo;	
	GetSystemInfo(&SystemInfo);	
	return SystemInfo.wProcessorLevel;
}

WORD CSystemInfo::GetProcessorModelNumber(void)
{
	SYSTEM_INFO SystemInfo;	
	GetSystemInfo(&SystemInfo);	
	return HIBYTE(SystemInfo.wProcessorRevision);
}

WORD CSystemInfo::GetProcessorSteppingID(void)
{
	SYSTEM_INFO SystemInfo;	
	GetSystemInfo(&SystemInfo);		
	return LOBYTE(SystemInfo.wProcessorRevision);
}

// Get CPU name, Support only INTEL CPU
void CSystemInfo::GetProcessorName(char* szPname)
{
	SYSTEM_INFO SystemInfo;	
	GetSystemInfo(&SystemInfo);

	switch(SystemInfo.wProcessorLevel)
	{
	case 6 : // Family code 0110 
		StringCchCopy(szPname, MAX_PROCESSOR_NAME, _T("Pentium III processor"));
		// Pentium Celeron or Pentium III
		switch(HIBYTE(SystemInfo.wProcessorRevision))
		{
		case 6 : // 0110
			// Intel Celeron processor, model 6
			StringCchCopy(szPname, MAX_PROCESSOR_NAME, _T("Intel Celeron processor, model 6"));
			break;
		case 7 : // 0111
			// Pentium III processor, model 7, and Pentium III Xeon processor, model 7
			StringCchCopy(szPname, MAX_PROCESSOR_NAME, _T("Pentium III processor model 7"));
			break;
		case 8 : // 1000
			// Pentium III processor, model 8, Pentium III Xeon processor, model 8, and intel Celeron processor, model 8
			StringCchCopy(szPname, MAX_PROCESSOR_NAME, _T("Pentium III processor, model 8"));
			break;
		case 10 : // 1010
			// Pentium III Xeon processor, model A
			StringCchCopy(szPname, MAX_PROCESSOR_NAME, _T("Pentium III Xeon processor, model A"));
			break;
		case 11 : // 1011
			// Pentium III processor, model B
			StringCchCopy(szPname, MAX_PROCESSOR_NAME, _T("Pentium III processor, model B"));
			break;
		case 3 : // 0011
			// Intel Pentium II OverDrive processor
			StringCchCopy(szPname, MAX_PROCESSOR_NAME, _T("Intel Pentium II OverDrive processor"));
			break;
		default :
			break;
		}
		break;
	case 15 : // Family code 1111
		// Pentium 4 processor, Mobile Intel Pentium 4 processor, 
		StringCchCopy(szPname, MAX_PROCESSOR_NAME, _T("Pentium 4 processor"));
		break;
	default :
		StringCchCopy(szPname, MAX_PROCESSOR_NAME, _T("Unknown Processor"));
		break;
	}
}

void CSystemInfo::GetOSName(char* szName)
{
	OSVERSIONINFO OSInfo;
	memset(&OSInfo, 0, sizeof(OSInfo));

	// Set size
	OSInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	StringCchCopy(szName, MAX_OS_NAME, _T("Unknown OS"));

	if (GetVersionEx((OSVERSIONINFO *) &OSInfo) == FALSE)
	{			
		return;
	}

	switch(OSInfo.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		// 2008 서버
		if ((OSInfo.dwMajorVersion == 6) && (OSInfo.dwMinorVersion == 0))
			StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Windows 2008 Server"));
		// Microsoft Windows 2003
		if ((OSInfo.dwMajorVersion == 5) && (OSInfo.dwMinorVersion == 2))
			StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Windows 2003"));

		// Microsoft Windows XP
		if ((OSInfo.dwMajorVersion == 5) && (OSInfo.dwMinorVersion == 1))
			StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Windows XP"));

		// Microsoft Windows 2000
		if ((OSInfo.dwMajorVersion == 5) && (OSInfo.dwMinorVersion == 0))
			StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Windows 2000"));

		// Microsoft Windows NT
		if (OSInfo.dwMajorVersion <= 4)
			StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Windows NT"));
		break;

	case VER_PLATFORM_WIN32_WINDOWS:
		if((OSInfo.dwMajorVersion == 4) && (!OSInfo.dwMinorVersion)) 
		{		
			// Microsoft Windows 95 OSR2
			if(OSInfo.szCSDVersion[1] == 'C') 
			{
				StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Windows 95 OSR2"));
			}
			// Microsoft Windows 95
			else
			{
				StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Windows 95"));
			}			
		}
		if((OSInfo.dwMajorVersion == 4) && (OSInfo.dwMinorVersion == 10))
		{
			// Microsoft Windows 98 SE
			if(OSInfo.szCSDVersion[1] == 'A')
			{
				StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Windows 98 SE"));
			}			
			// Microsoft Windows 98
			else
			{
				StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Windows 98"));
			}
		}
		// Microsoft Windows ME
		if((OSInfo.dwMajorVersion == 4) && (OSInfo.dwMinorVersion == 90))
			StringCchCopy(szName, MAX_OS_NAME, "Microsoft Windows ME");
		break;
	case VER_PLATFORM_WIN32s:
		// Microsoft Win32s
		StringCchCopy(szName, MAX_OS_NAME, _T("Microsoft Win32s"));
		break;
	}
}

#ifndef UINT_MAX
#define UINT_MAX 0xffffffff
#endif

DWORD CSystemInfo::GetCpuSpeed(void)
{
	const int MAX_TRIES = 10;
	DWORD dwSpeed = 0;
	DWORD dwTestValue = UINT_MAX;
	int   nNumTimes = 0;

	DWORD dwStartingPriority = GetPriorityClass(GetCurrentProcess());
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	//--------------------------------------------------------
	// Find the first two similarities up to ten times.
	// This should be a maximum of five seconds.
	// 
	while ((dwSpeed != dwTestValue) && (nNumTimes < MAX_TRIES))
	{
		dwTestValue = dwSpeed;
		dwSpeed = CalculateCpuSpeed();
		nNumTimes++;
	}	
	SetPriorityClass(GetCurrentProcess(), dwStartingPriority);
	return dwSpeed;
}

// Stolen from AMD
DWORD CSystemInfo::DetermineTimeStamp(void)
{
	DWORD dwTickVal;
	__asm
	{
		_emit 0Fh
		_emit 31h
		mov   dwTickVal, eax
	}
	return (dwTickVal);
}

DWORD CSystemInfo::CalculateCpuSpeed()
{
	int   nTimeStart = 0;
	int   nTimeStop = 0;
	DWORD dwStartTicks = 0;
	DWORD dwEndTicks = 0;
	DWORD dwTotalTicks = 0;
	DWORD dwCpuSpeed = 0;

	nTimeStart = timeGetTime();
	for (;;)
	{
		nTimeStop = timeGetTime();
		if ((nTimeStop - nTimeStart) > 1)
		{
			dwStartTicks = DetermineTimeStamp();
			break;
		}
	}
	nTimeStart = nTimeStop;
	for (;;)
	{
		nTimeStop = timeGetTime();
		if ((nTimeStop - nTimeStart) > 500)    // one-half second
		{
			dwEndTicks = DetermineTimeStamp();
			break;
		}
	}
	dwTotalTicks = dwEndTicks - dwStartTicks;
	dwCpuSpeed = dwTotalTicks / 500000;
	return (dwCpuSpeed);
}

// Retrieves the current directory for the current process
// DWORD nBufferLength : size of directory buffer
// LPTSTR lpBuffer : directory buffer
int CSystemInfo::GetCurrentDirectory(DWORD nBufferLength, LPTSTR lpBuffer)
{
	if (::GetCurrentDirectory(nBufferLength, lpBuffer) == 0)
		return -1;
	return 0;
}

CString	CSystemInfo::GetAppPath ()
{
	CString strFullPath;
	CString strCommandLine;

	TCHAR szPath[MAX_PATH] = {0};
	GetModuleFileName(::AfxGetInstanceHandle(), szPath, MAX_PATH);
	strCommandLine = szPath;

	if ( !strCommandLine.IsEmpty() )
	{
		DWORD dwFind = strCommandLine.ReverseFind ( '\\' );
		if ( dwFind != -1 )
		{
			strFullPath = strCommandLine.Left ( dwFind );
			
			if ( !strFullPath.IsEmpty() )
			if ( strFullPath.GetAt(0) == '"' )
				strFullPath = strFullPath.Right ( strFullPath.GetLength() - 1 );
		}
	}
	
	if (strFullPath.GetLength() < 4)
	{
		TCHAR szTest[MAX_PATH] = {0};
		::GetCurrentDirectory(MAX_PATH, szTest);		
		CString strTest(szTest);
		return strTest;
	}
	else
	{
		return strFullPath;
	}
}

MEMORYSTATUSEX CSystemInfo::GetMemoryStatus()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	return statex;
}

DWORD CSystemInfo::GetMemoryUsagePercent()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	return statex.dwMemoryLoad;
}

DWORDLONG CSystemInfo::GetMemoryPhysicalTotal()
{
	MEMORYSTATUSEX statex;	
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	return statex.ullTotalPhys; 
}

DWORDLONG CSystemInfo::GetMemoryPhysicalAvail()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	return statex.ullAvailPhys; 
}

DWORDLONG CSystemInfo::GetMemoryPhysicalUse()
{
	MEMORYSTATUSEX statex;	
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	return (statex.ullTotalPhys - statex.ullAvailPhys);
}

DWORDLONG CSystemInfo::GetMemoryVirtualTotal()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	return statex.ullTotalVirtual;	
}

DWORDLONG CSystemInfo::GetMemoryVirtualAvail()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	return statex.ullAvailVirtual;
}

DWORDLONG CSystemInfo::GetMemoryVirtualUse()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	return (statex.ullTotalVirtual - statex.ullAvailVirtual);
}

//void CSystemInfo::GetMemoryUsage()
//{
//	MEMORYSTATUSEX statex;
//	int nDIV = 1024;
//	char *divisor = "K";
//	// Handle the width of the field in which to print numbers this way to
//	// make changes easier. The asterisk in the print format specifier
//	// "%*I64d" takes an int from the argument list, and uses it to pad 
//	// and right-justify the number being formatted.
//	int nWIDTH = 7;
//
//	statex.dwLength = sizeof (statex);
//
//	GlobalMemoryStatusEx (&statex);
//
//	printf ("%ld percent of memory is in use.\n",
//			statex.dwMemoryLoad);
//	printf ("There are %*I64d total %sbytes of physical memory.\n",
//			nWIDTH, statex.ullTotalPhys/nDIV, divisor);
//	printf ("There are %*I64d free %sbytes of physical memory.\n",
//			nWIDTH, statex.ullAvailPhys/nDIV, divisor);
//	printf ("There are %*I64d total %sbytes of paging file.\n",
//			nWIDTH, statex.ullTotalPageFile/nDIV, divisor);
//	printf ("There are %*I64d free %sbytes of paging file.\n",
//			nWIDTH, statex.ullAvailPageFile/nDIV, divisor);
//	printf ("There are %*I64x total %sbytes of virtual memory.\n",
//			nWIDTH, statex.ullTotalVirtual/nDIV, divisor);
//	printf ("There are %*I64x free %sbytes of virtual memory.\n",
//			nWIDTH, statex.ullAvailVirtual/nDIV, divisor);
//	// Show the amount of extended memory available.
//	printf ("There are %*I64x free %sbytes of extended memory.\n",
//			nWIDTH, statex.ullAvailExtendedVirtual/nDIV, divisor);
//}
BOOL CSystemInfo::SystemDown(int nMethod)
{
	OSVERSIONINFO os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&os);
	if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{			
		if (os.dwMajorVersion == 4) // Microsoft Windows NT
		{
		}				
		if ((os.dwMajorVersion == 5) && (!os.dwMinorVersion)) // Microsoft Windows 2000
		{
		}				
		if ((os.dwMajorVersion == 5) && (os.dwMinorVersion == 1)) // Microsoft Windows XP
		{
		}				
	}
	else if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) // 95, 98, ME
	{
	}
	else
	{
	}
	return ExitWindowsEx(nMethod ,EWX_FORCE);
}

BOOL CSystemInfo::SystemLogoff()
{
	return SystemDown(EWX_LOGOFF);
}

BOOL CSystemInfo::SystemShutdown()
{
	return SystemDown(EWX_SHUTDOWN);		
}

BOOL CSystemInfo::SystemReboot()
{	
	return SystemDown(EWX_REBOOT);
}

BOOL CSystemInfo::SystemPoweroff()
{
	return SystemDown(EWX_POWEROFF);
}

} // namespace sc