

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

#include "../SigmaCore/Xml/2.5.2/tinyxml.h"

#include "../SigmaCore/String/StringUtils.h"
#include "../SigmaCore/String/StringUtil.h"
#include "../SigmaCore/File/StringFile.h"
#include "../SigmaCore/File/SerialFile.h"
#include "../SigmaCore/Encrypt/Rijndael.h"
#include "../SigmaCore/String/basestring.h"
#include "../SigmaCore/File/TxtFile.h"
#include "../SigmaCore/Lua/MinLua.h"
#include "../EngineLib/DxTools/DxFontDefine.h"
#include "../EngineLib/DxTools/DxFontMan.h"

#include "../SigmaCore/Xml/2.5.3/tinystr.h"
#include "XmlConverter.h"
#include "../SigmaCore/File/Excel/MinExcel.h"

bool XmlConverter::RanTextConvert_Excel(const char* szFile, const char* szSave, const char* szNational, bool bSave)
{
	TIXMLOLD::TiXmlDocument doc;
	if ( doc.LoadFile( szFile, TiXmlEncoding::TIXML_ENCODING_UTF8 ) )
	{
		m_XmlDocument = doc;
		m_pXmlRootElem = m_XmlDocument.RootElement();
		doc.Clear();
	}

	TIXMLOLD::TiXmlElement* pElem = m_pXmlRootElem->FirstChildElement();
	static int nCount = 0;

	std::string strSaveFile;
	static int sheet = 0, Row = 0; int Col = 0;
	sc::Excel ExcelFile(szSave, strSaveFile, sheet, false);
	if ( nCount == 0 )
	{
		ExcelFile.SetCell(sheet, Row, Col++, L"NUM"		);
		ExcelFile.SetCell(sheet, Row, Col++, L"ID"		);
		ExcelFile.SetCell(sheet, Row, Col++, L"TYPE"	);
		ExcelFile.SetCell(sheet, Row, Col++, L"VERSION"	);
		ExcelFile.SetCell(sheet, Row, Col++, L"INDEX"	);
		ExcelFile.SetCell(sheet, Row, Col++, L"STATE"	);
		ExcelFile.SetCell(sheet, Row, Col++, L"DATANUM"	);
		ExcelFile.SetCell(sheet, Row, Col++, L"COUNTRY"	);
		ExcelFile.SetCell(sheet, Row, Col++, L"C_NAME"	);
		ExcelFile.SetCell(sheet, Row, Col++, L"DATA"	);
	}
	else
	{
		bool bResult = ExcelFile.Load(strSaveFile);
		if ( bResult == false )
		{
			MessageBox(NULL, szSave, "LoadFail", MB_OK);
		}
	}

	for ( pElem = m_pXmlRootElem->FirstChildElement(); pElem; pElem = pElem->NextSiblingElement())
	{
		/*
		TIXMLOLD::TiXmlElement* pValueElem;
		if ( !(pValueElem = pElem->FirstChildElement("VALUE")) )
			continue;
		
		char szValue[1024] = {0};
		_tcscpy( szValue, pValueElem->FirstChild()->Value());
		if ( szValue[0] == _T('\0') )
			continue;

		char szID[1024] = {0};
		_tcscpy( szID, pElem->Attribute("Id") );

//		wchar_t wsValue[1024], wsID[1024];
//		const int nResultValue = MultiByteToWideChar(CP_UTF8,0,szValue,strlen(szValue)+1,wsValue,sizeof(wsValue)/sizeof(wsValue[0]));
//		const int nResultID = MultiByteToWideChar(CP_UTF8,0,szID,strlen(szID)+1,wsID,sizeof(wsID)/sizeof(wsID[0]));
		
//		std::wstring wstrTYPE = L"SETENSE";
// 		std::wstring wstrID = wsID;
// 		std::wstring wstrDATA = wsValue;

		Row++;
		Col = 0;
 		ExcelFile.SetCell(sheet, Row, Col++, nCount);
		ExcelFile.SetCell(sheet, Row, Col++, );
 		ExcelFile.SetCell(sheet, Row, Col++, );
 		ExcelFile.SetCell(sheet, Row, Col++, 1);
 		ExcelFile.SetCell(sheet, Row, Col++, 0);
 		ExcelFile.SetCell(sheet, Row, Col++, 0);
 		ExcelFile.SetCell(sheet, Row, Col++, 0);
 		ExcelFile.SetCell(sheet, Row, Col++, 1);
 		ExcelFile.SetCell(sheet, Row, Col++, 0);
 		ExcelFile.SetCell(sheet, Row, Col++, );
		nCount++;
		*/
		const int nVersion		= atoi(pElem->Attribute("Ver"));
		const std::string strID	= pElem->Attribute("Id");
		const std::string strType = pElem->Value();

		TIXMLOLD::TiXmlElement* pValueElem = pElem->FirstChildElement("VALUE");
		
		while ( pValueElem )
		{
			int nIndex = 0;
			if ( strcmp(strType.c_str(), "WORD") == 0 )
				nIndex = atoi(pValueElem->Attribute("Index"));

			const std::string strLanguage = pValueElem->Attribute("Lang");
			language::EMPROVIDE_LANGFLAG emProvideLangflag = language::StringToProvidelang(const_cast<char*>(strLanguage.c_str()));
			const int nCountryNum = static_cast<int>(language::ProvidelangToTexttoolcountrynum(emProvideLangflag));
			
			std::string strData;
			if ( pValueElem->FirstChild() )
				strData = pValueElem->FirstChild()->Value();
			else
				strData = "skip";

			// wstring으로 컨버팅;
			char    szID[4095] = {0}, szType[4095] = {0}, szData[4095] = {0};
			wchar_t wsID[4095] = {0}, wsType[4095] = {0}, wsData[4095] = {0};
			MultiByteToWideChar(CP_UTF8//CD3DFontPar::nCodePage[(int)language::NationToLanguage(emNation)]
				, 0
				, strID.c_str()
				, strlen(strID.c_str())+1
				, wsID
				, sizeof(wsID)/sizeof(wsID[0]));
//			WideCharToMultiByte(CP_UTF8, 0, wsID, -1, szID, sizeof(szID)/sizeof(szID[0]), NULL,NULL );

			MultiByteToWideChar(CP_UTF8//CD3DFontPar::nCodePage[(int)language::NationToLanguage(emNation)]
				, 0
				, strType.c_str()
				, strlen(strType.c_str())+1
				, wsType
				, sizeof(wsType)/sizeof(wsType[0]));
//			WideCharToMultiByte(CP_UTF8, 0, wsType, -1, szType, sizeof(szType)/sizeof(szType[0]), NULL,NULL );

			MultiByteToWideChar(CP_UTF8//CD3DFontPar::nCodePage[(int)language::NationToLanguage(emNation)]
				, 0
				, strData.c_str()
				, strlen(strData.c_str())+1
				, wsData
				, sizeof(wsData)/sizeof(wsData[0]));
//			WideCharToMultiByte(CP_UTF8, 0, wsData, -1, szData, sizeof(szData)/sizeof(szData[0]), NULL,NULL );
			
			// Locale Info Set;
			const char* const pChangerLocale = setlocale( LC_ALL, CD3DFontPar::szLocaleName[(int)language::ProvidelangToLangflag(emProvideLangflag)] );

			std::string _strID(szID), _strType(szType), _strData(szData);
			std::wstring wstrID(wsID), wstrType(wsType), wstrData(wsData);
			Row++;
			Col = 0;
			ExcelFile.SetCell(sheet, Row, Col++, nCount);		// NUL
			ExcelFile.SetCell(sheet, Row, Col++, wstrID);		// ID		//
			ExcelFile.SetCell(sheet, Row, Col++, wstrType);		// TYPE		//
			ExcelFile.SetCell(sheet, Row, Col++, nVersion);		// VERSION
			ExcelFile.SetCell(sheet, Row, Col++, nIndex);		// INDEX
			ExcelFile.SetCell(sheet, Row, Col++, 0);			// STATE
			ExcelFile.SetCell(sheet, Row, Col++, 0);			// DATANUM
			ExcelFile.SetCell(sheet, Row, Col++, nCountryNum);	// COUNTRY
			ExcelFile.SetCell(sheet, Row, Col++, 0);			// C_NAME
			ExcelFile.SetCell(sheet, Row, Col++, wstrData);		// DATA		//
			nCount++;

			pValueElem = pValueElem->NextSiblingElement();
		}
	}
	bool bSaveExcel = ExcelFile.Save();

//	SFile.close();
	return true;
}

bool XmlConverter::RanTextConvert_ExcelToXml(const char* szFile, const char* szSave, const char* szSheet, bool bSave)
{
	// Excel Set
	int row = 0, col = 0, max_row = 0, sheet = atoi(szSheet);
	sc::Excel ExcelFile;
	if (ExcelFile.Load(szFile) == false )
	{
		std::string Msg(
			sc::string::format(
			"%1% Excel 파일 읽기에 실패하였습니다. 파일이 열려 있는지 확인하세요.",
			szFile));
		AfxMessageBox(Msg.c_str(), MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	if ( ExcelFile.WorkSheetTotalCols(sheet) != 10 )
	{
		std::string Msg(
			sc::string::format(
			"%1% Excel 파일 읽기에 실패하였습니다. 엑셀 열을 확인하세요.",
			szFile));
		AfxMessageBox(Msg.c_str(), MB_OK|MB_ICONEXCLAMATION);
		return false;
	}

	// Xml Set
	TIXMLOLD::TiXmlDocument doc;
	if ( doc.LoadFile( szSave, TiXmlEncoding::TIXML_ENCODING_UTF8 ) )
	{
		m_XmlDocument = doc;
		m_pXmlRootElem = m_XmlDocument.RootElement();
		doc.Clear();
	}

	// Convert;
	max_row = ExcelFile.WorkSheetTotalRow(sheet);
	for ( row = 1; row < max_row; ++row )	// 0 is Header cell;
	{
		bool bErr = false;
		std::string strId, strType, strIndex, strCountry, strValue;
		int iCountry, iIndex, itest;
		bErr |= !ExcelFile.GetValueStrAndInteger(sheet, row, 1, strId);		// Id;		
		bErr |= !ExcelFile.GetValue(sheet, row, 2, strType);			// type;		
		bErr |= !ExcelFile.GetValue(sheet, row, 4, iIndex);				// type;		
		bErr |= !ExcelFile.GetValue(sheet, row, 7, iCountry);			// type;		
		bErr |= !ExcelFile.GetValue(sheet, row, 9, strValue);			// Data;

		if ( bErr )
		{
			std::string Msg(
				sc::string::format(
				"%1% 줄을 읽는 도중 빈값이 존재하여 저장하고 종료합니다.",
				row));
			AfxMessageBox(Msg.c_str(), MB_OK|MB_ICONEXCLAMATION);
			break;
		}
// 		int iCountry = atoi(strCountry.c_str());
// 		int iIndex = atoi(strIndex.c_str());
		char szCountry[256], szIndex[4];
		strCountry = itoa(iCountry, szCountry, 10);
		strIndex = itoa(iIndex, szIndex, 10);


		char szTemp[255];
		strcpy(szTemp, /*strlen((char*)strValue.c_str()),*/ (char*)strValue.c_str());
		WCHAR wszTemp[255];
//		MultiByteToWideChar(CP_UTF8, 0, szTemp, strlen(szTemp)+1,wszTemp,sizeof(wszTemp)/sizeof(wszTemp[0]));			
//		WideCharToMultiByte(CP_UTF8, 0, wszTemp, -1, szTemp, sizeof(szTemp)/sizeof(szTemp[0]), NULL,NULL );

		language::EMPROVIDE_LANGFLAG emProvideLangflag = language::StringToProvidelang(const_cast<char*>(strCountry.c_str()));
		const int nLang = (int)language::ProvidelangToLangflag(emProvideLangflag);
		const int nCodePage = CD3DFontPar::nCodePage[nLang];
		MultiByteToWideChar(nCodePage, 0, szTemp, strlen(szTemp)+1,wszTemp,sizeof(wszTemp)/sizeof(wszTemp[0]));			
		WideCharToMultiByte(CP_UTF8, 0, wszTemp, -1, szTemp, sizeof(szTemp)/sizeof(szTemp[0]), NULL,NULL );
		strValue = szTemp;
		
		strCountry = language::CountryNumToNationStr((language::TEXTTOOL_COUNTRYNUM)iCountry);

		TIXMLOLD::TiXmlElement* pElem, *pChild;
		if ( strcmp(strType.c_str(), "WORD") == 0 )
		{
			QueryAtrb FindAtrb[2] =
			{
				QueryAtrb("Ver", const_cast<char*>("1")),
				QueryAtrb("Id", const_cast<char*>(strId.c_str()))
			};
			if ( OK != GetChildElement_MultiAtrb(
				&pElem, m_pXmlRootElem, FindAtrb, 2 ) )
			{
				pElem = new TIXMLOLD::TiXmlElement("WORD");
				pElem->SetAttribute( "Ver", "1" );
				pElem->SetAttribute( "Id", const_cast<char*>(strId.c_str()) );
				m_pXmlRootElem->LinkEndChild( pElem );
			}
			FindAtrb[0] = QueryAtrb("Lang", const_cast<char*>(strCountry.c_str()));
			FindAtrb[1] = QueryAtrb("Index", const_cast<char*>(strIndex.c_str()));
			
			if ( OK != GetChildElement_MultiAtrb(
				&pChild, pElem, FindAtrb, 2 ) )
			{
				pChild = new TIXMLOLD::TiXmlElement("VALUE");
				pChild->SetAttribute( "Lang", const_cast<char*>(strCountry.c_str()) );
				pChild->SetAttribute( "Index", const_cast<char*>(strIndex.c_str()) );
				pElem->LinkEndChild( pChild );
			}
			else
			{
				if ( pChild->FirstChild() )
					pChild->RemoveChild( pChild->FirstChild() );
			}
			TIXMLOLD::TiXmlText* pXmlText = new TIXMLOLD::TiXmlText(strValue.c_str());
			pChild->LinkEndChild(pXmlText);
		}
		else //sentense;
		{
			if ( OK != GetChildElement( &pElem, m_pXmlRootElem, "Id", strId.c_str() ) )
			{
				pElem = new TIXMLOLD::TiXmlElement("SENTENSE");
				pElem->SetAttribute("Ver", 1);
				pElem->SetAttribute("Id", strId.c_str() );
				m_pXmlRootElem->LinkEndChild( pElem );
			}

			if ( OK != GetChildElement( &pChild, pElem, "Lang", strCountry.c_str() ) )
			{
				pChild = new TIXMLOLD::TiXmlElement("VALUE");
				pChild->SetAttribute("Lang", strCountry.c_str() );
				pElem->LinkEndChild( pChild );
			}
			else
			{
				if ( pChild->FirstChild() )
					pChild->RemoveChild( pChild->FirstChild() );
			}
			TIXMLOLD::TiXmlText* pXmlText = new TIXMLOLD::TiXmlText(strValue.c_str());
			pChild->LinkEndChild(pXmlText);
		}
	}
	m_XmlDocument.SaveFile( szSave );
	return true;
}