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

	// ������ ���� �ڵ�
	//

	// 1.4 ��ó�� �� �ִ� ����� ��Ű�� ��ġ �ڵ��̴�. 1.5���� ��ó������
	// ������� �ʴ´�. ���߿� ���ߵ����� 2010 �̻����� ����ȴٸ� �� ������ �´�
	// �������Ű���� ��ġ�� �ø��� �Ʒ� �ּ��� Ǯ� �ڵ���ġ �ǵ��� �Ѵ�.
	// delete �Լ����� �ּ��� Ǯ��� �Ѵ�.
/*	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_011, 0 );
	
	if ( !GETFILE_USEHTTP ( pHttpPatch, "\\", NS_GLOBAL_VAR::g_szVcredist_x86CabFile, "" ) )
	{
		if ( !IsForceTerminate() )
		{
			CHAR * szListMessage = new CHAR[MAX_PATH];
			StringCchCopy( szListMessage, MAX_PATH, pHttpPatch->GetErrMsg() );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, (LPARAM)szListMessage );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_020, 0 );
			SetFail (); // MEMO : ó������ �ٽ� �ٿ�޾ƾ� �Ѵ�.
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

			SetExtractError(); // Cab ���� Ǯ�� ����

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
			SetFail (); // MEMO : ó������ �ٽ� �ٿ�޾ƾ� �Ѵ�.
		}
		goto LFail;
	}
	else ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_013, 0 );

	//	������ Ran Auto Patch ���� ( *.exe )
	//	
	DELETEFILE( NS_GLOBAL_VAR::g_szLauncherCabFile, FALSE ); // cab Ȯ���ڰ� ������� false
	//DELETEFILE( NS_GLOBAL_VAR::g_szVcredist_x86CabFile, FALSE );

	if ( IsForceTerminate() ) goto LFail;

	// ������ ���� ���� ����
	DeleteLauncherFile( NS_GLOBAL_VAR::g_szLauncherCabFile );
	//DeleteLauncherFile( NS_GLOBAL_VAR::g_szVcredist_x86CabFile );

	//	�� ���� ��ġ
	::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_014, 0 );

	str.Format(
        "%s%s%s",
        NS_GLOBAL_VAR::strProFile,
        m_pPath->SaveRoot().c_str(), //SUBPATH::SAVE_ROOT,
        NS_GLOBAL_VAR::g_szLauncherCabFile);
    if (!mincab::Extract(m_szAppPath, str.GetString()))
	{
        AfxMessageBox(mincab::GetErrorMsg(), MB_OK);

		SetExtractError(); // Cab ���� Ǯ�� ����

		if ( !IsForceTerminate() )
		{
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_012, 0 );
			SetFail ();
		}
		goto LFail;
	}
	else ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_015, 0 );

	if ( IsForceTerminate() ) goto LFail;

	//	Ran Auto Patch ���� �ø���
	//
	if ( !VersionUp( sPatchVer ) )
	{		
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_016, 0 );
		goto LFail;
	}
	else ::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, IDS_MESSAGE_017, 0 );

	DELETEFILE( NS_GLOBAL_VAR::g_szLauncherCabFile ); // �ٿ�ε� �� Ran Auto Patch ���� ( *.exe.cab )
	//DELETEFILE( NS_GLOBAL_VAR::g_szVcredist_x86CabFile );

	return;

LFail:
	DELETEFILE( NS_GLOBAL_VAR::g_szLauncherCabFile ); // �ٿ�ε� �� Ran Auto Patch ���� ( *.exe.cab )
	//DELETEFILE( NS_GLOBAL_VAR::g_szVcredist_x86CabFile );
}

BOOL CHttpPatchThread::GETFILE_USEHTTP( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir )
{
	if ( !pHttpPatch )
	{
        GASSERT( 0 && "�߸��� ������ ������ �� �ֽ��ϴ�." );
		return FALSE;
	}

	if ( !strFileName.size () )
	{
		GASSERT( 0 && "������ �������� �ʾҽ��ϴ�." );
		return FALSE;
	}

	//	'\\'���ڸ� '/'�� �����Ѵ�.
	std::replace ( strRemoteSubPath.begin(), strRemoteSubPath.end(), '\\','/' );	

	EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();

	// MEMO : �н� ����
	//#if !defined(_DEBUG) && !defined(DAUMPARAM) && !defined(_RELEASED) && !defined(GS_PARAM) && !defined(ID_PARAM) 
#ifndef _DEBUG
	
	// 20131030 khkim
	// �� �������� ��ġ ����� �ٸ���. ���� �ڵ�� �ݴ�� �Ǿ��־��� ������ �����Ѵ�.
	// �ѱ�,GS �� ������ ��� ������ ��ġ��ȣ�ؿ� ��ġ������ �����ϴ� ����̸�,
	// �ѱ�,GS �� �� ����� ������� �ʰ� �����Ŵ������� ���� FTP�� ���ӱ��� �ؼ� 
	// ������ ����ؼ� ���㾲�� ����̴�. 
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
		//		�ִ� �õ� ȸ�� �ʰ���
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

		//	�� üũ�� ����ߴٴ� ���� nADDRESS_NULL_COUNT�� �ʱ�ȭ�ؾ����� �ǹ��Ѵ�.
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
			return TRUE; // NET_OK, ������ ��
		}

		Sleep( 0 );
	}

	return FALSE;
}

BEGIN_MESSAGE_MAP(CHttpPatchThread, CLPatchThread)
END_MESSAGE_MAP()