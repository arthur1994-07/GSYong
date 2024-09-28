#include "stdafx.h"

#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "../SigmaCore/File/DepthWithTextToFile.h"
#include "../SigmaCore/Log/LogMan.h"
#include "../SigmaCore/Loading/NSLoadingDirect.h"
#include "../SigmaCore/String/StringFormat.h"
#include "../SigmaCore/Xml/2.5.2/tinyxml.h"
#include "../SigmaCore/DebugInclude.h"

#include "../EngineLib/DxTools/DxFontMan.h"
#include "../EngineLib/DxTools/RENDERPARAM.h"

#include "UICompTexListXML.h"

namespace HTML5UI
{

namespace UICompTexList
{
	bool IsValidData( const void* szValue, const TCHAR* szMsg, int nCount, std::string& strID )
	{
		if( szValue == NULL )
		{
			std::string ErrorMsg(
				sc::string::format(
				"[UICompTexList/%1%] Count:%2%, ID:%3% Data is not valid",
				szMsg, nCount, strID ) );
			sc::writeLogError( ErrorMsg );

			return false;
		}

		return true;
	}

	bool IsValidData( const TCHAR* szValue, const TCHAR* szMsg, int nCount, std::string& strID )
	{
		if( szValue[ 0 ] == _T( '\0' ) )
		{
			std::string ErrorMsg(
				sc::string::format(
				"[UICompTexList/%1%] Count:%2%, ID:%3% Data is not valid",
				szMsg, nCount, strID ) );
			sc::writeLogError( ErrorMsg );

			return false;
		}

		return true;
	}

	bool Parse( TIXMLOLD::TiXmlDocument& XmlDoc, SCOMPONENT_VECTOR& componentVector )
	{
		LONG nCount = 0;
		TCHAR szValue[ 1024 ] = { 0 };
		WCHAR szValueW[ 1024 ] = { 0 };

		TIXMLOLD::TiXmlNode* pRootNode = NULL;
		TIXMLOLD::TiXmlElement* pRootElement = NULL;
		TIXMLOLD::TiXmlElement* pCurrentElement = NULL;

		try
		{
			pRootNode = XmlDoc.FirstChild( "TEXTURE_LIST" );
			if( !pRootNode )
				return false;

			pRootElement = pRootNode->FirstChildElement( "COMPONENT" );
			while( pRootElement )
			{
				//---------------------------------------------------//
				NSLoadingDirect::FrameMoveRender( FALSE );
				//---------------------------------------------------//

				std::string strID;
				SCOMPONENT sComponent;

				if( !IsValidData( pRootElement, "COMPONENT", nCount, std::string( "" ) ) )
					goto GotoNextElement;

				_tcscpy( szValue, pRootElement->Attribute( "Id" ) );
				if( !IsValidData( szValue, "COMPONENT/Id", nCount, std::string( "" ) ) )
					goto GotoNextElement;
				strID = szValue;

				for( int i=0; i<componentVector.size(); ++i )
				{
					if( componentVector.at( i ).m_textureID == strID )
					{
						std::string ErrorMsg(
							sc::string::format(
							"[UICompTexList] Count:%1%, ID:%2% Duplicate ID",
							nCount, strID ) );
						sc::writeLogError( ErrorMsg );

						goto GotoNextElement;
					}
				}

				sComponent.m_textureID = strID;

				pCurrentElement = pRootElement->FirstChildElement( "TEXTURE_POS" );
				if( pCurrentElement )
				{
					_tcscpy( szValue, pCurrentElement->Attribute( "X" ) );
					if( !IsValidData( szValue, "COMPONENT/TEXTURE_POS/X", nCount, strID ) )
						goto GotoNextElement;
					sComponent.m_vPos.x = (float)_tstof( szValue );

					_tcscpy( szValue, pCurrentElement->Attribute( "Y" ) );
					if( !IsValidData( szValue, "COMPONENT/TEXTURE_POS/Y", nCount, strID ) )
						goto GotoNextElement;
					sComponent.m_vPos.y = (float)_tstof( szValue );

					_tcscpy( szValue, pCurrentElement->Attribute( "W" ) );
					if( !IsValidData( szValue, "COMPONENT/TEXTURE_POS/W", nCount, strID ) )
						goto GotoNextElement;
					sComponent.m_vPos.z = (float)_tstof( szValue );

					_tcscpy( szValue, pCurrentElement->Attribute( "H" ) );
					if( !IsValidData( szValue, "COMPONENT/TEXTURE_POS/H", nCount, strID ) )
						goto GotoNextElement;
					sComponent.m_vPos.w = (float)_tstof( szValue );
				}

				pCurrentElement = pRootElement->FirstChildElement( "TEXTURE" );
				if( pCurrentElement )
				{
					_tcscpy( szValue, pCurrentElement->Attribute( "SizeX" ) );
					if( !IsValidData( szValue, "COMPONENT/TEXTURE/SizeX", nCount, strID ) )
						goto GotoNextElement;
					sComponent.m_vSize.x = (float)_tstof( szValue );

					_tcscpy( szValue, pCurrentElement->Attribute( "SizeY" ) );
					if( !IsValidData( szValue, "COMPONENT/TEXTURE/SizeY", nCount, strID ) )
						goto GotoNextElement;
					sComponent.m_vSize.y = (float)_tstof( szValue );

					_tcscpy( szValue, pCurrentElement->FirstChild()->Value() );
					if( !IsValidData( szValue, "COMPONENT/TEXTURE", nCount, strID ) )
						goto GotoNextElement;

					MultiByteToWideChar(
						CP_UTF8,
						0,
						szValue,
						(int)pCurrentElement->FirstChild()->ValueStr().size() + 1,
						szValueW,
						sizeof( szValueW ) / sizeof( szValueW[ 0 ] ) );

					WideCharToMultiByte(
						CD3DFontPar::nCodePage[ RENDERPARAM::emLangSet ],
						0,
						szValueW,
						-1,
						szValue,
						sizeof( szValue ) / sizeof( szValue[ 0 ] ),
						NULL,
						NULL );	

					sComponent.m_strFileName = szValue;
				}

				pCurrentElement = pRootElement->FirstChildElement( "BORDER" );
				if( pCurrentElement )
				{
					_tcscpy( szValue, pCurrentElement->Attribute( "X" ) );
					if( !IsValidData( szValue, "COMPONENT/BORDER/X", nCount, strID ) )
						goto GotoNextElement;
					sComponent.m_vBorder.x = (float)_tstof( szValue );

					_tcscpy( szValue, pCurrentElement->Attribute( "Y" ) );
					if( !IsValidData( szValue, "COMPONENT/BORDER/Y", nCount, strID ) )
						goto GotoNextElement;
					sComponent.m_vBorder.y = (float)_tstof( szValue );

					_tcscpy( szValue, pCurrentElement->Attribute( "W" ) );
					if( !IsValidData( szValue, "COMPONENT/BORDER/W", nCount, strID ) )
						goto GotoNextElement;
					sComponent.m_vBorder.z = (float)_tstof( szValue );

					_tcscpy( szValue, pCurrentElement->Attribute( "H" ) );
					if( !IsValidData( szValue, "COMPONENT/BORDER/H", nCount, strID ) )
						goto GotoNextElement;
					sComponent.m_vBorder.w = (float)_tstof( szValue );
				}

				sComponent.m_rcTexturePos = GUIBase::UIRECT(
					( sComponent.m_vPos.x + 0.25f ) / sComponent.m_vSize.x,
					( sComponent.m_vPos.y + 0.25f ) / sComponent.m_vSize.y,
					sComponent.m_vPos.z / sComponent.m_vSize.x,
					sComponent.m_vPos.w / sComponent.m_vSize.y );

				componentVector.push_back( sComponent );

GotoNextElement:
				pRootElement = pRootElement->NextSiblingElement( "COMPONENT" );
				++nCount;
			}
		}
		catch( ... )
		{
			return false;
		}

		return true;
	}

	bool CXmlLoader::Load( const std::string& strFilePath, SCOMPONENT_VECTOR& componentVector )
	{
		TIXMLOLD::TiXmlDocument XmlDoc;

		if( !XmlDoc.LoadFile( strFilePath.c_str() ) )
			return false;

		bool bReturn = Parse( XmlDoc, componentVector );

		XmlDoc.Clear();

		return bReturn;
	}

}

}