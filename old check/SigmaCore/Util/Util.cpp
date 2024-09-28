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
        //! int ���� ������ ���ڸ� �߻���Ų��.
        //! ���ڴ� 1 ���� nMax ���� �߻���Ų��.
        //! ����:0 �� �߻���Ű�� �ʴ´�.
        int makeRandomNumber(
            int nMax )
        {
            return (rand() % nMax + 1);
        }

        //! ���� �������� ��� ������ â�� minimize �Ѵ�.
        //! Windows Key + M �� ���� ȿ���� ����.
        void minimizeAllWindow()
        {	
            // 77 : character code for the letter 'M'
            keybd_event( VK_LWIN, 0, 0, 0 );
            keybd_event( 77, 0, 0, 0 );
            keybd_event( VK_LWIN, 0, KEYEVENTF_KEYUP, 0 );
        }

        //! ���� �������� ��� ������ â�� Ʈ���̷� �����ų� �����Ѵ�.
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

			//	�����̸��� ��ħǥ�� ������ ���� �ʴ�.
			if ( idx == TSTRING::npos )
			{
				strName = strSrcName;
			}
			else
			{
				//	�����̸��� ���̽��� Ȯ���ڷ� �и��Ѵ�.
				strBaseName = strSrcName.substr ( 0, idx );
				strExtName = strSrcName.substr ( idx+1 );

				//	Ȯ���ڰ� ���� ���
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
			TSTRING::size_type idx = strSrcName.find_last_of('.');	// �� ���������� ã�ƾ� �Ѵ�.

			//	�����̸��� ��ħǥ�� ������ ���� �ʴ�.
			if ( idx == TSTRING::npos )
			{
				strName = strSrcName;
			}
			else
			{
				if( strExtName.size() )
				{
					strBaseName = strSrcName.substr ( 0, idx );			// �����̸��� ���̽��� Ȯ���ڷ� �и��Ѵ�.
					strName = strBaseName + _T('.') + strExtName.c_str();	// Ȯ���ڸ� ���δ�.
				}
				else
				{
					strName = strSrcName.substr ( 0, idx );				// Ȯ���� ���� ����.
				}
			}

			return	strName;
		}

		TSTRING	GetName_ExtErase( const TSTRING& strSrcName )
		{
			TSTRING	strBaseName;
			TSTRING::size_type idx = strSrcName.find('.');

			//	�����̸��� ��ħǥ�� ������ ���� �ʴ�.
			if ( idx == TSTRING::npos )
			{
				return strSrcName;
			}
			else
			{
				//	�����̸��� ���̽��� Ȯ���ڷ� �и��Ѵ�.
				strBaseName = strSrcName.substr ( 0, idx );
				return strBaseName;
			}
		}

		TSTRING	GetChangeTCHAR( const TSTRING& strSrcName, const TCHAR cErase, const TCHAR cNew )
		{
			TSTRING strName;
			TSTRING	strBaseName;
			TSTRING	strExtName;

			//	�����̸��� [�� ������ ���� �ʴ�.
			TSTRING::size_type idx = strSrcName.find(cErase);
			if ( idx == TSTRING::npos )
			{
				strName = strSrcName;
			}
			else
			{
				//	�����̸��� ���̽��� Ȯ���ڷ� �и��Ѵ�.
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

			//	�����̸��� [�� ������ ���� �ʴ�.
			TSTRING::size_type idx = strSrcName.find(cErase);
			if ( idx == TSTRING::npos )
			{
				strName = strSrcName;
			}
			else
			{
				//	�����̸��� ���̽��� Ȯ���ڷ� �и��Ѵ�.
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

			//	�����̸��� .�� ������ ���� �ʴ�.
			TSTRING::size_type idx = strSrcName.find('.');
			if ( idx == TSTRING::npos )
			{
				strName = strSrcName;
			}
			else
			{
				//	�����̸��� ���̽��� Ȯ���ڷ� �и��Ѵ�.
				strBaseName = strSrcName.substr ( 0, idx );
				strExtName = strSrcName.substr ( idx+1 );

				// 4���ں��� ������ ������ �ִ�.
				if( strBaseName.size() < 4 )
				{
					strName = strSrcName;
				}

				// �ڿ� 3 ���ڸ� ����.
				strBaseName = strSrcName.substr ( 0, idx-3 );

				// ����� �̸�
				strName = strBaseName + strChange + _T('.') + strExtName;
			}

			return	strName;
		}

    } // namespace util
} // namespace sc