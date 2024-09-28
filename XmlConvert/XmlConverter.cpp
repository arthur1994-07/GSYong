

#define _AFXDLL


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
#include "../SigmaCore/File/TxtFile.h"
#include "../SigmaCore/Lua/MinLua.h"

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
	CString strChild, strElem, strValue, strAtrbData;

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
			
// 			if ( StrCmp(strAtrbData.GetString(), atrb->Value()) == 0)
// 				return NOEXIST_ELEM_OF_ATRB;
				
			strAtrbData = atrb->Value();
			atrb = atrb->Next();
		}
		if ( nodeElemChild == nodeElemChild->NextSiblingElement() )
			break;

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
	TiXmlElement* pXmlElem;		CString strLineBackup;
	while ( (dwTest = StrFile.GetNextLine( szLine )) != 0 )
	{
		++i;
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

			if ( iNameCodeCompare == 0 || i == 1 )
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
				if ( tokens.size() < 4 )
					continue;

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
				if ( i%500 == 0 )
					m_XmlDocument.SaveFile( szSave );

				int tt = 0;
				tt = 1;
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

bool XmlConverter::RanTextConvert_Skip(const char* szFile, const char* szSave, const char* szNational, bool bSave)
{
	TIXMLOLD::TiXmlDocument doc;
	if ( doc.LoadFile( szSave, TiXmlEncoding::TIXML_ENCODING_UTF8  ))
	{
		m_XmlDocument = doc;
		m_pXmlRootElem = m_XmlDocument.RootElement();
		doc.Clear();
	}

	// TiXmlElement* pParentElem = m_pXmlRootElem->NextSiblingElement("TEXT");
	TiXmlElement* pXmlElem;
	for ( pXmlElem = m_pXmlRootElem->FirstChildElement(); 
		  pXmlElem != NULL; pXmlElem = pXmlElem->NextSiblingElement() )
	{
		TiXmlElement* pChildElem;
		bool bDefaultText = false;
		for ( pChildElem = pXmlElem->FirstChildElement();
			pChildElem != NULL; pChildElem = pChildElem->NextSiblingElement() )
		{
			TIXMLOLD::TiXmlAttribute* atrb = pChildElem->FirstAttribute();
			if ( StrCmp( atrb->Name(), "Lang" ) == 0 &&
				StrCmp( atrb->Value(), szNational ) == 0)
			{
				bDefaultText = true;
				break;
			}
		}

		if ( bDefaultText == false )
		{
			if ( StrCmp(pXmlElem->Value(), "SENTENSE") == 0 )
			{
				TiXmlElement* pDefaultElem = new TiXmlElement("VALUE");
				pDefaultElem->SetAttribute( "Lang", szNational );

				TiXmlText* pXmlText = new TiXmlText( "Skip" );
				pDefaultElem->LinkEndChild(pXmlText);
				pXmlElem->LinkEndChild( pDefaultElem );
			}
		}
	}

	m_XmlDocument.SaveFile( szSave );
	return true;
}