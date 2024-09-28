#include "pch.h"

#include "../../../=LuaPlus/src/LuaPlus/src/lua.h"

#include "../../../SigmaCore/Util/Util.h"
#include "../../../SigmaCore/Lua/MinLua.h"

#include "./DxBillboard.h"

#include "NSBillboardLua.h"


//////////////////////////////////////////////////////////////////////////
//							NSBillboard
namespace NSBillboard
{
	//////////////////////////////////////////////////////////////////////////
	//								Path
	TSTRING	g_strTexturePath;

	void SetTexturePath( const TCHAR* pName )
	{
		g_strTexturePath = pName;
	}

	const TCHAR* GetTexturePath()
	{
		return g_strTexturePath.c_str();
	}
	//								Path
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	//							CharEdit 에서만 사용함.
	const BILLBOARD_CHF_DATA*	g_pBillboardChfData(NULL);

	void ImportBillboard( const BILLBOARD_CHF_DATA*	pBillboardChfData )
	{
		g_pBillboardChfData = pBillboardChfData;
	}

	const BILLBOARD_CHF_DATA* GetBillboardChfData()
	{
		return g_pBillboardChfData;
	}
	//							CharEdit 에서만 사용함.
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	//							pushBillboard
	std::map<TSTRING,boost::shared_ptr<BILLBOARD_CHF>>	g_mapChfBillboard;

	void CleanUp()
	{
		g_mapChfBillboard.clear();
	}

	boost::shared_ptr<BILLBOARD_CHF> pushBillboard( const TCHAR* pNameChf )
	{
		std::map<TSTRING,boost::shared_ptr<BILLBOARD_CHF>>::iterator iter = g_mapChfBillboard.find( pNameChf );
		if ( iter != g_mapChfBillboard.end() )
			return (*iter).second;

		const NSBillboardLua::BILLBOARD* pBillboard = NSBillboardLua::FindBillboard( pNameChf );
		if ( !pBillboard )
		{
			boost::shared_ptr<BILLBOARD_CHF> spTemp;
			return spTemp;
		}

		BILLBOARD_CHF_DATA sData;
		if ( !sData.Load( pBillboard->m_strBillboardData.c_str() ) )
		{
			boost::shared_ptr<BILLBOARD_CHF> spTemp;
			return spTemp;
		}

		boost::shared_ptr<BILLBOARD_CHF> spBillboard( new BILLBOARD_CHF( sData, pBillboard->m_strBillboardTexture ) );
		g_mapChfBillboard.insert( std::make_pair( pNameChf, spBillboard ) );

		return spBillboard;
	}
	//							pushBillboard
	//////////////////////////////////////////////////////////////////////////
};


//////////////////////////////////////////////////////////////////////////
//							NSBillboardLua
namespace NSBillboardLua
{
	TSTRING	g_strPath;
	TSTRING g_strFileName(_T("billboard.lua"));

	//		   CHF | Billboard
	std::map<TSTRING,BILLBOARD>	g_mapChfBillboard;

	void SetPath( const TCHAR* pName )
	{
		g_strPath = pName;
	}

	const TCHAR* GetPath()
	{
		return g_strPath.c_str();
	}

	//////////////////////////////////////////////////////////////////////////
	BOOL ParseCommentLua()
	{
		try
		{
			g_mapChfBillboard.clear();

			LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "BILLBOARD_LIST" ) );
			for ( LuaPlus::LuaTableIterator Iter(ob); Iter; Iter.Next() )
			{
				LuaPlus::LuaTableIterator IterA(Iter.GetValue());
				TSTRING	strNameChf( IterA.GetValue().GetString() );
				IterA.Next();

				BILLBOARD sBillboard;
				if ( IterA )
				{
					sBillboard.m_strBillboardData = IterA.GetValue().GetString();
					IterA.Next();
				}
				else
				{
					sBillboard.m_strBillboardData = sc::util::ChangeExtName( strNameChf, _T("blb") );
				}

				if ( IterA )
				{
					sBillboard.m_strBillboardTexture = IterA.GetValue().GetString();
				}
				else
				{
					sBillboard.m_strBillboardTexture = sc::util::GetName_ExtErase( sBillboard.m_strBillboardData );
					sBillboard.m_strBillboardTexture += _T("_blb.dds");
				}

				g_mapChfBillboard.insert( std::make_pair( strNameChf.c_str(), sBillboard ) );
			}
		}
		catch( const LuaPlus::LuaException& e )
		{        
			MessageBox( NULL, e.GetErrorMessage(), _T( "NSBillboardLua" ), MB_OK );
		}

		return TRUE;
	}


	//////////////////////////////////////////////////////////////////////////
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


	//////////////////////////////////////////////////////////////////////////
	BOOL LuaLoad()
	{
		if ( LuaLoad_RELEASED_DEBUG() )
			return TRUE;

		return FALSE;
	}


	//////////////////////////////////////////////////////////////////////////
	// Find
	const BILLBOARD* FindBillboard( const TCHAR* szNameChf )
	{
		std::map<TSTRING,BILLBOARD>::const_iterator iter = g_mapChfBillboard.find( szNameChf );
		if ( iter != g_mapChfBillboard.end() )
		{
			return &(*iter).second;
		}
		return NULL;
	}
};
