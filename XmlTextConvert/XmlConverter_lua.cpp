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

bool XmlConverter::RanTextConvert_Lua(const char* szFile, const char* szSave, const char* szNational, bool bSave)
{
	TIXMLOLD::TiXmlDocument doc;
	if ( doc.LoadFile( szSave, TiXmlEncoding::TIXML_ENCODING_UTF8 ) )
	{
		m_XmlDocument = doc;
		m_pXmlRootElem = m_XmlDocument.RootElement();
		doc.Clear();
	}
	std::stringstream	ss;
	std::ifstream		fin(szFile);
	ss << fin.rdbuf();

	std::string const &s = ss.str();	
	std::vector<std::string> vecSTRING;
	boost::split(vecSTRING, s, boost::is_any_of("\r\n"));

	CString strSplit[3] = {"\t", "\r\n", " "};

	INT iMaxDepth = 0, iDepth = 0;
	INT iTEST = 0;
	CString strSentence;
	for ( INT i = 0; i < (INT)vecSTRING.size(); ++i )
	{
		CString strLine = vecSTRING[i].c_str();
		// 주석은 생략한다;		
		if ( strncmp(strLine.GetString(), "--", 2) == 0)
			continue;

		strSentence.Append(strLine);
		INT iFind = 0;
		CString strBeginBracket = strLine, strEndBracket = strLine;
		while ( (iFind = strLine.Find("{", iFind)) != -1 )
		{
			iMaxDepth = (++iDepth);			//iFind += 1;
			if ( (iFind += 1) >= strLine.GetLength())
				break;
		}
		
		BOOL bMaybe = FALSE;
		iFind = 0;
		while ( (iFind = strLine.Find("}", iFind)) != -1 )
		{
			iDepth--;			//iFind += 1;
			if ( iFind == 0 )
				bMaybe = TRUE;
			if ( (iFind += 1) >= strLine.GetLength())
				break;
		}

		//
// 		WCHAR wDATA[1024] = {0};
// 		MultiByteToWideChar(CP_UTF8,0
// 			,strLine.GetString()
// 			,strLine.GetLength()+1
// 			,wDATA,sizeof(wDATA)/sizeof(wDATA[0]));			
// 	
// 		for ( INT z = 0; z < wcslen(wDATA); ++z )
// 		{
// 			if (wcsncmp(&wDATA[z], L"{", 1) == 0)
// 			{
// 				iMaxDepth = (++iDepth);
// 			}
// 			else if (wcsncmp(&wDATA[z], L"}", 1) == 0)
// 			{
// 				iMaxDepth--;
// 			}
// 		}
	
		//

		// 한문장으로 만듬;
		if (( iMaxDepth != 0 && iDepth == 0 ) || bMaybe == TRUE)
		{
			CString strCompletion;
			for ( INT j = 0; j < 3; ++j )
			{
				std::vector<std::string>	token;
				std::string res = const_cast<char*>(strSentence.GetString());
				boost::split ( token, res, boost::is_any_of(strSplit[j].GetString()) );
				strCompletion.Empty();
				for ( INT k = 0; k < (INT)token.size(); ++k )
				{
					strCompletion.Append( token[k].c_str() );
				}
			}

			{
				INT iEOFFind = 0;
				while ( (iEOFFind = strCompletion.Find(static_cast<char>(EOF), iEOFFind)) != -1 )
					strCompletion.Delete( iEOFFind, strlen("\r\n") );
			}
			CompositionSentence_Lua(strCompletion, m_pXmlRootElem, szNational);
			strSentence.Empty();	
			iMaxDepth = 0;
			iDepth = 0;
		}
	}
	m_XmlDocument.SaveFile(szSave);
	return true;
}

enum EMSEPERATOR_LUA
{
	OPERATOR_EQUAR = 0,
	BRAKET_BEGIN,
	BRAKET_END,
	MARK_START,
	MARK_END,
	COMMA,
};

struct SPERATOR_LUA
{
	EMSEPERATOR_LUA	EM_MARK;
	INT				N_STRINGAT;
};

BOOL XmlConverter::CompositionSentence_Lua(CString& strSentence, TiXmlElement* pParentElem, const char* szNation)
{
	CString			_strSeperator[6] = { "=", "{", "} ", "L\"", "\"", ","};
	INT				_iSeperatorLen[6];
	
	std::vector<SPERATOR_LUA> vecSperator;
	for ( INT i = 0; i < (INT)strSentence.GetLength(); ++i )
	{
		for ( INT j = 0; j < 6; ++j )
		{
			if (strncmp(&strSentence.GetString()[i], _strSeperator[j].GetString(), j == 3? 2:1) == 0)
			{
				// 따옴표 시작시 따옴표 끝날때까지 다른 기호들은 건너뛴다;
				const INT iSperSize = static_cast<INT>(vecSperator.size());
				if ( iSperSize != 0 && 
					 vecSperator[iSperSize-1].EM_MARK == MARK_START && 
					 static_cast<EMSEPERATOR_LUA>(j) != MARK_END )
					 break;

				SPERATOR_LUA sper;
				sper.EM_MARK = static_cast<EMSEPERATOR_LUA>(j);
				sper.N_STRINGAT = i;
				vecSperator.push_back( sper );

				if ( static_cast<EMSEPERATOR_LUA>(j) == MARK_START )
					++i;
				break;
			}
		}
	}

	TiXmlElement* pElem = NULL;
	{
		const INT iBraket = strSentence.Find( "{" );
		char szNAME[255];
		strncpy(szNAME, strSentence.GetString(), iBraket);
		if ( strncmp(&szNAME[iBraket-1], "=", 1) == 0 )
			pElem = new TiXmlElement("WORD");
		else
			pElem = pParentElem;
	}

	INT iDataCount = 0, iBracket = 0;
	const INT iterSize = vecSperator.size();
	for ( INT i = 0; i < iterSize; ++i )
	{
		const SPERATOR_LUA& sper = vecSperator[i];
		switch (sper.EM_MARK)
		{
		case OPERATOR_EQUAR:
			{
				char szName[512] = {0};
				strncpy(szName, strSentence.GetString(), sper.N_STRINGAT);
				// 공백 제거;
				std::vector<std::string> token;
				boost::split(token, szName, boost::is_any_of(" "));
				std::string strName;
				for ( INT j = 0; j < token.size(); ++j )
					strName.append(token[j].c_str());

				if (OK != GetChildElement(&pElem, pParentElem, "Id", szName))
				{
					pElem->SetAttribute( "Ver", 1 );
					pElem->SetAttribute( "Id", szName );
					m_XmlDocument.RootElement()->LinkEndChild( pElem );
				}
				printf ( "Current Element \"%s\"\n", szName );

				if ( strcmp (szName, "ELEMENT") == 0 )
				{
					int test;
					test = 0;
				}
			}
			break;
		case BRAKET_BEGIN:
			{
				if ( i+1 < iterSize && vecSperator[i+1].EM_MARK == COMMA )
				{
					char szDATA[512] = {0};
					strncpy(szDATA
						, &strSentence.GetString()[sper.N_STRINGAT+1]
						, vecSperator[i+1].N_STRINGAT-sper.N_STRINGAT-1);

					// 공백 제거;
					std::vector<std::string> token;
					boost::split(token, szDATA, boost::is_any_of(" "));
					CString strNAME;
					for ( INT j = 0; j < token.size(); ++j )
						strNAME.Append(token[j].c_str());

					char STRINDEX[5] = {0};
					itoa(iDataCount, STRINDEX, 10);
					QueryAtrb FindAtrb[2] =
					{
						QueryAtrb(const_cast<char*>("Lang"), const_cast<char*>(szNation)),
						QueryAtrb("Index", const_cast<char*>(STRINDEX))
					};

					TiXmlElement* pXmlElement_Child = NULL;
					if ( OK != GetChildElement_MultiAtrb( &pXmlElement_Child, pElem, FindAtrb, 2 ))
					{
						pXmlElement_Child = new TiXmlElement("VALUE");
						pXmlElement_Child->SetAttribute("Lang", szNation);
						pXmlElement_Child->SetAttribute("Index", iDataCount);
						pElem->LinkEndChild(pXmlElement_Child);
					}
					else
					{
						if ( pXmlElement_Child->FirstChild() )
							pXmlElement_Child->RemoveChild( pXmlElement_Child->FirstChild() );
					}
					iDataCount++;
					TiXmlText* pXmlText = new TiXmlText(strNAME.GetString());
					pXmlElement_Child->LinkEndChild(pXmlText);
					iBracket++;
				}
			}
			break;
		case BRAKET_END:			break;
		case MARK_START:
			{
				if ( i+1 >= iterSize )
					continue;

				if ( vecSperator[i+1].EM_MARK != MARK_END ){
					MessageBoxW( NULL, L"따옴표 시작 후 따옴표 끝이없습니다;", L"ERROR", MB_OK );
					break;
				}

				char szDATA[255] = {0};
				strncpy(szDATA
					, &strSentence.GetString()[sper.N_STRINGAT+2]
					, vecSperator[i+1].N_STRINGAT-sper.N_STRINGAT-2);

				if ( strlen(szDATA) <= 0 )
					break;

				char STRINDEX[5] = {0};
				itoa(iDataCount, STRINDEX, 10);
				QueryAtrb FindAtrb[2] =
				{
					QueryAtrb(const_cast<char*>("Lang"), const_cast<char*>(szNation)),
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
				iDataCount++;
				TiXmlText* pXmlText = new TiXmlText(szDATA);
				pXmlElement_Child->LinkEndChild(pXmlText);
			}
			break;
		case MARK_END:
			iBracket--;
			break;
		case COMMA:
			{
				if ( vecSperator[i+1].EM_MARK != COMMA && vecSperator[i+1].EM_MARK != BRAKET_END )
					break;

				char szDATA[255] = {0};
				strncpy(szDATA
					, &strSentence.GetString()[sper.N_STRINGAT+1]
					, vecSperator[i+1].N_STRINGAT-sper.N_STRINGAT-1);

				// 공백 제거;
				std::vector<std::string> token;
				boost::split(token, szDATA, boost::is_any_of(" "));
				CString strNAME;
				for ( INT j = 0; j < token.size(); ++j )
					strNAME.Append(token[j].c_str());

				if ( token.size() == 0 || strNAME.GetLength() <= 0)
					break;
				char STRINDEX[5] = {0};
				itoa(iDataCount, STRINDEX, 10);

				QueryAtrb FindAtrb[2] =
				{
					QueryAtrb(const_cast<char*>("Lang"), const_cast<char*>(szNation)),
					QueryAtrb("Index", const_cast<char*>(STRINDEX))
				};
				TiXmlElement* pXmlElement_Child = NULL;
				if ( OK != GetChildElement_MultiAtrb( &pXmlElement_Child, pElem, FindAtrb, 2 ))
				{
					pXmlElement_Child = new TiXmlElement("VALUE");
					pXmlElement_Child->SetAttribute("Lang", szNation);
					pXmlElement_Child->SetAttribute("Index", iDataCount);
					pElem->LinkEndChild(pXmlElement_Child);
				}
				else
				{
					if ( pXmlElement_Child->FirstChild() )
						pXmlElement_Child->RemoveChild( pXmlElement_Child->FirstChild() );
				}
				iDataCount++;
				TiXmlText* pXmlText = new TiXmlText(szDATA);
				pXmlElement_Child->LinkEndChild(pXmlText);
			}
			break;
		}
	}
	return iBracket == 0 ? TRUE : FALSE;
}