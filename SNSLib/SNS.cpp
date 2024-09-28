#include "stdafx.h"
#include <iostream>
#include "SNS.h"
#include "../=cURL/include/curl/curl.h"

#include <ShlObj.h>
#include <winInet.h>

#pragma warning(disable: 4995)



namespace SNS {
	std::string	exWhat;	//XML exception message: ex.what()

	BOOL bInitialized;
}


//
//initSNS() MUST be called once before using of SNS.
//
int SNS::initSNS()
{
	CURLcode cc = curl_global_init(CURL_GLOBAL_ALL);

    if (cc == CURLE_OK)
		SNS::bInitialized = TRUE;

	return (int)cc;
}

//
//cleanSNS() MUST be called once after you finished use of SNS.
//
void SNS::cleanSNS()
{
	SNS::bInitialized = FALSE;

	curl_global_cleanup();
}




//
//mjeon.sns
// the url MUST start with protocol type such like "http://facebook.com"
//
BOOL SNS::BypassCookie(std::string &url)
{
#ifndef INTERNET_COOKIE_HTTPONLY
#define INTERNET_COOKIE_HTTPONLY      0x00002000
#endif

	BOOL bDone = FALSE;
	
	CString fb_url(url.c_str()); //_T("http://www.facebook.com");
	DWORD cookie_size = 0;
	
	InternetGetCookieEx( fb_url, NULL, NULL, &cookie_size, INTERNET_COOKIE_HTTPONLY, NULL );
	//InternetGetCookie( fb_url, NULL, NULL, &cookie_size);//, INTERNET_COOKIE_HTTPONLY, NULL );

	CString buf;
	if (cookie_size)
	{	
		InternetGetCookieEx( fb_url, NULL, buf.GetBufferSetLength( cookie_size ), &cookie_size, INTERNET_COOKIE_HTTPONLY, NULL );
		//InternetGetCookie( fb_url, NULL, buf.GetBufferSetLength( cookie_size ), &cookie_size);//, INTERNET_COOKIE_HTTPONLY, NULL );

		CString cur_name;
		CString c_data;

		int size = buf.GetLength();
		for (int i = 0; i < size; i++)
		{
			if ( buf[i] != '=' )
			{
				if (buf[i] != ' ')
					cur_name+= buf[i];
				
			}
			else
			{
				break;
			}
		}

		//COleDateTime dt(2008, 9, 9, 16,0,0);
		//SYSTEMTIME st;
		//dt.GetAsSystemTime( st );
		//InternetTimeFromSystemTime( &st, INTERNET_RFC1123_FORMAT, date.GetBufferSetLength( MAX_PATH ), MAX_PATH );

		//c_data.Format( _T("%s = 0; expires = %Sat,01-Jan-2000 00:00:00 GMT; path=/; domain=.facebook.com; httponly"), cur_name );
		c_data.Format( _T("%s = 0; expires = %Sat,01-Jan-2000 00:00:00 GMT; path=/; domain=.facebook.com"), cur_name );
        
		HRESULT hr = E_FAIL;
		
		hr = InternetSetCookieEx( fb_url, NULL, c_data, INTERNET_COOKIE_HTTPONLY,  NULL );
		//hr = InternetSetCookie( fb_url, NULL, c_data );// INTERNET_COOKIE_HTTPONLY,  NULL );
		bDone = TRUE;
	}

	return bDone;
}


BOOL SNS::DeleteCookie()
{
	TCHAR szDir[MAX_PATH];
	ZeroMemory(szDir, sizeof(szDir));

	BOOL bDeleted = FALSE;

	SHGetSpecialFolderPath(NULL, szDir, CSIDL_COOKIES, FALSE);

	WIN32_FIND_DATA ffd;
	//LARGE_INTEGER filesize;

	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError=0;

	// Find the first file in the directory.
	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		return dwError;
	}

	// List all the files in the directory with some info about them.
	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			continue;
		}
		else	//Files
		{
			//CString filename = CString(ffd.cFileName);
			//filename.Find(

			TCHAR *p = NULL;
			p = _tcsstr(ffd.cFileName, _T("facebook"));

			if (p == NULL)
				continue;
			else
			{
				//DeleteFile(

				size_t nLen = _tcslen(szDir);

				if ( szDir[nLen-1] != _T('\\') )
				{
					szDir[nLen] = _T('\\');
					szDir[nLen+1] = _T('\0');
				}

				TCHAR szPath[MAX_PATH];
				ZeroMemory(szPath, sizeof(szPath));

				_stprintf(szPath, _T("%s%s"), szDir, ffd.cFileName);

				bDeleted = DeleteFile(szPath);

				break;
			}


			//filesize.LowPart = ffd.nFileSizeLow;
			//filesize.HighPart = ffd.nFileSizeHigh;
		}
	}
	while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();

	if (dwError != ERROR_NO_MORE_FILES)
	{
		;
	}

	FindClose(hFind);

	return bDeleted;
}

