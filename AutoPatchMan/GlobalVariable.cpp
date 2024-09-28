#include "stdafx.h"
#include "../RanLogic/GLogicDataMan.h"
#include "GlobalVariable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace	NS_GLOBAL_VAR
{	
	UINT	nLOGIN_CONNECT_TIMELEFT = 10;
	int		nLOGIN_TRY = 5;
	UINT	nFTP_CONNECT_TIMELEFT = 6;
	int		nFTP_TRY = 5;
	UINT	nFORCE_EXIT_DELAYTIME = 3;	
	CString	strAppPath;
	CString strProFile;

	EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();

	/*
//#ifdef CH_PARAM
	if (sp == SP_CHINA)
		CString	strTitle = "직槿빻蹈 Launcher";
//#elif TW_PARAM
	else if (sp == SP_TAIWAN)
		CString	strTitle = "쳃 online Launcher";
//#elif HK_PARAM 
	else if (sp == SP_HONGKONG)
		CString	strTitle = "쳃 online Launcher";
//#else 
	else
		CString	strTitle = "Ran Launcher";
//#endif
	*/
	CString strTitle = (sp == SP_CHINA) ? "직槿빻蹈 Launcher" : (sp == SP_TAIWAN) ? "쳃 online Launcher" : (sp == SP_HONGKONG) ? "쳃 online Launcher" : "Ran Launcher";

	CString	strDownloadTemp = "Temp\\";
	CString	strServerCabFileList = "filelist.bin.cab";
	CString	strDownList = "cDownList.bin";
	CString strCopyList = "cCopyList.bin";
	
//	CString strCompDir = "package\\"; // 압축할 파일들의 경로

	TCHAR	g_szPatchMessage[128] = {0};

	/*
	#ifdef KR_PARAM
		TCHAR*	g_szRanFileName	= "RanOnline.exe";
//	#elif defined(CH_PARAM)
//		TCHAR*	g_szRanFileName	= "2046.exe";
	#elif GS_PARAM
		TCHAR*	g_szRanFileName	= "RanOnlineGS.exe";
	#elif _RELEASED
        TCHAR*	g_szRanFileName	= "GameR.exe";
    #else
		TCHAR*	g_szRanFileName	= "Game.exe";
	#endif
	*/
	TCHAR *g_szRanFileName = (sp == SP_KOREA) ? "RanOnline.exe" : (sp == SP_KOREA_TEST) ? "RanOnline.exe" : (sp == SP_CHINA) ? "2046.exe" : (sp == SP_GS) ? "Game.exe" : (sp == SP_OFFICE_TEST) ? "GameR.exe" : "Game.exe";

	TCHAR *g_szPreDownloaderName = "PreDownloader.exe"; 
	BOOL	g_bRunningPreDownloader = FALSE; 
	BOOL	g_bNeedPreDownloader = FALSE; 
	CString	strPreDownloadFolder = "PreDownload\\"; 

	//TCHAR*	g_szAlphaFtpServerIP = "mincontrol.mincoms.co.kr";
	//TCHAR*	g_szAlphaFtpID = "ranalpha";
	//TCHAR*	g_szAlphaFtpPW = "ranalpha";

	//TCHAR*	g_szBetaFtpID = "ranbeta";
	//TCHAR*	g_szBetaFtpPW = "tjdrlduq";	
	
	TCHAR*	g_szServerFileList = "FileList.bin";
	TCHAR*	g_szClientFileList = "cFileList.bin";	
	TCHAR*	g_szPreDownFileList = "preFileList.bin";

	TCHAR*	g_szServerPreDownFileList = "PreDownFileList.bin"; 	// 사전 다운로드 테스트를 위해서 임시로 생성하는 파일. 작업 완료후 코드 정리 필요
	
	DWORD	g_DxVersion = 0x801;	

	TCHAR*	g_szLauncherPatchCabFile = "launcherpatch.exe.cab";
	TCHAR*	g_szLauncherPatchWndName = "Launcher Patch";

	BOOL	g_bApplyPreDown = FALSE;
};