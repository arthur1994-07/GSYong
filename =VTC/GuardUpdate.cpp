#include <windows.h>
#include    <stdio.h>
#include    <stdlib.h>
//#include    <strsafe.h>
#include "Update.h"
#include <wininet.h>

#pragma comment(lib, "Wininet.lib")

#define			BUFFER_LEN				10*1024
#define			SIZE_OF_BUFFER			16000

//#define  _SYS_FILE  "http://vtcguard.a2.vtc.vn/AU2/kvtcguard.sys"
//#define  _TEM_FILE  "http://vtcguard.a2.vtc.vn/AU2/Template.bin"
//#define  _PL_FILE   "http://vtcguard.a2.vtc.vn/AU2/vtguard.pl"
//#define  _VT_FILE   "http://vtcguard.a2.vtc.vn/AU2/vtguard.vt"

//#define  _SYS_FILE  "http://vtcguard.a2.vtc.vn/RAN/kvtcguard.sys"
//#define  _TEM_FILE  "http://vtcguard.a2.vtc.vn/RAN/Template.bin"
//#define  _PL_FILE   "http://vtcguard.a2.vtc.vn/RAN/vtguard.pl"
//#define  _VT_FILE   "http://vtcguard.a2.vtc.vn/RAN/vtguard.vt"

#define  _SYS_FILE  "http://guard.vtc.co.id/ran/kvtcguard.sys"
#define  _TEM_FILE  "http://guard.vtc.co.id/ran/Template.bin"
#define  _PL_FILE   "http://guard.vtc.co.id/ran/vtguard.pl"
#define  _VT_FILE   "http://guard.vtc.co.id/ran/vtguard.vt"


//#define  _SYS_FILE  "http://vtcguard.squad.vtc.vn/SQUAD/kvtcguard.sys"
//#define  _TEM_FILE  "http://vtcguard.squad.vtc.vn/SQUAD/Template.bin"
//#define  _PL_FILE   "http://vtcguard.squad.vtc.vn/SQUAD/vtguard.pl"
//#define  _VT_FILE   "http://vtcguard.squad.vtc.vn/SQUAD/vtguard.vt"

//#define  _SYS_FILE  "http://117.103.194.93/SQUAD/kvtcguard.sys"
//#define  _TEM_FILE  "http://117.103.194.93/SQUAD/Template.bin"
//#define  _PL_FILE   "http://117.103.194.93/SQUAD/vtguard.pl"
//#define  _VT_FILE   "http://117.103.194.93/SQUAD/vtguard.vt"

void strcpyntom(char *des,char *src, int n, int m)
{
	if ((n >=0)&&(m>0) && (m> n))
	{
		for (int i = 0; i<= m -n; i++)
		{
			des[i] = src[n+i];
		}
		des[m-n+1]= '\0';
	}
}

VOID vt_Autoupdate(LPCSTR lpGamePath,LPCSTR Msg)
{
	char sysfile[256] = {0};
	char tempfile[256] = {0};
	char plfile[256] = {0};
	char vtfile[256] = {0};
	sprintf(sysfile,"%s\\kvtcguard.sys",lpGamePath);
	sprintf(tempfile,"%s\\Template.bin",lpGamePath);
	sprintf(plfile,"%s\\vtguard.pl",lpGamePath);
	sprintf(vtfile,"%s\\vtguard.vt",lpGamePath);

	HINTERNET hInternetOpen=NULL;
	hInternetOpen		= InternetOpen("vtcguard", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	//<
	HINTERNET hInternetConnect=NULL;
	int length = 0;
	char buffer[BUFFER_LEN];
	FILE* pfdest = NULL;

	hInternetConnect	= InternetOpenUrl(hInternetOpen,_SYS_FILE,NULL,0,INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0) ;
	pfdest = fopen(sysfile, "wb");
	if(pfdest)
	{
		while (1)
		{
			if (InternetReadFile(hInternetConnect, buffer, BUFFER_LEN, reinterpret_cast<LPDWORD> (&length)) != 0 && length)
			{
				fwrite(buffer, length, 1, pfdest);
			}
			else
			{
				break;
			}
		}
		fclose(pfdest);
	}
	InternetCloseHandle(hInternetConnect);

	//<
	hInternetConnect	= InternetOpenUrl(hInternetOpen,_TEM_FILE,NULL,0,INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0) ;
	pfdest = fopen(tempfile, "wb");
	if(pfdest)
	{
		while (1)
		{
			if (InternetReadFile(hInternetConnect, buffer, BUFFER_LEN, reinterpret_cast<LPDWORD> (&length)) != 0 && length)
			{
				fwrite(buffer, length, 1, pfdest);
			}
			else
			{
				break;
			}
		}
		fclose(pfdest);
	}
	InternetCloseHandle(hInternetConnect);

	//<
	hInternetConnect	= InternetOpenUrl(hInternetOpen,_PL_FILE,NULL,0,INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0) ;
	pfdest = fopen(plfile, "wb");
	if(pfdest)
	{
		while (1)
		{
			if (InternetReadFile(hInternetConnect, buffer, BUFFER_LEN, reinterpret_cast<LPDWORD> (&length)) != 0 && length)
			{
				fwrite(buffer, length, 1, pfdest);
			}
			else
			{
				break;
			}
		}
		fclose(pfdest);
	}
	InternetCloseHandle(hInternetConnect);

	//<
	hInternetConnect	= InternetOpenUrl(hInternetOpen,_VT_FILE,NULL,0,INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0) ;
	pfdest = fopen(vtfile, "wb");
	if(pfdest)
	{
		while (1)
		{
			if (InternetReadFile(hInternetConnect, buffer, BUFFER_LEN, reinterpret_cast<LPDWORD> (&length)) != 0 && length)
			{
				fwrite(buffer, length, 1, pfdest);
			}
			else
			{
				break;
			}
		}
		fclose(pfdest);
	}
	InternetCloseHandle(hInternetConnect);

	InternetCloseHandle(hInternetOpen);

}

VOID vt_run(LPCSTR lpArg,LPCSTR Msg)
{
	if(lpArg)
	{
		//char *szparam = (char *)lpArg;
		//char *sztemp = strchr(szparam,' ');
		//int result1 = (int)(sztemp - szparam-1);

		//char szPathGuard[_MAX_PATH]={0};
		//strcpyntom(szPathGuard,szparam,0,result1);

		char szPathGuard[MAX_PATH]={0},szGuardParam[256]={0},szGameParam[256]={0}; 
		sscanf(lpArg,"%s %s %s",szPathGuard,szGuardParam,szGameParam);

		char dir[_MAX_DIR], drive[_MAX_DRIVE], path[_MAX_PATH];
		_splitpath(szPathGuard, drive, dir, NULL, NULL);
		_makepath(szPathGuard, drive, dir, NULL, NULL);
		strcat(szPathGuard,"vtguard.vt");
		


		OSVERSIONINFO osInfo;
		osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osInfo);

		PROCESS_INFORMATION  m_pProcessInfo; 
		STARTUPINFO si;
		ZeroMemory( &si, sizeof(si) );;
		si.cb = sizeof(si);
		ZeroMemory(&m_pProcessInfo,sizeof(m_pProcessInfo));

		if( !CreateProcess( NULL,		
			szPathGuard,						
			NULL,						
			NULL,						
			FALSE,						
			0,							
			NULL,						
			lpArg,						
			&si,						
			&m_pProcessInfo)						
			) 
		{
			
		}
	}	
}