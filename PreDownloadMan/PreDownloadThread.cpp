#include "stdAfx.h"
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/Compress/Unzipper.h"
#include "../SigmaCore/Compress/Zipper.h"

#include "../EngineLib/Common/SUBPATH.h"
#include "../EngineLib/DxTools/DebugSet.h"

#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLogicDataMan.h"

#include "./PreDownloadThread.h"
#include "./Resource.h"
#include "./LogControl.h"
#include "./PreDownloadMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CPreDownloadThread, CPreHttpThread)

CPreDownloadThread::CPreDownloadThread(CHttpPatch* pHttpPatch, DWORD nThreadID)
{
	VERIFY( pHttpPatch != NULL);
	m_pHttpPatch = pHttpPatch;

	m_nDlgThreadID = nThreadID;
}

void CPreDownloadThread::ThreadMain()
{
	CPreDownloadManApp* pApp = (CPreDownloadManApp*) AfxGetApp();
	const SUBPATH* pPath = pApp->GetSubPath();

	CString str				= PreDownloadDefine::g_strProFile + pPath->SaveRoot().c_str() + PreDownloadDefine::g_strPreDownloadDirectory;      
	CString strDownList		= PreDownloadDefine::g_strProFile + pPath->SaveRoot().c_str() + PreDownloadDefine::g_strDownedList;
	CString cPreFileList	= PreDownloadDefine::g_strAppPath + PreDownloadDefine::g_strClientPreFileList;
	m_strPreFileList		= PreDownloadDefine::g_strAppPath + PreDownloadDefine::g_strServerPreFileList;

	NS_LOG_CONTROL::SetProcessAllPosition ( 0, 100 );

	if ( IsForceTerminate() ) return ;	

	//CDebugSet::ToLogFile(std::string("프리 다운로드 시작"));
	//----------------------------------------------------------------------------------------
	// 패치 파일 리스트 로드;
	if ( !LoadList() )
		goto LFail;

	if ( IsForceTerminate() )
		goto LFail;

	// 패치 파일 리스트 비교 진행;
	if ( !MakeNewList() )
		goto LFail;    

	NS_LOG_CONTROL::SetProcessAllPosition ( 10, 100 );

	if ( IsForceTerminate() )
		goto LFail;

	//	새 목록에 나온 파일 다운 로드;
	CreateDirectory ( str.GetString(), NULL );

	if ( !DownloadFilesByHttp ( m_pHttpPatch ) )
		goto LFail;

	NS_LOG_CONTROL::SetProcessAllPosition ( 70, 100 );

	if ( IsForceTerminate() )
		goto LFail;

	NS_LOG_CONTROL::SetProcessAllPosition ( 80, 100 );

#ifdef PREDOWNLOAD
	// 프리다운로더에서 이미 게임에 적용된 파일을 삭제해서는 안됨. 21041201
	//if ( !DeleteIsNotDownFile() )
	//	goto LFail;
#else
	if ( !DeleteIsNotDownFile() )
	goto LFail;
#endif

	//CDebugSet::ToLogFile(std::string("다운로드된 파일 처리"));

	NS_LOG_CONTROL::SetProcessAllPosition ( 90, 100 );

	DeleteFile( strDownList.GetString() );

	//CDebugSet::ToLogFile(sc::string::format("다운로드 한 리스트 삭제. %s", strDownList.GetString()));

	DeleteFile( cPreFileList.GetString () );
	MoveFile( m_strPreFileList.GetString(), cPreFileList.GetString() );

	//CDebugSet::ToLogFile(sc::string::format("사전 다운로드 리스트 갱신. %s", cPreFileList.GetString ()));

	Destroy();

	//CDebugSet::ToLogFile(std::string("프리 다운로드 Destroy"));

#ifdef PREDOWNLOAD
	// 모든 패치 성공
	NS_LOG_CONTROL::SetProcessAllPosition ( 100, 100 );
	PreDownLoadComplete();
#endif
	
	//CDebugSet::ToLogFile(std::string("프리 다운로드 완료"));
	return;

LFail:
	//CDebugSet::ToLogFile(std::string("[ERROR] LFail"));
#ifdef PREDOWNLOAD	
	PreDownLoadFail();
#endif

	return;
}

void CPreDownloadThread::PreDownLoadComplete()
{
	Destroy();
	SetComplete();
}

void CPreDownloadThread::PreDownLoadFail()
{
	// 다운로드 중단 될 경우;
	// 현재까지 다운로드한 파일;
	SaveDownList();

	DeleteFile( m_strPreFileList.GetString() );

	//CDebugSet::ToLogFile(sc::string::format("		PreDownLoadFail 파일 삭제. %s", m_strPreFileList.GetString()));

	Destroy();
	SetFail();
}

BOOL CPreDownloadThread::SaveDownList()
{
    CPreDownloadManApp* pApp = (CPreDownloadManApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	CString strTemp;
	strTemp = PreDownloadDefine::g_strProFile + pPath->SaveRoot().c_str() + PreDownloadDefine::g_strDownedList;

	FILE* fp = _fsopen ( strTemp, "wb", _SH_DENYNO );
	if ( !fp )
	{
		return FALSE;
	}

	SFILENODE FileInfo;

	FileInfo.Ver = (int) m_vectorDownFile.size ();
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		CDebugSet::ToLogFile(
            std::string("[ERROR]SaveDownList File Size Write Failed!"));
		return FALSE;
	}

	for ( int i = 0; i < (int)m_vectorDownFile.size (); i++ )
	{
		SFILENODE* pNewFile = m_vectorDownFile[i];

		if ( 1 != fwrite ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			fclose ( fp );

			CDebugSet::ToLogFile(
                std::string("[ERROR]SaveDownList File Write Failed!"));
			return FALSE;
		}
	}

	fclose ( fp );
	
	return TRUE;
}

BOOL CPreDownloadThread::LoadDownList()
{
    CPreDownloadManApp* pApp = (CPreDownloadManApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	CString strTemp;
	strTemp = PreDownloadDefine::g_strProFile + pPath->SaveRoot().c_str() + PreDownloadDefine::g_strDownedList;

	//CDebugSet::ToLogFile(sc::string::format("기존에 다운받은 사전 다운로드 파일 리스트 작성. %s", strTemp.GetString()));

	FILE* fp = _fsopen ( strTemp, "rb", _SH_DENYNO );
	if ( !fp )
	{
		return FALSE;
	}

	SFILENODE FileInfo;

	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		CDebugSet::ToLogFile(
            std::string("[ERROR]LoadDownList File Size Read Failed!"));
		return FALSE;
	}	

	for ( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE* pNewFile = new SFILENODE;
		if ( 1 != fread ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			fclose ( fp );

			CDebugSet::ToLogFile(
                std::string("[ERROR]LoadDownList File Read Failed!"));
			return FALSE;
		}

		m_mapDownedFile.insert ( std::make_pair(std::string(pNewFile->FileName), pNewFile) );
	}

	fclose ( fp );

	return TRUE;
}

BOOL CPreDownloadThread::MakeNewList()
{
	NS_LOG_CONTROL::SetProcessCurPosition ( 0, 1 );

	FILEVECTOR::size_type nServerFileSize = m_vectorServerFile.size ();

	//CDebugSet::ToLogFile(sc::string::format("사전다운로드 해야할 파일 리스트 만들기 시작. %d", nServerFileSize));

	for ( FILEVECTOR::size_type i = 0; i < nServerFileSize; i++ )
	{	
		SFILENODE* pServerGetFile = m_vectorServerFile[i];

		CPatchSetNode* pNode = m_ClientFileTree.Find ( pServerGetFile );
		if ( pNode )	//	기존 파일
		{
			// IsPreDown 3 일경우 삭제;
			if ( pServerGetFile->IsPreDown == 3 )
			{
				//CDebugSet::ToLogFile(sc::string::format("사전다운로드 목록에서 삭제. %s", pServerGetFile->FileName));
				continue;
			}

			SFILENODE* pClientFile = pNode->GetFile ();
			pNode->SetUseFlag ();

			if ( pClientFile->Ver < pServerGetFile->Ver )
			{
				SFILENODE* pNewFile = new SFILENODE;
				*pNewFile = *pServerGetFile;
				m_vectorNewFile.push_back ( pNewFile );
				//CDebugSet::ToLogFile(sc::string::format("사전다운로드 받아야하는 파일(버젼이 올라간 파일). %s", pNewFile->FileName));
			}
			else
			{
				//CDebugSet::ToLogFile(sc::string::format("이미 사전다운로드 받은 파일. %s", pServerGetFile->FileName));
			}
		}
		else	//	추가되는 파일
		{
			SFILENODE* pNewFile = new SFILENODE;
			*pNewFile = *pServerGetFile;
			m_vectorNewFile.push_back ( pNewFile );
			//CDebugSet::ToLogFile(sc::string::format("사전다운로드 받아야하는 파일(신규로 사전다운로드 등록). %s", pNewFile->FileName));
		}

		NS_LOG_CONTROL::SetProcessCurPosition ( i, nServerFileSize );

		if ( IsForceTerminate() )	return FALSE;
	}

	//CDebugSet::ToLogFile(sc::string::format("사전다운로드 해야할 파일 리스트 만들기 완료. %d", nServerFileSize));


	NS_LOG_CONTROL::SetProcessCurPosition ( 1, 1 );

	m_ClientFileTree.GetNotUseItem( &m_vectorDeleteFile );

	LoadDownList ();	

	return TRUE;
}

BOOL CPreDownloadThread::DownloadFilesByHttp( CHttpPatch* pHttpPatch )
{
	int DownCount = 0;

	// Temp 폴더패스를 설정한다
    CPreDownloadManApp* pApp = (CPreDownloadManApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	CString	DownloadDir( PreDownloadDefine::g_strProFile + pPath->SaveRoot().c_str() + PreDownloadDefine::g_strPreDownloadDirectory );

	int nVectorNewFileSize = (int)m_vectorNewFile.size();

	//CDebugSet::ToLogFile(sc::string::format("FTP 다운로드 할 파일 갯수. %d", nVectorNewFileSize));

	m_nDownloadFileCount = nVectorNewFileSize;

	for ( int i = 0; i < nVectorNewFileSize; ++i )
	{
		m_nDownloadedFileCount = i + 1;

		SFILENODE* pNewFile = m_vectorNewFile[i];		

		FILEMAP_ITER found = m_mapDownedFile.find( std::string ( pNewFile->FileName ) );
		FILEMAP_ITER iter_end = m_mapDownedFile.end();

		BOOL bAlreadyDown( FALSE );
		if ( found != iter_end )
		{
			//CDebugSet::ToLogFile(sc::string::format("기존에 다운받은 파일임. %s", pNewFile->FileName));
			bAlreadyDown = TRUE; // Note : 이미 정상적으로 다운로드 완료된 것.
		}


		// puase 상태면 resume버튼 누르기 전까지 기다린다;
		WatingForResume();

		if ( IsForceTerminate() ) return FALSE;

		ULONGLONG TotalPos = 10 + (++DownCount * 60) / nVectorNewFileSize;			
		NS_LOG_CONTROL::SetProcessAllPosition( TotalPos, 100 );

		CString FullSubPath = pNewFile->SubPath;

		if ( !bAlreadyDown ) //	Note : 아직 다운로드 안된 것만 GetFile한다.
		{
			if ( !GETFILE_USEHTTP ( FullSubPath.GetString(), pNewFile->FileName ) )
			{
				//CDebugSet::ToLogFile(sc::string::format("[ERROR] FTP 다운로드 실패 . %s", pNewFile->FileName));
				return FALSE;
			}
			//CDebugSet::ToLogFile(sc::string::format("FTP 다운로드 성공. %s", pNewFile->FileName));
		}

		//	<--	정상적으로 다운로드 했으면 리스트 작성한다.	--> //
		m_vectorDownFile.push_back( pNewFile );

		if( IsForceTerminate() ) return FALSE;

		Sleep( 0 ); // 파일받다가 멈추는것 방지용
	}

	if ( m_vectorNewFile.size() != m_vectorDownFile.size () )
	{
		//CDebugSet::ToLogFile(sc::string::format("[ERROR] 파일 갯수가 일치하지 않음. m_vectorNewFile %d, m_vectorDownFile %d", m_vectorNewFile.size(), m_vectorDownFile.size ()));
		return FALSE; // 다운받은 파일과 목록에 있는 파일의 개수가 다르다면 실패
	}

	return TRUE;
}

BOOL CPreDownloadThread::GETFILE_USEHTTP( std::string strRemoteSubPath, std::string strFileName, CString strTempDir )
{
	CHttpPatch* pHttpPatch = m_pHttpPatch;
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

	strSubPath += strFileName;

	CPreDownloadManApp* pApp = (CPreDownloadManApp*) AfxGetApp();
	const SUBPATH* pPath = pApp->GetSubPath();

	std::string strLocalFullPath;
	strLocalFullPath += PreDownloadDefine::g_strProFile.GetString();	
	strLocalFullPath += pPath->SaveRoot();
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

		//	최대 시도 회수 초과시
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

		CString strHttpAddress = strHTTP + strRealAddress;

		std::string DownUrl(strHttpAddress.GetString());
		DownUrl.append(strSubPath);

		if ( NET_ERROR == pHttpPatch->SetBaseURL ( strHttpAddress ) )
		{
			nTRY++;
			nERROR_RETRY++;
			continue;
		}

		if ( NET_ERROR == pHttpPatch->GetFile ( strSubPath.c_str(), strLocalFullPath.c_str() ) )
		{
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

BOOL CPreDownloadThread::DeleteIsNotDownFile()
{
	CString	strDelFile, strCompFile;
	int nCnt = static_cast<int>( m_vectorDeleteFile.size() );

	strCompFile = PreDownloadDefine::g_strAppPath + RANPARAM::RPFDataPath;

	m_FileSystem.OpenFileSystem(strCompFile);	


	for ( int i = 0; i < nCnt; i++ )
	{
		SFILENODE* pNewFile = m_vectorDeleteFile[i];

		strDelFile.Format( "%s%s%s", PreDownloadDefine::g_strAppPath.GetString(), pNewFile->SubPath, pNewFile->FileName );
		strDelFile.MakeLower();
		strDelFile = strDelFile.Left( strDelFile.ReverseFind ( '.' ) );

		//	압축내 파일인지 확인
		bool bPack = false;
		if( strstr(pNewFile->SubPath,".rcc\\") )	bPack = true;

		if ( !bPack )
		{
			DeleteFile( strDelFile.GetString() );
			//CDebugSet::ToLogFile(sc::string::format("		!bPack - DeleteIsNotDownFile 파일 삭제. %s", strDelFile.GetString()));
		}
		else
		{

			CString strExtPath, strZipFile;				
			strZipFile = strDelFile;
			
			strZipFile = strZipFile.Left ( strZipFile.ReverseFind ( '\\' ) );	//	Zip 파일 경로

            CPreDownloadManApp* pApp = (CPreDownloadManApp*) AfxGetApp();
            const SUBPATH* pPath = pApp->GetSubPath();

			strExtPath = PreDownloadDefine::g_strProFile + pPath->SaveRoot().c_str() + PreDownloadDefine::g_strPreDownloadDirectory; 
			strExtPath += strZipFile.Right( strZipFile.GetLength() - strZipFile.ReverseFind ( '\\' ) );
			strExtPath = strExtPath.Left ( strExtPath.ReverseFind ( '.' )  );	//	Zip 파일 상위 경로


			//	폴더가 없다면
			if ( GetFileAttributes( strExtPath ) == UINT_MAX )
			{
				//	신규 Rcc파일
				if ( GetFileAttributes( strZipFile ) == UINT_MAX )
				{
					continue;
				}
				else
				{
					//	압축을 해제한다.
					if ( GetFileAttributes( strExtPath ) == UINT_MAX )
					{
						CUnzipper cUnZip;
						m_vecZipFile.push_back( strZipFile.GetString() );
						
						if ( !cUnZip.Unzip( strZipFile, strExtPath ) )
						{
							return FALSE;
						}						
					}

				}
			}

			strDelFile = strExtPath + '\\' + pNewFile->FileName;
			strDelFile = strDelFile.Left ( strDelFile.ReverseFind ( '.' ) );
			DeleteFile( strDelFile.GetString() );
			//CDebugSet::ToLogFile(sc::string::format("		DeleteIsNotDownFile 파일 삭제. %s", strDelFile.GetString()));
		}

	}

	m_FileSystem.CloseFileSystem();	

	return TRUE;
}

BOOL CPreDownloadThread::Destroy()
{
	for ( int i = 0; i < (int)m_vectorServerFile.size (); i++ )
	{
		delete m_vectorServerFile[i];
	}
	m_vectorServerFile.clear();

	for ( int i = 0; i < (int)m_vectorNewFile.size (); i++ )
	{
		delete m_vectorNewFile[i];
	}
	m_vectorNewFile.clear();
	m_vectorDownFile.clear();

	// m_mapDownedFile 삭제 ///////////////////////////
	FILEMAP_ITER iter = m_mapDownedFile.begin ();
	FILEMAP_ITER iter_end = m_mapDownedFile.end ();
	for ( ; iter != iter_end; iter++ )
	{
		delete (*iter).second;
	}
	m_mapDownedFile.clear();

	m_vectorDeleteFile.clear();
	m_vecZipFile.clear();

	return TRUE;
}

BEGIN_MESSAGE_MAP(CPreDownloadThread, CPreHttpThread)
END_MESSAGE_MAP()