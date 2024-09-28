#include "StdAfx.h"
#include "../SigmaCore/gassert.h"

#include "../EngineLib/Common/SUBPATH.h"

#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLogicDataMan.h"

#include "./PreHttpthread.h"
#include "./Resource.h"
#include "./PreDownloadMan.h"

#include "../EngineLib/DxTools/DebugSet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CPreHttpThread, CLPatchThread)

CPreHttpThread::CPreHttpThread()
: m_pHttpPatch(NULL)
{
}

CPreHttpThread::~CPreHttpThread()
{
}

BOOL CPreHttpThread::LoadServerPreFileList()
{
	CString str;
	str.Format( "%s%s", PreDownloadDefine::g_strAppPath.GetString(), PreDownloadDefine::g_strServerPreFileList );

	FILE* fp = _fsopen( str.GetString(), "rb", _SH_DENYNO );
	if( !fp )
	{
//		CDebugSet::ToLogFile(sc::string::format("[ERROR] 서버용 프리다운로드 리스트가 존재하지 않습니다. %s", str.GetString()));
		return FALSE;
	}

	SFILENODE FileInfo;
	INT nOffset = sizeof(SFILENODE);

	// 파일 개수
	if( 1 != fread ( &FileInfo, nOffset, 1, fp ) )
	{
		fclose ( fp );
//		CDebugSet::ToLogFile(sc::string::format("[ERROR] 서버용 리스트 파일 갯수 확인 실패. %s", str.GetString()));
		return FALSE;
	}

	for( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE* pNewFile = new SFILENODE;
		if( 1 != fread( pNewFile, nOffset, 1, fp ) )
		{
			fclose ( fp );
			//CDebugSet::ToLogFile(sc::string::format("[ERROR] 서버용 리스트 파일 버젼이 일치하지 않습니다. %s", pNewFile->FileName));
			return FALSE;
		}

		m_vectorServerFile.push_back( pNewFile );
	}

	fclose ( fp );

	//CDebugSet::ToLogFile(sc::string::format("서버용 프리다운로드 리스트 확인 완료. %s", str.GetString()));

	return TRUE;
}

BOOL CPreHttpThread::LoadClientPreFileList()
{
	CString str;
	str.Format( "%s%s", PreDownloadDefine::g_strAppPath.GetString(), PreDownloadDefine::g_strClientPreFileList );

	FILE* fp = _fsopen( str.GetString(), "rb", _SH_DENYNO );
	if( !fp )
	{
		//CDebugSet::ToLogFile(sc::string::format("[ERROR] 클라이언트용 프리다운로드 리스트가 존재하지 않습니다. %s", str.GetString()));
		return TRUE;
	}

	SFILENODE FileInfo;
	INT nOffset = sizeof(SFILENODE);

	// 파일 개수
	if( 1 != fread ( &FileInfo, nOffset, 1, fp ) )
	{
		fclose ( fp );
		//CDebugSet::ToLogFile(sc::string::format("[ERROR] 클라이언트용 리스트 파일 갯수 확인 실패. %s", str.GetString()));
		return FALSE;
	}

	for( int i = 0; i < FileInfo.Ver; i++ )
	{
		SFILENODE NewFile;
		if( 1 != fread( &NewFile, nOffset, 1, fp ) )
		{
			fclose ( fp );
			//CDebugSet::ToLogFile(sc::string::format("[ERROR] 클라이언트용 리스트 파일 버젼이 일치하지 않습니다. %s", NewFile.FileName));
			return FALSE;
		}

		m_ClientFileTree.Add( &NewFile );
	}

	fclose ( fp );

	//CDebugSet::ToLogFile(sc::string::format("클라이언트용 프리다운로드 리스트 확인 완료. %s", str.GetString()));

	return TRUE;
}

BOOL CPreHttpThread::LoadList()
{
	//	<--	리스트 읽기	-->	//
	if( !LoadServerPreFileList() )		return FALSE;
	if( IsForceTerminate() )			return FALSE;
	if( !LoadClientPreFileList() )		return FALSE;

	return TRUE;
}

BEGIN_MESSAGE_MAP(CPreHttpThread, CLPatchThread)
END_MESSAGE_MAP()