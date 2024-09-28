#include "pch.h"
#include <boost/function.hpp>
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/basestring.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/String/StringUtils.h"
#include "../DxTools/DxFontDefine.h"
#include "../DxTools/DxFontMan.h"
#include "../DxTools/RENDERPARAM.h"
#include "../../RanLogic/RANPARAM.h"
#include "../G-Logic/GLogic.h"
#include "./GameTextControl.h"
#include "RanXMLParser.h"
#include "../../SigmaCore/gassert.h"

using namespace NS_TEXT_LOADER;
using namespace TIXMLOLD;

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

CRanXMLParser::CRanXMLParser(const CString & strLang )
	: m_strLang(strLang)
{
}

CRanXMLParser::~CRanXMLParser()
{
	m_XmlDoc.Clear();
}

BOOL CRanXMLParser::IsValidData( const void * szValue, const TCHAR * szMsg, int nCount, const TCHAR * szID )
{
	if( szValue == NULL )
	{
        std::string ErrorMsg(
            sc::string::format(
                "[TEXT/%1%]Count:%2%, ID:%3% Data is not valid",
                szMsg, nCount, szID));
		sc::writeLogError(ErrorMsg);
		return FALSE;
	}

	return TRUE;
}

BOOL CRanXMLParser::IsValidData( const TCHAR * szValue, const TCHAR * szMsg, int nCount, const TCHAR * szID )
{
	if( szValue[0] ==_T('\0') )
	{
        std::string ErrorMsg(
            sc::string::format(
                "[TEXT/%1%]Count:%2%, ID:%3% Data is not valid",
                szMsg, nCount, szID));
        sc::writeLogError(ErrorMsg);
		return FALSE;
	}

	return TRUE;
}

BOOL CRanXMLParser::Parse_XML( void* pData, const char* lpszFilename, DWORD dwParseOP )
{
	GASSERT( lpszFilename );

	if( !m_XmlDoc.LoadFile(lpszFilename) )
	{
		sc::writeLogError(sc::string::format("Xml Load Failed \"%1%\"", lpszFilename));
		return FALSE;
	}

	try
	{	
		if ( !_ParseWORD( pData, dwParseOP ) )		throw;
		if ( !_ParseSENTENSE( pData, dwParseOP ) )	throw;
		if ( !_ParseNUMBER( pData, dwParseOP ) )		throw;

		return TRUE;
	}
	catch(...)
	{
		sc::writeLogError(sc::string::format("Xml Parse Failed \"%1%\"", lpszFilename));
		return FALSE;
	}	
}

BOOL CRanXMLParser::Parse_XMLInZip( void* pInData, const char* szZipFileFullPath, const char* szPath, const char* szFile, const BOOL bPack)
{
	GASSERT( szZipFileFullPath );
	GASSERT( szPath );
	GASSERT( szFile );

	std::auto_ptr<sc::CBaseString> pBString(
		GLOGIC::openfile_basestring(
		TRUE, 
		szZipFileFullPath, 
		szPath, 
		szFile, 
		false, 
		bPack));

	if (!pBString.get())
	{
		std::string ErrorMsg(
			sc::string::format(
			"XML Parser::LOAD %1%", szFile));
		sc::writeLogError(ErrorMsg);
		return FALSE;
	}
	char* pText = (char*)pBString->GetText();

	//  [6/11/2014 gbgim];
	// - 무조건 컨버팅해줘야함;
	// - TinyXml에서 문자열을 통한 컨버팅이나, 일반 xml file을 읽어오더라도;
	//   UTF-8의 읽을 수 없는 코드셋의 문자열이 있다면 로드 실패한다;
	std::wstring wszUTF8 = sc::string::multiToWide(pText, -1, CP_UTF8);
	std::string strUTF8 = sc::string::wideToMulti(wszUTF8.c_str(), -1, CP_UTF8/*CD3DFontPar::nCodePage[iCodesetIdx]*/);
	
	if ( !m_XmlDoc.LoadString(strUTF8.c_str()/*, TIXMLOLD::TiXmlEncoding::TIXML_ENCODING_UTF8*/) )
	{
		sc::writeLogError(sc::string::format("Xml Load Failed \"%1%\" in zip", szPath));
		return FALSE;
	}

	try
	{	
		if ( !_ParseWORD( pInData, FALSE ) )		throw;
		if ( !_ParseSENTENSE( pInData, FALSE ) )	throw;
		if ( !_ParseNUMBER( pInData, FALSE ) )		throw;

		return TRUE;
	}
	catch(...)
	{
		sc::writeLogError(sc::string::format("Xml Parse Failed \"%1%\" in zip", szPath));
		return FALSE;
	}	
}

BOOL CRanXMLParser::_ParseSENTENSE( void* pInData, DWORD dwParseOP )
{
	TCHAR				szValue[MAX_TEXTLEN]={0};
	TCHAR				szId[MAX_TEXTLEN]={0};
	WCHAR				szTemp[MAX_TEXTLEN]={0};
	LONG				nCount(0), nCount2(0);
	TIXMLOLD::TiXmlNode* TextNode;
	TIXMLOLD::TiXmlElement* SentenseElem;	
	const int iCodesetIdx = language::ProvidelangToLangflag(RANPARAM::emProvideLangFlag);
	
	try
	{
		CONST CString strCRLF("\r\n");
		CONST CString strCommon("Common");
		
		TextNode = m_XmlDoc.FirstChild("TEXT");

		if( !TextNode ) return FALSE;

		STRFLAG* pData = static_cast<STRFLAG*>(pInData);
		SentenseElem = TextNode->FirstChildElement("SENTENSE");
		
		while(SentenseElem){			
			if( !IsValidData( SentenseElem, "SENTENSE", nCount ) ){
				SentenseElem = SentenseElem->NextSiblingElement("SENTENSE");
				nCount++;
				continue;
			}

			_tcscpy(szValue,SentenseElem->Attribute("Ver"));
			if( !IsValidData( szValue, "SENTENSE/Ver", nCount ) ){
				SentenseElem = SentenseElem->NextSiblingElement("SENTENSE");
				nCount++;
				continue;
			}

			_tcscpy(szId,SentenseElem->Attribute("Id"));
			if( !IsValidData( szId, "SENTENSE/Id", nCount ) ){
				SentenseElem = SentenseElem->NextSiblingElement("SENTENSE");
				nCount++;
				continue;
			}

			if ( dwParseOP & RANXMLPARSER_PARSEOP_CONVUTF8 )
			{
				MultiByteToWideChar(CP_UTF8,0,szId, _tcslen(szId)+1 ,szTemp,sizeof(szTemp)/sizeof(szTemp[0]));			
				WideCharToMultiByte(CD3DFontPar::nCodePage[iCodesetIdx], 0, szTemp, -1, szId, sizeof(szId)/sizeof(szId[0]), NULL,NULL );
			}
			
			if ( dwParseOP & RANXMLPARSER_PARSEOP_ALLLANG )
			{
				for ( int i = 0; i < language::LANGFLAG_TOTAL_NUM; ++i )
				{
					CStringArray* pStringArray = NULL;
					if ( _ParseSENTENCEExtract( &pStringArray, SentenseElem, static_cast<language::EMPROVIDE_LANGFLAG>(i), szId ) )
					{
						STRFLAG_IT iter = pData[i].find( std::string(szId) );
						if ( iter == pData[i].end() )
						{
							pData[i][std::string(szId)] = pStringArray;
						}
						else
						{
							sc::writeLogWarn( sc::string::format( "CRanXMLParser::_ParseSENTENSE : %1% 중복입니다.", szId ) );
						}
					}
				}
			}
			else
			{
				CStringArray* pStringArray = NULL;
				if ( _ParseSENTENCEExtract( &pStringArray, SentenseElem, RANPARAM::emProvideLangFlag, szId ) )
				{
					if ( pStringArray == NULL || pStringArray->GetSize() == 0 )
					{
						sc::writeLogInfo(sc::string::format("\"%1%\" xml keyword is invaild",szId));
					}
					else
					{
						STRFLAG_IT iter = pData[RANPARAM::emProvideLangFlag].find( std::string(szId) );
						if ( iter == pData[RANPARAM::emProvideLangFlag].end() )
						{
							pData[RANPARAM::emProvideLangFlag][std::string(szId)] = pStringArray;
						}
						else
						{
							sc::writeLogWarn( sc::string::format( "CRanXMLParser::_ParseSENTENSE : %1% 중복입니다.", szId ) );
						}
					}
				}
			}
			SentenseElem = SentenseElem->NextSiblingElement("SENTENSE");
			++nCount;
		}

		return TRUE;
	}

	catch(...)
	{
		return FALSE;
	}
}

BOOL CRanXMLParser::_ParseWORD( void * pInData, DWORD dwParseOP )
{
	TCHAR				szValue[MAX_TEXTLEN]={0};
	TCHAR				szId[MAX_TEXTLEN]={0};
	WCHAR				szTemp[MAX_TEXTLEN]={0};
	LONG				nCount(0), nCount2(0);
	TIXMLOLD::TiXmlNode* TextNode;
	TIXMLOLD::TiXmlElement* WordElem, *ValueElem;
	INT					nIndex(0);
	const int iCodesetIdx = language::ProvidelangToLangflag(RANPARAM::emProvideLangFlag);

	try
	{
		CONST CString strCommon("Common");
		
		TextNode = m_XmlDoc.FirstChild("TEXT");

		if( !TextNode ) return FALSE;

		STRFLAG* pData = static_cast<STRFLAG*>(pInData);
		WordElem = TextNode->FirstChildElement("WORD");
		
		while(WordElem){
			
			if( !IsValidData( WordElem, "WORD", nCount ) ){
				WordElem = WordElem->NextSiblingElement("WORD");
				nCount++;
				continue;
			}

			_tcscpy(szValue,WordElem->Attribute("Ver"));
			if( !IsValidData( szValue, "WORD/Ver", nCount ) ){
				WordElem = WordElem->NextSiblingElement("WORD");
				nCount++;
				continue;
			}

			_tcscpy(szId,WordElem->Attribute("Id"));
			if( !IsValidData( szId, "WORD/Id", nCount ) ){
				WordElem = WordElem->NextSiblingElement("WORD");
				nCount++;
				continue;
			}

			if ( dwParseOP & RANXMLPARSER_PARSEOP_CONVUTF8 )
			{
				MultiByteToWideChar(CP_UTF8,0,szId, _tcslen(szId)+1 ,szTemp,sizeof(szTemp)/sizeof(szTemp[0]));			
				WideCharToMultiByte(CD3DFontPar::nCodePage[iCodesetIdx], 0, szTemp, -1, szId, sizeof(szId)/sizeof(szId[0]), NULL,NULL );
			}

			ValueElem = WordElem->FirstChildElement("VALUE");
			if( !ValueElem ){
				WordElem = WordElem->NextSiblingElement("WORD");
				nCount++;
				continue;
			}

			if ( dwParseOP & RANXMLPARSER_PARSEOP_ALLLANG )
			{
				for ( int i = 0; i < language::LANGFLAG_TOTAL_NUM; ++i )
				{
					CStringArray* pStringArray = NULL;
					if ( _ParseWORDExtract( &pStringArray, WordElem, static_cast<language::EMPROVIDE_LANGFLAG>(i), szId ) )
					{
						STRFLAG_IT iter = pData[i].find( std::string(szId) );
						if ( iter == pData[i].end() )
						{
							pData[i][std::string(szId)] = pStringArray;
						}
						else
						{
							sc::writeLogWarn( sc::string::format( "CRanXMLParser::_ParseSENTENSE : %1% 중복입니다.", szId ) );
						}
					}
				}
			}
			else
			{
				CStringArray* pStringArray = NULL;
				if ( _ParseWORDExtract( &pStringArray, WordElem, RANPARAM::emProvideLangFlag, szId ) )
				{
					if ( pStringArray == NULL || pStringArray->GetSize() == 0 )
					{
						sc::writeLogInfo(sc::string::format("\"%1%\" xml keyword is invaild",szId));
					}
					else
					{
						STRFLAG_IT iter = pData[RANPARAM::emProvideLangFlag].find( std::string(szId) );
						if ( iter == pData[RANPARAM::emProvideLangFlag].end() )
						{
							pData[RANPARAM::emProvideLangFlag][std::string(szId)] = pStringArray;
						}
						else
						{
							sc::writeLogWarn( sc::string::format( "CRanXMLParser::_ParseSENTENSE : %1% 중복입니다.", szId ) );
						}
					}
				}
			}
			WordElem = WordElem->NextSiblingElement("WORD");
			++nCount;			
		}

		return TRUE;
	}

	catch(...)
	{
		return FALSE;
	}	
}

BOOL CRanXMLParser::_ParseNUMBER( void* pInData, DWORD dwParseOP )
{
	TCHAR				szValue[MAX_TEXTLEN]={0};
	TCHAR				szId[MAX_TEXTLEN]={0};
	LONG				nCount(0), nCount2(0);
	TIXMLOLD::TiXmlNode* TextNode;
	TIXMLOLD::TiXmlElement* NumberElem, *ValueElem;

	try
	{
		CONST CString strCRLF("\r\n");
		CONST CString strCommon("Common");
		
		TextNode = m_XmlDoc.FirstChild("TEXT");

		if( !TextNode ) return FALSE;

		STRFLAG* pData = static_cast<STRFLAG*>(pInData);
		NumberElem = TextNode->FirstChildElement("NUMBER");
		
		while(NumberElem){
			
			if( !IsValidData( NumberElem, "SENTENSE", nCount ) ){
				NumberElem = NumberElem->NextSiblingElement("SENTENSE");
				nCount++;
				continue;
			}

			_tcscpy(szValue,NumberElem->Attribute("Ver"));
			if( !IsValidData( szValue, "SENTENSE/Ver", nCount ) ){
				NumberElem = NumberElem->NextSiblingElement("SENTENSE");
				nCount++;
				continue;
			}

			_tcscpy(szId,NumberElem->Attribute("Id"));
			if( !IsValidData( szId, "SENTENSE/Id", nCount ) ){
				NumberElem = NumberElem->NextSiblingElement("SENTENSE");
				nCount++;
				continue;
			}

			ValueElem = NumberElem->FirstChildElement("VALUE");
			if( !ValueElem ){
				NumberElem = NumberElem->NextSiblingElement("SENTENSE");
				nCount++;
				continue;
			}

			if ( dwParseOP & RANXMLPARSER_PARSEOP_ALLLANG )
			{
				for ( int i = 0; i < language::LANGFLAG_TOTAL_NUM; ++i )
				{
					CStringArray* pStringArray = NULL;
					if ( _ParseWORDExtract( &pStringArray, NumberElem, static_cast<language::EMPROVIDE_LANGFLAG>(i), szId ) )
					{
						STRFLAG_IT iter = pData[i].find( std::string(szId) );
						if ( iter == pData[i].end() )
						{
							pData[i][std::string(szId)] = pStringArray;
						}
						else
						{
							sc::writeLogWarn( sc::string::format( "CRanXMLParser::_ParseSENTENSE : %1% 중복입니다.", szId ) );
						}
					}
				}
			}
			else
			{
				CStringArray* pStringArray = NULL;
				if ( _ParseWORDExtract( &pStringArray, NumberElem, static_cast<language::EMPROVIDE_LANGFLAG>(RANPARAM::emProvideLangFlag), szId ) )
				{
					STRFLAG_IT iter = pData[RANPARAM::emProvideLangFlag].find( std::string(szId) );
					if ( iter == pData[RANPARAM::emProvideLangFlag].end() )
					{
						pData[RANPARAM::emProvideLangFlag][std::string(szId)] = pStringArray;
					}
					else
					{
						sc::writeLogWarn( sc::string::format( "CRanXMLParser::_ParseSENTENSE : %1% 중복입니다.", szId ) );
					}
				}
			}
			NumberElem = NumberElem->NextSiblingElement("WORD");
			++nCount;
 		}

		return TRUE;
	}

	catch(...)
	{
		return FALSE;
	}	
}

const BOOL CRanXMLParser::_ParseSENTENCEExtract( CStringArray** ppResStringArr, TIXMLOLD::TiXmlElement* pSentense, const language::EMPROVIDE_LANGFLAG emProvideLang, const TCHAR* const szId )
{
	TIXMLOLD::TiXmlElement* pValueElem = pSentense->FirstChildElement("VALUE");
	if( !pValueElem )
		return FALSE;

	CString strCountry = language::strProvideLanguage[emProvideLang];
	if ( strCountry.IsEmpty() )
		return FALSE;

	WCHAR	szTemp[MAX_TEXTLEN]={0};
	TCHAR	szValue[MAX_TEXTLEN]={0};
	INT		nCount = 0;
	
	CONST CString strCRLF("\r\n");
	CONST CString strCommon("Common");
	CString strBuffer;
	const int iCodesetIdx = language::ProvidelangToLangflag(emProvideLang);

	CStringArray strArr;
	while( pValueElem )
	{
		_tcscpy(szValue, pValueElem->Attribute("Lang"));
		if( !IsValidData( szValue, "SENTENSE/VALUE/Lang", nCount , szId ) )
		{
			pValueElem = pValueElem->NextSiblingElement("VALUE");
			nCount++;
			continue;
		}

		if ( strCountry == szValue || strCommon == szId )
		{
			if ( !pValueElem->FirstChild() )
			{
				sc::writeLogInfo(sc::string::format("The %1% keyword is Invaild", szId));
			}
			else
			{
				_tcscpy( szValue, pValueElem->FirstChild()->Value());
				if( !IsValidData( szValue, "SENTENSE/VALUE", nCount, szId ) )
				{
					pValueElem = pValueElem->NextSiblingElement("VALUE");
					nCount++;
					continue;
				}

				MultiByteToWideChar(CP_UTF8,0,szValue,(int)pValueElem->FirstChild()->ValueStr().size()+1,szTemp,sizeof(szTemp)/sizeof(szTemp[0]));			
				WideCharToMultiByte(CD3DFontPar::nCodePage[iCodesetIdx], 0, szTemp, -1, szValue, sizeof(szValue)/sizeof(szValue[0]), NULL,NULL );

				std::string strOrign(szValue), strResult;
				STRUTIL::StringReplace(strOrign, "\\r\\n", "\r\n", true, &strResult);
				strArr.SetAtGrow(nCount, strOrign.c_str());
				//strBuffer += strResult.c_str();//szValue;
				//strBuffer += strCRLF;
			}
		}
		pValueElem = pValueElem->NextSiblingElement("VALUE");
		nCount++;
	}

	const INT nArrSize = strArr.GetCount();
	if ( nArrSize != 1 )
	{
		for (INT i = 0; i < nArrSize; ++i)
		{
			CString str = strArr[i];
			// 중문에서 아래 크래쉬난다;
			// str = str.MakeLower();
			if ( str == "Skip" || str == "skip" )
			{
				strArr[i] = "\n";
			}
		}
	}

	for ( int i = 0; i < nArrSize; ++i )
	{
		CString str(strArr[i].GetString());
		strBuffer += str;
		if ( str != "\n" )
			strBuffer += strCRLF;
	}


	int nLen = strBuffer.GetLength();

	if( nLen > 1 ) // Note : 길이가 1보다 크다는 것은 무조건 마지막에 \r\n이 존재한다는 것이다.
	{
		strBuffer.SetAt( strBuffer.GetLength()-2, NULL ); // 마지막 \r\n은 제거한다.
	}

	if ( strBuffer.IsEmpty() == FALSE )
	{
		//	데이타 만들기
		CStringArray* pStringArr = new CStringArray;
		pStringArr->Add( strBuffer );
		*ppResStringArr = pStringArr;
	}
	return TRUE;

	// pData[RENDERPARAM::emLangSet][std::string(szId)] = pNewStrArray;

	// SentenseElem = SentenseElem->NextSiblingElement("SENTENSE");
}

const BOOL CRanXMLParser::_ParseWORDExtract( CStringArray** ppResStringArr, TIXMLOLD::TiXmlElement* pSentense, const language::EMPROVIDE_LANGFLAG emProvideLang, const TCHAR* const szId )
{
	TIXMLOLD::TiXmlElement* pValueElem = pSentense->FirstChildElement("VALUE");
	if( !pValueElem )
		return FALSE;

	CString strCountry = language::strProvideLanguage[emProvideLang];
	if ( strCountry.IsEmpty() )
		return FALSE;

	WCHAR	szTemp[MAX_TEXTLEN]={0};
	TCHAR	szValue[MAX_TEXTLEN]={0};
	INT		nCount = 0, nIndex;
	const int iCodesetIdx = language::ProvidelangToLangflag(emProvideLang);

	CONST CString strCommon("Common");

	CStringArray* pStringData = new CStringArray;
	while( pValueElem )
	{
		_tcscpy(szValue, pValueElem->Attribute("Lang"));
		if( !IsValidData( szValue, "WORD/VALUE/Lang", nCount , szId ) )
		{
			pValueElem = pValueElem->NextSiblingElement("VALUE");
			++nCount;
			continue;
		}

		if ( strCountry == szValue || strCommon == szId )
		{
			if ( !pValueElem->FirstChild() )
			{
				sc::writeLogInfo(sc::string::format("The %1% keyword is Invaild", szId));
			}
			else
			{
				_tcscpy(szValue,pValueElem->Attribute("Index"));
				if( !IsValidData( szValue, "WORD/VALUE/Index", nCount, szId ) )
				{
					pValueElem = pValueElem->NextSiblingElement("VALUE");
					++nCount;
					continue;
				}
				nIndex = _tstoi( szValue );

				_tcscpy(szValue,pValueElem->FirstChild()->Value());
				if( !IsValidData( szValue, "WORD/VALUE", nCount, szId ) ){
					++nCount;
					pValueElem = pValueElem->NextSiblingElement("VALUE");
					continue;
				}
			}

			MultiByteToWideChar(CP_UTF8,0,szValue,(int)pValueElem->FirstChild()->ValueStr().size()+1,szTemp,sizeof(szTemp)/sizeof(szTemp[0]));			
			WideCharToMultiByte(CD3DFontPar::nCodePage[iCodesetIdx], 0, szTemp, -1, szValue, sizeof(szValue)/sizeof(szValue[0]), NULL,NULL );

			std::string strOrign(szValue), strResult;
			STRUTIL::StringReplace(strOrign, "\\r\\n", "\r\n", true, &strResult);
			pStringData->SetAtGrow( nIndex, strResult.c_str() );
		}

		++nCount;
		pValueElem = pValueElem->NextSiblingElement("VALUE");
	}

	if ( pStringData->IsEmpty() )
		delete pStringData;
	else
		*ppResStringArr = pStringData;

	return pStringData->IsEmpty()? FALSE : TRUE;
}

const BOOL CRanXMLParser::_ParseNUMBERExtract( CStringArray** ppResStringArr, TIXMLOLD::TiXmlElement* pSentense, const language::EMPROVIDE_LANGFLAG emProvideLang, const TCHAR* const szId )
{
	TIXMLOLD::TiXmlElement* pValueElem = pSentense->FirstChildElement("VALUE");
	if( !pValueElem )
		return FALSE;

	CString strCountry = language::strProvideLanguage[emProvideLang];
	if ( strCountry.IsEmpty() )
		return FALSE;

	WCHAR	szTemp[MAX_TEXTLEN]={0};
	TCHAR	szValue[MAX_TEXTLEN]={0};
	INT		nCount = 0;
	const int iCodesetIdx = language::ProvidelangToLangflag(emProvideLang);

	CONST CString strCommon("Common");
	CONST CString strBuffer;

	CStringArray* pStringData = new CStringArray;
	while( pValueElem )
	{
		_tcscpy(szValue, pValueElem->Attribute("Lang"));
		if( !IsValidData( szValue, "NUMBER/VALUE/Lang", nCount , szId ) )
		{
			pValueElem = pValueElem->NextSiblingElement("VALUE");
			++nCount;
			continue;
		}

		if ( strCountry == szValue || strCommon == szId )
		{
			_tcscpy( szValue, pValueElem->FirstChild()->Value());
			if( !IsValidData( szValue, "SENTENSE/VALUE", nCount, szId ) )
			{
				pValueElem = pValueElem->NextSiblingElement("VALUE");
				nCount++;
				continue;
			}

			MultiByteToWideChar(CP_UTF8,0,szValue,(int)pValueElem->FirstChild()->ValueStr().size()+1,szTemp,sizeof(szTemp)/sizeof(szTemp[0]));			
			WideCharToMultiByte(CD3DFontPar::nCodePage[iCodesetIdx], 0, szTemp, -1, szValue, sizeof(szValue)/sizeof(szValue[0]), NULL,NULL );
			pStringData->Add(szValue);
		}
		pValueElem = pValueElem->NextSiblingElement("VALUE");
		nCount++;
	}

	if ( pStringData->IsEmpty() )
		delete pStringData;
	else
		*ppResStringArr = pStringData;

	return pStringData->IsEmpty()? FALSE : TRUE;
}
