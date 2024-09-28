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

// SigmaCore �ٸ� �������� ����Ѵٸ� stdafx.h �־ �ɰ���.
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
        //! int ���� ������ ���ڸ� �߻���Ų��.
        //! ���ڴ� 1 ���� nMax ���� �߻���Ų��.
        //! ����:0 �� �߻���Ű�� �ʴ´�.
        int makeRandomNumber(int nMax);

        //! ���� �������� ��� ������ â�� minimize �Ѵ�.
        //! Windows Key + M �� ���� ȿ���� ����.
        void minimizeAllWindow();

        //! ���� �������� ��� ������ â�� Ʈ���̷� �����ų� �����Ѵ�.
        void trayAllWindow( bool bMinimize );

        template<class T>
        bool ToBool(const T& value) 
        {
            return value ? true : false;
        }

        //! 2007-04-03 Jgkim
        //! DumpMini.dmp ������ �����Ѵ�.
        void makeMiniDump();

        //! 2007-04-03 Jgkim
        //! DumpFull.dmp �޸� ��ü ������ dump �Ѵ�.
        //! ����:�뷮�� ũ�� ����ð��� �����ɸ���.
        void makeFullDump();

		//	Note : �����̸� �ڿ� �̸��� �� �ִ´�.
		TSTRING	GetSpecularName( const TSTRING& strSrcName, const TSTRING& strAddName );
		TSTRING	ChangeExtName( const TSTRING& strSrcName, const TSTRING& strExtName );
		TSTRING	GetName_ExtErase( const TSTRING& strSrcName );					// ���ϰ� : Ȯ���ڿ� . �� ���ذ�
		TSTRING	GetChangeTCHAR( const TSTRING& strSrcName, const TCHAR cErase, const TCHAR cNew );
		TSTRING	GetChangeTCHAR( const TSTRING& strSrcName, const TCHAR cErase );
		TSTRING	GetChange3LastTCHAR( const TSTRING& strSrcName, const TSTRING& strChange );
       
    } // namespace util
} // namespace sc

#endif // _CORE_COMMON_UTIL_H_
