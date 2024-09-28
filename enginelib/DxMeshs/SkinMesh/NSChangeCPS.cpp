#include "pch.h"

#include "../../../=LuaPlus/src/LuaPlus/src/lua.h"

#include "../../../SigmaCore/Lua/MinLua.h"

#include "NSChangeCPS.h"



//////////////////////////////////////////////////////////////////////////
//	���غο��� ó�� ����.
// A.chf �� B.chf �� ������ �����ϰ� ��.
// ����� Skin �� �Ǿ��ִµ� �̰� A.cps �� A.chf, B.chf �� ������ �����ؾ� �ߴ�. ( A.cps �� A.chf �� Bone �� ��� )
// �׷��� B.chf �� A.cps �� �ְ� ������, Bone ���̷� ���� ���� ���� ����. (�۾��� �ؼ� ������ �ص� ������ ������ �߻��̵ȴ�.)
// �׷��� A.cps �� ���� �ʰ� B.cps �� ������ �� �ֵ��� �۾��Ѵ�. ( B.cps �� B.chf �� Bone �� ��� )
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
	//		   CPS | ��ü CPS
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
		// ���� �̸��� ���.
		TSTRING strFullPath( g_strPath );
		strFullPath += g_strFileName;

		FILE* file = _fsopen ( strFullPath.c_str(), "rb", _SH_DENYNO );
		if ( !file )	
			return FALSE;   // ������ ����.
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
