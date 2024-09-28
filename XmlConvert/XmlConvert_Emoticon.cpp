

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

bool XmlConverter::RanTextConvert_Emoticon(const char* szFile, const char* szSave, const char* szNational, bool bSave)
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

	TIXMLOLD::TiXmlElement* pMotionElement, *pTermElement, *pListElement;
// 
// 	if ( OK != GetChildElement( &pMotionElement, m_pXmlRootElem, "Id", "EMOTICON_MOTIONLIST" ) )
// 	{
// 		pMotionElement = new TIXMLOLD::TiXmlElement("SENTENSE");
// 		pMotionElement->SetAttribute("Ver", 1);
// 		pMotionElement->SetAttribute("Id", "EMOTICON_MOTIONLIST" );
// 		m_pXmlRootElem->LinkEndChild( pMotionElement );
// 	}
// 	if ( OK != GetChildElement( &pTermElement, m_pXmlRootElem, "Id", "EMOTICON_TERMLIST" ) )
// 	{
// 		pTermElement = new TIXMLOLD::TiXmlElement("SENTENSE");
// 		pTermElement->SetAttribute("Ver", 1);
// 		pTermElement->SetAttribute("Id", "EMOTICON_TERMLIST" );
// 		m_pXmlRootElem->LinkEndChild( pTermElement );
// 	}

	if ( OK != GetChildElement( &pListElement, m_pXmlRootElem, "Id", "EMOTICON_LIST" ) )
	{
		pListElement = new TIXMLOLD::TiXmlElement("WORD");
		pListElement->SetAttribute("Ver", 1);
		pListElement->SetAttribute("Id", "EMOTICON_LIST" );
		m_pXmlRootElem->LinkEndChild( pListElement );
	}

	CString strLine, strSentence, strSplit[2] = {" ", "="};
	INT iLine = 0;
	DWORD dwTemp;
	while ( (dwTemp=StrFile.GetNextLine(strLine)) != 0 )
	{
		// BOM 처리;
		if ( iLine == 0 )
		{
			unsigned char szBOM[4];
			strncpy((char*)szBOM, strLine.GetString(), 4);
			// UTF-8 Bom이 있다면;
			if ( szBOM[0] == 0xEF && szBOM[1] == 0xBB )
			{
				char	szTEMP[255];
				strcpy(szTEMP, &strLine.GetString()[3]);
				//strcpy(szTEMP, strLine.GetString());
				wchar_t	wsTEMP[255];
				strLine.GetString();
				MultiByteToWideChar(CP_UTF8,0
					, szTEMP
					, static_cast<INT>(strLine.GetLength())+1
					, wsTEMP
					, sizeof(wsTEMP)/sizeof(wsTEMP[0]));			
				WideCharToMultiByte(CP_UTF8, 0
					, wsTEMP
					, -1, szTEMP
					, sizeof(wsTEMP)/sizeof(wchar_t)
					, NULL,NULL );

				strLine = szTEMP;
			}
		}

		iLine++;
		// 주석은 건너뛴다;
		if ( strncmp(strLine.GetString(),";",1) == 0 && strSentence.GetLength() == 0 )		
		{
			continue;
		}
		else if ( strncmp(strLine.GetString(),";",1) != 0)
		{
			strSentence.Append(strLine);
			strSentence.Append("\n");
			continue;
		}
		
		const INT iStart = strSentence.Find("[");
		const INT iEnd = strSentence.Find("]");
		char szElemName[255] = {0};
		strncpy_s( szElemName, &strSentence.GetString()[iStart+1], iEnd - (iStart+1) );
		
		std::vector<std::string> vecLineArray;
		std::string strTmp = const_cast<char*>(strSentence.GetString());
		boost::split(vecLineArray, strTmp, boost::is_any_of("\n"));
		INT iListIndex = 0;
		HRESULT isMotion;
		CString strElemName;
		for ( INT i = 0; i < (INT)vecLineArray.size(); ++i )
		{
			CString strString = vecLineArray[i].c_str();
			
			if ( (isMotion = strncmp(strString.GetString(), "Motion", strlen("Motion"))) != 0 && 
				 strncmp(strString.GetString(), "TermList", strlen("TermList")) != 0 )
			{
				if ( strncmp(strString.GetString(), "Id", strlen("Id") ) == 0 )
				{
					std::vector<std::string> vecMotionIDArray;
					std::string strMotionIDCheck = const_cast<char*>(strString.GetString());
					boost::split(vecMotionIDArray, strMotionIDCheck, boost::is_any_of("="));
					char szMotionID[4];
					iListIndex = atoi(vecMotionIDArray[1].c_str());
					// itoa(,szMotionID,10);
				}
				continue;
			}

			CString strComplet;			
			std::vector<std::string>	token;
			std::string res = const_cast<char*>(vecLineArray[i].c_str());
			boost::split ( token, res, boost::is_any_of(" ") );
			res.clear();
			for ( INT j = 0; j < (INT)token.size(); ++j )
				res.append(token[j].c_str());

			token.clear();
			boost::split ( token, res, boost::is_any_of("=") );
			if ( token.size() != 2 )
			{
				CString strErrorMsg;
				strErrorMsg.Format("%d Line Error", iLine);
				MessageBoxA(NULL, strErrorMsg.GetString(), strErrorMsg.GetString(), MB_OK);
				continue;
			}

			TIXMLOLD::TiXmlElement* pXmlElem, *pXmlEmem_Child;
			strElemName = szElemName;
			if ( isMotion == S_OK )
				strElemName.Append("_MOTION");
			else
				strElemName.Append("_TERM");

			if ( OK != GetChildElement( &pXmlElem, m_pXmlRootElem, "Id", strElemName.GetString()) )
			{
				pXmlElem = new TIXMLOLD::TiXmlElement("WORD");
				pXmlElem->SetAttribute("Ver", 1);
				pXmlElem->SetAttribute("Id", strElemName.GetString() );
				m_pXmlRootElem->LinkEndChild( pXmlElem );
			}

			std::vector<std::string>	vecDataToken;
			boost::split(vecDataToken, token[1], boost::is_any_of(",") );
			for ( INT j = 0; j < (INT)vecDataToken.size(); ++j )
			{
				char szIndex[4] = {0};
				itoa(j, szIndex, 10);
				QueryAtrb FindAtrb[2] =
				{
					QueryAtrb("Lang", const_cast<char*>(szNational)),
					QueryAtrb("Index", szIndex)
				};
				if ( OK != GetChildElement_MultiAtrb(
					&pXmlEmem_Child, pXmlElem, FindAtrb, 2 ) )
				{
					pXmlEmem_Child = new TIXMLOLD::TiXmlElement("VALUE");
					pXmlEmem_Child->SetAttribute( "Lang", szNational );
					pXmlEmem_Child->SetAttribute( "Index", FindAtrb[1].atrbData );
					pXmlElem->LinkEndChild( pXmlEmem_Child );
				}
				else
				{
					if ( pXmlEmem_Child->FirstChild() )
						pXmlEmem_Child->RemoveChild( pXmlEmem_Child->FirstChild() );
				}
				TIXMLOLD::TiXmlText* pXmlText = new TIXMLOLD::TiXmlText(vecDataToken[j].c_str());
				pXmlEmem_Child->LinkEndChild(pXmlText);
			}
			TIXMLOLD::TiXmlElement* pListChild;
			char szIndex[4] = {0};
			itoa(iListIndex, szIndex, 10);
			QueryAtrb FindAtrb[2] =
			{
				QueryAtrb("Lang", const_cast<char*>(szNational)),
				QueryAtrb("Index", szIndex)
			};
			if ( OK != GetChildElement_MultiAtrb(
				&pListChild, pListElement, FindAtrb, 2 ) )
			{
				pListChild = new TIXMLOLD::TiXmlElement("VALUE");
				pListChild->SetAttribute( "Lang", szNational );
				pListChild->SetAttribute( "Index", FindAtrb[1].atrbData );
				pListElement->LinkEndChild( pListChild );
			}
			else
			{
				if ( pListChild->FirstChild() )
					pListChild->RemoveChild( pListChild->FirstChild() );
			}
			
			TIXMLOLD::TiXmlText* pText = new TIXMLOLD::TiXmlText(szElemName);
			pListChild->LinkEndChild(pText);
// 			if ( isMotion == S_OK )
// 			{
// 				TIXMLOLD::TiXmlElement* pMotionChild;
// 				char szIndex[4] = {0};
// 				itoa(iListIndex, szIndex, 10);
// 				QueryAtrb FindAtrb[2] =
// 				{
// 					QueryAtrb("Lang", const_cast<char*>(szNational)),
// 					QueryAtrb("Index", szIndex)
// 				};
// 				if ( OK != GetChildElement_MultiAtrb(
// 					&pMotionChild, pMotionElement, FindAtrb, 2 ) )
// 				{
// 					pMotionChild = new TIXMLOLD::TiXmlElement("VALUE");
// 					pMotionChild->SetAttribute( "Lang", szNational );
// 					pMotionChild->SetAttribute( "Index", FindAtrb[1].atrbData );
// 					pMotionElement->LinkEndChild( pMotionChild );
// 				}
// 				else
// 				{
// 					if ( pMotionChild->FirstChild() )
// 						pMotionChild->RemoveChild( pMotionChild->FirstChild() );
// 				}
// 
// 				TIXMLOLD::TiXmlText* pText = new TIXMLOLD::TiXmlText(strElemName.GetString());
// 				pMotionChild->LinkEndChild(pText);
// 			}
// 			else
// 			{
// 				TIXMLOLD::TiXmlElement* pTermChild;
// 				char szIndex[4] = {0};
// 				itoa(iListIndex, szIndex, 10);
// 				QueryAtrb FindAtrb[2] =
// 				{
// 					QueryAtrb("Lang", const_cast<char*>(szNational)),
// 					QueryAtrb("Index", szIndex)
// 				};
// 				if ( OK != GetChildElement_MultiAtrb(
// 					&pTermChild, pTermElement, FindAtrb, 2 ) )
// 				{
// 					pTermChild = new TIXMLOLD::TiXmlElement("VALUE");
// 					pTermChild->SetAttribute( "Lang", szNational );
// 					pTermChild->SetAttribute( "Index", FindAtrb[1].atrbData );
// 					pTermElement->LinkEndChild( pTermChild );
// 				}
// 				else
// 				{
// 					if ( pTermChild->FirstChild() )
// 						pTermElement->RemoveChild( pTermChild );
// 				}
// 				
// 				TIXMLOLD::TiXmlText* pText = new TIXMLOLD::TiXmlText(strElemName.GetString());
// 				pTermChild->LinkEndChild(pText);
// 			}
		}

		
// 		if ( isMotion != S_OK )
// 		{
// 			TIXMLOLD::TiXmlElement* pTermChild;
// 			char szIndex[4] = {0};
// 			itoa(iListIndex, szIndex, 10);
// 			QueryAtrb FindAtrb[2] =
// 			{
// 				QueryAtrb("Lang", const_cast<char*>(szNational)),
// 				QueryAtrb("Index", szIndex)
// 			};
// 			if ( OK != GetChildElement_MultiAtrb(
// 				&pTermChild, pTermElement, FindAtrb, 2 ) )
// 			{
// 				pTermChild = new TIXMLOLD::TiXmlElement("VALUE");
// 				pTermChild->SetAttribute( "Lang", szNational );
// 				pTermChild->SetAttribute( "Index", FindAtrb[1].atrbData );
// 				pTermElement->LinkEndChild( pTermChild );
// 			}
// 			else
// 			{
// 				if ( pTermChild->FirstChild() )
// 					pTermElement->RemoveChild( pTermChild );
// 			}
// 
// 			TIXMLOLD::TiXmlText* pText = new TIXMLOLD::TiXmlText(strElemName.GetString());
// 			pTermChild->LinkEndChild(pText);
// 		}

		strSentence.Empty();
		iListIndex = 0;
	}

	m_XmlDocument.SaveFile( szSave );
	StrFile.Close();
	return true;
}