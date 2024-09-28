#include <afxwin.h>
#include <afxext.h>
#include <afxole.h>
#include <atlbase.h>
#include <atlwin.h>
#include <afxcoll.h>

#include <windows.h>
#include <math.h>
#include <time.h>
#include <strsafe.h>

#include <stdio.h>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <boost/lexical_cast.hpp>

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
#include "../SigmaCore/String/StringFormat.h"

#include "../EngineLib/G-Logic/GLogic.h"
#include "../InternalCommonLib/dxstdafx.h"

#include "../SigmaCore/Xml/2.5.3/tinystr.h"
#include "XmlConverter.h"

#include "../enginelib/DxTools/DxFontMan.h"
#include "../RanLogic/Pet/GLPet.h"
#include "../RanLogic/Pet/GLPetDefine.h"
#include "../RanLogic/GLogicDataMan.h"


int g_nCodePage_PetCom;

void ClearString(std::vector<std::string>& vecString )
{
	vecString.clear();
}

void XmlConverter::ClearPetLuaDefaultString()
{
	ClearString(COMMENT::PET_TYPE);
	ClearString(COMMENT::szPetInitFile);
	ClearString(COMMENT::szPetStyleCardText);
	COMMENT::PetStyleCardUI.clear();
}

bool XmlConverter::_Lua_3_Composition(const char* atrbData
						, std::vector<std::string>& vecString
						, const char* szNation )
{
	TiXmlElement* pElem;
	if (OK != GetChildElement(&pElem, m_pXmlRootElem, "Id", atrbData))
	{
		pElem = new TIXMLOLD::TiXmlElement("WORD");
		pElem->SetAttribute( "Ver", 1 );
		pElem->SetAttribute( "Id", atrbData );
		m_XmlDocument.RootElement()->LinkEndChild( pElem );
	}

	for ( int i=0; i<vecString.size(); ++i )
	{
		TiXmlElement* pChildElem = new TiXmlElement("VALUE");
		pChildElem->SetAttribute( "Lang", szNation );
		pChildElem->SetAttribute( "Index", i );

		char szTemp[512];
		WCHAR wszTemp[512];
		strcpy(szTemp, vecString[i].c_str());

		TiXmlText* pText = new TiXmlText( strlen(szTemp) == 0 ? "--TEMP--":szTemp );//strArray[i].c_str());
		pChildElem->LinkEndChild(pText);
		pElem->LinkEndChild(pChildElem);
	}

	return true;
}

bool XmlConverter::RanTextConvertToXml_PetCommentLua_OnlyText(const char* szFile, const char* szSave, const char* szNational, bool bSave)
{
	TIXMLOLD::TiXmlDocument doc;
	if ( doc.LoadFile( szSave, TiXmlEncoding::TIXML_ENCODING_UTF8 ) )
	{
		m_XmlDocument = doc;
		m_pXmlRootElem = m_XmlDocument.RootElement();
		doc.Clear();
	}
	ClearPetLuaDefaultString();
	if ( LOADFILE_LUA_PET(szFile) == false )
		return false;

	const language::SERVICENATION sNation = language::StrNationToEMNation(szNational);
	const int nLang = (int)language::NationToLanguage(sNation);
	g_nCodePage_PetCom = CD3DFontPar::nCodePage[nLang];

	_Lua_3_Composition("PET_TYPE",					COMMENT::PET_TYPE,			szNational);
	_Lua_3_Composition("PET_INITFILE",				COMMENT::szPetInitFile,		szNational);
	_Lua_3_Composition("PET_STYLECARDTEXT",			COMMENT::szPetStyleCardText,szNational);

	std::vector<std::string> vecString;
	for ( int i = 0; i < COMMENT::PetStyleCardUI.size(); ++i )
	{
		vecString.push_back(COMMENT::PetStyleCardUI[i].strTexture);
	}
	_Lua_3_Composition("PET_STYLECARDUI",			vecString,	szNational);

	m_XmlDocument.SaveFile(szSave);
	return true;
}

bool XmlConverter::LOADFILE_LUA_PET( const std::string& strPath )
{
	if( strPath.empty() )
		return false;

	char szName[ _MAX_FNAME ] = _T( "" );
	char szExt[ _MAX_EXT ] = _T( "" );
	_splitpath( strPath.c_str(), NULL, NULL, szName, szExt );
	StringCchCat( szName, _MAX_FNAME, szExt );

	std::auto_ptr< sc::BaseStream > pStream(
		GLOGIC::openfile_basestream(
		GLOGIC::UseLogicZipFile(),
		GLogicData::GetInstance().GetGlogicZipFile(),
		strPath.c_str(), 
		szName, 
		false,
		GLOGIC::UseLogicPackFile() ) );

	if( !pStream.get() )
	{
		sc::writeLogError(
			sc::string::format(
			"CGLPetCommentFile::LOADFILE_LUA(), File Open %1%", strPath ) );

		return false;
	}

	size_t nSize = static_cast< size_t >( pStream->GetFileSize() );
	void* pBuffer = malloc( nSize );
	pStream->ReadBuffer( pBuffer, 0 );
	CTypeSmartPtr cPtrData( pBuffer );

	try
	{
		sc::lua_init();

		if( !sc::lua_doBuffer( static_cast< char* >( pBuffer ), nSize, NULL ) )
		{
			MessageBox( NULL, sc::lua_getError().c_str(), strPath.c_str(), MB_OK );

			return false;
		}

		return ParsePetCommentLua();
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), strPath.c_str(), MB_OK );

		return false;
	}

	return false;
}

bool XmlConverter::ParsePetCommentLua()
{
	std::wstring ws;
	try
	{
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "PET_DEFINE" ) );
		for( LuaPlus::LuaTableIterator ListIter( ob ); ListIter.IsValid(); ListIter.Next() )
		{
			for( LuaPlus::LuaTableIterator ValIter( ListIter.GetValue() ); ValIter.IsValid(); ValIter.Next() )
			{
				int IntType = ValIter.GetValue().GetInteger();
				COMMENT::PET_DEFINE.push_back( IntType );
				ValIter.Next();

				ws = reinterpret_cast< const wchar_t* >( ValIter.GetValue().GetWString() );
				COMMENT::PET_TYPE.push_back( unicodeToUtf8( ws ) );
				ValIter.Next();

				ws = reinterpret_cast< const wchar_t* >( ValIter.GetValue().GetWString() );
				COMMENT::szPetInitFile.push_back( unicodeToUtf8( ws ) );
			}
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "PET_DEFINE" ), MB_OK );
	}

	//////////////////////////////////////////////////////////////////////////

	try
	{
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "PET_STYLECARD_TEXT" ) );
		for( LuaPlus::LuaTableIterator ListIter( ob ); ListIter.IsValid(); ListIter.Next() )
		{
			ws = reinterpret_cast< const wchar_t* >( ListIter.GetValue().GetWString() );
			COMMENT::szPetStyleCardText.push_back( unicodeToUtf8( ws ) );
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "PET_STYLECARD_TEXT" ), MB_OK );
	}

	//////////////////////////////////////////////////////////////////////////

	try
	{
		LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "PET_STYLECARD_UI" ) );
		for( LuaPlus::LuaTableIterator ListIter( ob ); ListIter.IsValid(); ListIter.Next() )
		{
			for( LuaPlus::LuaTableIterator ValIter( ListIter.GetValue() ); ValIter.IsValid(); ValIter.Next() )
			{
				WORD wPetType, wPetStyle;

				WORD wWindowPosX, wWindowPosY, wWindowWidth, wWindowHeight;

				std::string strTexture;
				WORD wTextureSizeX, wTextureSizeY;
				WORD wTexturePosX, wTexturePosY, wTextureWidth, wTextureHeight;

				wPetType = static_cast<WORD>(ValIter.GetValue().GetInteger());
				ValIter.Next();

				wPetStyle = static_cast<WORD>(ValIter.GetValue().GetInteger());
				ValIter.Next();

				wWindowPosX = static_cast<WORD>(ValIter.GetValue().GetInteger());
				ValIter.Next();

				wWindowPosY = static_cast<WORD>(ValIter.GetValue().GetInteger());
				ValIter.Next();

				wWindowWidth = static_cast<WORD>(ValIter.GetValue().GetInteger());
				ValIter.Next();

				wWindowHeight = static_cast<WORD>(ValIter.GetValue().GetInteger());
				ValIter.Next();

				ws = reinterpret_cast< const wchar_t* >( ValIter.GetValue().GetWString() );
				strTexture = unicodeToUtf8( ws );
				ValIter.Next();

				wTextureSizeX = static_cast<WORD>(ValIter.GetValue().GetInteger());
				ValIter.Next();

				wTextureSizeY = static_cast<WORD>(ValIter.GetValue().GetInteger());
				ValIter.Next();

				wTexturePosX = static_cast<WORD>(ValIter.GetValue().GetInteger());
				ValIter.Next();

				wTexturePosY = static_cast<WORD>(ValIter.GetValue().GetInteger());
				ValIter.Next();

				wTextureWidth = static_cast<WORD>(ValIter.GetValue().GetInteger());
				ValIter.Next();

				wTextureHeight = static_cast<WORD>(ValIter.GetValue().GetInteger());

				PET_STYLECARD_UI sPetCardUI( wPetType, wPetStyle,
					wWindowPosX, wWindowPosY, wWindowWidth, wWindowHeight,
					strTexture, wTextureSizeX, wTextureSizeY,
					wTexturePosX, wTexturePosY, wTextureWidth, wTextureHeight );

				COMMENT::PetStyleCardUI.push_back( sPetCardUI );
			}
		}
	}
	catch( const LuaPlus::LuaException& e )
	{        
		MessageBox( NULL, e.GetErrorMessage(), _T( "PET_STYLECARD_UI" ), MB_OK );
	}

	return true;
}