#include "stdafx.h"
#include "./RanFilter.h"

#include "../SigmaCore/Encrypt/CompByte.h"
#include "../SigmaCore/String/basestring.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Loading/NSLoadingDirect.h"
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/Xml/2.5.2/tinyxml.h"
#include "../SigmaCore/Xml/2.5.3/tinystr.h"

#include "../RanLogic/RANPARAM.h"
#include "../RanLogic/GLUseFeatures.h"
#include "../EngineLib/DxTools/DxFontMan.h"
#include "../../EngineLib/DxTools/DxFontDefine.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"
#include "../EngineLib/GUInterface/GameTextLoader.h"

#include "../EngineLib/G-Logic/GLOGIC.h"


#define UNLEN 512

// ----------------------------------------------------------------------------
#include "../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CRanFilter& CRanFilter::GetInstance()
{
	static CRanFilter Instance;
	return Instance;
}

CRanFilter::CRanFilter()
	: m_nCodePage(CP_ACP)
	, m_pCurrentFilter(NULL)
	, m_bZipFile(FALSE)
	, m_bPackFile(FALSE)
{
}

CRanFilter::~CRanFilter()
{
}

BOOL CRanFilter::Init( CONST BOOL bZipFile, CONST BOOL bPackFile, CONST TCHAR* szZipFile, CONST TCHAR* szPath )
{
	m_bZipFile = bZipFile;
	m_bPackFile = bPackFile;

	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		m_nCodePage = CD3DFontPar::nCodePage[RENDERPARAM::emLangSet];

	m_ChatFilter.Init();
	m_NameFilter.Init();
	m_ChatFilter.SetCodePage( m_nCodePage );
	m_NameFilter.SetCodePage( m_nCodePage );

	if( !ReadToFile( szZipFile, szPath ) ) 
		return FALSE; 

	return TRUE;
}

BOOL CRanFilter::ReadToFile( CONST TCHAR* szZipFile, CONST TCHAR* szPath )
{
	if( !LOADCHAT( szZipFile, szPath ) ) return FALSE;
	if( !LOADNAME( szZipFile, szPath ) ) return FALSE;

	return TRUE;
}

UINT CRanFilter::GetSectionCode( CONST CString & strSection )
{
	if( strSection == _T("[UNICODE]") ) return EM_UNICODE;
	if( strSection == _T("[HEX]") )		return EM_HEX;
	if( strSection == _T("[CHAR]") )	return EM_CHAR;
	
	return EM_ERROR_CODE;
}

UINT CRanFilter::GetSectionCode_XML( const CString& strSection )
{
	if( strSection == _T("UNICODE") ) return EM_UNICODE;
	if( strSection == _T("HEX") )		return EM_HEX;
	if( strSection == _T("CHAR") )	return EM_CHAR;

	return EM_ERROR_CODE;
}

BOOL CRanFilter::LOADCHAT( CONST TCHAR* szZipFile, CONST TCHAR* szPath )
{
	m_pCurrentFilter = &m_ChatFilter;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		return LOAD( szZipFile, szPath, _T("Filter_chat.xml") );
	else
		return LOAD( szZipFile, szPath, _T("moblogic1.bin") );
}

BOOL CRanFilter::LOADNAME( CONST TCHAR* szZipFile, CONST TCHAR* szPath )
{
	m_pCurrentFilter = &m_NameFilter;
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() )
		return LOAD( szZipFile, szPath, _T("Filter_name.xml") );
	else
		return LOAD( szZipFile, szPath, _T("moblogic2.bin") );
}

BOOL CRanFilter::LOAD(const std::string& ZipFileName, CONST TCHAR* szPath, CONST TCHAR* szFile )
{
	if ( GLUseFeatures::GetInstance().IsUsingDataIntegrate() == TRUE )
	{
		CString strPATH(szPath);
		strPATH += szFile;

		NS_TEXT_LOADER::STRFLAG mapFlag[language::LANGFLAG_TOTAL_NUM];
		NS_TEXT_LOADER::LoadTextInZip( ZipFileName.c_str()
			, strPATH
			, szFile
			, m_bPackFile
			, mapFlag
			, FALSE, RANPARAM::strCountry );

		NS_TEXT_LOADER::STRFLAG_IT itr = mapFlag[RANPARAM::emProvideLangFlag].begin();
		NS_TEXT_LOADER::STRFLAG_IT itrEND = mapFlag[RANPARAM::emProvideLangFlag].end();
		if ( itr == itrEND )
		{
			std::string ErrorMsg = "Filter Load Error";
			sc::writeLogError(ErrorMsg);
			AfxMessageBox(ErrorMsg.c_str(), MB_OK|MB_ICONSTOP);
			NS_TEXT_LOADER::ClearText(mapFlag);
			return false;
		}
		while ( itr != itrEND )
		{
			const CString strNAME = static_cast<std::string>((*itr).first).c_str();
			const UINT SectionCode = GetSectionCode_XML(strNAME);
			CStringArray* pTextArray = static_cast<CStringArray*>((*itr).second);
			for ( INT i = 0; i < pTextArray->GetSize(); ++i )
			{
				switch (SectionCode)
				{
				case EM_UNICODE:	InputStringUNICODE( pTextArray->GetAt(i) );	break;
				case EM_HEX:		InputStringHEX( pTextArray->GetAt(i) );		break;
				case EM_CHAR:		InputStringCHAR( pTextArray->GetAt(i) );		break;
				case EM_ERROR_CODE: //에러
				default:
					{
						std::string ErrorMsg(
							sc::string::format(
							"CRanFilter::LOAD %1% SectionCode %2%",
							pTextArray->GetAt(i),
							SectionCode));
						sc::writeLogError(ErrorMsg);
					}
					break;
				}
			}
			++itr;
		}	
		NS_TEXT_LOADER::ClearText(mapFlag);
	}
	else
	{
		CString strPATH( szPath );
		strPATH += szFile;

		std::auto_ptr<sc::CBaseString> pBString(
			GLOGIC::openfile_basestring(
			m_bZipFile, 
			ZipFileName, 
			strPATH, 
			szFile, 
			true, 
			m_bPackFile));

		if (!pBString.get())
		{
			std::string ErrorMsg(
				sc::string::format(
				"CRanFilter::LOAD %1%", szFile));
			sc::writeLogError(ErrorMsg);
			return FALSE;
		}

		sc::CBaseString &cBString = *pBString;

		UINT SectionCode(EM_ERROR_CODE);
		CString strBUFFER;

		while( cBString.GetNextLine(strBUFFER) > 0 )
		{
			if( strBUFFER.IsEmpty() )
				continue;

			//---------------------------------------------------//
			NSLoadingDirect::FrameMoveRender( FALSE );
			//---------------------------------------------------//

			if( strBUFFER[0] == _T('[') )
			{
				// Note : 해당 섹션 코드를 대입한다.
				SectionCode = GetSectionCode( strBUFFER );
			}
			else
			{
				// Note : 만약 센셕이 아니면 스트링을 해당 코드 함수에 보낸다.
				switch (SectionCode)
				{
				case EM_UNICODE:	InputStringUNICODE( strBUFFER );	break;
				case EM_HEX:		InputStringHEX( strBUFFER );		break;
				case EM_CHAR:		InputStringCHAR( strBUFFER );		break;
				case EM_ERROR_CODE: //에러
				default:
					{
						std::string ErrorMsg(
							sc::string::format(
							"CRanFilter::LOAD %1% SectionCode %2%",
							strBUFFER,
							SectionCode));
						sc::writeLogError(ErrorMsg);
					}
					break;
				}
			}
		}
	}
	return TRUE;
}

VOID CRanFilter::InputStringHEX( CONST CString & strBUFFER)
{
	WCHAR wszBUFFER[UNLEN+1]={0};
	CHAR szBUFFER[256]={0};

	compbyte::string2hex( (UCHAR*)strBUFFER.GetString(), (UCHAR*)szBUFFER, strBUFFER.GetLength() );

	MultiByteToWideChar( m_nCodePage, 0, szBUFFER, (INT)(strlen(szBUFFER)+1),
		wszBUFFER, (INT)(sizeof(wszBUFFER)/sizeof(wszBUFFER[0])) );

	m_pCurrentFilter->addSlang( wszBUFFER );
}

VOID CRanFilter::InputStringCHAR( CONST CString & strBUFFER )
{
	WCHAR wszBUFFER[UNLEN+1]={0};

	MultiByteToWideChar( m_nCodePage, 0, strBUFFER.GetString(), strBUFFER.GetLength()+1,
		wszBUFFER, sizeof(wszBUFFER)/sizeof(wszBUFFER[0]) );

	m_pCurrentFilter->addSlang( wszBUFFER );
}

VOID CRanFilter::InputStringUNICODE(CONST CString& strBUFFER)
{
	if( strBUFFER[0] == _T('$') )	// Note : 범위를 가진 입력버퍼, 문자 하나일때 가능하다.
	{
		INT curPos(0);

		CString strStart = strBUFFER.Tokenize( _T("$ ~"), curPos );
		if (strStart == _T(""))
		{
            std::string ErrorMsg("CRanFilter::InputStringUNICODE UNICODE Range Data");
			sc::writeLogError(ErrorMsg);
			return;
		}

		WCHAR wszBUFFER[UNLEN+1]={0};
		compbyte::wstring2hex( (UCHAR*)strStart.GetString(), wszBUFFER, strStart.GetLength() );
		// Note : 문자 하나라고 가정한다. 문자열이 될경우 범위 적용을 할 수 없다.
		USHORT dwStart = (USHORT)wszBUFFER[0]; 

		CString strEnd = strBUFFER.Tokenize( _T("$ ~"), curPos );
		if( strEnd == _T("") )
		{
            std::string ErrorMsg("CRanFilter::InputStringUNICODE UNICODE Range Data");
            sc::writeLogError(ErrorMsg);
			return ;
		}

		SecureZeroMemory( wszBUFFER, sizeof(wszBUFFER) );
		compbyte::wstring2hex( (UCHAR*)strEnd.GetString(), wszBUFFER, strEnd.GetLength() );
		// Note : 문자 하나라고 가정한다. 문자열이 될경우 범위 적용을 할 수 없다.
		USHORT dwEnd = (USHORT)wszBUFFER[0];

		for( USHORT i=dwStart; i<=dwEnd; ++i )
		{
			SecureZeroMemory( wszBUFFER, sizeof(wszBUFFER) );
			wszBUFFER[0] = (WCHAR)i;

			m_pCurrentFilter->addSlang( wszBUFFER );
		}
	}
	else
	{
		WCHAR wszBUFFER[UNLEN+1]={0};
		compbyte::wstring2hex( (UCHAR*)strBUFFER.GetString(), wszBUFFER, strBUFFER.GetLength() );

		m_pCurrentFilter->addSlang( wszBUFFER );
	}
}

BOOL CRanFilter::ChatFilter(CONST CString & strBuf)
{
	if (Filter(strBuf, m_ChatFilter))
        return TRUE;
    else		
	    return FALSE;
}

BOOL CRanFilter::ChatIgnorFilter(CONST CString & strName)
{
	if (IgnorFilter(strName))
        return TRUE;
    else		
	    return FALSE;
}

BOOL CRanFilter::NameFilter(CONST CString & strBuf)
{
	CString strBUFFER( strBuf );

	WCHAR wszBUFFER[UNLEN+1]={0};

	MultiByteToWideChar(
		m_nCodePage,
		0,
		strBUFFER.GetString(),
		strBUFFER.GetLength()+1,
		wszBUFFER, 
		sizeof(wszBUFFER)/sizeof(wszBUFFER[0]));

	for (int i = 0; i < UNLEN+1; i++ )
	{
		if ( wszBUFFER[i] == 63735 || 
			wszBUFFER[i] == 63736 )
			return TRUE;
	}

	if (Filter(strBuf, m_ChatFilter))
        return TRUE;
	if (Filter(strBuf, m_NameFilter))
        return TRUE;	
	return FALSE;
}

BOOL CRanFilter::Filter( CONST CString & strBuf, CONST SlangFilter & currentFilter )
{
	if ( strBuf.IsEmpty() )	return FALSE;

	CString strBUFFER( strBuf );

	WCHAR wszBUFFER[UNLEN+1]={0};
	BOOL bFind(FALSE);

	switch( RENDERPARAM::emLangSet )
	{
	case language::KOREAN :
		{
			MultiByteToWideChar(
				m_nCodePage,
				0,
				strBUFFER.GetString(),
				strBUFFER.GetLength()+1,
				wszBUFFER, 
				sizeof(wszBUFFER)/sizeof(wszBUFFER[0]));

			std::wstring strFilted( wszBUFFER )
				, strRes(currentFilter.filter( strFilted ));
			INT nCOUNT = WideCharToMultiByte( m_nCodePage, 0, strRes.c_str(), -1, NULL, 0, NULL, NULL );
			TCHAR szBUFFER[UNLEN+1] = {0};
			WideCharToMultiByte( m_nCodePage, 0, strRes.c_str(), -1, szBUFFER, nCOUNT+1, NULL, NULL );

			m_strFilted = szBUFFER;

			if ( strFilted != strRes )
			{
				sc::writeLogError(sc::string::format("Chat Filted Error : %1%, %2%", szBUFFER, m_strFilted.GetString() ));
				sc::writeLogError(sc::string::format("Chat Filted Error : %1%, %2%", strFilted.c_str(), strRes.c_str() ));
				bFind = TRUE;
			}
		}
		break;

	default :
		{
			int bufSize = MultiByteToWideChar( CP_UTF8, 0, strBUFFER.GetBuffer(), strBUFFER.GetLength(), NULL, 0 );
			int strSize = bufSize;

			std::vector<wchar_t> wsv(bufSize);
			MultiByteToWideChar(CP_UTF8, 0, strBUFFER.GetBuffer(), strBUFFER.GetLength(), &wsv[0], bufSize);

			std::wstring strFilted( wsv.begin(), wsv.begin()+strSize );

			strFilted = currentFilter.filter( strFilted );
			INT nCOUNT = WideCharToMultiByte( CP_UTF8, 0, strFilted.c_str(), -1, NULL, 0, NULL, NULL );

			TCHAR szBUFFER[UNLEN+1] = {0};
			WideCharToMultiByte( CP_UTF8, 0, strFilted.c_str(), -1, szBUFFER, nCOUNT+1, NULL, NULL );

			m_strFilted = szBUFFER;

			if ( strBuf != m_strFilted )
			{
				bFind = TRUE;
			}
		}
		break;
	}

	return bFind;
}

BOOL CRanFilter::CompareChatTime( _SYSTEMTIME &currentTime, _SYSTEMTIME &prevTime )
{
	if ( currentTime.wDay == prevTime.wDay && currentTime.wHour == prevTime.wHour && currentTime.wMinute == prevTime.wMinute && currentTime.wSecond == prevTime.wSecond 
		&& (currentTime.wMilliseconds - prevTime.wMilliseconds) <= 200 ) 
	{
		return TRUE;
	}
		
	return FALSE;
}

BOOL CRanFilter::IgnorFilter( CONST CString & strName )
{
	if ( strName.IsEmpty() )	return FALSE;

	CHAT_IGNOR_MAP_ITER iter = m_ChatIgnorFilter.find(strName);

	if ( iter != m_ChatIgnorFilter.end() )
	{
		ChatIgnorUser& ignorData = iter->second;

		if ( ignorData.m_bIgnor == FALSE )
		{
			_SYSTEMTIME curTime;
			GetSystemTime(&curTime);

			if (CompareChatTime(curTime, ignorData.m_chatTime) == FALSE)
			{
				GetSystemTime(&ignorData.m_chatTime);
			}
			else
			{
				ignorData.m_bIgnor = TRUE;
				sc::writeLogError(sc::string::format("Chat Block User Name : %1%", strName ));
				return TRUE;
			}
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		ChatIgnorUser newUser;
		m_ChatIgnorFilter.insert(CHAT_IGNOR_MAP_VALUE(strName, newUser));
	}

	return FALSE;
}

//BOOL CRanFilter::LOADPROVERB( CONST BOOL bZipFile, CONST TCHAR* szZipFile, CONST TCHAR* szPath )
//{
//	CString strPATH( CString(szPath) + "moblogic3.bin" );
//
//	std::auto_ptr<sc::CBaseString> pBString( GLOGIC::openfile_basestring( bZipFile, szZipFile, strPATH, "moblogic3.bin", TRUE ) );
//
//	if ( !pBString.get() )
//		return FALSE;
//
//	sc::CBaseString &cBString = *pBString;
//
//	CString strBUFFER;
//
//	while ( cBString.GetNextLine(strBUFFER) > 0 )
//	{
//		if ( strBUFFER.IsEmpty() )	continue;
//
//		WCHAR wszBUFFER[UNLEN+1]={0};
//
//		MultiByteToWideChar( m_nCodePage, 0, strBUFFER.GetString(), strBUFFER.GetLength()+1,
//							wszBUFFER, sizeof(wszBUFFER)/sizeof(wszBUFFER[0]) );
//
//		m_vecStringProverb.push_back( wszBUFFER );
//	}
//
//	return TRUE;
//}

//INT CRanFilter::Nrand()
//{
//	INT n = static_cast<INT>(m_vecStringProverb.size());
//
//	if ( n <= 0 || n > RAND_MAX )
//		throw std::domain_error( "CRanFilter::Nrand()" );
//
//	CONST INT bucket_size = RAND_MAX / n;
//	INT r;
//
//	do r = rand() / bucket_size;
//	while ( r >= n );
//
//	return r;
//}