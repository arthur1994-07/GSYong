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
	//str.Format( "%s%s", NS_GLOBAL_VAR::strAppPath.GetString(), NS_GLOBAL_VAR::g_szServerPreDownFileList ); // PREDOWNLOADER // �����ٿ�ε� �׽�Ʈ�� ���ؼ� �ӽ÷� ������ ���Ͽ��� �е��� ����. �Ϸ��� �ڵ� ���� �ʿ�

	FILE* fp = _fsopen( str.GetString(), "rb", _SH_DENYNO );
	if( !fp ) return FALSE;

	SFILENODE FileInfo;

	// ù��° ���� ����
	if( 1 != fread ( &FileInfo, VER_1_OFFSET, 1, fp ) )
	{
		fclose ( fp );
		return FALSE;
	}

	// ���� �ɼ� ����
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

	// ���� ������ �ɼ� ��ŭ ���� �������� �ڷ� �̵��Ѵ�.
	LONG lOffset = nVerOffset - VER_1_OFFSET;

	if( fseek( fp, lOffset, SEEK_CUR) )
	{
		fclose ( fp );
		return FALSE;
	}

	// �ι�° ���� ����
	if( 1 != fread ( &FileInfo, nVerOffset, 1, fp ) )
	{
		fclose ( fp );
		return FALSE;
	}

	// Ran ���� ������ ����
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
			// ���� �ٿ�ε��ϰų� ���� �ٿ�ε�� ������ �����ϴ� ��쿡�� ���Ϳ� ��Ƽ� ���� ó��
			// ���°��� 3���� ���� �ٿ�ε� ������ preFileList.bin �� �߰���Ű�� ������ PreDownloader���� cpreFileList.bin�� ���ؼ� �˾Ƽ� �����Ѵ�.
			if( pNewFile->IsPreDown == 2 )
			{
				if( NS_GLOBAL_VAR::g_bNeedPreDownloader == FALSE )
					NS_GLOBAL_VAR::g_bNeedPreDownloader = TRUE;

				m_vectorPreDownFile.push_back( pNewFile );

				//NS_GLOBAL_VAR::g_bApplyPreDown = TRUE; // PREDOWNLOADER // �׽�Ʈ�� ���ؼ� �ӽ÷� �����ϵ��� ����
				//m_vectorApplyPreDownFile.push_back( pNewFile ); // PREDOWNLOADER // �׽�Ʈ�� ���ؼ� �ӽ÷� �����ϵ��� ����
			}
			else if( pNewFile->IsPreDown == 1 )
			{
				// ���� �ٿ�ε� �����ؾ��ϴ� ������ �ִµ� �����ٿ�ε尡 �������̶�� ��ó�� ����ǰ� ���� �ٿ�ε带 �Ϸ��ϰų� ���� �����ؾ� ��
				if( NS_GLOBAL_VAR::g_bRunningPreDownloader == TRUE )
				{
					// ��ó ����
					// ��ó ���۽ÿ� PreDownloader �� �������̶�� üũ�� �� ������ ���� ����ڰ� ��ó ������ �Ŀ� ���Ƿ� PreDownloader�� �����ϴ� ��� ������ �߻��� �� ����.
					// �׷��ٰ� PreDownloader ���ۻ翡 ��ó�� �������̶�� ������ �� ������ �ҷ��� �ص� ��ó���� PreDownloader�� �����ϹǷ� �Ұ�����........
					::AfxMessageBox(_T("���� �ٿ�ε�� ������ �����ؾ� �ϴµ� PreDownloader�� �������Դϴ�."));
					return FALSE;
				}

				// IsPreDown �� 1�� ���߿��� �̹� ���ӿ� ����Ǿ� �ִ°��� ó������ ����
				// ���� ��� ������ ����

				strAppFile.Format("%s%s%s", AppDir.Left ( AppDir.ReverseFind ( '\\' ) ), pNewFile->SubPath, pNewFile->FileName);
				// ftp�� �ö� ������ cab Ȯ�����̰� ���� ������ cab ������ �����̹Ƿ� Ȯ���� ó��
				if(CFile::GetStatus(strAppFile.Left ( strAppFile.ReverseFind ( '.' ) ), status) == FALSE)
				{
					// ���� �ٿ�ε�� ���Ŀ� �������� ������ ������ ���� Ŭ���̾�Ʈ ���Ͽ� ���ٸ� �̹��� ����Ǵ� �����̴�.
					// ������ �̹� ����� ������ �������� ������ ���ؼ� ���°�찡 �߻���. 
					// ���� Temp ������ Ȯ���ؼ� ������ �����ٿ�� ���� �����̰� ���ٸ� �ٽ� �ٿ�ε� �޾ƾ���.
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

	// ���� �ɼ� ����
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

	// ���� ������ �ɼ� ��ŭ ���� �������� �ڷ� �̵��Ѵ�.
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
	//	<--	����Ʈ �б�	-->	//
	// FileList.bin �� �о ���� �ٿ�ε� �� ������ �ִٸ� ���� �����ϰ� CreatePrePatchFileList���� preFileList.bin �� ���
	if( !LoadListServer() )
	{
		//CDebugSet::ToLogFile( std::string("[ERROR] ���� ����Ʈ ��� �б� ����"));
		return FALSE;
	}
	// preFileList.bin ����
	if( !CreatePrePatchFileList() )
	{
		//CDebugSet::ToLogFile( std::string("[ERROR] �����ٿ�ε� ����Ʈ ��� �б� ����"));
		return FALSE;
	}

	if( IsForceTerminate() )	return FALSE;
	if( !LoadListClient() )
	{
		//CDebugSet::ToLogFile( std::string("[ERROR] Ŭ���̾�Ʈ ����Ʈ ��� �б� ����"));
		return FALSE;
	}

	return TRUE;
}

BOOL CHttpThread::GETFILE_USEHTTP( CHttpPatch* pHttpPatch, std::string strRemoteSubPath, std::string strFileName, CString strTempDir )
{
	if ( !pHttpPatch )
	{
		GASSERT ( 0 && "�߸��� ������ ������ �� �ֽ��ϴ�." );
		return FALSE;
	}

	if ( !strFileName.size () )
	{
		GASSERT ( 0 && "������ �������� �ʾҽ��ϴ�." );
		return FALSE;
	}

	std::string strSubPath( strRemoteSubPath );

	//	'\\'���ڸ� '/'�� �����Ѵ�.
	std::replace ( strSubPath.begin(), strSubPath.end(), '\\','/' );

	GLogicData::GetInstance().LoadCountryFile();
	EMSERVICE_PROVIDER sp = GLogicData::GetInstance().GetServiceProvider();

	//#if !defined(_DEBUG) && !defined(KR_PARAM) && !defined(GS_PARAM) && !defined(_RELEASED) && !defined(ID_PARAM)
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
		//	���� �����
		if ( IsForceTerminate () ) return FALSE;		

		//	NOTE
		//		�ִ� �õ� ȸ�� �ʰ���
		if ( nADDRESS_NULL_COUNT == RANPARAM::MAX_HTTP ) return FALSE;

		if ( RANPARAM::MAX_HTTP == nTRY ) nTRY = 0;
		if ( nERROR_RETRY == 5 ) return FALSE;		

		static const CString strHTTP = "http://";

		CString strRealAddress = RANPARAM::HttpAddressTable[nTRY];
		if ( !strRealAddress.GetLength () )
		{
			nADDRESS_NULL_COUNT++;		//	MAX_HTTP�� ��� ���ΰ�?
			nTRY++;
			continue;
		}

		//	�� üũ�� ����ߴٴ� ���� nADDRESS_NULL_COUNT�� �ʱ�ȭ�ؾ����� �ǹ��Ѵ�.
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

		//	���� �����
		if (IsForceTerminate())
            return FALSE;	

		ULONGLONG ulRECEIVED, ulTOTALSIZE;
		NS_LOG_CONTROL::GetProcessCurPosition ( &ulRECEIVED, &ulTOTALSIZE );

		if ( ulRECEIVED != ulTOTALSIZE )
		{
			nTRY_FILESIZE_CHECK++;
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, (WPARAM)ID2LAUNCHERTEXT("IDS_MESSAGE", 46 ), 0 ); // ���� �޴� ���� ũ�Ⱑ ���� �ʴ�.
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
		// preFileList.bin �� �̹� �����ϰ� PreDownloader ���� ���̶�� �ƹ��� ó���� ���� �ʰ� 
		// �׷��� ������ �˼����� ������ ���� preFileList.bin�� �����ϴ� ���̹Ƿ� ����
		return FALSE;
	}
	else
	{
		// ���� ���Ͽ� ����ų� ���� ����
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