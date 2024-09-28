#ifndef SC_SYSTEM_INFO_H_
#define SC_SYSTEM_INFO_H_

#pragma once

#include <string>
#include <psapi.h>
#pragma comment(lib, "Psapi.lib")

namespace sc
{
    /**
    * \ingroup sc
    * \par requirements
    * win98 or later\n
    * win2k or later\n
    * MFC\n
    * \version 1.0
    * first version
    * \date 2002.05.30
    * \author Jgkim
    * \history
    * 2002.05.30 jgkim First Coding
    * 2003.01.18 jgkim Add memory function
    * \note
    * Some of class member functions do not support Windows 95/98/Me.
    * Use Windows 2000 or Higher OS
    * This class need winmm lib, must link with Winmm.lib
    * #pragma comment (lib,"Winmm.lib")
    * #pragma message ("Auto linking Windows mm library")
    */
    enum ENUM_SYSTEMINFO
    {
        MAX_PROCESSOR_NAME = 50,
        MAX_OS_NAME = 50,
    };

    enum EM_MS_OS_TYPE
    {
        MS_OS_WIN32         =  1, // Win32s
        MS_OS_WIN95         =  2, // Windows 95
        MS_OS_WIN95_OSR2    =  3, // Windows 95 OSR2
        MS_OS_WIN98         =  4, // Windows 98
        MS_OS_WIN98_SE      =  5, // Windows 98 SE
        MS_OS_WINME         =  6, // Windows ME
        MS_OS_WINNT         =  7, // Windows NT
        MS_OS_WIN2000       =  8, // Windows 2000
        MS_OS_WINXP         =  9, // Windows XP
        MS_OS_WINXP_PRO_X64 = 10, // Windows XP Professional x64 Edition
        MS_OS_WIN2003       = 11, // Windows Server 2003
        MS_OS_WIN_HOME_SVR  = 12, // Windows Home Server
        MS_OS_WIN2003_R2    = 13, // Windows Server 2003 R2
        MS_OS_WIN_VISTA     = 14, // Windows Vista
        MS_OS_WIN2008       = 15, // Windows Server 2008
        MS_OS_WIN2008_R2    = 16, // Windows Server 2008 R2
        MS_OS_WIN7          = 17, // Windows 7
        MS_OS_WIN8          = 18, // Windows 8
        MS_OS_WIN2012       = 19, // Windows Server 2012
        MS_OS_WIN8_1        = 20, // Windows 8.1
    };
    
    DWORD getProcessorNumber(void);
    DWORD getProcessorType(void);
    WORD  getProcessorFamilyCode(void);
    WORD  getProcessorModelNumber(void);
    WORD  getProcessorSteppingID(void);
    std::string getProcessorName();

    DWORD calculateCpuSpeed();

    DWORD getCpuSpeed(void);
    EM_MS_OS_TYPE getOSType();
    std::string getOSName(EM_MS_OS_TYPE& OsType);
    std::string getOSName();
    int getCurrentDirectory(DWORD nBufferLength, LPTSTR lpBuffer);

    //! 실행파일의 실행경로를 돌려준다.
    std::string getAppPath();
    std::wstring getAppPathW();

    //! Low-fragmentation Heap (LFH) enable.
    //! http://msdn.microsoft.com/en-us/library/aa366750(v=vs.85).aspx
    void setLowFragmentationHeap();

    //! 메모리를 청소한다.
    //! 시스템이 가능한 만큼 큰 메모리를 할당하고
    //! 지워준다.
    void cleanupMemory();
    void heapCompact();
    bool cleanup512M();
    bool cleanup256M();
    bool cleanup128M();

    DWORD determineTimeStamp(void);

    MEMORYSTATUSEX getMemoryStatus();

    //! Percentage of system memory usage
    DWORD getMemoryUsagePercent();

    //! Total bytes of physical memory
    DWORDLONG getMemoryPhysicalTotal();

    //! Total bytes of available physical memory
    DWORDLONG getMemoryPhysicalAvail();

    //! Total bytes of useage physical memory
    DWORDLONG getMemoryPhysicalUse();

    //! Total bytes of virtual memory
    DWORDLONG getMemoryVirtualTotal();

    //! Total bytes of available physical memory
    DWORDLONG getMemoryVirtualAvail();

    //! Total bytes of useage physical memory
    DWORDLONG getMemoryVirtualUse();

    void getProcessMemoryInfo(PROCESS_MEMORY_COUNTERS& MemoryCounter);

    //! MByte
    DWORD getMemoryWorkingSet();

	//! KByte
	DWORD getMemoryWorkingSetKB();

    BOOL systemLogoff();
    BOOL systemShutdown();
    BOOL systemReboot();
    BOOL systemPoweroff();
    BOOL systemDown(int nMethod);

	//! 버전을 가져온다.
	std::string getFileVersion();


	class CSystemInfo
	{
	public:
		CSystemInfo();
		~CSystemInfo();
	public:
		DWORD	GetProcessorNumber(void);
		DWORD	GetProcessorType(void);
		WORD	GetProcessorFamilyCode(void);
		WORD	GetProcessorModelNumber(void);
		WORD	GetProcessorSteppingID(void);
		void	GetProcessorName(char* szPname);

		DWORD	CalculateCpuSpeed();
		
		DWORD	GetCpuSpeed(void);
		void	GetOSName(char* szName);
		int		GetCurrentDirectory(DWORD nBufferLength, LPTSTR lpBuffer);
		static CString GetAppPath();

		DWORD	DetermineTimeStamp(void);

		MEMORYSTATUSEX	GetMemoryStatus();

		/**
		* Percentage of system memory usage
		* \return Percentage
		*/
		DWORD			GetMemoryUsagePercent(); 
		DWORDLONG		GetMemoryPhysicalTotal(); // Total bytes of physical memory
		DWORDLONG		GetMemoryPhysicalAvail(); // Total bytes of available physical memory
		DWORDLONG		GetMemoryPhysicalUse();  // Total bytes of useage physical memory
		DWORDLONG		GetMemoryVirtualTotal(); // Total bytes of virtual memory
		DWORDLONG		GetMemoryVirtualAvail(); // Total bytes of available physical memory
		DWORDLONG		GetMemoryVirtualUse(); // Total bytes of useage physical memory


		BOOL	SystemLogoff();
		BOOL	SystemShutdown();
		BOOL	SystemReboot();
		BOOL	SystemPoweroff();

	protected:
		BOOL	SystemDown(int nMethod);
	};	


} // namespace sc

#endif // SC_SYSTEM_INFO_H_