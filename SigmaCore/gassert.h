#ifndef GASSERT_H_
#define GASSERT_H_

#pragma once

#include <boost/static_assert.hpp>
#include "String/StringFormat.h"

#define MIN_STATIC_ASSERT BOOST_STATIC_ASSERT

//! 커스텀 Assert 함수.
bool _gassert(bool bExp, const char *const szExp, const char *const szFile, const long nLine);

#ifdef WIN64
#include <assert.h>
#define MIN_ASSERT(exp) assert(exp)
#else
#define MIN_ASSERT(exp) \
{ \
    if ( _gassert((exp)?true:false, #exp, __FILE__, __LINE__ )) \
{ \
    _asm { int 3 } \
} \
}
#endif

/// GASSERT 동작 안하던 문제 수정;
// NDEBUG 시 GASSERT와 MIN_ASSERT의 동작 방식이 달라서, GASSERT 와 MIN_ASSERT 를 합치지 않음.
#ifndef GASSERT
    #if defined(_DEBUG) || defined(_RELEASED)
		#define GASSERT(exp) { if ( _gassert ( (exp)?true:false, #exp, __FILE__, __LINE__ ) )	{ _asm { int 3 } } }	

		// msg 단순 문자열 혹은 , sc::string::format로 집어 넣는다 
		#define GASSERT_MSG(exp,msg) { if ( _gassert ( (exp)?true:false, (std::string(#exp) + " => " + std::string(msg)).c_str(), __FILE__, __LINE__ ) )	{ _asm { int 3 } } }
	#else
		#define GASSERT(exp)	((void)0)
		#define GASSERT_MSG(exp,msg) ((void)0)
	#endif
#endif


//	Note : 순수 가상 함수 호출 오류 검출.
void smtm_PurecallHandler(void);

//	Note : 런타임 오류 검출.
#include <rtcapi.h>
#include <windows.h>

extern void DoRTC_Initialization (void);
extern void DoRTC_Termination (void);

#ifdef __MSVC_RUNTIME_CHECKS
#ifdef NOCRT
extern TCHAR *IntToString ( int i );
#endif

extern int Catch_RTC_Failure ( int errType, const char *file, int line, const char *module, const char *format, ... );
#endif

class DO_RTC_CATCH
{
public:
    DO_RTC_CATCH()
    {
#ifndef NDEBUG
        DoRTC_Initialization ();
#endif
    }
    ~DO_RTC_CATCH()
    {
#ifndef NDEBUG
        DoRTC_Termination ();
#endif
    }
};

void _LOOPWATCH(const char* szTAG);

void SETMESSAGEBOX(bool bToLogFile);

int MESSAGEBOX(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);

#endif // GASSERT_H_