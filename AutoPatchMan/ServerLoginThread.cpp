#include "StdAfx.h"
#include ".\serverloginthread.h"
#include "Resource.h"

#include "../EngineLib/Common/SUBPATH.h"
#include "../MfcExLib/ExLibs/CompactFdi.h"

#include "./AutoPatchMan.h"
#include "./GlobalVariable.h"
#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLogicDataMan.h"

#include "../RanLogicClient/Network/s_NetClient.h"
#include "LauncherText.h"
#include "../BugTrap/BugTrap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CServerLoginThread, CLPatchThread)

CServerLoginThread::CServerLoginThread(
    LPVOID pParam,
    const CString& strLoginAddress,
    DWORD nThreadID,
    int LoginPort)
    : m_strLoginAddress(strLoginAddress)
    , m_nDlgThreadID(nThreadID)
    , m_LoginPort(LoginPort)
{
	VERIFY( pParam != NULL);
	m_pNetClient = (CNetClient*) pParam;
	m_pHttpPatch = new CHttpPatch;
}

void CServerLoginThread::ThreadMain()
{
    BT_SetTerminate();

	if (IsForceTerminate())
        return;

	// GETFILE_USEHTTP 안에 코드 보면 GS 만 사용하도록 되어있다.
	// 로그인서버 주소가 param.ini 에 파일안에 들어있어서 로그인서버 주소가 변경될 경우 어려움이 있었는데
	// 로그인서버에 접속하기 전에 패치서버에서 param.ini 파일을 먼저 받는 과정을 추가하였다. 로그인서버 주소가
	// 변경됭 경우 패치서버에 변경된 로그인 서버 주소가 들어있는 param.ini.cab 파일을 올리면 그 주소로 접속을 할것이다.
	// 이 기능이 들어간 런처를 받지 못한 유저들은 수동패치를 제공해야 한다.
	if ( GETFILE_USEHTTP( m_pHttpPatch, "\\", "param.ini.cab", "" ) )
	{
		CAutoPatchManApp* pApp = ( CAutoPatchManApp* ) AfxGetApp();
		const SUBPATH* pPath = pApp->GetSubPath();
		CString str = NS_GLOBAL_VAR::strProFile + pPath->SaveRoot().c_str() + "param.ini.cab";

		if ( mincab::Extract( NS_GLOBAL_VAR::strAppPath.GetString (), str.GetString() ) )
		{
			RANPARAM::LOAD( 
				NS_GLOBAL_VAR::strAppPath.GetString(),
				pApp->GetServiceProvider(),
				false,
				false );

			m_strLoginAddress = RANPARAM::LoginAddress;

			DeleteFile( str.GetString() );
		}
	}

	int nRetCode = m_pNetClient->ConnectLoginServer(m_strLoginAddress.GetString(), m_LoginPort);
	if( nRetCode != NET_OK )
	{
		switch ( nRetCode )
		{
			case CNetClient::NET_ERR_CREATE_EVENT:	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 20 ), 0 );  break;
			case CNetClient::NET_ERR_EVENT_SELECT:	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 21 ), 0 );  break;
			case CNetClient::NET_ERR_CONNECT:		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 22 ), 0 );  break;
			case CNetClient::NET_ERR_EVENT_WAIT:	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 23 ), 0 );  break;
			case CNetClient::NET_ERR_CONTACT_EVENT: ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 24 ), 0 );  break;
			case CNetClient::NET_ERR_CREATE_THREAD: ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 25 ), 0 );  break;

			default: ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 26 ), 0 ); 
		}

		SetFail();
		return;
	}

	if( IsForceTerminate() ) return;

    m_pNetClient->GetClientVer();
	nRetCode = m_pNetClient->SndVersion();
	if( nRetCode <= NET_ERROR )
	{
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING,  (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 27 ), 0 );  // 버전 전송에 실패
		return;
	}
}

BOOL CServerLoginThread::GETFILE_USEHTTP( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir )
{
	if ( !pHttpPatch )
	{
		return FALSE;
	}

	if ( !strFileName.size () )
	{
		return FALSE;
	}

	std::string strSubPath( strRemoteSubPath );

	//	'\\'문자를 '/'로 변경한다.
	std::replace( strSubPath.begin(), strSubPath.end(), '\\','/' );

	GLogicData::GetInstance().LoadCountryFile();
	EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();

	if ( sp != SP_GS )
	{
		return FALSE;
	}

	strSubPath += strFileName;

	CAutoPatchManApp* pApp = ( CAutoPatchManApp* ) AfxGetApp();
	const SUBPATH* pPath = pApp->GetSubPath();

	std::string strLocalFullPath;
	strLocalFullPath += NS_GLOBAL_VAR::strProFile.GetString();	
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

	srand( ( unsigned )time( NULL ) );

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

		std::string DownUrl( strHttpAddress.GetString() );
		DownUrl.append( strSubPath );

		if ( NET_ERROR == pHttpPatch->SetBaseURL( strHttpAddress ) )
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
		if ( IsForceTerminate() )
			return FALSE;

		return TRUE;

		Sleep( 0 );
	}

	return FALSE;
}

BEGIN_MESSAGE_MAP(CServerLoginThread, CLPatchThread)
END_MESSAGE_MAP()