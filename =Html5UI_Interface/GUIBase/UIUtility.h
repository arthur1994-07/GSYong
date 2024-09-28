#pragma once

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif

#ifdef UNICODE
#define TSTRING		  std::wstring
#define TCERR		  std::wcerr
#define TOFSTREAM	  std::wofstream
#define TFSTREAM	  std::wfstream
#define TSTRINGSTREAM std::wstringstream
#else
#define TSTRING		  std::string
#define TCERR		  std::cerr
#define TOFSTREAM	  std::ofstream
#define TSTREAM		  std::fstream
#define TSTRINGSTREAM std::stringstream
#endif

namespace GUIBase
{
	const char* ToMbcs( const char* psz );
	const char* ToMbcs( const wchar_t* psz );
	const wchar_t* ToUnicode( const char* psz );
	const wchar_t* ToUnicode( const wchar_t* psz );
	const TCHAR* ToTString( const char* psz );
	const TCHAR* ToTString( const wchar_t* psz );
}