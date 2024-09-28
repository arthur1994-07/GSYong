#include "pch.h"

#include "../../../=LuaPlus/src/LuaPlus/src/lua.h"

#include "../../../SigmaCore/Lua/MinLua.h"

#include "NSChangeCPS.h"



//////////////////////////////////////////////////////////////////////////
//	연극부에서 처음 사용됨.
// A.chf 가 B.chf 로 변경이 가능하게 됨.
// 우산은 Skin 이 되어있는데 이걸 A.cps 를 A.chf, B.chf 에 셋팅이 가능해야 했다. ( A.cps 는 A.chf 의 Bone 을 사용 )
// 그런데 B.chf 에 A.cps 를 넣고 싶지만, Bone 차이로 인해 넣을 수가 없다. (작업을 해서 넣은다 해도 스케일 문제가 발생이된다.)
// 그래서 A.cps 를 넣지 않고 B.cps 를 셋팅할 수 있도록 작업한다. ( B.cps 는 B.chf 의 Bone 을 사용 )
//////////////////////////////////////////////////////////////////////////

namespace NSChangeCPS
{
	//////////////////////////////////////////////////////////////////////////
	//
	TSTRING	g_strPath;
	TSTRING g_strFileName(_T("changecps.lua"));

	void SetPath( const TCHAR* pName )
	{
		g_strPath = pName;
	}

	const TCHAR* GetPath()
	{
		return g_strPath.c_str();
	}
	//
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	//
	//		   CPS | 대체 CPS
	std::map<TSTRING,TSTRING>	g_mapChangeCPS;
	//
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	//
	BOOL ParseCommentLua()
	{
		try
		{
			g_mapChangeCPS.clear();

			LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "CHANGE_CPS_LIST" ) );
			for ( LuaPlus::LuaTableIterator Iter(ob); Iter; Iter.Next() )
			{
				LuaPlus::LuaTableIterator IterA(Iter.GetValue());
				TSTRING	strFirstCPS( IterA.GetValue().GetString() );
				IterA.Next();

				g_mapChangeCPS.insert( std::make_pair( strFirstCPS.c_str(), IterA.GetValue().GetString() ) );
			}
		}
		catch( const LuaPlus::LuaException& e )
		{        
			MessageBox( NULL, e.GetErrorMessage(), _T( "NSChangeCPS" ), MB_OK );
		}

		return TRUE;
	}

	BOOL LuaLoad_RELEASED_DEBUG()
	{
		// 파일 이름과 경로.
		TSTRING strFullPath( g_strPath );
		strFullPath += g_strFileName;

		FILE* file = _fsopen ( strFullPath.c_str(), "rb", _SH_DENYNO );
		if ( !file )	
			return FALSE;   // 파일이 없다.
		fclose(file);

		try
		{
			sc::lua_destroyState();
			sc::lua_init();

			if (!sc::lua_doFile(strFullPath.c_str()))
			{
				MessageBox( NULL, sc::lua_getError().c_str(), g_strFileName.c_str(), MB_OK );
				return FALSE;
			}

			return ParseCommentLua();
		}
		catch( const LuaPlus::LuaException& e )
		{        
			MessageBox( NULL, e.GetErrorMessage(), strFullPath.c_str(), MB_OK );
			return FALSE;
		}

		return FALSE;
	}
	//
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	//
	BOOL LuaLoad()
	{
		if ( LuaLoad_RELEASED_DEBUG() )
			return TRUE;

		return FALSE;
	}

	const TCHAR* GetNewCPS( const TCHAR* pNameCPS )
	{
		std::map<TSTRING,TSTRING>::const_iterator citer = g_mapChangeCPS.find( pNameCPS );
		if ( citer != g_mapChangeCPS.end() )
		{
			return (*citer).second.c_str();
		}
		return NULL;
	}
	//
	//////////////////////////////////////////////////////////////////////////
};
