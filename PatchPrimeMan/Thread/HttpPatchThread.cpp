#include "StdAfx.h"

#include <algorithm>

#include "./HttpPatchThread.h"
#include "../Resource.h"

#include "../../MinWebLauncher/s_CHttpPatch.h"
#include "../GlobalVariable.h"
#include "../Log/LogControl.h"
#include "../../MfcExLib/ExLibs/CompactFdi.h"
#include "../../RanLogic/RANPARAM.h"
#include "../../RanLogic/GLogicDataMan.h"
#include "../../EngineLib/Common/SUBPATH.h"
#include "../../SigmaCore/gassert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CHttpPatchThread, CLPatchThread)

CHttpPatchThread::CHttpPatchThread( LPVOID pParam, const TCHAR * szAppPath, DWORD nThreadID, SUBPATH* pPath )
    : CLPatchThread( pParam, szAppPath, nThreadID, pPath )
{
}

void CHttpPatchThread::LauncherPatch()
{
	CHttpPatch* pHttpPatch	= m_pPatchThreadParam->pHttpPatch;
	const int sPatchVer		= m_pPatchThreadParam->sPatchVer;

	CString str;

	if ( IsForceTerminate() )
        return;

	// 스레드 종료 코드
	//

	// 1.4 런처에 들어가 있는 재배포 패키지 설치 코드이다. 1.5버전 런처에서는
	// 사용하지 않는다. 나중에 개발도구가 2010 이상으로 변경된다면 그 버전에 맞는
	// 재배포패키지를 패치에 올리고 아래 주석을 풀어서 자동설치 되도록 한다.
	// delete 함수들의 주석도 풀어야 한다.
/*	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_011, 0 );
	
	if ( !GETFILE_USEHTTP ( pHttpPatch, "\\", NS_GLOBAL_VAR::g_szVcredist_x86CabFile, "" ) )
	{
		if ( !IsForceTerminate() )
		{
			CHAR * szListMessage = new CHAR[MAX_PATH];
			StringCchCopy( szListMessage, MAX_PATH, pHttpPatch->GetErrMsg() );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_020, 0 );
			SetFail (); // MEMO : 처음부터 다시 다운받아야 한다.
		}
		goto LFail;
	}
	else ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_013, 0 );

	{
		CString str;
		str.Format(
			"%s%s%s",
			NS_GLOBAL_VAR::strProFile,
			m_pPath->SaveRoot().c_str(), //SUBPATH::SAVE_ROOT,
			NS_GLOBAL_VAR::g_szVcredist_x86CabFile);
		if (!mincab::Extract(m_szAppPath, str.GetString()))
		{
			AfxMessageBox(mincab::GetErrorMsg(), MB_OK);

			SetExtractError(); // Cab 파일 풀기 실패

			if ( !IsForceTerminate() )
			{
				::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_012, 0 );
				SetFail ();
			}
			goto LFail;
		}
		else ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_015, 0 );

		CString strVALUE("/q /T:c:\temp");
		CString strTemp;
		CString strVcredistFile( NS_GLOBAL_VAR::g_szVcredist_x86CabFile );
		strVcredistFile = strVcredistFile.Left( strVcredistFile.ReverseFind( '.' ) );

		strTemp.Format( "\"%s%s\"", m_szAppPath,strVcredistFile);


		if( ! ShellExecute( NULL , "open", strTemp.GetString() ,strVALUE.GetString(), NULL, SW_SHOW) )
		{
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_012, 0 );
			SetFail ();
		}
	}
*/
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_011, 0 );

	if ( !GETFILE_USEHTTP ( pHttpPatch, "\\", NS_GLOBAL_VAR::g_szLauncherCabFile, "" ) )
	{
		if ( !IsForceTerminate() )
		{
			CHAR * szListMessage = new CHAR[MAX_PATH];
			StringCchCopy( szListMessage, MAX_PATH, pHttpPatch->GetErrMsg() );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_020, 0 );
			SetFail (); // MEMO : 처음부터 다시 다운받아야 한다.
		}
		goto LFail;
	}
	else ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_013, 0 );

	//	구버전 Ran Auto Patch 삭제 ( *.exe )
	//	
	DELETEFILE( NS_GLOBAL_VAR::g_szLauncherCabFile, FALSE ); // cab 확장자가 없을경우 false
	//DELETEFILE( NS_GLOBAL_VAR::g_szVcredist_x86CabFile, FALSE );

	if ( IsForceTerminate() ) goto LFail;

	// 구버전 런쳐 파일 삭제
	DeleteLauncherFile( NS_GLOBAL_VAR::g_szLauncherCabFile );
	//DeleteLauncherFile( NS_GLOBAL_VAR::g_szVcredist_x86CabFile );

	//	새 파일 설치
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_014, 0 );

	str.Format(
        "%s%s%s",
        NS_GLOBAL_VAR::strProFile,
        m_pPath->SaveRoot().c_str(), //SUBPATH::SAVE_ROOT,
        NS_GLOBAL_VAR::g_szLauncherCabFile);
    if (!mincab::Extract(m_szAppPath, str.GetString()))
	{
        AfxMessageBox(mincab::GetErrorMsg(), MB_OK);

		SetExtractError(); // Cab 파일 풀기 실패

		if ( !IsForceTerminate() )
		{
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_012, 0 );
			SetFail ();
		}
		goto LFail;
	}
	else ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_015, 0 );

	if ( IsForceTerminate() ) goto LFail;

	//	Ran Auto Patch 버전 올리기
	//
	if ( !VersionUp( sPatchVer ) )
	{		
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_016, 0 );
		goto LFail;
	}
	else ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_017, 0 );

	DELETEFILE( NS_GLOBAL_VAR::g_szLauncherCabFile ); // 다운로드 한 Ran Auto Patch 삭제 ( *.exe.cab )
	//DELETEFILE( NS_GLOBAL_VAR::g_szVcredist_x86CabFile );

	return;

LFail:
	DELETEFILE( NS_GLOBAL_VAR::g_szLauncherCabFile ); // 다운로드 한 Ran Auto Patch 삭제 ( *.exe.cab )
	//DELETEFILE( NS_GLOBAL_VAR::g_szVcredist_x86CabFile );
}

BOOL CHttpPatchThread::GETFILE_USEHTTP( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir )
{
	if ( !pHttpPatch )
	{
        GASSERT( 0 && "잘못된 연산을 수행할 수 있습니다." );
		return FALSE;
	}

	if ( !strFileName.size () )
	{
		GASSERT( 0 && "파일이 지정되지 않았습니다." );
		return FALSE;
	}

	//	'\\'문자를 '/'로 변경한다.
	std::replace ( strRemoteSubPath.begin(), strRemoteSubPath.end(), '\\','/' );	

	EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();

	// MEMO : 패스 설정
	//#if !defined(_DEBUG) && !defined(DAUMPARAM) && !defined(_RELEASED) && !defined(GS_PARAM) && !defined(ID_PARAM) 
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
				strFolder += strRemoteSubPath.c_str();
				strRemoteSubPath = strFolder;
			}
		}
		break;
	}

#endif

	strRemoteSubPath += strFileName;

	std::string strLocalFullPath;
	strLocalFullPath += NS_GLOBAL_VAR::strProFile.GetString();	
	strLocalFullPath += m_pPath->SaveRoot(); //SUBPATH::SAVE_ROOT;
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
		if( IsForceTerminate() ) return FALSE;

		//	NOTE
		//		최대 시도 회수 초과시
		if ( nADDRESS_NULL_COUNT == RANPARAM::MAX_HTTP ) return FALSE;

		if ( RANPARAM::MAX_HTTP == nTRY ) nTRY = 0;
		if ( nERROR_RETRY == 3 ) return FALSE;		

		static const CString strHTTP = "http://";

		CString strRealAddress = RANPARAM::HttpAddressTable[nTRY];
		if ( !strRealAddress.GetLength () )
		{
			nADDRESS_NULL_COUNT++;
			nTRY++;
			continue;
		}

		//	널 체크를 통과했다는 것은 nADDRESS_NULL_COUNT을 초기화해야함을 의미한다.
		nADDRESS_NULL_COUNT = 0;

		CString strHttpAddress = strHTTP + strRealAddress;

		if ( NET_ERROR == pHttpPatch->SetBaseURL ( strHttpAddress ) )
		{
			//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_020, 0 );
			nTRY++;
			nERROR_RETRY++;
			continue;
		}

		if ( NET_ERROR == pHttpPatch->GetFile ( strRemoteSubPath.c_str(), strLocalFullPath.c_str() ) )
		{
			//::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_020, 0 );
			nTRY ++;
			nERROR_RETRY++;
			continue;
		}

		if( IsForceTerminate() ) return FALSE;

		ULONGLONG ulRECEIVED, ulTOTALSIZE;
		NS_LOG_CONTROL::GetProcessCurPosition ( &ulRECEIVED, &ulTOTALSIZE );

		if ( ulRECEIVED != ulTOTALSIZE )
		{
			nTRY_FILESIZE_CHECK++;
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_018, 0 );
			continue;
		}
		else
		{
			return TRUE; // NET_OK, 성공한 것
		}

		Sleep( 0 );
	}

	return FALSE;
}

BEGIN_MESSAGE_MAP(CHttpPatchThread, CLPatchThread)
END_MESSAGE_MAP()