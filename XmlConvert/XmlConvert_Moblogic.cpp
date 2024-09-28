

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

#include "../EngineLib/G-Logic/GLOGIC.h"

#include "../SigmaCore/Xml/2.5.3/tinystr.h"
#include "XmlConverter.h"

bool XmlConverter::RanTextConvert_MobLogic(const char* szFile, const char* szSave, const char* szNational, bool bSave)
{
	//
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

	TIXMLOLD::TiXmlElement* pNameElement;
	
	CString strLine;
	INT iLine = 0, iIndex = 0;
	DWORD dwLineFeedResult;
	while ( (dwLineFeedResult = StrFile.GetNextLine(strLine)) != 0 )
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
// 				wchar_t	wsTEMP[255];
// 				strLine.GetString();
// 				MultiByteToWideChar(CP_UTF8,0
// 					, szTEMP
// 					, static_cast<INT>(strLine.GetLength())+1
// 					, wsTEMP
// 					, sizeof(wsTEMP)/sizeof(wsTEMP[0]));			
// 				WideCharToMultiByte(CP_UTF8, 0
// 					, wsTEMP
// 					, -1, szTEMP
// 					, sizeof(wsTEMP)/sizeof(wchar_t)
// 					, NULL,NULL );

				strLine = szTEMP;
			}
		}
		iLine++;
		if ( strLine.GetLength() == 0 )
			continue;

		if ( strncmp(strLine.GetString(), "[", 1) == 0 )
		{
			const INT iStart = strLine.Find("[", 0);
			const INT iEnd = strLine.Find("]", 0);
			char szName[255] = {0};
			strncpy(szName, &strLine.GetString()[iStart+1], iEnd - (iStart+1));

			if ( OK != GetChildElement(&pNameElement, m_pXmlRootElem, "Id", szName) )
			{
				pNameElement = new TIXMLOLD::TiXmlElement("WORD");
				pNameElement->SetAttribute("Ver", 1);
				pNameElement->SetAttribute("Id", szName );
				m_pXmlRootElem->LinkEndChild(pNameElement);
			}

			iIndex = 0;
		}
		else
		{
			char szIndex[10] = {0};
			itoa(iIndex, szIndex, 10);
			TIXMLOLD::TiXmlElement* pChildElem = NULL;

			QueryAtrb FindAtrb[2] = 
			{
				QueryAtrb("Lang", const_cast<char*>(szNational)),
				QueryAtrb("Index", szIndex),
			};
			if ( OK != GetChildElement_MultiAtrb(
				&pChildElem, pNameElement, FindAtrb, 2 ) )
			{
				pChildElem = new TIXMLOLD::TiXmlElement("VALUE");
				pChildElem->SetAttribute( "Lang", szNational );
				pChildElem->SetAttribute( "Index", FindAtrb[1].atrbData );
				pNameElement->LinkEndChild( pChildElem );
			}
			else
			{
				if ( pChildElem->FirstChild() )
					pChildElem->RemoveChild( pChildElem->FirstChild() );
			}

			TIXMLOLD::TiXmlText*	pText = new TIXMLOLD::TiXmlText(strLine.GetString());
			pChildElem->LinkEndChild(pText);
			iIndex++;
		}
	}
	m_XmlDocument.SaveFile( szSave );
	StrFile.Close();
	return true;
}