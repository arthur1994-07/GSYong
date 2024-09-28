#include "stdafx.h"

#include "../LogControl.h"
#include "../HttpPatch.h"
#include "./PatchThread.h"

#ifndef NET_ERROR
#define NET_ERROR -1
#endif

FILEVECTOR PatchThread::m_ExtractedFile;

PatchThread::PatchThread( DWORD nDlgThreadID, CHttpPatch* pHttpPatch )
: EventThread( nDlgThreadID, 1, false )
, m_pHttpPatch( pHttpPatch )
{
	start();
}

PatchThread::~PatchThread()
{   
	end();
}

void PatchThread::AddJob( PatchFile& File )
{
	m_Job.push( File );
}


bool PatchThread::Empty()
{
	return m_Job.empty();
}

void PatchThread::start()
{
	startThread();
}

void PatchThread::end()
{
	endThread();
}

void PatchThread::endHook()
{
}

int PatchThread::process()
{
	return DownloadFile();
}

int PatchThread::DownloadFile()
{
	if ( m_Job.empty() )
	{
		return 0;
	}

	PatchFile& File = m_Job.front();

	std::string strLocalPath = PathFindFileName( File.FileName.c_str() );

	std::string strDownloadPath( autopatch::strRanOnlineSavePath );
	strDownloadPath += strLocalPath/*File.FileName*/;

	std::string strSubPath( "/" );
	switch ( ENGINE::emServiceProvider )
	{
	case SP_OFFICE_TEST:
	case SP_KOREA:
	case SP_KOREA_TEST:
	case SP_GS:
		break;
	default:
		{
			if ( File.GameVersion != -1 )
			{
				CString strFolder;
				strFolder.Format( "%04d", File.GameVersion );
				strSubPath += strFolder.GetString();
				strSubPath += "/";
			}
		}
		break;
	}

	strSubPath += File.FileName;

	int nHttpIndex = 0;
	for ( int i = 0; i < autopatch::MAX_HTTP; ++i ) 
	{
		std::string strHttpAddressTemp = autopatch::HttpAddressTable[i];
		if ( !strHttpAddressTemp.empty() )
		{
			nHttpIndex = i;
			break;
		}
	}

	std::string strHttpAddress = autopatch::HttpAddressTable[nHttpIndex];
	if ( strHttpAddress.empty() )
	{
		CString* pMessage = new CString;
		pMessage->Format( "error strHttpAddress.empty() %s.", File.FileName.c_str() );
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, ( LPARAM ) pMessage );
		return -1;
	}

	std::string strRealHttpAddress( "http://" );
	strRealHttpAddress += strHttpAddress;

	if ( NET_ERROR == m_pHttpPatch->SetBaseURL ( strRealHttpAddress.c_str() ) )
	{
		CString* pMessage = new CString;
		pMessage->Format( "error SetBaseURL %s.", File.FileName.c_str() );
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, ( LPARAM ) pMessage );
		return -1;
	}

	{
		CString* pMessage = new CString;
		pMessage->Format( "%s", File.FileName.c_str() );
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, ( LPARAM ) pMessage );
	}

	if ( NET_ERROR == m_pHttpPatch->GetFile( strSubPath.c_str(), strDownloadPath.c_str() ) )
	{
		CString* pMessage = new CString;
		pMessage->Format( "error GetFile %s.", File.FileName.c_str() );
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, ( LPARAM ) pMessage );
		return -1;
	}

	DWORD RECEIVED, TOTALSIZE;
	NS_LOG_CONTROL::GetProcessCurPosition ( &RECEIVED, &TOTALSIZE );

	if ( RECEIVED != TOTALSIZE )
	{
		CString* pMessage = new CString;
		pMessage->Format( "error RECEIVED != TOTALSIZE %s.", File.FileName.c_str() );
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, ( LPARAM ) pMessage );
		return -1;
	}

	std::wstring strDownloadPathW = autopatch::ansiToUnicode( strDownloadPath );
	std::wstring strTargetPathW = autopatch::ansiToUnicode( autopatch::strRanOnlinePath );

	if ( false == File.FilePath.empty() )
	{
		std::wstring strTargetPathExpandW = autopatch::ansiToUnicode( File.FilePath );
		strTargetPathW += strTargetPathExpandW;
	}

	Cabinet::CExtract i_ExtrFile;

	if ( File.NeedVersionUpdate )
	{
		Cabinet::CExtract::kCallbacks k_Callbacks;
		k_Callbacks.f_OnBeforeCopyFile = &PatchThread::OnBeforeCopySingleFile;
		i_ExtrFile.SetCallbacks( &k_Callbacks );
	}

	if ( !i_ExtrFile.CreateFDIContext() )
	{
		CString* pMessage = new CString;
		pMessage->Format( "error CreateFDIContext %s.", File.FileName.c_str() );
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, ( LPARAM ) pMessage );
		return -1;
	}

	if ( !i_ExtrFile.ExtractFileW( strDownloadPathW.c_str(), strTargetPathW.c_str(), ( void* )&File ) )
	{
		CString* pMessage = new CString;
		pMessage->Format( "error ExtractFileW %s", File.FileName.c_str() );
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, ( LPARAM ) pMessage );
		return -1;
	}

	i_ExtrFile.CleanUp();

	if ( autopatch::strVcredistCabFileName == File.FileName )
	{
		CString strVcredistFileName = autopatch::strVcredistCabFileName.c_str();
		strVcredistFileName = strVcredistFileName.Left( strVcredistFileName.ReverseFind ( '.' ) );
		CString strParameter("/q /T:c:\temp");
		CString strTemp;
		strTemp.Format( "\"%s%s\"", autopatch::strRanOnlinePath.c_str(), strVcredistFileName.GetString() );
		if ( !ShellExecute( NULL , "open", strTemp.GetString() ,strParameter.GetString(), NULL, SW_SHOW ) )
		{
			CString* pMessage = new CString;
			pMessage->Format( "error ShellExecute %s.", File.FileName.c_str() );
			::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, ( LPARAM ) pMessage );
			return -1;
		}
	}

	// download end message
	{
		CString* pMessage = new CString;
		pMessage->Format( "%s download ok.", File.FileName.c_str() );
		::PostThreadMessage( m_nDlgThreadID, WM_LISTADDSTRING, 0, ( LPARAM ) pMessage );
	}

	m_Job.pop();
	return 1;
}

BOOL PatchThread::OnBeforeCopySingleFile( Cabinet::CExtract::kCabinetFileInfo* pk_Info, void* p_Param )
{
	CString ExtractedFileName( pk_Info->u16_File );
	ExtractedFileName += ".cab";

	PatchFile* pPatchFile = ( PatchFile* ) p_Param;
	if ( !pPatchFile )
	{
		return FALSE;
	}

	SFILENODE* pNewFile = new SFILENODE;
	pNewFile->SetFileName( ExtractedFileName.GetString() );
	pNewFile->SetSubPath( pPatchFile->FilePath.c_str() );

	m_ExtractedFile.push_back( pNewFile );

	return TRUE;
}
