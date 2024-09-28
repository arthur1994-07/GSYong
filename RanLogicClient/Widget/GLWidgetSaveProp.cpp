#include "pch.h"
#include "GLWidgetSaveProp.h"

#include "../../SigmaCore/Xml/2.5.2/tinyxml.h"
#include "../../enginelib/Common/SUBPATH.h"

#include "../Stage/DxGlobalStage.h"
#include "../GLGaeaClient.h"
#include "../../RanLogic/RANPARAM.h"

using namespace TIXMLOLD;

GLWidgetProp::GLWidgetProp()
{
}

GLWidgetProp::~GLWidgetProp()
{
	m_mapProp.clear();
}

void GLWidgetProp::InsertValue( SVALUE::EMVALUETYPE emType, const std::string& strProp )
{
	SVALUE sValue;
	sValue.emType = emType;

	m_mapProp.insert( std::make_pair( strProp, sValue ) );
}

void GLWidgetProp::SetInt( const std::string& strProp, int nValue )
{
	MAP_PROP_ITER iter = m_mapProp.find( strProp );
	if( iter == m_mapProp.end() )
		return;

	SVALUE& sValue = iter->second;
	sValue.nValue = nValue;
	sValue.bIsValid = true;
}

void GLWidgetProp::SetDWORD( const std::string& strProp, DWORD dwValue )
{
	MAP_PROP_ITER iter = m_mapProp.find( strProp );
	if( iter == m_mapProp.end() )
		return;

	SVALUE& sValue = iter->second;
	sValue.dwValue = dwValue;
	sValue.bIsValid = true;
}

void GLWidgetProp::SetDouble( const std::string& strProp, double dValue )
{
	MAP_PROP_ITER iter = m_mapProp.find( strProp );
	if( iter == m_mapProp.end() )
		return;

	SVALUE& sValue = iter->second;
	sValue.dValue = dValue;
	sValue.bIsValid = true;
}

void GLWidgetProp::SetBool( const std::string& strProp, bool bValue )
{
	MAP_PROP_ITER iter = m_mapProp.find( strProp );
	if( iter == m_mapProp.end() )
		return;

	SVALUE& sValue = iter->second;
	sValue.bValue = bValue;
	sValue.bIsValid = true;
}

void GLWidgetProp::SetString( const std::string& strProp, const std::string& strValue )
{
	MAP_PROP_ITER iter = m_mapProp.find( strProp );
	if( iter == m_mapProp.end() )
		return;

	SVALUE& sValue = iter->second;
	sValue.strValue = strValue;
	sValue.bIsValid = true;
}

bool GLWidgetProp::GetInt( const std::string& strProp, int& nValue )
{
	MAP_PROP_ITER iter = m_mapProp.find( strProp );
	if( iter == m_mapProp.end() )
		return false;

	const SVALUE& sValue = iter->second;
	if( false == sValue.bIsValid )
		return false;

	nValue = sValue.nValue;

	return true;
}

bool GLWidgetProp::GetDWORD( const std::string& strProp, DWORD& dwValue )
{
	MAP_PROP_ITER iter = m_mapProp.find( strProp );
	if( iter == m_mapProp.end() )
		return false;

	const SVALUE& sValue = iter->second;
	if( false == sValue.bIsValid )
		return false;

	dwValue = sValue.dwValue;

	return true;
}

bool GLWidgetProp::GetDouble( const std::string& strProp, double& dValue )
{
	MAP_PROP_ITER iter = m_mapProp.find( strProp );
	if( iter == m_mapProp.end() )
		return false;

	const SVALUE& sValue = iter->second;
	if( false == sValue.bIsValid )
		return false;

	dValue = sValue.dValue;

	return true;
}

bool GLWidgetProp::GetBool( const std::string& strProp, bool& bValue )
{
	MAP_PROP_ITER iter = m_mapProp.find( strProp );
	if( iter == m_mapProp.end() )
		return false;

	const SVALUE& sValue = iter->second;
	if( false == sValue.bIsValid )
		return false;

	bValue = sValue.bValue;

	return true;
}

bool GLWidgetProp::GetString( const std::string& strProp, std::string& strValue )
{
	MAP_PROP_ITER iter = m_mapProp.find( strProp );
	if( iter == m_mapProp.end() )
		return false;

	const SVALUE& sValue = iter->second;
	if( false == sValue.bIsValid )
		return false;

	strValue = sValue.strValue;

	return true;
}

//--------------------------------------------------------------------------------------------------------

const std::string GLWidgetSaveProp::m_XMLNAME = "widgetprop.xml";

GLWidgetSaveProp::GLWidgetSaveProp()
: fUIAlpha( 1.0f ),
	m_nBasicChat_Size( -1 )
{
}

GLWidgetSaveProp::GLWidgetSaveProp( const GLWidgetSaveProp& value )
{
}

GLWidgetSaveProp::~GLWidgetSaveProp()
{
	m_mapWidgetProp.clear();
}

GLWidgetSaveProp& GLWidgetSaveProp::GetInstance()
{
	static GLWidgetSaveProp Instance;
	return Instance;
}

void GLWidgetSaveProp::SaveXML()
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();

	std::string strPath = pGlobalStage->GetAppPath();
	strPath.append( SubPath::GUI_FILE_ROOT );
	strPath.append( m_XMLNAME );

	TiXmlDocument doc;
	if( doc.LoadFile( strPath.c_str() ) == false )
	{
		TiXmlDeclaration* pDecl = new TiXmlDeclaration( "1.0", "", "" );
		doc.LinkEndChild( pDecl );
	}
	
	TiXmlElement* pRoot = doc.FirstChildElement("WIDGETPROP");
	if( pRoot == NULL )
	{
		pRoot = new TiXmlElement( "WIDGETPROP" );
		doc.LinkEndChild( pRoot );
	}

	TiXmlElement* pID = pRoot->FirstChildElement( RANPARAM::GETUSERID_DEC() );
	if( pID == NULL )
	{	
		pID = new TiXmlElement( RANPARAM::GETUSERID_DEC() );
		pRoot->LinkEndChild( pID );
	}

	TiXmlElement* pCha = pID->FirstChildElement(m_strName.c_str());
	if( pCha == NULL )
	{
		pCha = new TiXmlElement( m_strName );
		pID->LinkEndChild( pCha );
	}
	else
	{
		pID->RemoveChild(pCha);
		pCha = NULL;
		pCha = new TiXmlElement( m_strName );
		pID->LinkEndChild( pCha );
	}


	// 전체 UI 알파값
	TiXmlElement* pUIAlphaElem = new TiXmlElement( "UIALPHA" );
	pUIAlphaElem->SetDoubleAttribute( "value", fUIAlpha );
	pCha->LinkEndChild( pUIAlphaElem );

	// 채팅 폰트 크기
	TiXmlElement* pBasicChatSize = new TiXmlElement( "BASICCHATSIZE" );
	pBasicChatSize->SetDoubleAttribute( "value",
		GLWidgetScript::GetInstance().LuaCallFunc<int>(NSWIDGET_SCRIPT::g_strFunc_BasicChat_GetSize) );
	pCha->LinkEndChild( pBasicChatSize );
	
	// 이동한 UI인가 ( QUICKMENU ) 임시로만든 함수.. 차후에 다른 UI까지 파악해서 수정.. 
	TiXmlElement* pIsMoveUI_QuickMenu = new TiXmlElement( "IsMovedUI_QUICKMENU" );
	bool bIsMoveUI_QUCKMENU = false;

	bIsMoveUI_QUCKMENU = GLWidgetScript::GetInstance().LuaCallFunc<bool>(
		NSWIDGET_SCRIPT::g_strFunc_QuickMenu_IsMovedUI );		
	
	pIsMoveUI_QuickMenu->SetDoubleAttribute( "value", bIsMoveUI_QUCKMENU );
	pCha->LinkEndChild( pIsMoveUI_QuickMenu );


	MAP_SAVEWIDGETPROP_ITER iterWidgetProp = m_mapWidgetProp.begin();
	for( ; iterWidgetProp != m_mapWidgetProp.end(); ++iterWidgetProp )
	{
		WORD wID = iterWidgetProp->first;
		MAP_INSTPROP& mapInstProp = iterWidgetProp->second;

		MAP_INSTPROP_ITER iterInstProp = mapInstProp.begin();
		for( ; iterInstProp != mapInstProp.end(); ++iterInstProp )
		{
			const std::string& strInst = iterInstProp->first;
			GLWidgetProp& prop = iterInstProp->second;

			GLWidgetProp::MAP_PROP_ITER iterProp = prop.m_mapProp.begin();
			for( ; iterProp != prop.m_mapProp.end(); ++iterProp )
			{
				const std::string& strProp = iterProp->first;
				const GLWidgetProp::SVALUE& sValue = iterProp->second;

				if( false == sValue.bIsValid )
					continue;

				TiXmlElement* pElem = new TiXmlElement( "ELEM" );
				pElem->SetAttribute( "id", wID );
				pElem->SetAttribute( "type", (int) sValue.emType );
				pElem->SetAttribute( "prop", strProp.c_str() );

				switch( sValue.emType )
				{
				case GLWidgetProp::SVALUE::V_INT :
					{
						pElem->SetAttribute( "value", sValue.nValue );
					}
					break;

				case GLWidgetProp::SVALUE::V_DWORD :
					{
						char sz[ 64 ];
						ultoa( sValue.dwValue, sz, 10 );

						pElem->SetAttribute( "value", sz );
					}
					break;

				case GLWidgetProp::SVALUE::V_DOUBLE :
					{
						pElem->SetDoubleAttribute( "value", sValue.dValue );
					}
					break;

				case GLWidgetProp::SVALUE::V_BOOL :
					{
						int n = 0;
						if( true == sValue.bValue )
							n = 1;

						pElem->SetAttribute( "value", n );
					}
					break;

				case GLWidgetProp::SVALUE::V_STRING :
					{
						pElem->SetAttribute( "value", sValue.strValue.c_str() );
					}
					break;
				}

				pElem->SetAttribute( "inst", strInst.c_str() );

				pCha->LinkEndChild( pElem );
			}
		}
	}

	doc.SaveFile(strPath.c_str());
}

void GLWidgetSaveProp::LoadXML( std::string strName )
{
	DxGlobalStage* pGlobalStage = GLWidgetScript::GetInstance().m_pGaeaClient->GetGlobalStage();
	m_strName = strName;

	std::string strPath = pGlobalStage->GetAppPath();
	strPath.append( SubPath::GUI_FILE_ROOT );
	strPath.append( m_XMLNAME );

	TiXmlDocument doc;
	if( false == doc.LoadFile( strPath.c_str() ) )
		return;

	TiXmlElement* pRoot = doc.FirstChildElement( "WIDGETPROP" );
	if( NULL == pRoot )
		return;

	TiXmlElement* pID = pRoot->FirstChildElement( RANPARAM::GETUSERID_DEC() );
	if( pID == NULL )
		return;

	TiXmlElement* pCha = pID->FirstChildElement(strName.c_str());
	if( pCha == NULL )
		return;

	// 전체 UI 알파값
	TiXmlElement* pUIAlphaElem = pCha->FirstChildElement( "UIALPHA" );
	if( NULL != pUIAlphaElem )
	{
		TiXmlAttribute* pAtb = pUIAlphaElem->FirstAttribute();
		if( NULL != pAtb )
		{
			const char* szName = pAtb->Name();
			const char* szValue = pAtb->Value();

			if( NULL != szName && NULL != szValue )
			{
				if( 0 == strcmp( "value", szName ) )
					fUIAlpha = (float) atof( szValue );
			}
		}
	}

	TiXmlElement* pBaisChatSizeElem = pCha->FirstChildElement( "BASICCHATSIZE" );
	if( NULL != pBaisChatSizeElem )
	{
		TiXmlAttribute* pAtb = pBaisChatSizeElem->FirstAttribute();
		if( NULL != pAtb )
		{
			const char* szName = pAtb->Name();
			const char* szValue = pAtb->Value();

			if( NULL != szName && NULL != szValue )
			{
				if( 0 == strcmp( "value", szName ) )
					m_nBasicChat_Size = (float) atof( szValue );
			}
		}
	}

	// 이동한 UI인가 
	TiXmlElement* pIsMovedUI_QuickMenu = pCha->FirstChildElement( "IsMovedUI_QUICKMENU" );
	if( NULL != pIsMovedUI_QuickMenu )
	{
		TiXmlAttribute* pAtb = pIsMovedUI_QuickMenu->FirstAttribute();
		if( NULL != pAtb )
		{
			const char* szName = pAtb->Name();
			const char* szValue = pAtb->Value();

			if( NULL != szName && NULL != szValue )
			{
				bool bQuickMenu_Moved = false;
				if( 0 == strcmp( "value", szName ) )
					bQuickMenu_Moved  = (bool) atof( szValue );

				GLWidgetScript::GetInstance().LuaCallFunc<void>(
					NSWIDGET_SCRIPT::g_strFunc_QuickMenu_SetMovedUI
					, "-b", bQuickMenu_Moved );		

			}
		}
	}

	TiXmlElement* pElem = pCha->FirstChildElement( "ELEM" );
	if( NULL == pElem )
		return;

	while( NULL != pElem )
	{
		WORD wID = -1;
		std::string strProp = "";
		std::string strInst = "";
		std::string strValue = "";
		GLWidgetProp::SVALUE::EMVALUETYPE emType = GLWidgetProp::SVALUE::V_NONE;
		
		TiXmlAttribute* pAtb = pElem->FirstAttribute();
		while( NULL != pAtb )
		{
			const char* szName = pAtb->Name();
			const char* szValue = pAtb->Value();

			if( NULL != szName && NULL != szValue )
			{
				if( 0 == strcmp( "id", szName ) )
				{
					wID = (WORD) atoi( szValue );
				}
				else if( 0 == strcmp( "type", szName ) )
				{
					emType = (GLWidgetProp::SVALUE::EMVALUETYPE) atoi( szValue );
				}
				else if( 0 == strcmp( "prop", szName ) )
				{
					strProp = szValue;
				}
				else if( 0 == strcmp( "value", szName ) )
				{
					strValue = szValue;
				}
				else if( 0 == strcmp( "inst", szName ) )
				{
					strInst = szValue;
				}
			}

			pAtb = pAtb->Next();
		}

		MAP_SAVEWIDGETPROP_ITER iterWidgetProp = m_mapWidgetProp.find( wID );
		if( iterWidgetProp != m_mapWidgetProp.end() )
		{
			MAP_INSTPROP& mapInstProp = iterWidgetProp->second;

			MAP_INSTPROP_ITER iterInstProp = mapInstProp.find( strInst );
			if( iterInstProp != mapInstProp.end() )
			{
				GLWidgetProp& prop = iterInstProp->second;

				// value 값 설정
				switch( emType )
				{
				case GLWidgetProp::SVALUE::V_INT :
					{
						int n = atoi( strValue.c_str() );
						prop.SetInt( strProp, n );
					}
					break;

				case GLWidgetProp::SVALUE::V_DWORD :
					{
						DWORD dw = (DWORD) atol( strValue.c_str() );
						prop.SetDWORD( strProp, dw );
					}
					break;

				case GLWidgetProp::SVALUE::V_DOUBLE :
					{
						double d = atof( strValue.c_str() );
						prop.SetDouble( strProp, d );
					}
					break;

				case GLWidgetProp::SVALUE::V_BOOL :
					{
						bool b = 0 == atoi( strValue.c_str() ) ? false : true;
						prop.SetBool( strProp, b );
					}
					break;

				case GLWidgetProp::SVALUE::V_STRING :
					{
						prop.SetString( strProp, strValue );
					}
					break;
				}
			}
		}

		pElem = pElem->NextSiblingElement( "ELEM" );
	}
}

void GLWidgetSaveProp::Load( LuaTable& tbSaveProp )
{
	m_mapWidgetProp.clear();

	for( int i = 1; i <= tbSaveProp.TableLen(); ++i )
	{
		LuaTable tbProp = tbSaveProp.get< LuaTable >( i );
		if( 4 != tbProp.TableLen() )
		{
			CString str;
			str.Format( "WIDGET_SAVE_PROP table %d Index - 4개의 값이 필요함(nID, nType, \"prop\", \"inst\")", i );
			GLWidgetScript::PrintDebug( str.GetBuffer() );
			continue;
		}

		WORD wID = tbProp.get< WORD >( 1 );
		GLWidgetProp::SVALUE::EMVALUETYPE emType = (GLWidgetProp::SVALUE::EMVALUETYPE) tbProp.get< int >( 2 );
		const char* szProp = tbProp.get< const char* >( 3 );
		const char* szInst = tbProp.get< const char* >( 4 );

		if( NULL == szProp || NULL == szInst )
			continue;

		// 위젯 ID로 인스턴스 맵을 찾는다.
		MAP_SAVEWIDGETPROP_ITER iterWidgetProp = m_mapWidgetProp.find( wID );
		// 없으면 새 인스턴스 맵 만들기
		if( iterWidgetProp == m_mapWidgetProp.end() )
		{
			MAP_INSTPROP mapProp;
			m_mapWidgetProp.insert( std::make_pair( wID, mapProp ) );

			iterWidgetProp = m_mapWidgetProp.find( wID );
		}

		MAP_INSTPROP& mapInstProp = iterWidgetProp->second;
		
		// 인스턴스 이름으로 프로퍼티 맵을 찾는다.
		MAP_INSTPROP_ITER iterInstProp = mapInstProp.find( szInst );
		// 없으면 새 프로퍼티 맵 만들기
		if( iterInstProp == mapInstProp.end() )
		{
			mapInstProp.insert( std::make_pair( szInst, GLWidgetProp() ) );

			iterInstProp = mapInstProp.find( szInst );
		}

		// 저장할 프로퍼티 이름을 넣는다.
		GLWidgetProp& prop = iterInstProp->second;
		prop.InsertValue( emType, szProp );
	}

	// XML 로드해본다.
	//LoadXML();
}

void GLWidgetSaveProp::ResetValue()
{
	MAP_SAVEWIDGETPROP_ITER iterWidgetProp = m_mapWidgetProp.begin();
	for( ; iterWidgetProp != m_mapWidgetProp.end(); ++iterWidgetProp )
	{
		MAP_INSTPROP& mapInstProp = iterWidgetProp->second;

		MAP_INSTPROP_ITER iterInstProp = mapInstProp.begin();
		for( ; iterInstProp != mapInstProp.end(); ++iterInstProp )
		{
			GLWidgetProp& prop = iterInstProp->second;

			GLWidgetProp::MAP_PROP_ITER iterProp = prop.m_mapProp.begin();
			for( ; iterProp != prop.m_mapProp.end(); ++iterProp )
			{
				GLWidgetProp::SVALUE& sValue = iterProp->second;
				sValue.bIsValid = false;
			}
		}
	}
}

void GLWidgetSaveProp::SetProperty( WORD wWidgetID, WORD wSubID )
{
	// 위젯 ID로 인스턴스 맵을 찾는다.
	MAP_SAVEWIDGETPROP_ITER iterWidgetProp = m_mapWidgetProp.find( wWidgetID );
	if( iterWidgetProp == m_mapWidgetProp.end() )
		return;

	MAP_INSTPROP& mapInstProp = iterWidgetProp->second;

	MAP_INSTPROP_ITER iterInstProp = mapInstProp.begin();
	for( ; iterInstProp != mapInstProp.end(); ++iterInstProp )
	{
		const std::string& strInst = iterInstProp->first;
		GLWidgetProp& prop = iterInstProp->second;

		// 저장된 프로퍼티 맵에서 값을 가져와 적용
		GLWidgetProp::MAP_PROP_ITER iterProp = prop.m_mapProp.begin();
		for( ; iterProp != prop.m_mapProp.end(); ++iterProp )
		{
			const std::string& strProp = iterProp->first;
			GLWidgetProp::SVALUE& sValue = iterProp->second;

			// 아직 저장된 값이 없으면 패스
			if( false == sValue.bIsValid )
				continue;
			
			switch( sValue.emType )
			{
			case GLWidgetProp::SVALUE::V_INT :
				{
					// 위치 저장된 값이 현재 해상도 밖으로 넘어갈 수 있다.
					// 체크해보고 위치 다시 잡자.
					if( 0 == strInst.size() )
					{
						if( 0 == strcmp( "x", strProp.c_str() ) )
						{
							int nScreenW = GLWidgetScript::GetScreenSizeWidth();
							int nW =0;

							if( wWidgetID == 57 ) // 퀵메뉴는 독특한 방식이라 스크립트에서 자동으로 값이 수정됨.
							{
								nW = 0;
							}
							else
							{
								nW = GLWidgetScript::GetInt( wWidgetID, "width", "", wSubID );
							}

							int nNewX = sValue.nValue;
													
							// 스크린 W가 10이라면 최소화 상태..
							if( nScreenW > 10 )
							{
								if( 0 > nNewX )
									nNewX = 0;
								else if( nScreenW < nNewX + nW )
									nNewX = nScreenW - nW;
							}
							

							sValue.nValue = nNewX;
						}
						else if( 0 == strcmp( "y", strProp.c_str() ) )
						{
							int nScreenY = GLWidgetScript::GetScreenSizeHeight();
							int nH = GLWidgetScript::GetInt( wWidgetID, "height", "", wSubID );

							int nNewY = sValue.nValue;

							// 스크린Y가 10이라면 최소화 상태..
							if( nScreenY > 10)
							{
								if( 0 > nNewY )
									nNewY = 0;
								else if( nScreenY < nNewY + nH )
									nNewY = nScreenY - nH;

								sValue.nValue = nNewY;
							}
						}
					}

					GLWidgetScript::SetInt( 
						wWidgetID, strProp.c_str(), sValue.nValue, strInst.c_str(), wSubID );
				}
				break;

			case GLWidgetProp::SVALUE::V_DWORD :
				{
					GLWidgetScript::SetUInt( 
						wWidgetID, strProp.c_str(), sValue.dwValue, strInst.c_str(), wSubID );
				}
				break;

			case GLWidgetProp::SVALUE::V_DOUBLE :
				{
					GLWidgetScript::SetDouble( 
						wWidgetID, strProp.c_str(), sValue.dValue, strInst.c_str(), wSubID );
				}
				break;

			case GLWidgetProp::SVALUE::V_BOOL :
				{
					GLWidgetScript::SetBool( 
						wWidgetID, strProp.c_str(), sValue.bValue, strInst.c_str(), wSubID );
				}
				break;

			case GLWidgetProp::SVALUE::V_STRING :
				{
					GLWidgetScript::SetString( 
						wWidgetID, strProp.c_str(), sValue.strValue.c_str(), strInst.c_str(), wSubID );
				}
				break;
			}
		}
	}
}

void GLWidgetSaveProp::SaveProperty( WORD wWidgetID, WORD wSubID )
{
	// 위젯 ID로 인스턴스 맵을 찾는다.
	MAP_SAVEWIDGETPROP_ITER iterWidgetProp = m_mapWidgetProp.find( wWidgetID );
	if( iterWidgetProp == m_mapWidgetProp.end() )
		return;

	MAP_INSTPROP& mapInstProp = iterWidgetProp->second;

	MAP_INSTPROP_ITER iterInstProp = mapInstProp.begin();
	for( ; iterInstProp != mapInstProp.end(); ++iterInstProp )
	{
		const std::string& strInst = iterInstProp->first;
		GLWidgetProp& prop = iterInstProp->second;

		// 현재 UI 프로퍼티에 설정된 값을 가져와 저장
		GLWidgetProp::MAP_PROP_ITER iterProp = prop.m_mapProp.begin();
		for( ; iterProp != prop.m_mapProp.end(); ++iterProp )
		{
			const std::string& strProp = iterProp->first;
			GLWidgetProp::SVALUE& sValue = iterProp->second;

			// 저장됨
			sValue.bIsValid = true;

			switch( sValue.emType )
			{
			case GLWidgetProp::SVALUE::V_INT :
				{
					int n = GLWidgetScript::GetInt(
						wWidgetID, strProp.c_str(), strInst.c_str(), wSubID );

					sValue.nValue = n;
					
					if( wWidgetID == 57 && 0 == strcmp( "x", strProp.c_str() ) )
					{
						int nWidth = GLWidgetScript::GetInt(
							wWidgetID, "width", "QuickMenu.QuickDrag", wSubID ) - 60;

						sValue.nValue += nWidth;
					}

				}
				break;

			case GLWidgetProp::SVALUE::V_DWORD :
				{
					DWORD dw = GLWidgetScript::GetUInt(
						wWidgetID, strProp.c_str(), strInst.c_str(), wSubID );
					sValue.dwValue = dw;
				}
				break;

			case GLWidgetProp::SVALUE::V_DOUBLE :
				{
					double d = GLWidgetScript::GetDouble( 
						wWidgetID, strProp.c_str(), strInst.c_str(), wSubID );
					sValue.dValue = d;
				}
				break;

			case GLWidgetProp::SVALUE::V_BOOL :
				{
					bool b = GLWidgetScript::GetBool(
						wWidgetID, strProp.c_str(), strInst.c_str(), wSubID );
					sValue.bValue = b;
				}
				break;

			case GLWidgetProp::SVALUE::V_STRING :
				{
					std::string str = GLWidgetScript::GetString( 
						wWidgetID, strProp.c_str(), strInst.c_str(), wSubID );
					sValue.strValue = str;
				}
				break;
			}
		}
	}
}