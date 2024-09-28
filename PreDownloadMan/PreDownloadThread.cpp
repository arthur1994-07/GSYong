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

	//CDebugSet::ToLogFile(std::string("���� �ٿ�ε� ����"));
	//----------------------------------------------------------------------------------------
	// ��ġ ���� ����Ʈ �ε�;
	if ( !LoadList() )
		goto LFail;

	if ( IsForceTerminate() )
		goto LFail;

	// ��ġ ���� ����Ʈ �� ����;
	if ( !MakeNewList() )
		goto LFail;    

	NS_LOG_CONTROL::SetProcessAllPosition ( 10, 100 );

	if ( IsForceTerminate() )
		goto LFail;

	//	�� ��Ͽ� ���� ���� �ٿ� �ε�;
	CreateDirectory ( str.GetString(), NULL );

	if ( !DownloadFilesByHttp ( m_pHttpPatch ) )
		goto LFail;

	NS_LOG_CONTROL::SetProcessAllPosition ( 70, 100 );

	if ( IsForceTerminate() )
		goto LFail;

	NS_LOG_CONTROL::SetProcessAllPosition ( 80, 100 );

#ifdef PREDOWNLOAD
	// �����ٿ�δ����� �̹� ���ӿ� ����� ������ �����ؼ��� �ȵ�. 21041201
	//if ( !DeleteIsNotDownFile() )
	//	goto LFail;
#else
	if ( !DeleteIsNotDownFile() )
	goto LFail;
#endif

	//CDebugSet::ToLogFile(std::string("�ٿ�ε�� ���� ó��"));

	NS_LOG_CONTROL::SetProcessAllPosition ( 90, 100 );

	DeleteFile( strDownList.GetString() );

	//CDebugSet::ToLogFile(sc::string::format("�ٿ�ε� �� ����Ʈ ����. %s", strDownList.GetString()));

	DeleteFile( cPreFileList.GetString () );
	MoveFile( m_strPreFileList.GetString(), cPreFileList.GetString() );

	//CDebugSet::ToLogFile(sc::string::format("���� �ٿ�ε� ����Ʈ ����. %s", cPreFileList.GetString ()));

	Destroy();

	//CDebugSet::ToLogFile(std::string("���� �ٿ�ε� Destroy"));

#ifdef PREDOWNLOAD
	// ��� ��ġ ����
	NS_LOG_CONTROL::SetProcessAllPosition ( 100, 100 );
	PreDownLoadComplete();
#endif
	
	//CDebugSet::ToLogFile(std::string("���� �ٿ�ε� �Ϸ�"));
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
	// �ٿ�ε� �ߴ� �� ���;
	// ������� �ٿ�ε��� ����;
	SaveDownList();

	DeleteFile( m_strPreFileList.GetString() );

	//CDebugSet::ToLogFile(sc::string::format("		PreDownLoadFail ���� ����. %s", m_strPreFileList.GetString()));

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

	//CDebugSet::ToLogFile(sc::string::format("������ �ٿ���� ���� �ٿ�ε� ���� ����Ʈ �ۼ�. %s", strTemp.GetString()));

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

	//CDebugSet::ToLogFile(sc::string::format("�����ٿ�ε� �ؾ��� ���� ����Ʈ ����� ����. %d", nServerFileSize));

	for ( FILEVECTOR::size_type i = 0; i < nServerFileSize; i++ )
	{	
		SFILENODE* pServerGetFile = m_vectorServerFile[i];

		CPatchSetNode* pNode = m_ClientFileTree.Find ( pServerGetFile );
		if ( pNode )	//	���� ����
		{
			// IsPreDown 3 �ϰ�� ����;
			if ( pServerGetFile->IsPreDown == 3 )
			{
				//CDebugSet::ToLogFile(sc::string::format("�����ٿ�ε� ��Ͽ��� ����. %s", pServerGetFile->FileName));
				continue;
			}

			SFILENODE* pClientFile = pNode->GetFile ();
			pNode->SetUseFlag ();

			if ( pClientFile->Ver < pServerGetFile->Ver )
			{
				SFILENODE* pNewFile = new SFILENODE;
				*pNewFile = *pServerGetFile;
				m_vectorNewFile.push_back ( pNewFile );
				//CDebugSet::ToLogFile(sc::string::format("�����ٿ�ε� �޾ƾ��ϴ� ����(������ �ö� ����). %s", pNewFile->FileName));
			}
			else
			{
				//CDebugSet::ToLogFile(sc::string::format("�̹� �����ٿ�ε� ���� ����. %s", pServerGetFile->FileName));
			}
		}
		else	//	�߰��Ǵ� ����
		{
			SFILENODE* pNewFile = new SFILENODE;
			*pNewFile = *pServerGetFile;
			m_vectorNewFile.push_back ( pNewFile );
			//CDebugSet::ToLogFile(sc::string::format("�����ٿ�ε� �޾ƾ��ϴ� ����(�űԷ� �����ٿ�ε� ���). %s", pNewFile->FileName));
		}

		NS_LOG_CONTROL::SetProcessCurPosition ( i, nServerFileSize );

		if ( IsForceTerminate() )	return FALSE;
	}

	//CDebugSet::ToLogFile(sc::string::format("�����ٿ�ε� �ؾ��� ���� ����Ʈ ����� �Ϸ�. %d", nServerFileSize));


	NS_LOG_CONTROL::SetProcessCurPosition ( 1, 1 );

	m_ClientFileTree.GetNotUseItem( &m_vectorDeleteFile );

	LoadDownList ();	

	return TRUE;
}

BOOL CPreDownloadThread::DownloadFilesByHttp( CHttpPatch* pHttpPatch )
{
	int DownCount = 0;

	// Temp �����н��� �����Ѵ�
    CPreDownloadManApp* pApp = (CPreDownloadManApp*) AfxGetApp();
    const SUBPATH* pPath = pApp->GetSubPath();

	CString	DownloadDir( PreDownloadDefine::g_strProFile + pPath->SaveRoot().c_str() + PreDownloadDefine::g_strPreDownloadDirectory );

	int nVectorNewFileSize = (int)m_vectorNewFile.size();

	//CDebugSet::ToLogFile(sc::string::format("FTP �ٿ�ε� �� ���� ����. %d", nVectorNewFileSize));

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
			//CDebugSet::ToLogFile(sc::string::format("������ �ٿ���� ������. %s", pNewFile->FileName));
			bAlreadyDown = TRUE; // Note : �̹� ���������� �ٿ�ε� �Ϸ�� ��.
		}


		// puase ���¸� resume��ư ������ ������ ��ٸ���;
		WatingForResume();

		if ( IsForceTerminate() ) return FALSE;

		ULONGLONG TotalPos = 10 + (++DownCount * 60) / nVectorNewFileSize;			
		NS_LOG_CONTROL::SetProcessAllPosition( TotalPos, 100 );

		CString FullSubPath = pNewFile->SubPath;

		if ( !bAlreadyDown ) //	Note : ���� �ٿ�ε� �ȵ� �͸� GetFile�Ѵ�.
		{
			if ( !GETFILE_USEHTTP ( FullSubPath.GetString(), pNewFile->FileName ) )
			{
				//CDebugSet::ToLogFile(sc::string::format("[ERROR] FTP �ٿ�ε� ���� . %s", pNewFile->FileName));
				return FALSE;
			}
			//CDebugSet::ToLogFile(sc::string::format("FTP �ٿ�ε� ����. %s", pNewFile->FileName));
		}

		//	<--	���������� �ٿ�ε� ������ ����Ʈ �ۼ��Ѵ�.	--> //
		m_vectorDownFile.push_back( pNewFile );

		if( IsForceTerminate() ) return FALSE;

		Sleep( 0 ); // ���Ϲ޴ٰ� ���ߴ°� ������
	}

	if ( m_vectorNewFile.size() != m_vectorDownFile.size () )
	{
		//CDebugSet::ToLogFile(sc::string::format("[ERROR] ���� ������ ��ġ���� ����. m_vectorNewFile %d, m_vectorDownFile %d", m_vectorNewFile.size(), m_vectorDownFile.size ()));
		return FALSE; // �ٿ���� ���ϰ� ��Ͽ� �ִ� ������ ������ �ٸ��ٸ� ����
	}

	return TRUE;
}

BOOL CPreDownloadThread::GETFILE_USEHTTP( std::string strRemoteSubPath, std::string strFileName, CString strTempDir )
{
	CHttpPatch* pHttpPatch = m_pHttpPatch;
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
		//	���� �����
		if ( IsForceTerminate () ) return FALSE;		

		//	�ִ� �õ� ȸ�� �ʰ���
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

		//	���� �����
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

		//	���೻ �������� Ȯ��
		bool bPack = false;
		if( strstr(pNewFile->SubPath,".rcc\\") )	bPack = true;

		if ( !bPack )
		{
			DeleteFile( strDelFile.GetString() );
			//CDebugSet::ToLogFile(sc::string::format("		!bPack - DeleteIsNotDownFile ���� ����. %s", strDelFile.GetString()));
		}
		else
		{

			CString strExtPath, strZipFile;				
			strZipFile = strDelFile;
			
			strZipFile = strZipFile.Left ( strZipFile.ReverseFind ( '\\' ) );	//	Zip ���� ���

            CPreDownloadManApp* pApp = (CPreDownloadManApp*) AfxGetApp();
            const SUBPATH* pPath = pApp->GetSubPath();

			strExtPath = PreDownloadDefine::g_strProFile + pPath->SaveRoot().c_str() + PreDownloadDefine::g_strPreDownloadDirectory; 
			strExtPath += strZipFile.Right( strZipFile.GetLength() - strZipFile.ReverseFind ( '\\' ) );
			strExtPath = strExtPath.Left ( strExtPath.ReverseFind ( '.' )  );	//	Zip ���� ���� ���


			//	������ ���ٸ�
			if ( GetFileAttributes( strExtPath ) == UINT_MAX )
			{
				//	�ű� Rcc����
				if ( GetFileAttributes( strZipFile ) == UINT_MAX )
				{
					continue;
				}
				else
				{
					//	������ �����Ѵ�.
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
			//CDebugSet::ToLogFile(sc::string::format("		DeleteIsNotDownFile ���� ����. %s", strDelFile.GetString()));
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

	// m_mapDownedFile ���� ///////////////////////////
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