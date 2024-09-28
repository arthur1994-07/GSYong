#pragma once

#include "../../SigmaCore/Xml/2.5.2/tinyxml.h"
#include "GameTextLoader.h"

using namespace NS_TEXT_LOADER;
#define MAX_TEXTLEN 1024

class CRanXMLParser
{
    TIXMLOLD::TiXmlDocument m_XmlDoc;	
	CString m_strLang;

public:
	CRanXMLParser(const CString & strLang );
	~CRanXMLParser();

	enum {
		RANXMLPARSER_PARSEOP_ALLLANG	= 0x0001,
		RANXMLPARSER_PARSEOP_CONVUTF8	= 0x0002,
	};

private:
	BOOL _ParseWORD( void* pInData, DWORD dwParseOP );
	BOOL _ParseSENTENSE( void* pInData, DWORD dwParseOP );
	BOOL _ParseNUMBER( void* pInData, DWORD dwParseOP );

	//BOOL	_ParseVALUE( STRFLAG & pData, IXMLDOMNode* pNode );
	BOOL IsValidData( const void * szValue, const TCHAR * szMsg, int nCount, const TCHAR * szID = _T("") );
	BOOL IsValidData( const TCHAR * szValue, const TCHAR * szMsg, int nCount, const TCHAR * szID = _T("") );

private:
	const BOOL _ParseSENTENCEExtract( CStringArray** pResStringArr, TIXMLOLD::TiXmlElement* pSentense, const language::EMPROVIDE_LANGFLAG emProvideLang, const TCHAR* const szId );
	const BOOL _ParseWORDExtract( CStringArray** pResStringArr, TIXMLOLD::TiXmlElement* pSentense, const language::EMPROVIDE_LANGFLAG emProvideLang, const TCHAR* const szId );
	const BOOL _ParseNUMBERExtract( CStringArray** pResStringArr, TIXMLOLD::TiXmlElement* pSentense, const language::EMPROVIDE_LANGFLAG emProvideLang, const TCHAR* const szId );

public:
	BOOL Parse_XML( void* pInData, const char* lpszFilename, DWORD dwParseOP );

	BOOL Parse( void* pData, const char* lpszFilename, DWORD dwParseOP )
	{
		ASSERT( lpszFilename );
		return Parse_XML( pData, lpszFilename, dwParseOP );
	}

	BOOL Parse_XMLInZip( void* pInData, const char* szZipFileFullPath, const char* szPath, const char* szFile, const BOOL bPack);
};
