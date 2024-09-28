#include "pch.h"
#include "GLRanMobile.h"

#include "../../SigmaCore/Lua/lua_tinker.h"
#include "../../SigmaCore/Lua/MinLua.h"

extern "C"
{
#include "../../=LuaPlus/src/LuaPlus/src/lua.h"
#include "../../=LuaPlus/src/LuaPlus/src/lualib.h"
#include "../../=LuaPlus/src/LuaPlus/src/lauxlib.h"
}

#include "../../enginelib/G-Logic/GLogic.h"
#include "../../RanLogic/GLUseFeatures.h"
#include "../GLogicDataMan.h"

namespace NSRanMobile
{
	GLRanMobile::GLRanMobile()
		: m_llRecognizePrice( 0 )
		, m_llExtendPrice( 0 )
		, m_nFirstTime( 0 )
		, m_nExtendTime( 0 )
		, m_nMaxSendJewelCount( 100 )
		, m_sDailyRewardID( false )
	{
	}

	GLRanMobile::~GLRanMobile()
	{
		m_vecReawrd.clear();
	}

	GLRanMobile& GLRanMobile::GetInstance()
	{
		static GLRanMobile Instance;
		return Instance;
	}

	bool GLRanMobile::Load()
	{
		if( false == GLUseFeatures::GetInstance().IsUsingRanMobile() )
			return true;

		std::string strPath = GLOGIC::GetPath();
		strPath.append( GLogicData::GetInstance().GetRanMobileFileName() );

		lua_State* pLuaState = lua_open();
		luaL_openlibs( pLuaState );

		if( false == lua_tinker::dofile( pLuaState, strPath.c_str() ) )
		{
			MessageBoxA( NULL, sc::lua_getError().c_str(), strPath.c_str(), MB_OK );
			return false;
		}

		m_strHomePageURL = lua_tinker::get< const char* >( pLuaState, "g_strHomePageURL" );
		m_strAndroidDownloadURL = lua_tinker::get< const char* >( pLuaState, "g_strAndroidDownloadPageURL" );
		m_strIOSDownloadURL = lua_tinker::get< const char* >( pLuaState, "g_strIOSDownloadPageURL" );

		m_llRecognizePrice = lua_tinker::get< LONGLONG >( pLuaState, "g_nRecognizePrice" );
		m_llExtendPrice = lua_tinker::get< LONGLONG >( pLuaState, "g_nExtendPrice" );
		m_nFirstTime = lua_tinker::get< int >( pLuaState, "g_nFirstTime" );
		m_nExtendTime = lua_tinker::get< int >( pLuaState, "g_nExtendTime" );

		m_nMaxSendJewelCount = lua_tinker::get< int >( pLuaState, "g_nMaxSendJewelCount" );

		lua_tinker::table tbDailyReward = lua_tinker::get< lua_tinker::table >( pLuaState, "g_tbDailyReward" );
		m_sDailyRewardID = SNATIVEID( tbDailyReward.get< WORD >( 1 ), tbDailyReward.get< WORD >( 2 ) );
		m_wDailyRewardCount = tbDailyReward.get< WORD >( 3 );

		lua_tinker::table tbReward = lua_tinker::get< lua_tinker::table >( pLuaState, "g_tbReward" );
		for( int i = 1; i <= tbReward.TableLen(); ++i )
		{
			lua_tinker::table tb = tbReward.get< lua_tinker::table >( i );
			if( false == lua_istable( tb.m_obj->m_L, -1 ) )
				continue;

			WORD wMID = tb.get< WORD >( 1 );
			WORD wSID = tb.get< WORD >( 2 );
			WORD wCount = tb.get< WORD >( 3 );
			DWORD dwMPoint = tb.get< DWORD >( 4 );
			float fExpRaio = tb.get< float >( 5 );

			m_vecReawrd.push_back( SRewardItem( wMID, wSID, wCount, dwMPoint, fExpRaio ) );
		}

		lua_close( pLuaState );
		pLuaState = NULL;

		return true;
	}
}