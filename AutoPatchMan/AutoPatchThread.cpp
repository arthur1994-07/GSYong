#include "StdAfx.h"
#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/Compress/Unzipper.h"
#include "../SigmaCore/Compress/Zipper.h"

#include "../EngineLib/Common/SUBPATH.h"
#include "../EngineLib/DxTools/DebugSet.h"

#include "../RanLogic/GLogicDataMan.h"
#include "../RanLogic/RANPARAM.h"
#include "../MfcExLib/ExLibs/CompactFdi.h"

#include "./AutoPatchThread.h"
#include "./Resource.h"
#include "./LogControl.h"
//#include "../netclientlib/s_CHttpPatch.h"
#include "./LauncherText.h"
#include "./AutoPatchMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CAutoPatchThread, CHttpThread)

CAutoPatchThread::CAutoPatchThread(LPVOID pParam, DWORD nThreadID)
{
	VERIFY( pParam != NULL);
	m_pPatchThreadParam = (S_PATCH_THREAD_PARAM *)pParam;

	m_nDlgThreadID = nThreadID;
	m_sFailGameVer = 0;
}

void CAutoPatchThread::ThreadMain()
{
	CHttpPatch* pHttpPatch	= m_pPatchThreadParam->pHttpPatch;
	const bool	bUseHttp	= m_pPatchThreadParam->bUseHttp;
	const int	cPatchVer	= m_pPatchThreadParam->cPatchVer;
	const int	sPatchVer	= m_pPatchThreadParam->sPatchVer;
	const int	cGameVer	= m_pPatchThreadParam->cGameVer;
	const int	sGameVer	= m_pPatchThreadParam->sGameVer;	

	//CDebugSet::ToLogFile( std::string("1. 런처 시작"));

	CString cFileList, sFileList, str, strTemp;
	BOOL bInstalling(FALSE);

	NS_LOG_CONTROL::SetProcessAllPosition ( 0, 100 );

	if ( IsForceTerminate() ) return ;

	// 현재 PreDownloader 가 동작중인지 프로세스 확인. 동작여부를 preFileList.bin 생성시에 활용
	if( IsRunPreDownloader() == FALSE )
	{
		CHAR * szTempMessage = new CHAR[256];
		wsprintf( szTempMessage, "%s", "[ERROR] 런처 프로세스 검사 실패" );
		//CDebugSet::ToLogFile( std::string(szTempMessage));
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szTempMessage );	
		return;
	}

	// 리스트 전송중
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 49 ), 0 );
	// 파일 리스트 다운로드
	if ( !GETFILE_USEHTTP ( pHttpPatch, "\\", NS_GLOBAL_VAR::strServerCabFileList.GetString(), "" ) )
	{	
		if ( !IsForceTerminate() )
		{
			// 리스트 전송중 오류
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 50 ), 0 );
		}
		//CDebugSet::ToLogFile( std::string("[ERROR] 파일 리스트 다운로드 에러"));

		goto LFail;
	}
	//CDebugSet::ToLogFile( std::string("2. 파일 리스트 다운로드 완료"));

	if (IsForceTerminate())
        goto LFail;

    CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	//	압축 풀기위한 전체 경로 설정 및 압축 풀기&설치
	//str = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strServerCabFileList;
    str = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strServerCabFileList;

	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 51 ), 0 );
    if (!mincab::Extract(NS_GLOBAL_VAR::strAppPath.GetString (), str.GetString()))
	{
        AfxMessageBox(mincab::GetErrorMsg(), MB_OK);
		// 리스트 압축풀기 실패
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 52 ), 0 );

		//CDebugSet::ToLogFile( std::string("[ERROR] 리스트 압축 풀기 실패"));

		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 4, 100 );

	//CDebugSet::ToLogFile( std::string("3. 리스트 압축 풀기 성공"));
	//Initialize(); // MEMO : 함수 구현 안됨.

	if ( IsForceTerminate() ) goto LFail;

	// 파일 리스트 비교 및 새 목록 작성
	// 패치 파일 리스트 로드
	// 사전 패치 목록 작성 ( preFileList.bin )
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 53 ), 0 );
	if ( !LoadList() )
	{
		// 리스트 로드에 실패
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 54 ), 0 );

		//CHAR * szErrListMessage11 = new CHAR[256]; 
		//wsprintf( szErrListMessage11, "[ERROR] LoadList 실패");
		//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szErrListMessage11 );

		//CDebugSet::ToLogFile( std::string(szErrListMessage11));

		goto LFail;
	}

	//CDebugSet::ToLogFile( std::string("4. 패치 목록 리스트 완료"));

	if ( IsForceTerminate() ) goto LFail;

	// 사전 다운로드된 파일중에서 적용해야할 파일이 있다면 적용
	// 만약 사전 다운로드 해야하지만 다운로드 되지 않은 파일은 런처에서 업데이트 받로록 함
	if ( !ApplyPreDownFile() )
	{
		// 사전 다운로드파일 적용 실패 메세지 출력
		//CHAR * szErrListMessage12 = new CHAR[256]; 
		//wsprintf( szErrListMessage12, "[ERROR] ApplyPreDownFile 실패");
		//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szErrListMessage12 );

		//CDebugSet::ToLogFile( std::string(szErrListMessage12));

		goto LFail;
	}    

	//CDebugSet::ToLogFile( std::string("5. 사전 다운로드중 적용할 파일 처리 완료"));

	NS_LOG_CONTROL::SetProcessAllPosition ( 30, 100 );

	if ( IsForceTerminate() ) goto LFail;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 패치 파일 리스트 비교 진행
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 57 ), 0 );
	if ( !MakeNewList ( cPatchVer, sPatchVer, cGameVer, sGameVer ) )
	{
		// 리스트 비교에 실패
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 58 ), 0 );

		//CDebugSet::ToLogFile( std::string("[ERROR] 패치 파일 리스트 만들기 실패"));

		goto LFail;
	}    

	//CDebugSet::ToLogFile( std::string("6. 패치 파일 리스트 만들기 완료"));

	NS_LOG_CONTROL::SetProcessAllPosition ( 10, 100 );

	if ( IsForceTerminate() ) goto LFail;

	//	새 목록에 나온 파일 다운 로드
	//str = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strDownloadTemp;
    str = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strDownloadTemp;
	CreateDirectory ( str.GetString(), NULL );
	if ( !DownloadFilesByHttp ( pHttpPatch ) )
	{
		if ( !IsForceTerminate() )
		{
			// 패치 파일 전송중 오류
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 55 ), 0 );
		}

		//CHAR * szErrListMessage14 = new CHAR[256]; 
		//wsprintf( szErrListMessage14, "[ERROR] DownloadFilesByHttp 실패");
		//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szErrListMessage14 );

		//CDebugSet::ToLogFile( std::string(szErrListMessage14));

		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 70, 100 );

	//CDebugSet::ToLogFile( std::string("7. 패치 파일 FTP 다운로드 완료"));

	if ( IsForceTerminate() ) goto LFail;

	//	파일 복사
	if ( !Installing() )
	{	
		//	강제 종료가 아닌 진짜 오류
		//
		if ( IsExtractError() )
		{
			// 패치 파일중 일부 손상
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 59 ), 0 );

			//	Note : DS list 삭제            
			//CString strDownList = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strDownList;
            CString strDownList = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strDownList;
			DeleteFile ( strDownList.GetString() );

			// MEMO : 카피 리스트로 비교후에 설치된 파일은 다운로드 받지 않아도 된다.
			//
			//CString strCopyList = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT  + NS_GLOBAL_VAR::strCopyList;
            CString strCopyList = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strCopyList;
			CopyFile( strCopyList.GetString(), strDownList.GetString(), FALSE );

			//CDebugSet::ToLogFile( sc::string::format("[ERROR] 패치 파일 손상. strDownList : %s / strCopyList : %s", strDownList.GetString(), strCopyList.GetString()));
		}

		bInstalling = TRUE; // 설치중이다.

		//CHAR * szErrListMessage15 = new CHAR[256]; 
		//wsprintf( szErrListMessage15, "[ERROR] Installing 실패");
		//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szErrListMessage15 );

		//CDebugSet::ToLogFile( std::string(szErrListMessage15));

		goto LFail;
	}
	NS_LOG_CONTROL::SetProcessAllPosition ( 80, 100 );


	//CDebugSet::ToLogFile( std::string("8. 파일 복사 완료"));

	if ( !UnPackageFile() )
	{
		if ( !IsForceTerminate() )
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 59 ), 0 );

		//CHAR * szErrListMessage16 = new CHAR[256]; 
		//wsprintf( szErrListMessage16, "[ERROR] UnPackageFile 실패");
		//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szErrListMessage16 );

		//CDebugSet::ToLogFile( std::string(szErrListMessage16));

		goto LFail;
	}

	//CDebugSet::ToLogFile( std::string("9. 파일 압축해제 완료"));

	if ( !DeleteNotFoundFile() )
	{
		//CHAR * szErrListMessage17 = new CHAR[256]; 
		//wsprintf( szErrListMessage17, "[ERROR] DeleteNotFoundFile 실패");
		//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szErrListMessage17 );

		//CDebugSet::ToLogFile( std::string(szErrListMessage17));

		goto LFail;
	}

	//CDebugSet::ToLogFile( std::string("10. 불필요한 파일 제거 완료"));

	if ( !PackageFile() )
	{
		if ( !IsForceTerminate() )
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 71 ), 0 );

		//CHAR * szErrListMessage18 = new CHAR[256]; 
		//wsprintf( szErrListMessage18, "[ERROR] DeleteNotFoundFile 실패");
		//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szErrListMessage18 );

		//CDebugSet::ToLogFile( std::string(szErrListMessage18));

		goto LFail;
	}


	//CDebugSet::ToLogFile( std::string("11. 파일 패키지 완료"));

//	Package
//	if(!CompressFile()){
//		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 71 ), 0 );
//		goto LFail;
//	}

	NS_LOG_CONTROL::SetProcessAllPosition ( 90, 100 );

	//	Note : DS list 삭제    
	//strTemp = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT  + NS_GLOBAL_VAR::strDownList;
    strTemp = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strDownList;
	DeleteFile( strTemp.GetString() );

	//CDebugSet::ToLogFile( sc::string::format("12. 다운로드 리스트 삭제.  %s", strTemp.GetString()));

	// Note : 카피 리스트 삭제
	//strTemp = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT  + NS_GLOBAL_VAR::strCopyList;
    strTemp = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strCopyList;
	DeleteFile ( strTemp.GetString() );

	//CDebugSet::ToLogFile( sc::string::format("13. 복사 리스트 삭제.  %s", strTemp.GetString()));

	//	<--	클라이언트 목록를 서버목록으로 교체	-->	//
	cFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szClientFileList;
	//sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szServerFileList; // 사전 다운로드가 추가되면서 cFileList를 생성 ( 기존에는 FileList.bin을 cFileList.bin으로 변경 )

	DeleteFile( cFileList.GetString () );
	//CDebugSet::ToLogFile( sc::string::format("14. 클라이언트 리스트 삭제.  %s", cFileList.GetString ()));

	//MoveFile( sFileList.GetString(), cFileList.GetString() ); // 사전 다운로드가 추가되면서 cFileList를 생성 ( 기존에는 FileList.bin을 cFileList.bin으로 변경 )
	CreateClientFileList(); 

	//CDebugSet::ToLogFile( std::string("15. 클라이언트 리스트 생성 완료"));

	//sFileList = NS_GLOBAL_VAR::strProFile + SUBPATH::SAVE_ROOT + NS_GLOBAL_VAR::strServerCabFileList;
    sFileList = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strServerCabFileList;
	DeleteFile ( sFileList.GetString() );

	//CDebugSet::ToLogFile( sc::string::format("16. 서버 파일 리스트 삭제.  %s", sFileList.GetString()));

	DeleteDownFiles();

	//CDebugSet::ToLogFile( std::string("17. 다운로드 리스트 삭제 완료"));
//	DeleteCompressFile();
//	if(!OptimizeCompressFile()){
//		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 72 ), 0 );
//		goto LFail;
//	}

#ifdef PREDOWNLOAD
    // TBB 에러로 일단 주석 처리
	//Destroy ();
#else
	Destroy ();
#endif

	// 클라이언트 버전 갱신
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 60 ), 0 );
	if ( !VersionUp ( sGameVer ) )
	{
		// 클라이언트 버전 갱신 실패
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 61 ), 0 );

		//CDebugSet::ToLogFile( sc::string::format("[ERROR] 버젼 갱신 실패.  %d", sGameVer));

		goto LFail;
	}

	//CDebugSet::ToLogFile( std::string("18. 버젼 갱신 완료"));

	// 수동 패치 동작을 위한 버전정보 레지스트리에 저장
	// 현재는 GS 만 사용한다.
	SaveVersionInRegistry( sGameVer );

	// 모든 패치 성공
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 62 ), 0 );
	NS_LOG_CONTROL::SetProcessAllPosition ( 100, 100 );

	//CDebugSet::ToLogFile( std::string("19. 패치 완료"));

	return ;

LFail:
	//	다운로드할 파일들이 손상되어 실패한 경우에는
	//	리스트를 처음부터 다시 작성해야하기때문에 리스트를 저장하지 않는다.
	if ( IsExtractError () )
	{
		if ( IsForceTerminate () )
		{
			//CHAR * szErrListMessage19 = new CHAR[256]; 
			//wsprintf( szErrListMessage19, "[ERROR] LFail - SaveDownList (Extract) 실패");
			//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szErrListMessage19 );

			//CDebugSet::ToLogFile( std::string(szErrListMessage19));

			SaveDownList ( sGameVer );
		}

		//CHAR * szErrListMessage20 = new CHAR[256]; 
		//wsprintf( szErrListMessage20, "[ERROR] LFail - SaveCopyList (Extract) 실패");
		//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szErrListMessage20 );

		//CDebugSet::ToLogFile( std::string(szErrListMessage20));

		SaveCopyList ( sGameVer ); // MEMO :설치한 리스트는 설치중에 취소되었을때는 무조건 저장한다.
	}
	else
	{
		//CHAR * szErrListMessage21 = new CHAR[256]; 
		//wsprintf( szErrListMessage21, "[ERROR] LFail - SaveDownList 실패");
		//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szErrListMessage21 );

		//CDebugSet::ToLogFile( std::string(szErrListMessage21));

		SaveDownList ( sGameVer );

		if( bInstalling ) SaveCopyList ( sGameVer ); // MEMO :설치중에 중단되면 카피 리스트 작성.
	}

	sFileList = NS_GLOBAL_VAR::strAppPath + NS_GLOBAL_VAR::g_szServerFileList;
	DeleteFile ( sFileList.GetString() );
	sFileList = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strServerCabFileList;
	DeleteFile ( sFileList.GetString() );	

	DeleteUnPackFile();

#ifdef PREDOWNLOAD
    // TBB 에러로 일단 주석 처리
	//Destroy ();
#else
	Destroy ();
#endif

	SetFail();

	return ;
}

BOOL CAutoPatchThread::SaveDownList( int sGameVer )
{
    CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	CString strTemp;
	strTemp = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strDownList;

	FILE* fp = _fsopen ( strTemp, "wb", _SH_DENYNO );
	if ( !fp )
	{
		return FALSE;
	}

	SFILENODE FileInfo;

	FileInfo.Ver = sGameVer;
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		//CDebugSet::ToLogFile(std::string("		[ERROR]SaveDownList File Game Version Write Failed!"));
		return FALSE;
	}

	FileInfo.Ver = (int) m_vectorDownFile.size ();
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		//CDebugSet::ToLogFile(std::string("		[ERROR]SaveDownList File Size Write Failed!"));
		return FALSE;
	}

	for ( int i = 0; i < (int)m_vectorDownFile.size (); i++ )
	{
		SFILENODE* pNewFile = m_vectorDownFile[i];

		//if( pNewFile->IsPreDown == 2 )
		//	CDebugSet::ToLogFile( sc::string::format("		SaveDownList 사전 다운로드 리스트 파일.  %s", pNewFile->FileName));
		//else if( pNewFile->IsPreDown == 1 ) 
		//	CDebugSet::ToLogFile( sc::string::format("		SaveDownList 적용된 다운로드 리스트 파일.  %s", pNewFile->FileName));
		//else if( pNewFile->IsPreDown == 3 ) 
		//	CDebugSet::ToLogFile( sc::string::format("		SaveDownList 삭제된 다운로드 리스트 파일.  %s", pNewFile->FileName));

		if ( 1 != fwrite ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			fclose ( fp );

			//CDebugSet::ToLogFile(std::string("		[ERROR]SaveDownList File Write Failed!"));
			return FALSE;
		}
	}

	fclose ( fp );
	
	return TRUE;
}

BOOL CAutoPatchThread::LoadDownList()
{
    CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	CString strTemp;
	strTemp = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strDownList;

	FILE* fp = _fsopen ( strTemp, "rb", _SH_DENYNO );
	if ( !fp )
	{
		return FALSE;
	}

	SFILENODE FileInfo;

	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		//CDebugSet::ToLogFile(std::string("		[ERROR]LoadDownList File Game Version Read Failed!"));
		return FALSE;
	}
	m_sFailGameVer = FileInfo.Ver;

	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		//CDebugSet::ToLogFile(std::string("		[ERROR]LoadDownList File Size Read Failed!"));
		return FALSE;
	}	

	for ( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE* pNewFile = new SFILENODE;


		//if( pNewFile->IsPreDown == 2 )
		//	CDebugSet::ToLogFile( sc::string::format("		LoadDownList 사전 다운로드 리스트 파일.  %s", pNewFile->FileName));
		//else if( pNewFile->IsPreDown == 1 ) 
		//	CDebugSet::ToLogFile( sc::string::format("		LoadDownList 적용된 다운로드 리스트 파일.  %s", pNewFile->FileName));
		//else if( pNewFile->IsPreDown == 3 ) 
		//	CDebugSet::ToLogFile( sc::string::format("		LoadDownList 삭제된 다운로드 리스트 파일.  %s", pNewFile->FileName));


		if ( 1 != fread ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			fclose ( fp );

			//CDebugSet::ToLogFile(std::string("		[ERROR]LoadDownList File Read Failed!"));
			return FALSE;
		}

		m_mapDownedFile.insert ( std::make_pair(std::string(pNewFile->FileName), pNewFile) );
	}

	fclose ( fp );

	return TRUE;
}

BOOL CAutoPatchThread::SaveCopyList( int sGameVer )
{
    CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	CString strTemp;
	strTemp = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strCopyList;

	FILE* fp = _fsopen ( strTemp, "wb", _SH_DENYNO );
	if ( !fp )
	{
		return FALSE;
	}

	SFILENODE FileInfo;

	FileInfo.Ver = sGameVer;
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		//CDebugSet::ToLogFile(
            //std::string("		[ERROR]SaveCopyList File Game Version Write Failed!"));
		return FALSE;
	}

	FileInfo.Ver = (int) m_vectorCopyFile.size ();
	if ( 1 != fwrite ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		//CDebugSet::ToLogFile(
          //  std::string("		[ERROR]SaveCopyList File Size Write Failed!"));
		return FALSE;
	}

	for ( int i = 0; i < (int)m_vectorCopyFile.size (); i++ )
	{
		SFILENODE* pNewFile = m_vectorCopyFile[i];

		//if( pNewFile->IsPreDown == 2 )
		//	CDebugSet::ToLogFile( sc::string::format("		SaveCopyList 사전 다운로드 리스트 파일.  %s", pNewFile->FileName));
		//else if( pNewFile->IsPreDown == 1 ) 
		//	CDebugSet::ToLogFile( sc::string::format("		SaveCopyList 적용된 다운로드 리스트 파일.  %s", pNewFile->FileName));
		//else if( pNewFile->IsPreDown == 3 ) 
		//	CDebugSet::ToLogFile( sc::string::format("		SaveCopyList 삭제된 다운로드 리스트 파일.  %s", pNewFile->FileName));


		if ( 1 != fwrite ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			fclose ( fp );

			//CDebugSet::ToLogFile(
                //std::string("		[ERROR]SaveCopyList File Write Failed!"));
			return FALSE;
		}
	}

	fclose ( fp );

	return TRUE;
}

BOOL CAutoPatchThread::LoadCopyList()
{
    CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	CString strTemp;
	strTemp = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strCopyList;

	FILE* fp = _fsopen ( strTemp, "rb", _SH_DENYNO );
	if ( !fp )
	{
		return FALSE;
	}

	SFILENODE FileInfo;

	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		//CDebugSet::ToLogFile(
            //std::string("		[ERROR]LoadCopyList File Game Version Read Failed!"));
		return FALSE;
	}
	m_sFailGameVer = FileInfo.Ver;

	if ( 1 != fread ( &FileInfo, sizeof ( SFILENODE ), 1, fp ) )
	{
		fclose ( fp );

		//CDebugSet::ToLogFile(
          //  std::string("		[ERROR]LoadCopyList File Size Read Failed!"));
		return FALSE;
	}	

	for ( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE* pNewFile = new SFILENODE;
		if ( 1 != fread ( pNewFile, sizeof ( SFILENODE ), 1, fp ) )
		{
			fclose ( fp );

			//CDebugSet::ToLogFile(
              //  std::string("		[ERROR]LoadCopyList File Read Failed!"));
			return FALSE;
		}

		m_mapCopiedFile.insert ( std::make_pair(std::string(pNewFile->FileName), pNewFile) );

		//if( pNewFile->IsPreDown == 2 )
		//	CDebugSet::ToLogFile( sc::string::format("		LoadCopyList 사전 다운로드 리스트 파일.  %s", pNewFile->FileName));
		//else if( pNewFile->IsPreDown == 1 ) 
		//	CDebugSet::ToLogFile( sc::string::format("		LoadCopyList 적용된 다운로드 리스트 파일.  %s", pNewFile->FileName));
		//else if( pNewFile->IsPreDown == 3 ) 
		//	CDebugSet::ToLogFile( sc::string::format("		LoadCopyList 삭제된 다운로드 리스트 파일.  %s", pNewFile->FileName));



/*
		if(strstr(pNewFile->SubPath,NS_GLOBAL_VAR::strCompDir)){ // 압축할 파일인지 패스를 검색해서 알아본다.
			m_vecCompFile.push_back(pNewFile);
		}
*/
	}

	fclose ( fp );

	return TRUE;
}

BOOL CAutoPatchThread::MakeNewList( const int cPatchVer, const int sPatchVer, const int cGameVer, const int sGameVer )
{
	NS_LOG_CONTROL::SetProcessCurPosition ( 0, 1 );
	{
		FILEVECTOR::size_type nServerFileSize = m_vectorServerFile.size ();
		for ( FILEVECTOR::size_type i = 0; i < nServerFileSize; i++ )
		{	
			SFILENODE* pServerGetFile = m_vectorServerFile[i];

			CPatchSetNode* pNode = m_ClientFileTree.Find ( pServerGetFile );
			if ( pNode )	//	기존 파일
			{
				SFILENODE* pClientFile = pNode->GetFile ();
				pNode->SetUseFlag ();

				if ( pClientFile->Ver < pServerGetFile->Ver )
				{
					SFILENODE* pNewFile = new SFILENODE;
					*pNewFile = *pServerGetFile;
					m_vectorNewFile.push_back ( pNewFile );

					//CDebugSet::ToLogFile( sc::string::format("		버젼이 맞지않아서 새로 다운받는 파일.  %s", pNewFile->FileName));
				}
			}
			else	//	추가되는 파일
			{			
				SFILENODE* pNewFile = new SFILENODE;
				*pNewFile = *pServerGetFile;
				m_vectorNewFile.push_back ( pNewFile );

				//CDebugSet::ToLogFile( sc::string::format("		새로 추가되는 파일.  %s", pNewFile->FileName));
			}

			NS_LOG_CONTROL::SetProcessCurPosition ( i, nServerFileSize );

			if ( IsForceTerminate() )	return FALSE;
		}
	}
	NS_LOG_CONTROL::SetProcessCurPosition ( 1, 1 );

	//	<--	서버에 없는 파일이 클라이언트에 존재할 경우 삭제 -->	//
	m_ClientFileTree.GetNotUseItem ( &m_vectorDeleteFile );

#ifdef PREDOWNLOAD
	int nCnt = static_cast<int>( m_vectorDeleteFile.size() );

	CHAR * szDeletedFileName = new CHAR[256]; 
	for ( int i = 0; i < nCnt; i++ )
	{
		SFILENODE* pNewFile = m_vectorDeleteFile[i];

		memset(szDeletedFileName, 0, sizeof(szDeletedFileName));
		wsprintf( szDeletedFileName, "		[ERROR] 삭제할 파일 (서버x, 클라o). %s", pNewFile->FileName );
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szDeletedFileName );

		CDebugSet::ToLogFile( std::string(szDeletedFileName));
	}
#endif
	LoadDownList ();	

	return TRUE;
}

BOOL CAutoPatchThread::DownloadFilesByHttp( CHttpPatch* pHttpPatch )
{
	int DownCount = 0;
#ifdef PREDOWNLOAD
	BOOL bForceDown = FALSE;
#endif
	// Temp 폴더패스를 설정한다
    CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	CString	DownloadDir( NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strDownloadTemp );

	int nVectorNewFileSize = (int)m_vectorNewFile.size();
	for ( int i = 0; i < nVectorNewFileSize; ++i )
	{
		SFILENODE* pNewFile = m_vectorNewFile[i];		

#ifdef PREDOWNLOAD
		// 기존에는 프리다운로드 아닌 파일만 다운받았는데 어떤 이유에서든지 프리다운로드한 파일을 적용할때 없다면 다운로드 받는다
		// Temp 폴더에 파일이 존재하지 않는다면 다시 다운로드 받는다
		if( pNewFile->IsPreDown == 1 )
		{
			CFileStatus status;
			CString strTempDownFile;
			CString	DownloadDir( NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strDownloadTemp );
			strTempDownFile.Format("%s%s", DownloadDir, pNewFile->FileName);
			CString AppDir( NS_GLOBAL_VAR::strAppPath);
			CString strAppFile;
			strAppFile.Format("%s%s%s", AppDir.Left ( AppDir.ReverseFind ( '\\' ) ), pNewFile->SubPath, pNewFile->FileName);

			if( CFile::GetStatus(strAppFile.Left ( strAppFile.ReverseFind ( '.' ) ), status) == FALSE )
			{
				CHAR * szErrListMessage22 = new CHAR[256]; 
				wsprintf( szErrListMessage22, "		클라이언트 폴더에 파일 없음 %s", pNewFile->FileName );
				::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szErrListMessage22 );

				CDebugSet::ToLogFile( std::string(szErrListMessage22));

				// 강제로 다운받도록 변경
				 bForceDown = TRUE;

				 // 강제로 다운 받기때문에 Temp 폴더에 만약 파일이 있다면 강제로 삭제
				if( CFile::GetStatus(strTempDownFile, status) == TRUE )
				{
					CHAR * szErrListMessage23 = new CHAR[256]; 
					wsprintf( szErrListMessage23, "		[ERROR] Temp 폴더에 파일 없음 %s", pNewFile->FileName );
					::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szErrListMessage23 );

					CDebugSet::ToLogFile( std::string(szErrListMessage23));

					DeleteFile( strTempDownFile.GetString() );
				}
			}
			else
			{
				CHAR * szErrListMessage24 = new CHAR[256]; 
				wsprintf( szErrListMessage24, "		[ERROR] 클라이언트 폴더에 파일이 존재하기때문에 일단 PASS %s", pNewFile->FileName );
				::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szErrListMessage24 );

				CDebugSet::ToLogFile( std::string(szErrListMessage24));

				m_vectorDownFile.push_back( pNewFile );
			}
		}

		if( pNewFile->IsPreDown == 0 || bForceDown == TRUE )
#else
		if( pNewFile->IsPreDown == 0 )
#endif
		{
			FILEMAP_ITER found = m_mapDownedFile.find( std::string ( pNewFile->FileName ) );
			FILEMAP_ITER iter_end = m_mapDownedFile.end();

			BOOL bAlreadyDown( FALSE );
			if ( found != iter_end ) bAlreadyDown = TRUE; // Note : 이미 정상적으로 다운로드 완료된 것.

			if ( IsForceTerminate() ) return FALSE;


			//	<--	카운트는 현재 진행하고 있는것을 포함한다.	-->	//
			//	현재 첫번째것을 받고 있으면, 카운트는 1이 되는 것이다.
			//	완료 기준이 아니라, 진행 기준이다.	
#ifdef PREDOWNLOAD
			if ( !bAlreadyDown || bForceDown == TRUE ) // MEMO : 이미 다운받은 파일은 리스트에 표시하지 않는다.
#else
			if ( !bAlreadyDown ) // MEMO : 이미 다운받은 파일은 리스트에 표시하지 않는다.
#endif
			{
				CString	strMsg;
				strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 47 ); 
				// 동기화 문제 때문에 메세지를 보낼때 항상 메모리를 새로 생성한다.
				CHAR * szListMessage = new CHAR[256]; 
				wsprintf( szListMessage, "%s %s", strMsg.GetString(), pNewFile->FileName );
				::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );

				//CDebugSet::ToLogFile( std::string(szListMessage));
			}

			ULONGLONG TotalPos = 10 + (++DownCount * 60) / nVectorNewFileSize;			
			NS_LOG_CONTROL::SetProcessAllPosition( TotalPos, 100 );

			CString FullSubPath = pNewFile->SubPath;

#ifdef PREDOWNLOAD
			if ( !bAlreadyDown || bForceDown == TRUE ) //	Note : 아직 다운로드 안된 것만 GetFile한다.
#else
			if ( !bAlreadyDown ) //	Note : 아직 다운로드 안된 것만 GetFile한다.
#endif
			{
				if ( !GETFILE_USEHTTP ( pHttpPatch, FullSubPath.GetString(), pNewFile->FileName ) )
				{
					return FALSE;
				}
			}

			//	<--	정상적으로 다운로드 했으면 리스트 작성한다.	--> //
			m_vectorDownFile.push_back( pNewFile );
		}

		if( IsForceTerminate() ) return FALSE;

		Sleep( 0 ); // 파일받다가 멈추는것 방지용
	}

	if ( m_vectorNewFile.size() != m_vectorDownFile.size () )
	{
		//CHAR * szErrListMessage = new CHAR[256]; 
		//wsprintf( szErrListMessage, "		m_vectorNewFile Size %d, m_vectorDownFile Size %d", m_vectorNewFile.size(), m_vectorDownFile.size () );

		//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szErrListMessage );

		//CDebugSet::ToLogFile( std::string(szErrListMessage));

		return FALSE; // 다운받은 파일과 목록에 있는 파일의 개수가 다르다면 실패
	}

	LoadCopyList (); // 설치했던 리스트를 로딩한다.

	return TRUE;
}

BOOL CAutoPatchThread::Installing()
{
	CString strTemp;
	int InstallCount = 0;
	int nMaxPos = 0;

	int nVectorDownFileSize = (int)m_vectorDownFile.size();

	for ( int i = 0; i < nVectorDownFileSize; ++i )
	{

		//	<--	압축 풀기위한 전체 경로 설정 및 압축 풀기&설치	-->	//
		NS_LOG_CONTROL::SetProcessCurPosition ( 0, 1 );

		SFILENODE* pNewFile = m_vectorDownFile[i];	

		//	Note : 이미 정상적으로 설치 완료된 것.
		FILEMAP_ITER found = m_mapCopiedFile.find ( std::string ( pNewFile->FileName ) );
		FILEMAP_ITER iter_end = m_mapCopiedFile.end ();

		BOOL bAlreadyCopy(FALSE);
		if ( found != iter_end ) bAlreadyCopy = TRUE;

		if ( IsForceTerminate() ) return FALSE;

		CString	Seperator = "\\";
		CString FullSubPath = pNewFile->SubPath;
		CStringArray SubPathArray;

		STRUTIL::ClearSeparator ();
		STRUTIL::RegisterSeparator ( Seperator );
		STRUTIL::StringSeparate ( FullSubPath, SubPathArray );		


		//	새로 압축해야 하는 파일인지 확인
		bool bPack = false;
		if( strstr(pNewFile->SubPath,".rcc\\") )	bPack = true;
		
		ULONGLONG TotalPos = 70 + (++InstallCount * 10) / nVectorDownFileSize;
		NS_LOG_CONTROL::SetProcessAllPosition ( TotalPos, 100 );

		if ( !bAlreadyCopy )
		{
			if ( !bPack ) 
			{
				//	런쳐창에 파일명 출력
				{
					CString	strMsg;
					strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 48 );
					CHAR * szListMessage = new CHAR[256];
					wsprintf( szListMessage, "%s %s", strMsg.GetString(), pNewFile->FileName );
					::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );

					//CDebugSet::ToLogFile( sc::string::format("		파일 설치중. %s", szListMessage));

				}

				//	<--	없는 디렉토리 만들기	-->	//
				CString FullPath;
				FullPath.Format( "%s", NS_GLOBAL_VAR::strAppPath.GetString() );
				for ( int i = 0; i < SubPathArray.GetCount (); i++ )
				{
					FullPath += SubPathArray[i];			
					CreateDirectory ( FullPath.GetString(), NULL );
					FullPath += "\\";
				}

                CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
                const SUBPATH* pPath = pApp->GetSubPath();

				CString	DownloadDir;
				DownloadDir = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strDownloadTemp + pNewFile->FileName;		

				CString	DestFile;
				DestFile.Format( "%s%s", FullPath.GetString(), pNewFile->FileName );
				DestFile = DestFile.Left ( DestFile.ReverseFind ( '.' ) );

				SetFileAttributes( DestFile.GetString(), FILE_ATTRIBUTE_NORMAL );
				DeleteFile( DestFile.GetString() );

                if (!mincab::Extract(FullPath.GetString(), DownloadDir.GetString()))
				{
					DownloadDir.MakeLower();
					DownloadDir = DownloadDir.Left( DownloadDir.ReverseFind( '.' ) );

					if( ( DownloadDir.Find( _T(".dll") ) == -1 ) && ( !IsForceTerminate() ) )
					{
						CHAR * szListMessage = new CHAR[MAX_PATH];
                        StringCchCopy(szListMessage, MAX_PATH, mincab::GetErrorMsg());
						::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );

						//CDebugSet::ToLogFile( sc::string::format("		[ERROR] 압축해제 파일 찾기 실패. %s", szListMessage));

						SetExtractError (); // 오류메시지 : 오류일때 계속 시도한다. 무한루프 가능 ㅡ.ㅡ;;

						return FALSE;
					}
				}

				SetFileAttributes ( DestFile.GetString(), FILE_ATTRIBUTE_NORMAL );

				//	<--	정상적으로 설치했으면 리스트 작성한다. --> //
				m_vectorCopyFile.push_back ( pNewFile );
			}
			else
			{
				//	Package 파일들은 등록해놓고 나중에 패치한다.
				m_vectorPackFile.push_back(pNewFile);

			}
		}
		else
		{
			//	이미 설치된거 리스트에 삽입
			m_vectorCopyFile.push_back ( pNewFile );
		}

		NS_LOG_CONTROL::SetProcessCurPosition ( 1, 1 );
	

//		if(strstr(pNewFile->SubPath,NS_GLOBAL_VAR::strCompDir)){ // 압축할 파일인지 패스를 검색해서 알아본다.
//			m_vecCompFile.push_back(pNewFile);
//		}
		
		if( IsForceTerminate() ) return FALSE;

		Sleep( 0 );
	}

	if ( m_vectorNewFile.size() != m_vectorCopyFile.size () + m_vectorPackFile.size() )
	{
		//CHAR * szErrListMessage = new CHAR[256]; 
		//wsprintf( szErrListMessage, "		m_vectorNewFile Size %d, m_vectorCopyFile +  m_vectorPackFile Size %d", m_vectorNewFile.size(), m_vectorCopyFile.size () + m_vectorPackFile.size() );

		//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szErrListMessage );

		//CDebugSet::ToLogFile( std::string(szErrListMessage));

		return FALSE; // MEMO : 업데이트할 파일 개수와 설치한 파일 개수가 일치하지 않으면...
	}

	return TRUE;
}

BOOL CAutoPatchThread::UnPackageFile()
{
	int nVectorPackFileSize = (int)m_vectorPackFile.size();
	int InstallCount = 0;	

	for ( int i = 0; i < nVectorPackFileSize; ++i )
	{
		//	<--	압축 풀기위한 전체 경로 설정 및 압축 풀기&설치	-->	//
		NS_LOG_CONTROL::SetProcessCurPosition ( 0, 1 );

		SFILENODE* pNewFile = m_vectorPackFile[i];	

		//	Note : 이미 정상적으로 설치 완료된 것.
		FILEMAP_ITER found = m_mapCopiedFile.find ( std::string ( pNewFile->FileName ) );
		FILEMAP_ITER iter_end = m_mapCopiedFile.end ();

		BOOL bAlreadyCopy(FALSE);
		if ( found != iter_end ) bAlreadyCopy = TRUE;

		if ( IsForceTerminate() ) return FALSE;

		CString	Seperator = "\\";
		CString FullSubPath = pNewFile->SubPath;
		CStringArray SubPathArray;

		STRUTIL::ClearSeparator ();
		STRUTIL::RegisterSeparator ( Seperator );
		STRUTIL::StringSeparate ( FullSubPath, SubPathArray );		


		//	새로 압축해야 하는 파일인지 확인
		if( !strstr(pNewFile->SubPath,".rcc\\") )
		{
			CDebugSet::ToLogFile(
                std::string("Pack File Error %s", pNewFile->SubPath));
			continue;
		}
		
		ULONGLONG TotalPos = 80 + (++InstallCount * 5) / nVectorPackFileSize;
		NS_LOG_CONTROL::SetProcessAllPosition ( TotalPos, 100 );

		if ( !bAlreadyCopy )
		{
			//	런쳐창에 파일명 출력
			{
				CString	strMsg;
				strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 48 );
				CHAR * szListMessage = new CHAR[256];
				wsprintf( szListMessage, "%s %s", strMsg.GetString(), pNewFile->FileName );
				::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );

				//CDebugSet::ToLogFile( sc::string::format("		패키지 해제. %s", szListMessage));

			}


			CString strExtPath, strZipFile;				
			strZipFile.Format( "%s", NS_GLOBAL_VAR::strAppPath.GetString() );

			//	마지막폴더는 .rcc이므로 만들지 않는다.
			for ( int i = 0; i < SubPathArray.GetCount(); i++ )
			{
				strZipFile += SubPathArray[i];			
				if ( i < SubPathArray.GetCount()-1 )
					CreateDirectory ( strZipFile.GetString(), NULL );					
				strZipFile += "\\";
			}

            CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
            const SUBPATH* pPath = pApp->GetSubPath();

			CString	DownloadDir;
			DownloadDir = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strDownloadTemp + pNewFile->FileName;		

			
			strZipFile = strZipFile.Left ( strZipFile.ReverseFind ( '\\' ) );	//	Zip 파일 경로
            
			strExtPath = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strDownloadTemp; 
			strExtPath += strZipFile.Right( strZipFile.GetLength() - strZipFile.ReverseFind ( '\\' ) );
			strExtPath = strExtPath.Left ( strExtPath.ReverseFind ( '.' )  );	//	Zip 파일 상위 경로
			
			//	폴더가 없다면
			if ( GetFileAttributes( strExtPath ) == UINT_MAX )
			{
				//	신규 Rcc파일
				if ( GetFileAttributes( strZipFile ) == UINT_MAX )
				{
					
					if ( !CreateDirectory(strExtPath, NULL) ) 					
					{
						CString	strMsg;
						strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 75 );
						CHAR * szListMessage = new CHAR[256];
						wsprintf( szListMessage, "%s %s", strMsg.GetString(), pNewFile->FileName );
						::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );

						//CDebugSet::ToLogFile( sc::string::format("		[ERROR] 패키지 해제 중 파일폴더 생성 실패. %s", szListMessage));

						return FALSE;
					}

					m_vecZipFile.push_back( strZipFile.GetString() );

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
							CString	strMsg;
							strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 74 );
							CHAR * szListMessage = new CHAR[256];
							wsprintf( szListMessage, "%s %s", strMsg.GetString(), pNewFile->FileName );
							::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );	

							//CDebugSet::ToLogFile( sc::string::format("		[ERROR] 패키지 해제 중 압축 해제 실패. %s", szListMessage));

							return FALSE;
						}						
					}

				}
			}

			CString strExtFile;
			strExtFile = pNewFile->FileName;
			strExtFile = strExtFile.Left ( strExtFile.ReverseFind ( '.' ) );
			strExtPath += '\\';

            if (!mincab::Extract(strExtPath.GetString(), DownloadDir.GetString()))
			{
                AfxMessageBox(mincab::GetErrorMsg(), MB_OK);

				DownloadDir.MakeLower();
				DownloadDir = DownloadDir.Left( DownloadDir.ReverseFind( '.' ) );

				if( ( DownloadDir.Find( _T(".dll") ) == -1 ) && ( !IsForceTerminate() ) )
				{
					CHAR * szListMessage = new CHAR[MAX_PATH];
                    StringCchCopy(szListMessage, MAX_PATH, mincab::GetErrorMsg());
					::PostThreadMessage(m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage);

					//CDebugSet::ToLogFile( sc::string::format("		[ERROR] 패키지 해제 중 다운로드 파일 찾을수 없음. %s", szListMessage));

					SetExtractError (); // 오류메시지 : 오류일때 계속 시도한다. 무한루프 가능 ㅡ.ㅡ;;

					return FALSE;
				}
			}

			NS_LOG_CONTROL::SetProcessCurPosition ( 1, 1 );
		}

		if( IsForceTerminate() ) return FALSE;

		Sleep( 0 );
	}

	return TRUE;
}

BOOL CAutoPatchThread::PackageFile()
{
    CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	int nPackFile = 0;
	int nZipPath = (int)m_vecZipFile.size();

	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 70 ), 0 );

	for ( int i = 0; i < nZipPath; ++i )
	{
		CString strZipFile = m_vecZipFile[i].c_str();
		strZipFile = strZipFile.Left ( strZipFile.ReverseFind ( '.' )  );
		strZipFile += ".zip";

		CString strExtPath = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strDownloadTemp; 
		strExtPath += strZipFile.Right( strZipFile.GetLength() - strZipFile.ReverseFind ( '\\' ) );
		strExtPath = strExtPath.Left ( strExtPath.ReverseFind ( '.' )  );	//	Zip 파일 상위 경로
		

		CString strFind = strExtPath;
		strFind += "\\*.*";

		ULONGLONG TotalPos = 85 + (++nPackFile * 5) / nZipPath;
		NS_LOG_CONTROL::SetProcessAllPosition ( TotalPos, 100 );

		WIN32_FIND_DATA finfo;
		HANDLE hSearch = FindFirstFile(strFind, &finfo);

		int nFileNum = 0;

		if (hSearch != INVALID_HANDLE_VALUE) 
		{
			CZipper cZipFile;
			cZipFile.OpenZip( strZipFile, strExtPath );

			do 
			{
				if (finfo.cFileName[0] != '.') 
				{
					if ( !(finfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
					{
						CString strSubFile = strExtPath;
						strSubFile += '\\';
						strSubFile += finfo.cFileName;
						cZipFile.AddFileToZip( strSubFile, false, NULL );

						nFileNum++;

						DeleteFile( strSubFile );

						//CDebugSet::ToLogFile( sc::string::format("		압축 중 파일 삭제.  %s", strSubFile.GetString()));
					}
						
				}
			} 
			while (FindNextFile(hSearch, &finfo));
			
			FindClose(hSearch);
			
			cZipFile.CloseZip();
		}

//		압축해제한 파일 지운다.
		RemoveDirectory( strExtPath );

//		원래 파일 지운다.
		if ( nFileNum != 0 )
		{
			DeleteFile( m_vecZipFile[i].c_str() );
			rename( strZipFile , m_vecZipFile[i].c_str() );
		}
		else
		{
			DeleteFile( m_vecZipFile[i].c_str() );
			DeleteFile( strZipFile );
		}

		if( IsForceTerminate() ) return FALSE;

		Sleep( 0 );
	}

	//	<--	정상적으로 설치했으면 리스트 작성한다. --> //
//	m_vectorCopyFile.push_back ( pNewFile );

//	return FALSE;

	return TRUE;
}


BOOL CAutoPatchThread::DeleteUnPackFile()
{
    CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	int nZipPath = (int)m_vecZipFile.size();

	for ( int i = 0; i < nZipPath; ++i )
	{
		CString strZipFile = m_vecZipFile[i].c_str();

		CString strExtPath = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strDownloadTemp; 
		strExtPath += strZipFile.Right( strZipFile.GetLength() - strZipFile.ReverseFind ( '\\' ) );
		strExtPath = strExtPath.Left ( strExtPath.ReverseFind ( '.' )  );	//	Zip 파일 상위 경로		

		CString strFind = strExtPath;
		strFind += "\\*.*";

		WIN32_FIND_DATA finfo;
		HANDLE hSearch = FindFirstFile(strFind, &finfo);

		if (hSearch != INVALID_HANDLE_VALUE) 
		{
			do 
			{
				if (finfo.cFileName[0] != '.') 
				{
					if ( !(finfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
					{
						CString strSubFile = strExtPath;
						strSubFile += '\\';
						strSubFile += finfo.cFileName;

						DeleteFile( strSubFile );

						//CDebugSet::ToLogFile( sc::string::format("		압축해제 된 파일 삭제.  %s", strSubFile.GetString()));
					}
						
				}
			} 
			while (FindNextFile(hSearch, &finfo));
			
			FindClose(hSearch);
		}

//		압축해제한 파일 지운다.
		RemoveDirectory( strExtPath );
		//CDebugSet::ToLogFile( sc::string::format("		압축해제 디렉토리 삭제.  %s", strExtPath.GetString()));
	}

	return TRUE;

}


BOOL CAutoPatchThread::DeleteNotFoundFile()
{
	CString	strDelFile, strCompFile;
	int nCnt = static_cast<int>( m_vectorDeleteFile.size() );

	strCompFile = NS_GLOBAL_VAR::strAppPath + RANPARAM::RPFDataPath;

	m_FileSystem.OpenFileSystem(strCompFile);	


	for ( int i = 0; i < nCnt; i++ )
	{
		SFILENODE* pNewFile = m_vectorDeleteFile[i];
/*
		if(strstr(pNewFile->SubPath,NS_GLOBAL_VAR::strCompDir)){
			CString strSubPath, strSubDir;
			
			strSubPath = pNewFile->SubPath;
			strSubDir = strSubPath.Right( strSubPath.GetLength() - (int)strlen("\\package") ); // \\package 파일 뺌
			
			m_FileSystem.ChangeDir(strSubDir);
			
			str.Format( "%s",pNewFile->FileName );
			str.MakeLower();
			str = str.Left( str.ReverseFind ( '.' ) );
			
			m_FileSystem.Remove(pNewFile->FileName);			
			
			continue;
		}
*/

		strDelFile.Format( "%s%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), pNewFile->SubPath, pNewFile->FileName );
		strDelFile.MakeLower();
		strDelFile = strDelFile.Left( strDelFile.ReverseFind ( '.' ) );

		//	압축내 파일인지 확인
		bool bPack = false;
		if( strstr(pNewFile->SubPath,".rcc\\") )	bPack = true;

		if ( !bPack )
		{
			DeleteFile( strDelFile.GetString() );
			//CDebugSet::ToLogFile( sc::string::format("		해당 파일 삭제.  %s", strDelFile.GetString()));
		}
		else
		{
	

			CString strExtPath, strZipFile;				
			strZipFile = strDelFile;
			
			strZipFile = strZipFile.Left ( strZipFile.ReverseFind ( '\\' ) );	//	Zip 파일 경로

            CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
            const SUBPATH* pPath = pApp->GetSubPath();

			strExtPath = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strDownloadTemp; 
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
							CString	strMsg;
							strMsg = ID2LAUNCHERTEXT("IDS_MESSAGE", 76 );
							CHAR * szListMessage = new CHAR[256];
							wsprintf( szListMessage, "%s %s", strMsg.GetString(), pNewFile->FileName );
							::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );

							//CDebugSet::ToLogFile( sc::string::format("		압축 해제 실패.  %s", pNewFile->FileName));

							return FALSE;
						}						
					}

				}
			}

			strDelFile = strExtPath + '\\' + pNewFile->FileName;
			strDelFile = strDelFile.Left ( strDelFile.ReverseFind ( '.' ) );
			DeleteFile( strDelFile.GetString() );
		}

	}

	m_FileSystem.CloseFileSystem();	

	return TRUE;
}

BOOL CAutoPatchThread::DeleteDownFiles()
{
    CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	CString	strTemp;
	int nCnt = static_cast<int>( m_vectorDownFile.size() );

	for ( int i = 0; i < nCnt; ++i )
	{
		SFILENODE* pNewFile = m_vectorDownFile[i];
		strTemp.Format(
            "%s%s%s%s",
            NS_GLOBAL_VAR::strProFile.GetString(),
            pPath->SaveRoot().c_str(), //SUBPATH::SAVE_ROOT, 
            NS_GLOBAL_VAR::strDownloadTemp.GetString(),
            pNewFile->FileName);

		DeleteFile( strTemp.GetString() );

		//CDebugSet::ToLogFile( sc::string::format("		다운로드한 파일 삭제.  %s", strTemp.GetString()));
	}

	strTemp = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strDownloadTemp;
	RemoveDirectory ( strTemp.GetString() );

	//CDebugSet::ToLogFile( sc::string::format("		디렉토리 삭제 파일 삭제.  %s", strTemp.GetString()));

	return TRUE;
}


/*
BOOL CAutoPatchThread::DeleteCompressFile()
{
	CString	strTemp;
	int nCnt = static_cast<int>( m_vecCompFile.size() );

	for ( int i = 0; i < nCnt; ++i )
	{
		SFILENODE* pNewFile = m_vecCompFile[i];
		strTemp.Format( "%s%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), pNewFile->SubPath, pNewFile->FileName );
		strTemp.MakeLower();
		strTemp = strTemp.Left( strTemp.ReverseFind ( '.' ) );

		DeleteFile( strTemp.GetString() );
	}

	// directory 삭제 하는 부분
	CString strDir[5];
	strDir[0].Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(),"\\package\\data\\glogic\\level");
	strDir[1].Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(),"\\package\\data\\glogic\\npctalk");
	strDir[2].Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(),"\\package\\data\\glogic\\quest");
	strDir[3].Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(),"\\package\\data\\glogic");
	strDir[4].Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(),"\\package\\data");

	for(int i = 0; i < 5 ; ++i){
		RemoveDirectory(strDir[i].GetString());			
	}

	return TRUE;

}
*/


BOOL CAutoPatchThread::Destroy()
{
	for ( int i = 0; i < (int)m_vectorPreDownFile.size (); i++ )
	{
		delete m_vectorPreDownFile[i];
	}
	m_vectorPreDownFile.clear();

	for ( int i = 0; i < (int)m_vectorApplyPreDownFile.size (); i++ )
	{
		delete m_vectorApplyPreDownFile[i];
	}
	m_vectorApplyPreDownFile.clear();

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
	m_vectorDownFile.clear(); // MEMO : 중복된 포인터라서 delete 할 필요없음.

	// m_mapDownedFile 삭제 ///////////////////////////
	FILEMAP_ITER iter = m_mapDownedFile.begin ();
	FILEMAP_ITER iter_end = m_mapDownedFile.end ();
	for ( ; iter != iter_end; iter++ )
	{
		delete (*iter).second;
	}
	m_mapDownedFile.clear();
	m_vectorCopyFile.clear();

	// m_mapDownedFile 삭제 ///////////////////////////
	iter = m_mapCopiedFile.begin ();
	iter_end = m_mapCopiedFile.end ();
	for ( ; iter != iter_end; iter++ )
	{
		delete (*iter).second;
	}
	m_mapCopiedFile.clear();
	m_vectorDeleteFile.clear();
	m_vectorPackFile.clear();
	m_vecZipFile.clear();
//	m_vecCompFile.clear();

	return TRUE;
}

BOOL CAutoPatchThread::VersionUp( int sGameVer )
{
	CString strTemp;
	strTemp.Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), g_szClientVerFile );
	SetFileAttributes( strTemp.GetString(), FILE_ATTRIBUTE_NORMAL );

	FILE* cfp = _fsopen( strTemp.GetString(), "rb+", _SH_DENYNO );	
	if ( cfp )
	{		
		fseek( cfp, sizeof ( int ) * 1, SEEK_SET );
		if ( 1 != fwrite( &sGameVer, sizeof( int ), 1, cfp ) )
		{
			fclose( cfp );
			return FALSE;
		}
		fclose( cfp );
	}

	return TRUE;
}

// 사전 다운로드 적용 파일이 없다면 다음으로 진행
// 사전 다운로드 적용 중에 에러 발생한 경우만 FALSE
BOOL CAutoPatchThread::ApplyPreDownFile()
{
	if( NS_GLOBAL_VAR::g_bApplyPreDown == FALSE )
		return TRUE;

	CHAR * szTempMessage = new CHAR[256];
	wsprintf( szTempMessage, "%s", "사전 다운로드 완료된 파일을 적용합니다." );
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szTempMessage );	

	//CDebugSet::ToLogFile( std::string(szTempMessage));

	// Temp 폴더패스를 설정한다
    CAutoPatchManApp* pApp = (CAutoPatchManApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	CString	DownloadDir( NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strDownloadTemp );
	// PreDownloader 폴더패스를 설정한다
	CString	PreDownloadDir( NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + NS_GLOBAL_VAR::strPreDownloadFolder );


	CFileStatus status;
	CString strPreDownFile, strTempDownFile, strAppFile;
	FILEVECTOR::size_type nApplyFileSize = m_vectorApplyPreDownFile.size();

	//CDebugSet::ToLogFile( sc::string::format("		적용해야할 사전다운로드 파일 숫자.  %d", nApplyFileSize));

	// 사전 다운로드된 파일 이동시키고 m_vectorDownFile 에 등록. 만약 사전 다운로드된 파일이 없다면 다운로드해야할 벡터 ( m_vectorServerFile )에 추가
	for ( FILEVECTOR::size_type i = 0; i < nApplyFileSize; i++ )
	{	
		SFILENODE* pApplyFile = m_vectorApplyPreDownFile[i];

		strPreDownFile.Format("%s%s", PreDownloadDir, pApplyFile->FileName);
		strTempDownFile.Format("%s%s", DownloadDir, pApplyFile->FileName);

		if( CFile::GetStatus(strPreDownFile, status) == TRUE )
		{		
			if( SetFileAttributes( strPreDownFile.Left ( strPreDownFile.ReverseFind ( '.' ) ).GetString(), FILE_ATTRIBUTE_NORMAL ) )
			{
				//CDebugSet::ToLogFile( sc::string::format("		[ERROR] strPreDownFile 속성변환 실패.  %s", strPreDownFile.GetString()));
				return FALSE;
			}

			if( SetFileAttributes( strTempDownFile.Left ( strTempDownFile.ReverseFind ( '.' ) ).GetString(), FILE_ATTRIBUTE_NORMAL ) )
			{
				//CDebugSet::ToLogFile( sc::string::format("		[ERROR] strTempDownFile 속성변환 실패.  %s", strTempDownFile.GetString()));
				return FALSE;
			}

#ifdef PREDOWNLOAD
			// 파일 존재유무와 상관없이 무조건 삭제
			//DeleteFile( strTempDownFile );

			// Temp 폴더가 없을수도 있으므로 폴더 생성한다.
			CreateDirectory ( DownloadDir.GetString(), NULL );

			if( CopyFile( strPreDownFile, strTempDownFile, FALSE ) == 0 )
			{
				//CHAR * szTempMessage11 = new CHAR[256];
				//wsprintf( szTempMessage11, "		복사 실패 %s, %s", strPreDownFile, strTempDownFile );
				//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szTempMessage11 );	

				//CDebugSet::ToLogFile( std::string(szTempMessage11));

				return FALSE;
			}
#else
			// 파일 존재유무와 상관없이 무조건 삭제
			DeleteFile( strTempDownFile );

			if( CopyFile( strPreDownFile, strTempDownFile, TRUE ) == 0 )
				return FALSE;
#endif

			if( DeleteFile( strPreDownFile ) == 0 )
			{
				//CHAR * szTempMessage33 = new CHAR[256];
				//wsprintf( szTempMessage33, "		삭제 실패 %s", strPreDownFile );
				//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szTempMessage33 );	

				//CDebugSet::ToLogFile( std::string(szTempMessage33));

				return FALSE;
			}
	
			m_vectorDownFile.push_back(m_vectorApplyPreDownFile[i]);
			m_vectorNewFile.push_back(m_vectorApplyPreDownFile[i]);

			//CDebugSet::ToLogFile( sc::string::format("		PREDOWN 폴더에서 해당 파일 찾음.  %s", m_vectorApplyPreDownFile[i]->FileName));
		}
#ifdef PREDOWNLOAD
		else if( CFile::GetStatus(strTempDownFile, status) == TRUE )
		{
			m_vectorDownFile.push_back(m_vectorApplyPreDownFile[i]);
			m_vectorNewFile.push_back(m_vectorApplyPreDownFile[i]);

			CDebugSet::ToLogFile( sc::string::format("		[ERROR] TEMP 폴더에서 해당 파일 찾음.  %s", m_vectorApplyPreDownFile[i]->FileName));
		}
#endif
		else
		{
			// 사전 다운로드된 파일이 없다면 다운로드 받을수 있도록 m_vectorServerFile 에 추가
			m_vectorServerFile.push_back(m_vectorApplyPreDownFile[i]);

			//CDebugSet::ToLogFile( sc::string::format("		[ERROR] 폴더(TEMP,PREDOWN,CLIENT)에서 해당 파일을 찾을수 없음.  %s", m_vectorApplyPreDownFile[i]->FileName));
#ifdef PREDOWNLOAD
			CPatchSetNode* pNode = m_ClientFileTree.Find( (SFILENODE*)m_vectorApplyPreDownFile[i] );
			if ( pNode )	
			{
				CHAR * szTempMessage44 = new CHAR[256];
				wsprintf( szTempMessage44, "		[ERROR] cFileList.Bin에는 다운완료이지만 파일이 없음. %s", m_vectorApplyPreDownFile[i]->FileName );
				::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szTempMessage44 );	

				CDebugSet::ToLogFile( std::string(szTempMessage44));
				return FALSE;
			}
#endif
		}
	}

	return TRUE;
}

// 기존에는 FileList.bin을 cFileList.bin으로 교체를 했음
// 사전 다운로드가 추가되면서 cFileList에는 isPreDown 상태가 0 또는 1인것만 저장되어야 하므로 변경
BOOL CAutoPatchThread::CreateClientFileList()
{
	if( m_vectorServerFile.size() <= 0 )
	{
		CHAR * szTempMessage = new CHAR[256];
		wsprintf( szTempMessage, "%s", "cFileList 적용에 실패" );
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szTempMessage );	

		//CDebugSet::ToLogFile( std::string(szTempMessage));

		return FALSE;
	}

	//CDebugSet::ToLogFile( sc::string::format("		파일 전체 갯수.  %d", m_vectorServerFile.size()));

	CString strClientPreDown;
	strClientPreDown.Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), NS_GLOBAL_VAR::g_szClientFileList );

	CFile file;

	if (!file.Open(strClientPreDown, CFile::modeCreate | CFile::modeReadWrite))
	{
		return FALSE;
	}
	
	SFILENODE FileInfo;
	SFILENODE* pGetFile = NULL;

	FileInfo.Clear();
	FileInfo.Ver = 3;
	file.Write(&FileInfo, sizeof(SFILENODE));

	FileInfo.Clear();
	FileInfo.Ver = m_vectorServerFile.size();
	file.Write(&FileInfo, sizeof(SFILENODE));

	for( int i = 0; i < m_vectorServerFile.size(); i++ )
	{
		pGetFile = m_vectorServerFile[i];

		//if( pGetFile->IsPreDown == 2 )
		//	CDebugSet::ToLogFile( sc::string::format("		프리다운로드 파일.  %s", pGetFile->FileName));
		//else if( pGetFile->IsPreDown == 1 ) 
		//	CDebugSet::ToLogFile( sc::string::format("		적용된 프리다운로드 파일.  %s", pGetFile->FileName));
		//else if( pGetFile->IsPreDown == 3 ) 
		//	CDebugSet::ToLogFile( sc::string::format("		삭제된 프리다운로드 파일.  %s", pGetFile->FileName));

		FileInfo.Clear();
		FileInfo.SetData(pGetFile);
		file.Write(&FileInfo, sizeof(SFILENODE));
	}

	file.Close();
	return TRUE;
}

/*
BOOL CAutoPatchThread::CompressFile()
{
	CString strCompPath;
	strCompPath = NS_GLOBAL_VAR::strAppPath + RANPARAM::RPFDataPath;

	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 70 ), 0 );

	if(!m_FileSystem.OpenFileSystem(strCompPath)){ // 파일 자체가 없을경우
		m_FileSystem.NewFileSystem(strCompPath);
		m_FileSystem.OpenFileSystem(strCompPath);
	}

    TCHAR strFileName[256]; // 파일 이름
	CString strFilePath; // 파일 전체경로
	CString strSubDir; // 파일시스템 subpath
	CString strSubPath; // 원래 subpath
	CString strTemp;
	for ( int i = 0; i < (int)m_vecCompFile.size (); i++ )
	{
		strSubPath = m_vecCompFile[i]->SubPath;
		strSubDir = strSubPath.Right( strSubPath.GetLength() - (int)strlen("\\package") ); // \\package 이부분 삭제
		if(!m_FileSystem.ChangeDir(strSubDir)){ // 폴더가 생성되지 않았을 경우 폴더 생성
			m_FileSystem.ChangeDir(m_FileSystem.GetCurDir());

			int count = 0;

			if(strcmp(m_FileSystem.GetCurDir(),"/") == 0){ // 여기두 이상 
				strTemp = strSubDir;	
			}else{
				count =(int) ( strSubDir.GetLength() - strlen(m_FileSystem.GetCurDir()));
				strTemp = strSubDir.Right(count); 
			}
			
			CString token;
			
			int curPos = 0;
			token = strTemp.Tokenize("\\",curPos);
			while(token != ""){
				if(!m_FileSystem.AddDir(token))	return FALSE;
				if(!m_FileSystem.ChangeDir(token)) return FALSE;
				token= strTemp.Tokenize("\\",curPos);
			}			
			
		}

		strFilePath.Format("%s%s%s",NS_GLOBAL_VAR::strAppPath.GetString(),m_vecCompFile[i]->SubPath,m_vecCompFile[i]->FileName);
		strFilePath = strFilePath.Left(strFilePath.ReverseFind('.'));
		m_FileSystem.GetNameOnly(strFileName,strFilePath);
		if(m_FileSystem.CheckNameExist((LPCTSTR)strFileName)) // 같은 파일이 있는지 확인
		{
			m_FileSystem.Remove(strFileName);
		}

		if(!m_FileSystem.AddFile(LPCTSTR(strFilePath))) // 파일 추가
		{
			return FALSE;
		}
		if(!m_FileSystem.ChangeDir("/"))  return FALSE;
	}

	m_FileSystem.CloseFileSystem();
	return TRUE;
}
*/
/*
BOOL CAutoPatchThread::OptimizeCompressFile()
{

	CString strCompPath, strPath;
	strCompPath = NS_GLOBAL_VAR::strAppPath + RANPARAM::RPFDataPath;
	strPath = NS_GLOBAL_VAR::strAppPath + "package\\temp.rpf";

	m_FileSystem.OpenFileSystem(strCompPath);

	if(!m_FileSystem.OptimizeFileSystem(strPath)){
		m_FileSystem.CloseFileSystem();
		return FALSE;
	}

	m_FileSystem.CloseFileSystem();
	remove(strCompPath);
	rename(strPath,strCompPath);

	return TRUE;
}
*/

void CAutoPatchThread::SaveVersionInRegistry( int sGameVer )
{
	HKEY Versionkey = NULL;
	TCHAR data_buffer[256];  
	DWORD data_type;  
	DWORD data_size; 

	EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();
	switch ( sp )
	{
	case SP_GS:
		{
			int ret = RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Min Communications\\RanOnlineGS", 0, KEY_ALL_ACCESS, &Versionkey );
			if ( ret == ERROR_SUCCESS )
			{
				::memset( data_buffer, 0, sizeof( data_buffer ) );
				data_size = sizeof( data_buffer );
				int GameVersion = 0;

				// 문자열의 크기를 먼저 읽어온다.
				ret = ::RegQueryValueEx( Versionkey, "Version", 0, &data_type, ( BYTE* )data_buffer, &data_size );
				if ( ret == ERROR_SUCCESS )
				{
					GameVersion = atoi( data_buffer );
					if ( sGameVer != GameVersion )
					{
						TCHAR buffer[12] = { 0, };
						itoa( sGameVer, buffer, 10 );
						RegSetValueEx( Versionkey, "Version", 0, REG_SZ, ( const unsigned char* )buffer, strlen( buffer ) + 1 );
					}
				}
			}
		}
		break;
	}

	if ( Versionkey != NULL )
		RegCloseKey( Versionkey );
}

BEGIN_MESSAGE_MAP(CAutoPatchThread, CHttpThread)
END_MESSAGE_MAP()