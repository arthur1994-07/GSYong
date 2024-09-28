#include "pch.h"

#include "../../=LuaPlus/src/LuaPlus/src/lua.h"

#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/Math/Random.h"
#include "../../SigmaCore/Util/SystemInfo.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/Memory/SmartPtr.h"

#include "../../EngineLib/G-Logic/GLogic.h"

#include "../../RanLogic/GLogicDataMan.h"

#include "NSInitClientLua.h"


namespace NSInitClientLua
{
	TSTRING g_strFileName(_T("InitClient.lua"));

	BOOL					g_bLobbyLoadingImageFullSize(FALSE);
	std::vector<TSTRING>	g_arrstrLobbyLoadingImage;				// 로비 로딩 이미지

	BOOL					g_bBattleIslandsWarLoadingImageOn(FALSE);	// 맵을 들어오고, 나갈 때 둘다 적용을 위해 값을 저장함.
	std::set<DWORD>			g_setBattleIslandsWarLoadingImageOn;		// rm #2858 - [GS] 난사군도 우승자 국기 + 문구 추가 요청

	const TCHAR* GetRandomLobbyLoadingImage()
	{
		if ( g_arrstrLobbyLoadingImage.empty() )
		{
			g_arrstrLobbyLoadingImage.push_back( _T("loading_000.dds") );	// 아무것도 없으면 디폴트값이 사용된다.
			return g_arrstrLobbyLoadingImage[0].c_str();
		}

		size_t dwIndex = sc::Random::getInstance().GetValue( 0, g_arrstrLobbyLoadingImage.size()-1 );
		if ( dwIndex < g_arrstrLobbyLoadingImage.size() )
		{
			return g_arrstrLobbyLoadingImage[dwIndex].c_str();
		}
		else
		{
			return g_arrstrLobbyLoadingImage[0].c_str();
		}
	}

	BOOL GetLobbyLoadingImageFullSize()
	{
		return g_bLobbyLoadingImageFullSize;
	}

	// 로딩이미지에 난사군도 승리국가가 표시 되야 하는가~?
	BOOL IsBattleIslandsWarLoadingImageOn( DWORD dwMapID )
	{
		BOOL bPrev = g_bBattleIslandsWarLoadingImageOn;
		g_bBattleIslandsWarLoadingImageOn = g_setBattleIslandsWarLoadingImageOn.find( dwMapID ) == g_setBattleIslandsWarLoadingImageOn.end() ? FALSE : TRUE;

		if ( bPrev || g_bBattleIslandsWarLoadingImageOn )
			return TRUE;
		else
			return FALSE;
	}

	bool ParseCommentLua()
	{
		try
		{
			// 로비 로딩이미지 FullSize 로 보이게 할 것인가~?
			{
				LuaPlus::LuaObject ob(sc::lua_getGlobalFromName( _T( "LobbyLoadingImageFullSize" ) ));
				if ( ob.IsNil() )
				{
					g_bLobbyLoadingImageFullSize = FALSE;
				}
				else
				{
					g_bLobbyLoadingImageFullSize = ob.GetBoolean() ? TRUE : FALSE;
				}
			}

			// 로비 로딩이미지 랜덤하게 보이게 할 텍스쳐들은~?
			{
				LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "LobbyLoadingImage" ) );
				if( ob.IsTable() )
				{
					for (LuaPlus::LuaTableIterator Iter(ob); Iter; Iter.Next())
					{
						g_arrstrLobbyLoadingImage.push_back( Iter.GetValue().GetString() );
					}
				}
			}

			// 로비 로딩이미지 랜덤하게 보이게 할 텍스쳐들은~?
			{
				LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "BattleIslandsWarLoadingImage" ) );
				if( ob.IsTable() )
				{
					for (LuaPlus::LuaTableIterator Iter(ob); Iter; Iter.Next())
					{
						LuaPlus::LuaTableIterator IterB(Iter.GetValue());
						if ( IterB )
						{
							SNATIVEID sMapID;
							sMapID.wMainID = IterB.GetValue().GetInteger();
							IterB.Next();
							sMapID.wSubID = IterB.GetValue().GetInteger();

							g_setBattleIslandsWarLoadingImageOn.insert( sMapID.dwID );
						}
					}
				}
			}
		}
		catch( const LuaPlus::LuaException& e )
		{        
			MessageBox( NULL, e.GetErrorMessage(), _T( "LobbyLoadingImage" ), MB_OK );
		}

		return true;
	}

	HRESULT LuaLoad()
	{
		// 초기화
		g_bLobbyLoadingImageFullSize = FALSE;
		g_arrstrLobbyLoadingImage.clear();

		// 파일 이름과 경로.
		TSTRING strFullPath(GLOGIC::GetPath());
		strFullPath += g_strFileName;

		// 작업시작
		std::auto_ptr< sc::BaseStream > pStream
		(
			GLOGIC::openfile_basestream
			(
				GLOGIC::UseLogicZipFile(), 
				GLogicData::GetInstance().GetGlogicZipFile(), 
				strFullPath.c_str(), 
				g_strFileName.c_str(),
				false,
				GLOGIC::UseLogicPackFile()
			)
		);

		if( !pStream.get() )		
		{
			sc::writeLogError(
				sc::string::format(
				"InitClient::LuaLoad(), File Open %1%", strFullPath ) );

			return false;
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
				MessageBox( NULL, sc::lua_getError().c_str(), strFullPath.c_str(), MB_OK );

				return false;
			}

			return ParseCommentLua();
		}
		catch( const LuaPlus::LuaException& e )
		{        
			MessageBox( NULL, e.GetErrorMessage(), strFullPath.c_str(), MB_OK );

			return false;
		}

		return false;
	}

};