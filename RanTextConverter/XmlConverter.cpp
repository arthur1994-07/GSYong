
#include <afxwin.h>
#include <afxext.h>
#include <afxole.h>
#include <atlbase.h>
#include <atlwin.h>
#include <afxcoll.h>

#include <windows.h>
#include <math.h>
#include <time.h>

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <boost/lexical_cast.hpp>

// 링커에 넣어줘도됨;
#pragma comment( lib, "winmm.lib" )
#include "../SigmaCore/Xml/2.5.2/tinyxml.h"

#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/String/StringUtil.h"
#include "../SigmaCore/File/StringFile.h"
#include "../SigmaCore/File/SerialFile.h"
#include "../SigmaCore/Encrypt/Rijndael.h"
#include "../SigmaCore/String/basestring.h"
#include "../../SigmaCore/File/TxtFile.h"
#include "../../SigmaCore/Lua/MinLua.h"

#include "../SigmaCore/Xml/2.5.3/tinystr.h"
#include "XmlConverter.h"

#include "WStringFile.h"

using namespace TIXMLOLD;

const char*	STR_ROOTELEM_NAME = "TEXT";
const char* STR_ROOTELEM_NAME_02 = "BRANCH";
const char* STR_DATAELEM_NAME = "SENTENSE";
const char* STR_TEXTELEM_NAME = "VALUE";
const char* STR_TEXTATRB_NAME = "Lang";

INT iUTF_BYTEORDERMARK = 0xEFBBBF;

int nCountry = 6;
std::string szCountry[] = { "kr", "id", "ml", "pe", "th", "tw" };

XmlConverter::XmlConverter( const char* szVersion, const char* pEncoding )
{
// 	INT iBOM0 = 0xEF;
// 	INT iBOM1 = 0xBB;
// 	INT iBOM2 = 0xBF;
// 	// ks_c_5601-1987
// 	std::string strBOM;
// 	strBOM.append(reinterpret_cast<char*>(&iBOM0));
// 	strBOM.append(reinterpret_cast<char*>(&iBOM1));
// 	strBOM.append(reinterpret_cast<char*>(&iBOM2));
// 	strBOM.append(pEncoding);

	TiXmlDeclaration*	XmlDecl = new TiXmlDeclaration( szVersion, pEncoding, "");
	m_pXmlRootElem = new TiXmlElement(STR_ROOTELEM_NAME);
	m_XmlDocument.LinkEndChild( XmlDecl );
	m_XmlDocument.LinkEndChild( m_pXmlRootElem );
}

XmlConverter::~XmlConverter(void)
{
	m_XmlDocument.Clear();
}


const XmlConverter::XML_RETURN_GBGIM XmlConverter::GetChildElement( TIXMLOLD::TiXmlElement** outXmlElem, 
									   TIXMLOLD::TiXmlElement* rootElem,
									   const char* atrbName,
									   const char* atrbData)
{
	CString resTokenChild;
	CString strCompare;
	CString strChild, strElem, strValue;

	int nPosChild = 0;
	int nPosFormat = 0;

	if ( !rootElem ) return INVAILD_ROOTELEM;

	TIXMLOLD::TiXmlElement* nodeElem = rootElem; 
	TIXMLOLD::TiXmlElement* nodeElemChild = rootElem->FirstChildElement(); 	


	while ( nodeElemChild )
	{
		TIXMLOLD::TiXmlAttribute* atrb = nodeElemChild->FirstAttribute();
		while ( atrb )
		{
			// if ( atrb->Name() == atrbName && atrb->ValueStr().c_str() == atrbData )
			if ( StrCmp( atrb->Name(),  atrbName ) == 0 && 
				StrCmp( atrb->Value(), atrbData ) == 0)
			{
				(*outXmlElem) = nodeElemChild;
				return OK;
			}
			atrb = atrb->Next();
		}
		nodeElemChild = nodeElemChild->NextSiblingElement();
	}
	return NOEXIST_ELEM_OF_ATRB;
}

const XmlConverter::XML_RETURN_GBGIM XmlConverter::GetChildElement_MultiAtrb( TIXMLOLD::TiXmlElement** outXmlElem, 
									   TIXMLOLD::TiXmlElement* rootElem,
									   QueryAtrb* Atrb,
									   const int nCount )
{
	CString resTokenChild;
	CString strCompare;
	CString strChild, strElem, strValue;

	int nPosChild = 0;
	int nPosFormat = 0;

	if ( !rootElem ) return INVAILD_ROOTELEM;

	TIXMLOLD::TiXmlElement* nodeElem = rootElem; 
	TIXMLOLD::TiXmlElement* nodeElemChild = rootElem->FirstChildElement(); 	

	while ( nodeElemChild )
	{
		TIXMLOLD::TiXmlAttribute* atrb = nodeElemChild->FirstAttribute();
		while ( atrb )
		{
			INT nIndex;
			for ( nIndex = 0; nIndex < nCount; ++nIndex )
			{
				if ( StrCmp( atrb->Name(), Atrb[nIndex].atrbName ) != 0 ||
					StrCmp( atrb->Value(), Atrb[nIndex].atrbData ) != 0)
				{
					atrb = atrb->Next();
					break;
				}
				atrb = atrb->Next();
			}
			if ( nIndex == nCount )
			{
				(*outXmlElem) = nodeElemChild;
				return OK;
			}
		}
		nodeElemChild = nodeElemChild->NextSiblingElement();

// 		do{
// 			INT nIndex;
// 			for ( nIndex = 0; nIndex < nCount; ++nIndex )
// 			{
// 				if ( StrCmp( atrb->Name(), Atrb[nIndex].atrbName ) != 0 ||
// 					StrCmp( atrb->Value(), Atrb[nIndex].atrbData ) != 0)
// 				{
// 					atrb = atrb->Next();
// 					break;
// 				}
// 			}
// 			if ( nIndex == nCount )
// 				return OK;
// 		}
// 		while ((atrb = atrb->Next()))
	}
	return NOEXIST_ELEM_OF_ATRB;
}

/*
	<TEXT>
		<SENTENSE Id="SN_000_000">
			<V L="kr">발경</Data>
		</SENTENSE>
		<SENTENSE Id="SD_000_000">
			<V L="kr">전용무기 장착시 공격력 상승.</Data>
		</SENTENSE>
	</TEXT>
*/
bool XmlConverter::RanTextConvertToXml_Sentence( const char* szFile, const char* szSave, const char* szNational, bool bSave )
{
	TIXMLOLD::TiXmlDocument doc;
	if ( doc.LoadFile( szSave, TiXmlEncoding::TIXML_ENCODING_UTF8 ) )
	{
		m_XmlDocument = doc;
		m_pXmlRootElem = m_XmlDocument.RootElement();
		doc.Clear();
	}

	sc::CRijndael oRijndael;
	oRijndael.Initialize( sc::CRijndael::sm_Version[sc::CRijndael::VERSION-1].c_str(), sc::CRijndael::sm_KeyLength[sc::CRijndael::VERSION-1] );

	sc::CStringFile StrFile;
	// 스트링파일 클래스 오픈, 디코딩한다;
	if ( !StrFile.Open( szFile, FALSE ) )
		return 0;

	CString szLine;

	int i = 0;
	// ID : 항목 이름;
	// V : 항목 설명;
	DWORD dwTest;
	while ( (dwTest = StrFile.GetNextLine( szLine )) != 0 )
	{
		// 유니코드로 저장된것들 유니코드방식으로 읽는다;
		// std::string szUtf8 = sc::string::utf8_to_ansi(std::string(szLine.GetBuffer()));
		// std::string szConv = sc::string::ansi_to_utf8(szUtf8);
		CString szText( szLine );// szConv.c_str() );
		int iPosChild = 0;		

		CString szToken = szText.Tokenize( "\t", iPosChild );		// "\t"'Tab';

		TiXmlElement* pXmlElem;
		TiXmlElement* pXmlElement_child;
		try
		{
			if ( OK != GetChildElement( &pXmlElem, m_pXmlRootElem, "Id", szToken ) )
			{
				pXmlElem = new TiXmlElement(STR_DATAELEM_NAME);
				pXmlElem->SetAttribute("Ver", 1);
				pXmlElem->SetAttribute("Id", szToken );
				m_pXmlRootElem->LinkEndChild( pXmlElem );

				// default child construct
				for ( int i = 0; i < nCountry; ++i )
				{
					pXmlElement_child = new TiXmlElement(STR_TEXTELEM_NAME);
					pXmlElement_child->SetAttribute( STR_TEXTATRB_NAME, szCountry[i] );

					TiXmlText* pXmlText = new TiXmlText( "Skip" );
					pXmlElement_child->LinkEndChild(pXmlText);
					pXmlElem->LinkEndChild( pXmlElement_child );
				}
			}
			{
				if ( OK != GetChildElement( &pXmlElement_child, pXmlElem, STR_TEXTATRB_NAME, szNational ) )
				{
					pXmlElement_child = new TiXmlElement(STR_TEXTELEM_NAME);
					pXmlElement_child->SetAttribute( STR_TEXTATRB_NAME, szNational );
					pXmlElem->LinkEndChild( pXmlElement_child );
				}
				else
				{
					if ( pXmlElement_child->FirstChild() )
						pXmlElement_child->RemoveChild( pXmlElement_child->FirstChild() );
				}

				szToken = szText.Tokenize( "\t", iPosChild );
				int nFind;
				CString szTemp = szToken;
				std::string strFind = "\\r\\n";
				std::string strReplace = "\r\n";
				while( ( nFind = szTemp.Find( strFind.c_str() ) ) != -1 )
				{
					szTemp.Delete( nFind, strFind.length() );
					szTemp.Insert( nFind, strReplace.c_str() );
				}
				if ( strcmp(" ", szTemp) == 0 && strlen(szTemp) == 1 )
				{
					szTemp = "Skip";
				}
				TiXmlText* pXmlText = new TiXmlText( szToken.IsEmpty() ? "Skip" : szTemp );
				pXmlElement_child->LinkEndChild( pXmlText );
			}
		}
		// 여기에 들어오면 빈줄이있는것이다 ㅡㅡ;
		catch (COleException* e)
		{
			int aaa = 0;
			aaa = 0;
		}
	}

	m_XmlDocument.SaveFile( szSave );
	StrFile.Close();
	return true;
}

bool XmlConverter::RanTipTextConvertToXml( const char* szFile, const char* szSave, const char* szNational, bool bSave )
{
	TIXMLOLD::TiXmlDocument doc;
	if ( doc.LoadFile( szSave, TiXmlEncoding::TIXML_ENCODING_UTF8 ) )
	{
		m_XmlDocument = doc;
		m_pXmlRootElem = m_XmlDocument.RootElement();
		//doc.Clear();
	}
	else
	{
		//TiXmlDeclaration*	XmlDecl = new TiXmlDeclaration( "1.0", "utf-8", "");
		//m_pXmlRootElem = new TiXmlElement(STR_ROOTELEM_NAME);
		//m_XmlDocument.LinkEndChild(XmlDecl);
		//m_XmlDocument.LinkEndChild(XmlDecl);
	}


	sc::CRijndael oRijndael;
	oRijndael.Initialize( sc::CRijndael::sm_Version[sc::CRijndael::VERSION-1].c_str(), sc::CRijndael::sm_KeyLength[sc::CRijndael::VERSION-1] );

	sc::CStringFile StrFile;
	// 스트링파일 클래스 오픈, 디코딩한다;
	if ( !StrFile.Open( szFile, FALSE ) )
		return 0;

	char* szTokenSeparate = ";";
	CString szLine;
	DWORD dwTest, nCount = 0;
	
	CStringArray	strArray;
	while ( (dwTest = StrFile.GetNextLine( szLine )) != 0 )
	{
		if ( strcmp( &(szLine.GetString()[0]), szTokenSeparate ) != 0 )
		{
			strArray.Add( szLine );
			continue;
		}

		char szCombineText[500] = {0};
		INT iLenght = 0;
		const INT iSize = strArray.GetSize();
		for ( INT i = 0; i < iSize; ++i )
		{
			CString szString = strArray.GetAt(i);

// 			char showsz[255];
// 			strcpy( showsz, szString.GetString() );
// 			WCHAR showwsz[255];
// 			MultiByteToWideChar(CP_UTF8, 0, showsz, strlen(showsz)+1,showwsz,sizeof(showwsz)/sizeof(showwsz[0]));			
// 			WideCharToMultiByte(949, 0, showwsz, -1, showsz, sizeof(showsz)/sizeof(showsz[0]), NULL,NULL );

 		char szTemp[255] = { "\r\n" };
 		WCHAR wszTemp[255];
 		MultiByteToWideChar(CP_UTF8, 0, szTemp, strlen(szTemp)+1,wszTemp,sizeof(wszTemp)/sizeof(wszTemp[0]));			
 		WideCharToMultiByte(CP_UTF8, 0, wszTemp, -1, szTemp, sizeof(szTemp)/sizeof(szTemp[0]), NULL,NULL );

			szString.Append(szTemp);
			const char* pSz = static_cast<const char*>(szString.GetString());
			strcpy( &szCombineText[iLenght], pSz );
			iLenght += strlen(pSz);
		}
		strArray.RemoveAll();

		CString szText( szCombineText );
		int iPosChild = 0;

		TiXmlElement* pXmlElem;
		TiXmlElement* pXmlElement_child;
		
		char szCount[5] = {0};
		itoa(nCount, szCount, 10);
		try
		{
			if ( OK != GetChildElement( &pXmlElem, m_pXmlRootElem, "Id", szCount ) )
			{
				pXmlElem = new TiXmlElement(STR_DATAELEM_NAME);
				pXmlElem->SetAttribute("Ver", 1);
				pXmlElem->SetAttribute("Id", szCount );
				m_pXmlRootElem->LinkEndChild( pXmlElem );

				// default child construct
				for ( int i = 0; i < nCountry; ++i )
				{
					pXmlElement_child = new TiXmlElement(STR_TEXTELEM_NAME);
					pXmlElement_child->SetAttribute( STR_TEXTATRB_NAME, szCountry[i] );

					TiXmlText* pXmlText = new TiXmlText( "Skip" );
					pXmlElement_child->LinkEndChild(pXmlText);
					pXmlElem->LinkEndChild( pXmlElement_child );
				}
			}
			{
				if ( OK != GetChildElement( &pXmlElement_child, pXmlElem, STR_TEXTATRB_NAME, szNational ) )
				{
					pXmlElement_child = new TiXmlElement(STR_TEXTELEM_NAME);
					pXmlElement_child->SetAttribute( STR_TEXTATRB_NAME, szNational );
					pXmlElem->LinkEndChild( pXmlElement_child );
				}
				else
				{
					if ( pXmlElement_child->FirstChild() )
						pXmlElement_child->RemoveChild( pXmlElement_child->FirstChild() );
				}

				int nFind=0;
				CString szResult = szText;
				std::string strFind = "\\r\\n";
				std::string strReplace = "\r\n";
				while( ( nFind = szResult.Find( strFind.c_str() ) ) != -1 )
				{
					szResult.Delete( nFind, strFind.length() );
					szResult.Insert( nFind, strReplace.c_str() );
				}
				TiXmlText* pXmlText = new TiXmlText( szText.IsEmpty() ? "Skip" : szResult );
				pXmlElement_child->LinkEndChild( pXmlText );
			}
		}
		// 여기에 들어오면 빈줄이있는것이다 ㅡㅡ;
		catch (COleException* e)
		{
			int aaa = 0;
			aaa = 0;
		}
		nCount++;
	}

	m_XmlDocument.SaveFile( szSave );
	StrFile.Close();
	return true;
}

bool XmlConverter::RanTextConvertToXml_Word( const char* szFile, const char* szSave, const char* szNational, bool bSave )
{
	TIXMLOLD::TiXmlDocument doc;
	if ( doc.LoadFile( szSave, TiXmlEncoding::TIXML_ENCODING_UTF8 ) )
	{
		m_XmlDocument = doc;
		m_pXmlRootElem = m_XmlDocument.RootElement();
		doc.Clear();
	}

	sc::CRijndael oRijndael;
	oRijndael.Initialize( sc::CRijndael::sm_Version[sc::CRijndael::VERSION-1].c_str(), sc::CRijndael::sm_KeyLength[sc::CRijndael::VERSION-1] );

	sc::CStringFile StrFile;
	// 스트링파일 클래스 오픈, 디코딩한다;
	if ( !StrFile.Open( szFile, FALSE ) )
		return 0;

	CString szLine;

	int i = 0;
	// ID : 항목 이름;
	// V : 항목 설명;
	DWORD dwTest;
	TiXmlElement* pXmlElem;
	bool bTEST = 1;
	while ( (dwTest = StrFile.GetNextLine( szLine )) != 0 )
	{
		CString szText( szLine );
		int iPosChild = 0, iPosChildNext = 0;		

		CString szToken = szText.Tokenize( "\t", iPosChild );		// "\t"'Tab';

		TiXmlElement* pXmlElement_child;
		try
		{
			const INT iCMP = strncmp(&szLine.GetString()[1], "N", 1);
			if ( (iCMP == 0 && OK != GetChildElement( &pXmlElem, m_pXmlRootElem, "Id", szToken )) || bTEST )
			{
				bTEST = false;
				pXmlElem = new TiXmlElement("WORD");
				pXmlElem->SetAttribute("Ver", 1);
				pXmlElem->SetAttribute("Id", &szText.GetString()[iPosChild] );
				m_pXmlRootElem->LinkEndChild( pXmlElem );
			}
			else /*if ( strcmp( szLine[1], "D" ) == 0 )*/{
				std::vector<std::string> tokens;
				std::string strMessage = szLine.GetString();
				boost::split(tokens, strMessage, boost::is_any_of("\t"));
				if ( tokens.size() != 3)
				{
					::MessageBoxA( NULL, " Token\"TAB\" Count Error", "WOW", MB_OK );
				}
				CString szData = tokens[1].c_str();
				CString szIndex = tokens[2].c_str();

				QueryAtrb FindAtrb[2] = 
				{
					QueryAtrb(const_cast<char*>(STR_TEXTATRB_NAME), const_cast<char*>(szNational)),
					QueryAtrb("Index", const_cast<char*>(szIndex.GetString()))
				};

				if ( OK != GetChildElement_MultiAtrb(
					&pXmlElement_child, pXmlElem, FindAtrb, 2 ) )
				{
					pXmlElement_child = new TiXmlElement(STR_TEXTELEM_NAME);
					pXmlElement_child->SetAttribute( STR_TEXTATRB_NAME, szNational );
					pXmlElement_child->SetAttribute( "Index", FindAtrb[1].atrbData );
					pXmlElem->LinkEndChild( pXmlElement_child );
				}
				else
				{
					if ( pXmlElement_child->FirstChild() )
						pXmlElement_child->RemoveChild( pXmlElement_child->FirstChild() );
				}
				int nFind=0;
				CString szResult = szText;
				std::string strFind = "\\r\\n";
				std::string strReplace = "\r\n";
				while( ( nFind = szResult.Find( strFind.c_str() ) ) != -1 )
				{
					szResult.Delete( nFind, strFind.length() );
					szResult.Insert( nFind, strReplace.c_str() );
				}
				TiXmlText* pXmlText = new TiXmlText( szData.IsEmpty() ? "Skip" : szResult.GetString() );
				pXmlElement_child->LinkEndChild( pXmlText );
			}
		}
		// 여기에 들어오면 빈줄이있는것이다 ㅡㅡ;
		catch (COleException* e)
		{
			int aaa = 0;
			aaa = 0;
		}
	}

	m_XmlDocument.SaveFile( szSave );
	StrFile.Close();
	return true;
}

bool XmlConverter::RanTextConvertToXml_EX( const char* szFile, const char* szSave, const char* szNational, bool bSave )
{
	TIXMLOLD::TiXmlDocument doc;
	if ( doc.LoadFile( szSave, TiXmlEncoding::TIXML_ENCODING_UTF8 ) )
	{
		m_XmlDocument = doc;
		m_pXmlRootElem = m_XmlDocument.RootElement();
		doc.Clear();
	}

	sc::CRijndael oRijndael;
	oRijndael.Initialize( sc::CRijndael::sm_Version[sc::CRijndael::VERSION-1].c_str(), sc::CRijndael::sm_KeyLength[sc::CRijndael::VERSION-1] );

	sc::CStringFile StrFile;
	// 스트링파일 클래스 오픈, 디코딩한다;
	if ( !StrFile.Open( szFile, FALSE ) )
		return 0;

	CString szLine;

	int i = 0;
	// ID : 항목 이름;
	// V : 항목 설명;
	DWORD dwTest;
	TiXmlElement* pXmlElem;
	while ( (dwTest = StrFile.GetNextLine( szLine )) != 0 )
	{
		CString szText(szLine);
		std::vector<std::string> tokens;
		std::string strMessage = szLine.GetString();
		boost::split(tokens, strMessage, boost::is_any_of("\t"));
		if ( tokens.size() == 0 )
			continue;

		TiXmlElement* pXmlElement_Child;
		try
		{
			const INT iNameCodeCompare = strncmp( &tokens[0].c_str()[1], "N", 1 );

			if ( iNameCodeCompare == 0 )
			{
				if ( OK == GetChildElement( &pXmlElem, m_pXmlRootElem, "Id", tokens[1].c_str() ) )
					continue;

				pXmlElem = new TiXmlElement("NAME");
				pXmlElem->SetAttribute( "Ver", 1 );
				pXmlElem->SetAttribute( "Id", tokens[1].c_str() );
				m_pXmlRootElem->LinkEndChild( pXmlElem );
			}
			else
			{
				if ( OK != GetChildElement( &pXmlElement_Child, pXmlElem, "MID", tokens[2].c_str() ) )
				{
					pXmlElement_Child = new TiXmlElement("CONVERSATION");
					pXmlElement_Child->SetAttribute("MID", tokens[2].c_str());
					pXmlElem->LinkEndChild(pXmlElement_Child);
				}

				const std::string strID		= tokens[3].c_str();
				const std::string strIndex	= tokens.size() > 4 ? tokens[4].c_str() : "0";
				QueryAtrb FindAtrb[3] =
				{
					QueryAtrb("ID",		const_cast<char*>(strID.c_str())),
					QueryAtrb("Index",	const_cast<char*>(strIndex.c_str()) ),
					QueryAtrb("Lang",	const_cast<char*>(szNational) )
				};

				TiXmlElement* pXmlElem_Grandchild;
				if ( OK != GetChildElement_MultiAtrb(&pXmlElem_Grandchild, pXmlElement_Child, FindAtrb, 3))
				{
					pXmlElem_Grandchild = new TiXmlElement("SUB");
					pXmlElem_Grandchild->SetAttribute( "Id", strID.c_str() );
					pXmlElem_Grandchild->SetAttribute( "Index", strIndex.c_str() );
					pXmlElem_Grandchild->SetAttribute( "Lang", szNational );
					pXmlElement_Child->LinkEndChild( pXmlElem_Grandchild );
				}
				else
				{
					if ( pXmlElem_Grandchild->FirstChild() )
						pXmlElem_Grandchild->RemoveChild( pXmlElem_Grandchild->FirstChild() );
				}

// 				int nFind=0;
// 				CString szResult = szText;
// 				std::string strFind = "\\r\\n";
// 				std::string strReplace = "\r\n";
// 				while( ( nFind = szResult.Find( strFind.c_str() ) ) != -1 )
// 				{
// 					szResult.Delete( nFind, strFind.length() );
// 					szResult.Insert( nFind, strReplace.c_str() );
// 				}
				TiXmlText* pXmlText = NULL;
				if ( tokens[1].empty() )
				{
					pXmlText = new TiXmlText( "Skip" );
				}
				else
				{
					int nFind=0;
					CString szResult = tokens[1].c_str();
					std::string strFind = "\\r\\n";
					std::string strReplace = "\r\n";
					while( ( nFind = szResult.Find( strFind.c_str() ) ) != -1 )
					{
						szResult.Delete( nFind, strFind.length() );
						szResult.Insert( nFind, strReplace.c_str() );
					}
					pXmlText = new TiXmlText( szResult.GetString() );
				}
				pXmlElem_Grandchild->LinkEndChild( pXmlText );
			}
		}
		catch (...)
		{
			int nDummy;
			nDummy = 0;
		}
	}

	m_XmlDocument.SaveFile( szSave );
	StrFile.Close();
	return true;
}

enum EM_LUA_CONVERT
{
	NOT = -1,
	TABLE_FIND,
	TABLE_START,
	TABLE_READ,
	TABLE_END,
};

// enum TOKENTYPE
// {
// 	TTYPE_NAME = 0,
// 	TTYPE_OPERATOR,
// 	TTYPE_BEGIN,
// 	TTYPE_END,
// 	TTYPE_DATA,
// 	TTYPE_SPLIT,		// 사용안되는 문자
// };
// struct LUATOKEN
// {
// 	TOKENTYPE	emTYPE;
// 	CString		str;
// 	INT			iUserData;
// };

TiXmlElement*	XmlConverter::CompositionData( WStringFile& WStrFile, sc::CStringFile& StrFile, CString& strCurrentLine, TiXmlElement* pParentElem, const char* szNation )
{
	CString	strSplit[3] = { "\t", "\r\n", " " };
	std::wstring wstrSplit[3] = { L"\t", L"\r\n", L" " };
	CString strOperator = "=", strBegin = "{", strEnd = "}";

	INT iCount = 0;
	CString strLine, strSentence;
	INT iDepth = 0, iMaxDepth = 0;
	bool bBOMInit = false;
	
	std::string szLine;
	std::wstring wcsLine, wcsSentence;
	while ( WStrFile.GetNextLine(szLine, wcsLine) != false )
	{
		strLine = szLine.c_str();
		// 주석은 쌩깐다;
 		if ( strncmp( strLine.GetString(), "--", 2 ) == 0 )
 			continue;		

		strSentence.Append( strLine );
		wcsSentence.append( wcsLine );

		CString strBeginCheck = strLine, strEndCheck = strLine;
		INT iFind = 0;
		
		while ((iFind = wcsLine.find(L"{", iFind)) != -1 )// ( (iFind = strLine.Find( strBegin, iFind )) != -1 )
		{
			iMaxDepth = (++iDepth);			iFind += 1;
		}
		
		iFind = 0;
		while ((iFind = wcsLine.find(L"}", iFind)) != -1)//( (iFind = strLine.Find( strEnd, iFind )) != -1 )
		{
			iDepth--;			iFind += 1;
		}

		if ( iMaxDepth != 0 && iDepth == 0 )
		{
			// <CHAR>한문장으로 만든다;
			CString strConv = strSentence;
			for ( INT i = 0; i < 3; ++i )
			{
				std::vector<std::string>	tokens;
				std::string Res = const_cast<char*>( strConv.GetString() );
				char* szSplit = const_cast<char*>( strSplit[i].GetString() );
				boost::split( tokens, Res, boost::is_any_of(szSplit) );
				strConv.Empty();
				for ( INT j = 0; j < tokens.size(); ++j )
				{
					strConv.Append( tokens[j].c_str() );
				}
			}
			strSentence = strConv;

			unsigned char iEOF = EOF;
			INT iFind02=0;
			while ( (iFind02 = strSentence.Find(static_cast<char>(iEOF), iFind02)) != -1 )
			{
				strSentence.Delete( iFind02,strlen("\r\n") );
			}

			// <WCHAR_T>한문장으로 만든다;
			std::wstring wstrConv = wcsSentence;
			for ( INT i = 0; i < 2; ++i )
			{
				std::vector<std::wstring>	tokens;
				std::wstring wcsRes = const_cast<wchar_t*>(wstrConv.c_str());
				wchar_t* wcsSplit = const_cast<WCHAR*>(wstrSplit[i].c_str());
				boost::split( tokens, wcsRes, boost::is_any_of(wcsSplit) );
				wstrConv.clear();
				for ( INT j = 0; j < tokens.size(); ++j )
				{
					wstrConv.append( tokens[j].c_str() );
				}
			}
			wcsSentence = wstrConv;

			// TiXmlElement* pNode = CompositionSentence( WStrFile, strSentence, wcsSentence, NULL );
			WCSCompositionSentence( WStrFile, strSentence, wcsSentence, m_pXmlRootElem, szNation );
			iMaxDepth = 0;
			iCount++;
			strSentence.Empty();
			if ( wcsncmp( wcsSentence.c_str(), L"BRIGHT", wcslen(L"BRIGHT")) == 0 )
			{
				INT iDEBUGTEST;
				iDEBUGTEST = 0;
			}
			wcsSentence.clear();
		}
	}
	return NULL;
}

enum SEPERATOR_STD
{
	SS_OPERATOR_EQUAR = 0,
	SS_BRAKET_BEGIN,
	SS_BRAKET_END,
	SS_MARK_START,
	SS_MARK_END,
	SS_COMMA,
};

struct SPERATOR_STD_EX
{
	SEPERATOR_STD	EM_MARK;
	INT				N_STRINGAT;
};
CString			strSeperatorS[6] = { "=", "{", "} ", "L\"", "\"", ","};
std::wstring	wstrSeperator[6] = { L"=", L"{", L"} ", L"L\"", L"\"", L","};
INT				iSeperatorLen[6];

TiXmlElement* XmlConverter::WCSCompositionSentence( WStringFile& WStrFile, CString& strSentence, std::wstring& wstrSentence, TiXmlElement* pParentElem, const char* szNation )
{
	TiXmlNode::NodeType	EMNODE = TiXmlNode::UNKNOWN;

	// Value, Node, Realization;
	const INT iBraket = wstrSentence.find( L"{" );

	wchar_t wcsNAME[255];
	wcsncpy( wcsNAME, wstrSentence.c_str(), iBraket );
	if ( wcsncmp( &wcsNAME[iBraket-1], L"=", 1 ) == 0 )
	{
		EMNODE = TiXmlNode::ELEMENT;
		wcsNAME[iBraket-1] = 0;
	}
	else
	{
		EMNODE = TiXmlNode::TEXT;
	}


	std::vector<SPERATOR_STD_EX>	vecSS;
	for ( INT i = 0; i < static_cast<INT>(wstrSentence.size()); ++i )
	{
		for ( INT j = 0; j < 6; ++j )
		{
			if ( wcsncmp( &wstrSentence.c_str()[i], wstrSeperator[j].c_str(), j == 3 ? 2 : 1 ) == 0 )
			{
				// 따옴표 시작시 따옴표 끝날때까지 다른 기호들은 쌩깐다;
				const INT iSSSize = static_cast<INT>( vecSS.size() );
				if ( iSSSize != 0 )
				{
					if ( vecSS[iSSSize-1].EM_MARK == SS_MARK_START && static_cast<SEPERATOR_STD>(j) != SS_MARK_END )
						break;
				}
				SPERATOR_STD_EX SSE;
				SSE.EM_MARK = static_cast<SEPERATOR_STD>(j);
				SSE.N_STRINGAT = i;
				vecSS.push_back( SSE );

				if ( static_cast<SEPERATOR_STD>(j) == SS_MARK_START )
					++i;

				break;
			}
		}
	}

	INT nDataCount = 0;
	TiXmlElement* pElem = TiXmlNode::ELEMENT ? new TiXmlElement("WORD") : pParentElem;
// 	if ( strcmp( pElem->Value(), "WORD" ) == 0 )
// 	{
// 		m_XmlDocument.RootElement()->LinkEndChild( pElem );
// 	}

	INT iDepth = 0, iMaxDepth = 0, iVecSize = static_cast<INT>(vecSS.size());
	for ( INT i = 0; i < iVecSize; ++i )
	{
		switch ( static_cast<SEPERATOR_STD>( vecSS[i].EM_MARK ) )
		{
		case SS_OPERATOR_EQUAR:
			{
//	Byte oder mark delete;
// 				INT iBOM0 = 0xEF;
// 				INT iBOM1 = 0xBB;
// 				INT iBOM2 = 0xBF;
				wchar_t wcsName[512];
				wcsncpy(wcsName, wstrSentence.c_str(), vecSS[i].N_STRINGAT);
				wcsName[vecSS[i].N_STRINGAT] = 0;
// 				char szNAMEEX[ 255 ];;
// 				strcpy( szNAMEEX, reinterpret_cast<char*>(&iBOM0) );
// 				strcpy( &szNAMEEX[strlen(szNAMEEX)], reinterpret_cast<char*>(&iBOM1) );
// 				strcpy( &szNAMEEX[strlen(szNAMEEX)], reinterpret_cast<char*>(&iBOM2) );
// 				strcpy( &szNAMEEX[strlen(szNAMEEX)], szNAME );

				// 공백 제거;
				std::vector<std::wstring> tokens;
				boost::split(tokens, wcsName, boost::is_any_of(L" "));
				std::wstring wcsNameConv;
				for ( INT j = 0; j < tokens.size(); ++j )
				{
					wcsNameConv.append(tokens[j].c_str());
				}
				
				char szName[512];
				WideCharToMultiByte(CP_UTF8, 0, wcsNameConv.c_str(), -1, szName, sizeof(szName)/sizeof(szName[0]), NULL,NULL );

				if (OK != GetChildElement(&pElem, pParentElem, "Id", szName))
				{
					pElem->SetAttribute( "Ver", 1 );
					pElem->SetAttribute( "Id", szName );
					m_XmlDocument.RootElement()->LinkEndChild( pElem );
				}
			}
			break;
		case SS_BRAKET_BEGIN:
			{
				iMaxDepth = ++iDepth;
				if ( vecSS[i+1].EM_MARK == SS_COMMA )
				{
					wchar_t wcsData[512] = {0};
					wcsncpy(wcsData
						,&wstrSentence.c_str()[vecSS[i].N_STRINGAT + 1]
					, vecSS[i+1].N_STRINGAT-vecSS[i].N_STRINGAT-1 );

					// 공백 제거;
					std::vector<std::wstring> tokens;
					boost::split(tokens, wcsData, boost::is_any_of(L" "));
					std::wstring wcsNameConv;
					for ( INT j = 0; j < tokens.size(); ++j )
					{
						wcsNameConv.append(tokens[j].c_str());
					}
					char	szData[512] = {0};
					WideCharToMultiByte(CP_UTF8, 0, wcsNameConv.c_str(), -1, szData, sizeof(szData)/sizeof(szData[0]), NULL,NULL );

					char STRINDEX[5] = {0};
					itoa(nDataCount, STRINDEX, 10);

					QueryAtrb FindAtrb[2] =
					{
						QueryAtrb(const_cast<char*>(STR_TEXTATRB_NAME), const_cast<char*>(szNation)),
						QueryAtrb("Index", const_cast<char*>(STRINDEX))
					};
					TiXmlElement* pXmlElement_Child = NULL;
					if ( OK != GetChildElement_MultiAtrb( &pXmlElement_Child, pElem, FindAtrb, 2 ))
					{
						pXmlElement_Child = new TiXmlElement("VALUE");
						pXmlElement_Child->SetAttribute("Lang", szNation);
						pXmlElement_Child->SetAttribute("Index", nDataCount);
						pElem->LinkEndChild(pXmlElement_Child);
					}
					else
					{
						if ( pXmlElement_Child->FirstChild() )
							pXmlElement_Child->RemoveChild( pXmlElement_Child->FirstChild() );
					}
					nDataCount++;
					TiXmlText* pXmlText = new TiXmlText(szData);
					pXmlElement_Child->LinkEndChild(pXmlText);
				}
			}
			break;
		case SS_BRAKET_END:				{	--iDepth;	}						break;
		case SS_MARK_START:
			{
				if ( vecSS[i+1].EM_MARK != SS_MARK_END ){
					MessageBoxW( NULL, L"따옴표 시작 후 따옴표 끝이없습니다.", L"ERROR", MB_OK );
					break;
				}

				wchar_t wcsData[255] = {0};
				wcsncpy(wcsData
					, &wstrSentence.c_str()[vecSS[i].N_STRINGAT+2]
					, vecSS[i+1].N_STRINGAT - vecSS[i].N_STRINGAT-2);

				char szData[255] = {0};
				WideCharToMultiByte(CP_UTF8, 0, wcsData, -1, szData, sizeof(szData)/sizeof(szData[0]), NULL,NULL );

				if ( strlen(szData) <= 0 )
					break;

// 				TiXmlElement* pChildElem = new TiXmlElement("VALUE");
// 				pChildElem->SetAttribute( "Index", nDataCount++ );
// 				pChildElem->SetAttribute( "Lang", szNation );
// 				TiXmlText* pXmlText = new TiXmlText(szData);
// 
// 				pChildElem->LinkEndChild( pXmlText );
// 				pElem->LinkEndChild( pChildElem );

				char STRINDEX[5] = {0};
				itoa(nDataCount, STRINDEX, 10);

				QueryAtrb FindAtrb[2] =
				{
					QueryAtrb(const_cast<char*>(STR_TEXTATRB_NAME), const_cast<char*>(szNation)),
					QueryAtrb("Index", const_cast<char*>(STRINDEX))
				};
				TiXmlElement* pXmlElement_Child = NULL;
				if ( OK != GetChildElement_MultiAtrb( &pXmlElement_Child, pElem, FindAtrb, 2 ))
				{
					pXmlElement_Child = new TiXmlElement("VALUE");
					pXmlElement_Child->SetAttribute("Lang", szNation );
					pXmlElement_Child->SetAttribute("Index", STRINDEX );
					pElem->LinkEndChild(pXmlElement_Child);
				}
				else
				{
					if ( pXmlElement_Child->FirstChild() )
						pXmlElement_Child->RemoveChild( pXmlElement_Child->FirstChild() );
				}				
				nDataCount++;
				TiXmlText* pXmlText = new TiXmlText(szData);
				pXmlElement_Child->LinkEndChild(pXmlText);
			}
			break;
		case SS_MARK_END:			break;
		case SS_COMMA:
			{
				if ( vecSS[i+1].EM_MARK != SS_COMMA && vecSS[i+1].EM_MARK != SS_BRAKET_END )
					break;

				wchar_t wcsData[512] = {0};
				wcsncpy(wcsData
					,&wstrSentence.c_str()[vecSS[i].N_STRINGAT + 1]
					, vecSS[i+1].N_STRINGAT-vecSS[i].N_STRINGAT-1 );

				// 공백 제거;
				std::vector<std::wstring> tokens;
				boost::split(tokens, wcsData, boost::is_any_of(L" "));
				std::wstring wcsNameConv;
				for ( INT j = 0; j < tokens.size(); ++j )
				{
					wcsNameConv.append(tokens[j].c_str());
				}

				if ( tokens.size() == 0 || wcslen(wcsNameConv.c_str()) <= 0)
					break;

				char	szData[512] = {0};
				WideCharToMultiByte(CP_UTF8, 0, wcsNameConv.c_str(), -1, szData, sizeof(szData)/sizeof(szData[0]), NULL,NULL );

				char STRINDEX[5] = {0};
				itoa(nDataCount, STRINDEX, 10);

				QueryAtrb FindAtrb[2] =
				{
					QueryAtrb(const_cast<char*>(STR_TEXTATRB_NAME), const_cast<char*>(szNation)),
					QueryAtrb("Index", const_cast<char*>(STRINDEX))
				};
				TiXmlElement* pXmlElement_Child = NULL;
				if ( OK != GetChildElement_MultiAtrb( &pXmlElement_Child, pElem, FindAtrb, 2 ))
				{
					pXmlElement_Child = new TiXmlElement("VALUE");
					pXmlElement_Child->SetAttribute("Lang", szNation);
					pXmlElement_Child->SetAttribute("Index", nDataCount);
					pElem->LinkEndChild(pXmlElement_Child);
				}
				else
				{
					if ( pXmlElement_Child->FirstChild() )
						pXmlElement_Child->RemoveChild( pXmlElement_Child->FirstChild() );
				}
				nDataCount++;
				TiXmlText* pXmlText = new TiXmlText(szData);
				pXmlElement_Child->LinkEndChild(pXmlText);
			}
			break;
		}
		
	}
	return pElem;
}

/*
 *	0. Global
 *	1. Find "{"
 *	2. Value composition
 *		- Find Name?	-> Value Composition
 *		--Else ?		-> Current Node Data realization
 *	3. Find "}"
 *		- is Find "{"	-> Jump[2], Depth Increase
 *		--Else ?		-> Current Node Data realization
 */
bool XmlConverter::RanTextConvertToXml_CommentLua( const char* szFile, const char* szSave, const char* szNational, bool bSave )
{
	for ( int i = 0 ; i < 6 ; ++i )
	{		
		char szName[10];		strcpy( szName, strSeperatorS[i] );
		wchar_t wName[10];
		MultiByteToWideChar(CP_UTF8, 0, szName, strlen(szName)+1,wName,sizeof(wName)/sizeof(wName[0]));			
		WideCharToMultiByte(CP_UTF8, 0, wName, -1, szName, sizeof(szName)/sizeof(szName[0]), NULL,NULL );
		iSeperatorLen[i] = strlen(szName);
	}

	WStringFile WStrFile;
	WStrFile.LoadFile( szFile );

	TIXMLOLD::TiXmlDocument doc;
	if ( doc.LoadFile( szSave, TiXmlEncoding::TIXML_ENCODING_UTF8 ) )
	{
		m_XmlDocument = doc;
		m_pXmlRootElem = m_XmlDocument.RootElement();
		doc.Clear();
	}

	sc::CStringFile StrFile;
	// 스트링파일 클래스 오픈, 디코딩한다;
	if ( !StrFile.Open( szFile, FALSE ) )
		return 0;
	
	CString strEOL("\r\n");
	DWORD	dwTEST;

	TiXmlElement* pElem;
	CString strTEMP;
	CompositionData( WStrFile, StrFile, strTEMP, pElem, szNational );

	m_XmlDocument.SaveFile( szSave );
	StrFile.Close();
	return true;
}