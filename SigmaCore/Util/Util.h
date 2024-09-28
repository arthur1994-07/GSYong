#ifndef _CORE_COMMON_UTIL_H_
#define _CORE_COMMON_UTIL_H_

#pragma once

#include <string>
#include "../Memory/SmartPtr.h"
//! Util.h
//!
//! * History
//! 2002.05.30 Jgkim Create
//!
//! Copyright (c) Min Communications. All rights reserved.

// SigmaCore 다른 곳에서도 사용한다면 stdafx.h 넣어도 될것임.
#ifdef UNICODE
#define TSTRING		std::wstring
#define TCERR		std::wcerr
#define TOFSTREAM	std::wofstream
#define TFSTREAM	std::wfstream
#define TSTRINGSTREAM	std::wstringstream
#else
#define TSTRING		std::string
#define TCERR		std::cerr
#define TOFSTREAM	std::ofstream
#define TSTREAM		std::fstream
#define TSTRINGSTREAM	std::stringstream
#endif

namespace sc
{
    namespace util
    {
        //! int 형의 랜덤한 숫자를 발생시킨다.
        //! 숫자는 1 부터 nMax 까지 발생시킨다.
        //! 주의:0 은 발생시키지 않는다.
        int makeRandomNumber(int nMax);

        //! 현재 실행중인 모든 윈도우 창을 minimize 한다.
        //! Windows Key + M 을 누른 효과를 낸다.
        void minimizeAllWindow();

        //! 현재 실행중인 모든 윈도우 창을 트레이로 내리거나 복구한다.
        void trayAllWindow( bool bMinimize );

        template<class T>
        bool ToBool(const T& value) 
        {
            return value ? true : false;
        }

        //! 2007-04-03 Jgkim
        //! DumpMini.dmp 파일을 생성한다.
        void makeMiniDump();

        //! 2007-04-03 Jgkim
        //! DumpFull.dmp 메모리 전체 정보를 dump 한다.
        //! 주의:용량이 크고 실행시간이 오래걸린다.
        void makeFullDump();

		//	Note : 원본이름 뒤에 이름을 더 넣는다.
		TSTRING	GetSpecularName( const TSTRING& strSrcName, const TSTRING& strAddName );
		TSTRING	ChangeExtName( const TSTRING& strSrcName, const TSTRING& strExtName );
		TSTRING	GetName_ExtErase( const TSTRING& strSrcName );					// 리턴값 : 확장자와 . 을 없앤것
		TSTRING	GetChangeTCHAR( const TSTRING& strSrcName, const TCHAR cErase, const TCHAR cNew );
		TSTRING	GetChangeTCHAR( const TSTRING& strSrcName, const TCHAR cErase );
		TSTRING	GetChange3LastTCHAR( const TSTRING& strSrcName, const TSTRING& strChange );
       
    } // namespace util
} // namespace sc

#endif // _CORE_COMMON_UTIL_H_
