#include "StdAfx.h"
#include "UIUtility.h"

namespace GUIBase
{

	__declspec(thread) static const int		BUFFER_SIZE = 4096;
	__declspec(thread) static char			g_MbcsBuffer[ BUFFER_SIZE ];
	__declspec(thread) static wchar_t		g_UnicodeBuffer[ BUFFER_SIZE ];

	const char* ToMbcs( const char* psz )
	{
		return psz;
	}

	const char* ToMbcs( const wchar_t* psz )
	{
		g_MbcsBuffer[0] = '\0';
		WideCharToMultiByte(CP_ACP, 0, psz, -1, g_MbcsBuffer, BUFFER_SIZE, 0, 0);
		return g_MbcsBuffer;
	}

	const wchar_t* ToUnicode( const char* psz )
	{
		g_UnicodeBuffer[0] = L'\0';
		MultiByteToWideChar(CP_ACP, 0, psz, -1, g_UnicodeBuffer, BUFFER_SIZE);
		return g_UnicodeBuffer;
	}

	const wchar_t* ToUnicode( const wchar_t* psz )
	{
		return psz;
	}

	const TCHAR* ToTString( const char* psz )
	{
#ifdef _UNICODE
		return ToUnicode( psz );
#else
		return ToMbcs( psz );
#endif
	}

	const TCHAR* ToTString( const wchar_t* psz )
	{
#ifdef _UNICODE
		return ToUnicode( psz );
#else
		return ToMbcs( psz );
#endif
	}

}