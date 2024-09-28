#include "pch.h"
#include "GfxInterfaceBase.h"

#include <boost/scoped_array.hpp>

#include "./GLGaeaClient.h"
#include "../RanGfxUI/GfxInterface.h"
#include "../SigmaCore/Util/SystemInfo.h"
#include "../SigmaCore/File/FileUtil.h"
#include "../enginelib/DxTools/DxFontMan.h"
#include "../enginelib/DxTools/RENDERPARAM.h"

GfxInterfaceBase::GfxInterfaceBase(void)
:	m_nStageBG( EMSTAGE_BG_NONE )
,	m_nPrevStageBG( EMSTAGE_BG_NONE )
,	m_bMouseHitUI( false )
,	m_bDisplayOn( true )
,	m_bDisplayCountryMark( false )
,	m_bIsFocusInTextInput( false )
,	m_bCallProfiling( false )
{
}

GfxInterfaceBase::GfxInterfaceBase( const GfxInterfaceBase& value )
{
}

GfxInterfaceBase::~GfxInterfaceBase(void)
{
}

GfxInterfaceBase* GfxInterfaceBase::GetInstance()
{
	return GfxInterface::GetInstance();
}

void GfxInterfaceBase::SetGaeaClient( GLGaeaClient* pClient )
{
	m_pGaeaClient = pClient;
}

GLGaeaClient* GfxInterfaceBase::GetGaeaClient()
{
	return m_pGaeaClient;
}

void GfxInterfaceBase::PrintChatList( int nLogID, LIST_CHAT& listChat )
{
	LIST_CHAT_ITER iter = listChat.begin();
	for( ; iter != listChat.end(); ++iter )
	{
		if( true == iter->m_vecLinkTextData.empty() )
		{
			AddChatMessage( nLogID, iter->m_strMsg, iter->m_dwColor,
				iter->m_vecCustomColor, iter->m_sReferName );
		}
		else
		{
			AddLinkMessage( nLogID, iter->m_strMsg, iter->m_dwColor,
				iter->m_vecCustomColor, iter->m_vecLinkTextData, iter->m_sReferName );
		}
	}
}
void GfxInterfaceBase::SetProfiling(bool bSet)
{
	m_bCallProfiling=bSet;
	if ( bSet == false )
		m_sfProfiling.m_mapProfiling.clear();
}

void GfxInterfaceBase::SaveProfile(void)
{
	char szFileName[255] = {0};

	// Application 의 path 에 /log/ 를 더한다.
	std::string strAppPath = sc::getAppPath();
	strAppPath += "\\sflog";

	// Log Directory 를 생성한다.
	if (sc::file::createDirectory(strAppPath) == false)
	{
		DWORD dwError = GetLastError();
	}

	// 정확한 로그파일 경로 및 이름을 만들기 위해서 문자열 \\ 추가
	strAppPath += "\\";

	struct tm today;
	time_t ltime;
	// Use time structure to build a customized time string.
	time(&ltime);
	localtime_s( &today, &ltime);
	// Use strftime to build a customized time string. 
	_tcsftime(
		szFileName,
		255,
		"_%y%m%d_%H%M.csv",
		&today);

	std::string strLogFileName;
	strLogFileName = strAppPath;
	strLogFileName += "sflog";
	strLogFileName += szFileName;

	HANDLE hFile(INVALID_HANDLE_VALUE);

	hFile = ::CreateFile(
		strLogFileName.c_str(),
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL );
	if ( hFile == INVALID_HANDLE_VALUE )
	{
		SAFE_CLOSE_HANDLE(hFile);
		return;
	}

	DWORD dwT;
	// Head
	std::string strHead("Ui_id,Function,Count\r\n");	
	::WriteFile(
		hFile,
		strHead.c_str(),
		(DWORD) strHead.length() /* * sizeof(wchar_t) */,
		&dwT,
		NULL);

	SF_PROFILING::SFMAP_KEYUI_DATAFUNCCNT& sProfiling = m_sfProfiling.m_mapProfiling;
	SF_PROFILING::SFMAP_KEYUI_DATAFUNCCNT_ITR sProfiling_itr = sProfiling.begin()
		, sProfiling_end = sProfiling.end();
	for ( ; sProfiling_end != sProfiling_itr; ++sProfiling_itr )
	{
		char szBuff[255] = {0};
		std::string strUIID("");
		itoa((int)sProfiling_itr->first, szBuff, 10);
		strUIID += szBuff;
		strUIID += ",";

		SF_PROFILING::SFMAP_KEYFUNC_DATACNT& sFuncCntData = sProfiling_itr->second;
		SF_PROFILING::SFMAP_KEYFUNC_DATACNT_ITR sFuncCntItr = sFuncCntData.begin()
			, sFuncCntEnd = sFuncCntData.end();
		for ( ; sFuncCntItr != sFuncCntEnd; ++sFuncCntItr)
		{
			DWORD dwWritten;
			std::string strLog(strUIID);
			strLog += sFuncCntItr->first.c_str();
			strLog += ",";
			itoa((int)sFuncCntItr->second, szBuff, 10);
			strLog += szBuff;
			strLog += "\r\n";

			::WriteFile(
				hFile,
				strLog.c_str(),
				(DWORD) strLog.length() /* * sizeof(wchar_t) */,
				&dwWritten,
				NULL);
			::FlushFileBuffers( hFile );
		}

	}

	m_pGaeaClient->PrintConsoleText(
		std::string( "Scaleform ProfilingTest Save." ) );
	m_pGaeaClient->PrintConsoleText(
		std::string( strLogFileName ) );

	SAFE_CLOSE_HANDLE( hFile );
}

std::wstring GfxInterfaceBase::EncodeText( const std::string& str )
{
	int nCodePage = CD3DFontPar::nCodePage[ RENDERPARAM::emLangSet ];
	
	int len = (int) str.size() + 1;

	boost::scoped_array< WCHAR > unicode( new WCHAR[ len ] );
	MultiByteToWideChar( nCodePage, 0, str.c_str(), -1, unicode.get(), len );
	//MultiByteToWideChar( CP_UTF8, 0, str.c_str(), -1, unicode.get(), len );

	return std::wstring( unicode.get() );
}
