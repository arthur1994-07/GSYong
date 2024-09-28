#include "pch.h"
#include "GLWidgetsList.h"

#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/File/FileUtil.h"
#include "../../SigmaCore/Lua/lua_tinker.h"
#include "../../SigmaCore/Compress/ZipMan.h"
#include "../../enginelib/Common/SUBPATH.h"
#include "../../enginelib/GUInterface/GameTextControl.h"

extern "C"
{
#include "../../=LuaPlus/src/LuaPlus/src/lua.h"
#include "../../=LuaPlus/src/LuaPlus/src/lualib.h"
#include "../../=LuaPlus/src/LuaPlus/src/lauxlib.h"
}

#include "../GLGaeaClient.h"

namespace NSWIDGETLIST
{
	static const char* g_strWidgetListFile = "widgetslist.lua";
}

GLWidget::GLWidget(void)
: m_wWidgetID( 0 )
, m_bWindow( false )
, m_bModal( false )
, m_bEscCloseable( false )
, m_bSaveProp( false )
, m_bSnapping( false )
, m_bCloseToDelete( false )
, m_wDeleteTime( 0 )
, m_wPosX( 0 )
, m_wPosY( 0 )
, m_wLayer( 0 )
, m_wAlignType( 0 )
, m_nOffsetX( 0 )
, m_nOffsetY( 0 )
{
}

GLWidget::~GLWidget(void)
{
}

//----------------------------------------------------------------------------------------

GLWidgetsList::GLWidgetsList(void)
{
}

GLWidgetsList::GLWidgetsList( const GLWidgetsList& value )
{
}

GLWidgetsList::~GLWidgetsList(void)
{
	Clear();
}

GLWidgetsList& GLWidgetsList::GetInstance()
{
	static GLWidgetsList Instance;
	return Instance;
}

bool GLWidgetsList::LoadFile( lua_State* pLuaState, const SUBPATH* pPath )
{
	Clear(); 

	if( NULL == pLuaState )
		return false;

	std::string strFilePath( sc::getAppPath() );
	strFilePath.append( pPath->WidgetScriptPath() );
	strFilePath.append( NSWIDGETLIST::g_strWidgetListFile );

	if( false == lua_tinker::dofile( pLuaState, strFilePath.c_str() ) )
		return false;

	SetWidgetList( pLuaState );

	return true;
}

// zip 내부에 있는 WidgetsList lua 파일을 읽도록 한다.
bool GLWidgetsList::LoadBufferZIP( lua_State* pLuaState, const std::string& strPathWSF )
{
	Clear(); 

	if( NULL == pLuaState )
		return false;

	BYTE* pByte(NULL);
	std::string strLucFile( sc::file::changeFileExt( NSWIDGETLIST::g_strWidgetListFile, "luc" ) );
	std::transform( strLucFile.begin(), strLucFile.end(), strLucFile.begin(), tolower );
	UINT size = ZipMan::GetInstance().UnzipToMemory(strPathWSF, strLucFile, pByte);
	if ( size <= 0 || pByte == NULL )
	{
		return false;
	}

	if( false == lua_tinker::dobuffer( pLuaState, reinterpret_cast<char*>( pByte ), size ) )
		return false;

	SetWidgetList( pLuaState );

	return true;
}

void GLWidgetsList::SetWidgetList( lua_State* pLuaState )
{
	lua_tinker::table luaList = lua_tinker::get< lua_tinker::table >( pLuaState, "WIDGETS_LIST" );

	m_mapWidget.clear();
	for( int i = 1; i <= luaList.TableLen(); ++i )
	{
		lua_tinker::table luaWidget = luaList.get< lua_tinker::table >( i );

		std::string strTitle;

		GLWidget widget;

		widget.m_wWidgetID = luaWidget.get< WORD >( "wWidgetID" );
		widget.m_bWindow = luaWidget.get< bool >( "bWindow" );
		widget.m_bModal= luaWidget.get< bool >( "bModal" );
		widget.m_bEscCloseable = luaWidget.get< bool >( "bEscCloseable" );
		widget.m_bCloseToDelete = luaWidget.get< bool >( "bCloseToDelete" );
		widget.m_bSnapping = luaWidget.get< bool >( "bSnapping" );
		widget.m_bSaveProp = luaWidget.get< bool >( "bSaveProp" );

		widget.m_wDeleteTime = luaWidget.get< WORD >( "wDeleteTime" );

		widget.m_strFileName = luaWidget.get< const char* >( "strFileName" );
		widget.m_strWindowFile = luaWidget.get< const char* >( "strWindowFile" );
		strTitle = luaWidget.get< const char* >( "strTitleName" );		
		widget.m_strLoadCompleteCall = luaWidget.get< const char* >( "strLoadCompleteCall" );
		widget.m_strOpenCall = luaWidget.get< const char* >( "strOpenCall" );

		widget.m_wPosX = luaWidget.get< WORD >( "wPosX" );
		widget.m_wPosY = luaWidget.get< WORD >( "wPosY" );

		widget.m_wLayer = luaWidget.get< WORD >( "wLayer" );

		widget.m_wAlignType = luaWidget.get< WORD >( "wAlignType" );
		widget.m_nOffsetX = luaWidget.get< int >( "nOffsetX" );
		widget.m_nOffsetY = luaWidget.get< int >( "nOffsetY" );

		if( true == widget.m_bWindow && false == strTitle.empty() )
		{
			strTitle = ID2GAMEWORD( strTitle.c_str(), 0 );
			widget.m_strTitleName = GfxInterfaceBase::GetInstance()->EncodeText( strTitle );
		}

		m_mapWidget.insert( std::make_pair( widget.m_wWidgetID, widget ) );
	}
}

void GLWidgetsList::Clear()
{
	m_mapWidget.clear();
}

GLWidget* GLWidgetsList::GetWidget( WORD wWidgetID )
{
	MAP_WIDGET_ITER iter = m_mapWidget.find( wWidgetID );
	if( iter == m_mapWidget.end() )
		return NULL;

	return &(iter->second);
}
