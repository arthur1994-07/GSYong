#include "pch.h"

#include "../../../=LuaPlus/src/LuaPlus/src/lua.h"
#include "../../../SigmaCore/Memory/SmartPtr.h"
#include "../../../SigmaCore/File/BaseStream.h"
#include "../../../SigmaCore/File/FileUtil.h"
#include "../../../SigmaCore/Lua/MinLua.h"

#include "../../DxTools/TextureManager.h"
#include "../../G-Logic/GLogic.h"

#include "../Single/DxEffSinglePropGMan.h"

#include "./NSEffectLuaPlay.h"

#include "NSEffectLua.h"


namespace NSEffectLua
{
	TSTRING g_strFileName(_T("effect.lua"));


	std::map<TSTRING,VEC_STEP_DATA>	g_mapEffects;


	//////////////////////////////////////////////////////////////////////////
	BOOL ParseCommentLua()
	{
		try
		{
			g_mapEffects.clear();

			LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "EFFECT_LIST" ) );
			for ( LuaPlus::LuaTableIterator Iter(ob); Iter; Iter.Next() )
			{
				TSTRING			strEffectName( Iter.GetKey().GetString() );
				VEC_STEP_DATA	vecspSteps;

				for ( LuaPlus::LuaTableIterator IterA(Iter.GetValue()); IterA; IterA.Next() )	// EFFECT
				{
					boost::shared_ptr<STEP_DATA> spStep = boost::shared_ptr<STEP_DATA>(new STEP_DATA);

					for ( LuaPlus::LuaTableIterator IterB(IterA.GetValue()); IterB; IterB.Next() )
					{
						std::string strKeyB( IterB.GetKey().GetString() );
						if ( strKeyB == "m_emEndCondition" )
						{
							spStep->m_emEndCondition = static_cast<EM_END_CONDITION>( IterB.GetValue().GetInteger() );
						}
						else if ( strKeyB == "m_bEgpInFrontOfEye" )
						{
							spStep->m_bEgpInFrontOfEye = IterB.GetValue().GetBoolean();
						}
						else if ( strKeyB == "m_strEgpFile" )
						{
							spStep->m_strEgpFile = IterB.GetValue().GetString();

							// 미리 로딩하도록 하자.
							DxEffSinglePropGMan::GetInstance().LoadEffectGProp( spStep->m_strEgpFile.c_str(), TRUE, TRUE );
						}
						else if ( strKeyB == "m_emScreenEffectType" )
						{
							spStep->m_emScreenEffectType = static_cast<EM_SCREENEFFECT>( IterB.GetValue().GetInteger() );
						}
						else if ( strKeyB == "m_strScreenEffectTexture" )
						{
							spStep->m_strScreenEffectTexture = IterB.GetValue().GetString();

							TextureManager::GetInstance().LoadTexture
							(
								spStep->m_strScreenEffectTexture.c_str(),
								false,
								TEXTURE_ADD_DATA::TEXUTRE_TYPE_2D,
								0,
								D3DFMT_UNKNOWN,
								TEXTURE_RESIZE_NONE,
								TEXTURE_EFFECT,
								true,
								TRUE
							);
						}
						else if ( strKeyB == "m_strScreenEffectShader" )
						{
							spStep->m_strScreenEffectShader = IterB.GetValue().GetString();
						}
						else if ( strKeyB == "m_fScreenEffectCreatingTime" )
						{
							spStep->m_fScreenEffectCreatingTime = IterB.GetValue().GetFloat();
						}
						else if ( strKeyB == "m_fScreenEffectDestroyingTime" )
						{
							spStep->m_fScreenEffectDestroyingTime = IterB.GetValue().GetFloat();
						}
						else if ( strKeyB == "m_fNextStepTime" )
						{
							spStep->m_fNextStepTime = IterB.GetValue().GetFloat();
						}
					}

					vecspSteps.push_back( spStep );
				}

				// 하나의 EFFECT 가 끝났다. 정리해주도록 하자.
				g_mapEffects.insert( std::make_pair( strEffectName, vecspSteps ) );
			}

			int a = 0;
		}
		catch( const LuaPlus::LuaException& e )
		{        
			MessageBox( NULL, e.GetErrorMessage(), _T( "NSEffectLua" ), MB_OK );
		}

		return TRUE;
	}



	//////////////////////////////////////////////////////////////////////////
	BOOL LuaLoadInZipFile()
	{
		// 파일 이름과 경로.
		TSTRING strFullPath( DxEffSinglePropGMan::GetInstance().GetPath() );
		//std::string strLucFile( sc::file::changeFileExt( g_strFileName, "luc" ) );	// 굳이 압축해야 할까~?
		strFullPath += g_strFileName;
		
		// 작업시작
		std::auto_ptr< sc::BaseStream > pStream
		( 
			GLOGIC::openfile_basestream
			(
				GLOGIC::bENGLIB_ZIPFILE, 
				GLOGIC::strEFFECT_ZIPFILE.c_str(),
				strFullPath.c_str(), 
				g_strFileName.c_str() 
			) 
		);

		if( !pStream.get() )		
		{
			sc::writeLogError
			(
				sc::string::format( "NSEffectLua::LuaLoad(), File Open %1%", strFullPath ) 
			);

			return FALSE;
		}

		const size_t nSize = static_cast< size_t >( pStream->GetFileSize() );
		void* pBuffer = ::malloc(nSize);
		pStream->ReadBuffer(pBuffer, 0);
		CTypeSmartPtr _pSmartPtr(pBuffer);

		try
		{
			sc::lua_destroyState();
			sc::lua_init();

			if( !sc::lua_doBuffer( static_cast< char* >( pBuffer ), nSize, NULL ) )
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
	BOOL LuaLoad_RELEASED_DEBUG()
	{
		// 파일 이름과 경로.
		TSTRING strFullPath( DxEffSinglePropGMan::GetInstance().GetPath() );
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
		NSEffectPlay::DeleteALL();

#if defined( _RELEASED ) || defined( _DEBUG )
		if ( LuaLoad_RELEASED_DEBUG() )
			return TRUE;
#else
		if ( LuaLoadInZipFile() )
			return TRUE;
#endif
		return FALSE;
	}



	//////////////////////////////////////////////////////////////////////////
	// Find
	const VEC_STEP_DATA* Find( const TCHAR* szName )
	{
		std::map<TSTRING,VEC_STEP_DATA>::const_iterator iter = g_mapEffects.find( szName );
		if ( iter != g_mapEffects.end() )
		{
			return &(*iter).second;
		}

		return NULL;
	}
};