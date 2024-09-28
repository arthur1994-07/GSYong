#include "StdAfx.h"
#include "../SigmaCore/gassert.h"

#include "../EngineLib/Common/SUBPATH.h"

//#include "../netclientlib/s_CHttpPatch.h"
#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLogicDataMan.h"

#include "./httpthread.h"
#include "./Resource.h"
#include "./LogControl.h"
#include "./AutoPatchMan.h"
#include "LauncherText.h"
#include "io.h"
#include <tlhelp32.h>

#include "../EngineLib/DxTools/DebugSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CHttpThread, CLPatchThread)

CHttpThread::CHttpThread()
{
}

CHttpThread::~CHttpThread()
{
}

BOOL CHttpThread::LoadListServer()
{
	CString str;
	str.Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), NS_GLOBAL_VAR::g_szServerFileList );
	//str.Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), NS_GLOBAL_VAR::g_szServerPreDownFileList ); // PREDOWNLOADER // 사전다운로드 테스트를 위해서 임시로 생성한 파일에서 읽도록 변경. 완료후 코드 정리 필요

	FILE* fp = _fsopen( str.GetString(), "rb", _SH_DENYNO );
	if( !fp ) return FALSE;

	SFILENODE FileInfo;

	// 첫번째 버전 정보
	if( 1 != fread ( &FileInfo, VER_1_OFFSET, 1, fp ) )
	{
		fclose ( fp );
		return FALSE;
	}

	// 버젼 옵셋 설정
	INT nVerOffset(0);

	switch( FileInfo.Ver )
	{
	case 1: nVerOffset = VER_1_OFFSET; break;
	case 2: nVerOffset = VER_2_OFFSET; break;
	case 3: nVerOffset = VER_3_OFFSET; break; 
	default:
		::AfxMessageBox( _T("[ERROR]:Filelist is different!") );
		return FALSE;
	}

	// 현재 버젼의 옵셋 만큼 파일 포지션을 뒤로 이동한다.
	LONG lOffset = nVerOffset - VER_1_OFFSET;

	if( fseek( fp, lOffset, SEEK_CUR) )
	{
		fclose ( fp );
		return FALSE;
	}

	// 두번째 파일 개수
	if( 1 != fread ( &FileInfo, nVerOffset, 1, fp ) )
	{
		fclose ( fp );
		return FALSE;
	}

	// Ran 게임 폴더를 설정
	CString AppDir( NS_GLOBAL_VAR::strAppPath);
	CFileStatus status;
	CString strAppFile;

	for( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE* pNewFile = new SFILENODE;
		if( 1 != fread( pNewFile, nVerOffset, 1, fp ) )
		{
			fclose ( fp );
			return FALSE;
		}

		if( nVerOffset == VER_3_OFFSET )
		{
			// 사전 다운로드하거나 사전 다운로드된 파일을 삭제하는 경우에는 벡터에 담아서 별도 처리
			// 상태값이 3번인 사전 다운로드 파일은 preFileList.bin 에 추가시키지 않으면 PreDownloader에서 cpreFileList.bin과 비교해서 알아서 삭제한다.
			if( pNewFile->IsPreDown == 2 )
			{
				if( NS_GLOBAL_VAR::g_bNeedPreDownloader == FALSE )
					NS_GLOBAL_VAR::g_bNeedPreDownloader = TRUE;

				m_vectorPreDownFile.push_back( pNewFile );

				//NS_GLOBAL_VAR::g_bApplyPreDown = TRUE; // PREDOWNLOADER // 테스트를 위해서 임시로 적용하도록 설정
				//m_vectorApplyPreDownFile.push_back( pNewFile ); // PREDOWNLOADER // 테스트를 위해서 임시로 적용하도록 설정
			}
			else if( pNewFile->IsPreDown == 1 )
			{
				// 사전 다운로드 적용해야하는 파일이 있는데 사전다운로드가 진행중이라면 런처는 종료되고 사전 다운로드를 완료하거나 강제 종료해야 함
				if( NS_GLOBAL_VAR::g_bRunningPreDownloader == TRUE )
				{
					// 런처 종료
					// 런처 시작시에 PreDownloader 가 동작중이라면 체크할 수 있지만 만약 사용자가 런처 실행한 후에 임의로 PreDownloader를 실행하는 경우 에러가 발생될 수 있음.
					// 그렇다고 PreDownloader 시작사에 런처가 동작중이라면 시작할 수 없도록 할려고 해도 런처에서 PreDownloader를 실행하므로 불가능함........
					::AfxMessageBox(_T("사전 다운로드된 파일을 적용해야 하는데 PreDownloader가 동작중입니다."));
					return FALSE;
				}

				// IsPreDown 가 1인 것중에서 이미 게임에 적용되어 있는것은 처리하지 않음
				// 폴더 경로 때문에 수정

				strAppFile.Format("%s%s%s", AppDir.Left ( AppDir.ReverseFind ( '\\' ) ), pNewFile->SubPath, pNewFile->FileName);
				// ftp에 올라간 파일은 cab 확장자이고 현재 파일은 cab 해제한 파일이므로 확장자 처리
				if(CFile::GetStatus(strAppFile.Left ( strAppFile.ReverseFind ( '.' ) ), status) == FALSE)
				{
					// 사전 다운로드된 이후에 적용으로 설정된 파일이 만약 클라이언트 파일에 없다면 이번에 적용되는 파일이다.
					// 하지만 이미 적용된 파일이 여러가지 이유로 인해서 없는경우가 발생됨. 
					// 따라서 Temp 폴더를 확인해서 있으면 사전다운르드 적용 상태이고 없다면 다시 다운로드 받아야함.
					//CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
					//const SUBPATH* pPath = pApp->GetSubPath();
					//CString strTempDownFile;
					//CString	DownloadDir( NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strDownloadTemp );
					//strTempDownFile.Format("%s%s", DownloadDir, pNewFile->FileName);

					//if( CFile::GetStatus(strTempDownFile, status) == FALSE )
					//{
					//	m_vectorServerFile.push_back( pNewFile );
					//}
					//else
					//{
						if( NS_GLOBAL_VAR::g_bApplyPreDown == FALSE )
							NS_GLOBAL_VAR::g_bApplyPreDown = TRUE;

						m_vectorApplyPreDownFile.push_back( pNewFile );
					//}
				}
				else
				{
					m_vectorServerFile.push_back( pNewFile );
				}
	
			}
			else
			{
				m_vectorServerFile.push_back( pNewFile );
			}
		}
		else
		{
			m_vectorServerFile.push_back( pNewFile );
		}
	}

	fclose ( fp );

	return TRUE;
}

BOOL CHttpThread::LoadListClient()
{
	CString str;
	str.Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), NS_GLOBAL_VAR::g_szClientFileList );

	FILE* fp = _fsopen( str.GetString(), "rb", _SH_DENYNO );
	if( !fp ) return FALSE;

	SFILENODE FileInfo;

	if( 1 != fread ( &FileInfo, VER_1_OFFSET, 1, fp ) )
	{
		fclose ( fp );
		return FALSE;
	}

	// 버젼 옵셋 설정
	INT nVerOffset(0);

	switch( FileInfo.Ver )
	{
	case 1: nVerOffset = VER_1_OFFSET; break;
	case 2: nVerOffset = VER_2_OFFSET; break;
	case 3: nVerOffset = VER_3_OFFSET; break;
	default:
		::AfxMessageBox( _T("[ERROR]:Filelist is different!") );
		return FALSE;
	}

	// 현재 버젼의 옵셋 만큼 파일 포지션을 뒤로 이동한다.
	LONG lOffset = nVerOffset - VER_1_OFFSET;

	if( fseek( fp, lOffset, SEEK_CUR) )
	{
		fclose ( fp );
		return FALSE;
	}

	if( 1 != fread ( &FileInfo, nVerOffset, 1, fp ) )
	{
		fclose ( fp );
		return FALSE;
	}

	for( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE NewFile;
		if( 1 != fread( &NewFile, nVerOffset, 1, fp ) )
		{
			fclose ( fp );
			return FALSE;
		}

		m_ClientFileTree.Add( &NewFile );
	}

	fclose ( fp );

	return TRUE;
}

BOOL CHttpThread::LoadList()
{
	//	<--	리스트 읽기	-->	//
	// FileList.bin 을 읽어서 사전 다운로드 할 파일이 있다면 따로 보관하고 CreatePrePatchFileList에서 preFileList.bin 에 기록
	if( !LoadListServer() )
	{
		//CDebugSet::ToLogFile( std::string("[ERROR] 서버 리스트 목록 읽기 실패"));
		return FALSE;
	}
	// preFileList.bin 생성
	if( !CreatePrePatchFileList() )
	{
		//CDebugSet::ToLogFile( std::string("[ERROR] 사전다운로드 리스트 목록 읽기 실패"));
		return FALSE;
	}

	if( IsForceTerminate() )	return FALSE;
	if( !LoadListClient() )
	{
		//CDebugSet::ToLogFile( std::string("[ERROR] 클라이언트 리스트 목록 읽기 실패"));
		return FALSE;
	}

	return TRUE;
}

BOOL CHttpThread::GETFILE_USEHTTP( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir )
{
	if ( !pHttpPatch )
	{
		GASSERT ( 0 && "잘못된 연산을 수행할 수 있습니다." );
		return FALSE;
	}

	if ( !strFileName.size () )
	{
		GASSERT ( 0 && "파일이 지정되지 않았습니다." );
		return FALSE;
	}

	std::string strSubPath( strRemoteSubPath );

	//	'\\'문자를 '/'로 변경한다.
	std::replace ( strSubPath.begin(), strSubPath.end(), '\\','/' );

	GLogicData::GetInstance().LoadCountryFile();
	EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();

	//#if !defined(_DEBUG) && !defined(KR_PARAM) && !defined(GS_PARAM) && !defined(_RELEASED) && !defined(ID_PARAM)
#ifndef _DEBUG

	// 20131030 khkim
	// 각 국가별로 패치 방식이 다르다. 이전 코드는 반대로 되어있었기 때문에 수정한다.
	// 한국,GS 를 제외한 모든 국가는 패치번호밑에 패치파일이 존재하는 방식이며,
	// 한국,GS 는 이 방식을 사용하지 않고 버전매니저에서 실제 FTP로 접속까지 해서 
	// 파일을 계속해서 덮허쓰는 방식이다. 
	switch ( sp )
	{
	case SP_OFFICE_TEST:
	case SP_KOREA:
	case SP_KOREA_TEST:
	case SP_GS:
		break;
	default:
		{
			if ( m_pPatchThreadParam->sGameVer != -1 )
			{
				CString strFolder;
				strFolder.Format( "/%04d", m_pPatchThreadParam->sGameVer );
				strFolder += strSubPath.c_str();
				strSubPath = strFolder;
			}
		}
		break;
	}

#endif

	strSubPath += strFileName;

    CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	std::string strLocalFullPath;
	strLocalFullPath += NS_GLOBAL_VAR::strProFile.GetString();	
	strLocalFullPath += pPath->SaveRoot(); //SUBPATH::SAVE_ROOT;
	strLocalFullPath += strTempDir.GetString();
	strLocalFullPath += strFileName;

	int nMaxHttp = 0;
	for ( int i = 0; i < RANPARAM::MAX_HTTP; ++i ) 
	{
		CString strRealAddress = RANPARAM::HttpAddressTable[i];
		if ( !strRealAddress.GetLength () )
		{
			nMaxHttp = i;
			break;
		}
	}
	
	srand( (unsigned)time( NULL ) );

	static int nTRY = rand() % nMaxHttp;
	int nTRY_FILESIZE_CHECK = 0;
	int nERROR_RETRY = 0;
	int nADDRESS_NULL_COUNT = 0;
	while ( nTRY_FILESIZE_CHECK < 3 )
	{
		//	강제 종료시
		if ( IsForceTerminate () ) return FALSE;		

		//	NOTE
		//		최대 시도 회수 초과시
		if ( nADDRESS_NULL_COUNT == RANPARAM::MAX_HTTP ) return FALSE;

		if ( RANPARAM::MAX_HTTP == nTRY ) nTRY = 0;
		if ( nERROR_RETRY == 5 ) return FALSE;		

		static const CString strHTTP = "http://";

		CString strRealAddress = RANPARAM::HttpAddressTable[nTRY];
		if ( !strRealAddress.GetLength () )
		{
			nADDRESS_NULL_COUNT++;		//	MAX_HTTP가 모두 널인가?
			nTRY++;
			continue;
		}

		//	널 체크를 통과했다는 것은 nADDRESS_NULL_COUNT을 초기화해야함을 의미한다.
		nADDRESS_NULL_COUNT = 0;

		CString strHttpAddress = strHTTP + strRealAddress; // "http://ranp.daumgame.com"
        
        std::string DownUrl(strHttpAddress.GetString());
        DownUrl.append(strSubPath);

		if ( NET_ERROR == pHttpPatch->SetBaseURL ( strHttpAddress ) )
		{			
			//NS_LOG_CONTROL::Write ( pHttpPatch->GetErrMsg() );
			nTRY++;
			nERROR_RETRY++;
			continue;
		}

		if ( NET_ERROR == pHttpPatch->GetFile ( strSubPath.c_str(), strLocalFullPath.c_str() ) )
		{
			//NS_LOG_CONTROL::Write ( pHttpPatch->GetErrMsg() );

			CString	strMsg(ID2LAUNCHERTEXT("IDS_MESSAGE", 47));
			char* szListMessage = new char[256];
            StringCchPrintfA(szListMessage, 256, "%s %s", strMsg.GetString(), strFileName.c_str());
			::PostThreadMessage(m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM) szListMessage);
            
            char* szUrlNotify = new char[256];
            StringCchPrintfA(szUrlNotify, 256, "%s", DownUrl.c_str());
            ::PostThreadMessage(m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM) szUrlNotify);

			nTRY ++;
			nERROR_RETRY++;
			continue;
		}

		//	강제 종료시
		if (IsForceTerminate())
            return FALSE;	

		ULONGLONG ulRECEIVED, ulTOTALSIZE;
		NS_LOG_CONTROL::GetProcessCurPosition ( &ulRECEIVED, &ulTOTALSIZE );

		if ( ulRECEIVED != ulTOTALSIZE )
		{
			nTRY_FILESIZE_CHECK++;
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 46 ), 0 ); // 전송 받는 파일 크기가 맞지 않다.
			continue;
		}
		else
		{
			return TRUE;
		}

		Sleep( 0 );
	}

	return FALSE;
}

BOOL CHttpThread::IsRunPreDownloader()
{
	BOOL bRet = TRUE;
	HANDLE   hProcess = NULL;
    PROCESSENTRY32 pe32 = {0};

    hProcess = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    pe32.dwSize = sizeof( PROCESSENTRY32 );

    if( Process32First( hProcess, &pe32 ) )
    {
        do 
        {
			if( strcmp(pe32.szExeFile, "PreDownloader.exe") == 0 )
			{
				NS_GLOBAL_VAR::g_bRunningPreDownloader = TRUE;
				break;
			}
            
        } while ( Process32Next( hProcess, &pe32 ) );
    }
    else
    {
		bRet = FALSE;
    }

    CloseHandle (hProcess);

	return bRet;
}

BOOL CHttpThread::CreatePrePatchFileList()
{
	if( m_vectorPreDownFile.size() <= 0 )
		return TRUE;

	CString strPreDown;
	strPreDown.Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), NS_GLOBAL_VAR::g_szPreDownFileList );

	CFile file;
	CFileStatus status;

	if( CFile::GetStatus(strPreDown, status) && NS_GLOBAL_VAR::g_bRunningPreDownloader == TRUE)
	{
		// preFileList.bin 이 이미 존재하고 PreDownloader 동작 중이라면 아무런 처리를 하지 않고 
		// 그렇지 않으면 알수없는 에러로 인해 preFileList.bin이 존재하는 것이므로 삭제
		return FALSE;
	}
	else
	{
		// 기존 파일에 덮어쓰거나 새로 생성
		if (!file.Open(strPreDown, CFile::modeCreate | CFile::modeReadWrite))
		{
			return FALSE;
		}
	}
	
	SFILENODE FileInfo;
	SFILENODE* pGetFile = NULL;

	FileInfo.Clear();
	FileInfo.Ver = m_vectorPreDownFile.size();
	file.Write(&FileInfo, sizeof(SFILENODE));

	for( int i = 0; i < m_vectorPreDownFile.size(); i++ )
	{
		pGetFile = m_vectorPreDownFile[i];

		FileInfo.Clear();
		FileInfo.SetData(pGetFile);
		file.Write(&FileInfo, sizeof(SFILENODE));
	}

	file.Close();

	return TRUE;
}

BEGIN_MESSAGE_MAP(CHttpThread, CLPatchThread)
END_MESSAGE_MAP()