#include "stdafx.h"
#include <dbghelp.h>
#include <iostream>
#include <sstream>
#include "../File/FileUtil.h"
#include "./SystemInfo.h"
#include "./Util.h"

#include "../DebugInclude.h"

namespace sc
{
    namespace util
    {
        //! int 형의 랜덤한 숫자를 발생시킨다.
        //! 숫자는 1 부터 nMax 까지 발생시킨다.
        //! 주의:0 은 발생시키지 않는다.
        int makeRandomNumber(
            int nMax )
        {
            return (rand() % nMax + 1);
        }

        //! 현재 실행중인 모든 윈도우 창을 minimize 한다.
        //! Windows Key + M 을 누른 효과를 낸다.
        void minimizeAllWindow()
        {	
            // 77 : character code for the letter 'M'
            keybd_event( VK_LWIN, 0, 0, 0 );
            keybd_event( 77, 0, 0, 0 );
            keybd_event( VK_LWIN, 0, KEYEVENTF_KEYUP, 0 );
        }

        //! 현재 실행중인 모든 윈도우 창을 트레이로 내리거나 복구한다.
        void trayAllWindow( bool bMinimize )
        {
            HWND hTrayWnd = ::FindWindow( _T( "Shell_TrayWnd" ), NULL );
            if( NULL != hTrayWnd )
                ::SendMessage( hTrayWnd, WM_COMMAND, bMinimize ? 419 : 416, 0 );
        }

        void makeMiniDump()
        {
            std::string strPath = sc::getAppPath() + "\\DumpMini.dmp";
            sc::file::autoRename(strPath);

            HANDLE hFile = ::CreateFileA(
                strPath.c_str(),
                GENERIC_WRITE,
                0,
                NULL,
                OPEN_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL );

            if ( hFile != INVALID_HANDLE_VALUE )
            {
                MiniDumpWriteDump(
                    GetCurrentProcess(),
                    GetCurrentProcessId(),
                    hFile,
                    MiniDumpNormal,
                    NULL,
                    NULL,
                    NULL);
            }

            if (hFile)
            {
                CloseHandle( hFile );
                hFile = INVALID_HANDLE_VALUE;
            }
        }

        void makeFullDump()
        {
            std::string strPath = sc::getAppPath() + "\\DumpFull.dmp";
            sc::file::autoRename(strPath);

            HANDLE hFile = ::CreateFileA(
                strPath.c_str(),
                GENERIC_WRITE,
                0,
                NULL,
                OPEN_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                NULL );

            if ( hFile != INVALID_HANDLE_VALUE )
            {
                MiniDumpWriteDump(
                    GetCurrentProcess(),
                    GetCurrentProcessId(),
                    hFile,
                    MiniDumpWithFullMemory,
                    NULL,
                    NULL,
                    NULL);
            }

            if (hFile)
            {
                CloseHandle( hFile );
                hFile = INVALID_HANDLE_VALUE;
            }
        }

		TSTRING	GetSpecularName ( const TSTRING& strSrcName, const TSTRING& strAddName )
		{
			TSTRING	strName;
			TSTRING	strBaseName;
			TSTRING	strExtName;
			TSTRING::size_type idx = strSrcName.find('.');

			//	파일이름이 마침표를 가지고 있지 않다.
			if ( idx == TSTRING::npos )
			{
				strName = strSrcName;
			}
			else
			{
				//	파일이름을 베이스와 확장자로 분리한다.
				strBaseName = strSrcName.substr ( 0, idx );
				strExtName = strSrcName.substr ( idx+1 );

				//	확장자가 없을 경우
				if ( strExtName.empty() )
				{
					strName = strSrcName;
				}
				else
				{
					strName = strBaseName + strAddName + _T('.') + strExtName;
				}
			}

			return	strName;
		}

		TSTRING	ChangeExtName( const TSTRING& strSrcName, const TSTRING& strExtName )
		{
			TSTRING	strName;
			TSTRING	strBaseName;
			TSTRING::size_type idx = strSrcName.find_last_of('.');	// 맨 마지막것을 찾아야 한다.

			//	파일이름이 마침표를 가지고 있지 않다.
			if ( idx == TSTRING::npos )
			{
				strName = strSrcName;
			}
			else
			{
				if( strExtName.size() )
				{
					strBaseName = strSrcName.substr ( 0, idx );			// 파일이름을 베이스와 확장자로 분리한다.
					strName = strBaseName + _T('.') + strExtName.c_str();	// 확장자를 붙인다.
				}
				else
				{
					strName = strSrcName.substr ( 0, idx );				// 확장자 없이 보냄.
				}
			}

			return	strName;
		}

		TSTRING	GetName_ExtErase( const TSTRING& strSrcName )
		{
			TSTRING	strBaseName;
			TSTRING::size_type idx = strSrcName.find('.');

			//	파일이름이 마침표를 가지고 있지 않다.
			if ( idx == TSTRING::npos )
			{
				return strSrcName;
			}
			else
			{
				//	파일이름을 베이스와 확장자로 분리한다.
				strBaseName = strSrcName.substr ( 0, idx );
				return strBaseName;
			}
		}

		TSTRING	GetChangeTCHAR( const TSTRING& strSrcName, const TCHAR cErase, const TCHAR cNew )
		{
			TSTRING strName;
			TSTRING	strBaseName;
			TSTRING	strExtName;

			//	파일이름이 [를 가지고 있지 않다.
			TSTRING::size_type idx = strSrcName.find(cErase);
			if ( idx == TSTRING::npos )
			{
				strName = strSrcName;
			}
			else
			{
				//	파일이름을 베이스와 확장자로 분리한다.
				strBaseName = strSrcName.substr ( 0, idx );
				strExtName = strSrcName.substr ( idx+1 );

				strName = strBaseName + cNew + strExtName;
			}

			return	strName;
		}

		TSTRING	GetChangeTCHAR( const TSTRING& strSrcName, const TCHAR cErase )
		{
			TSTRING strName;
			TSTRING	strBaseName;
			TSTRING	strExtName;

			//	파일이름이 [를 가지고 있지 않다.
			TSTRING::size_type idx = strSrcName.find(cErase);
			if ( idx == TSTRING::npos )
			{
				strName = strSrcName;
			}
			else
			{
				//	파일이름을 베이스와 확장자로 분리한다.
				strBaseName = strSrcName.substr ( 0, idx );
				strExtName = strSrcName.substr ( idx+1 );

				strName = strBaseName + strExtName;
			}

			return	strName;
		}

		TSTRING	GetChange3LastTCHAR( const TSTRING& strSrcName, const TSTRING& strChange )
		{
			TSTRING strName;
			TSTRING	strBaseName;
			TSTRING	strExtName;

			//	파일이름이 .를 가지고 있지 않다.
			TSTRING::size_type idx = strSrcName.find('.');
			if ( idx == TSTRING::npos )
			{
				strName = strSrcName;
			}
			else
			{
				//	파일이름을 베이스와 확장자로 분리한다.
				strBaseName = strSrcName.substr ( 0, idx );
				strExtName = strSrcName.substr ( idx+1 );

				// 4글자보다 작으면 문제가 있다.
				if( strBaseName.size() < 4 )
				{
					strName = strSrcName;
				}

				// 뒤에 3 글자를 뺀다.
				strBaseName = strSrcName.substr ( 0, idx-3 );

				// 변경된 이름
				strName = strBaseName + strChange + _T('.') + strExtName;
			}

			return	strName;
		}

    } // namespace util
} // namespace sc