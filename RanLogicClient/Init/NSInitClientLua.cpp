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
	std::vector<TSTRING>	g_arrstrLobbyLoadingImage;				// �κ� �ε� �̹���

	BOOL					g_bBattleIslandsWarLoadingImageOn(FALSE);	// ���� ������, ���� �� �Ѵ� ������ ���� ���� ������.
	std::set<DWORD>			g_setBattleIslandsWarLoadingImageOn;		// rm #2858 - [GS] ���籺�� ����� ���� + ���� �߰� ��û

	const TCHAR* GetRandomLobbyLoadingImage()
	{
		if ( g_arrstrLobbyLoadingImage.empty() )
		{
			g_arrstrLobbyLoadingImage.push_back( _T("loading_000.dds") );	// �ƹ��͵� ������ ����Ʈ���� ���ȴ�.
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

	// �ε��̹����� ���籺�� �¸������� ǥ�� �Ǿ� �ϴ°�~?
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
			// �κ� �ε��̹��� FullSize �� ���̰� �� ���ΰ�~?
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

			// �κ� �ε��̹��� �����ϰ� ���̰� �� �ؽ��ĵ���~?
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

			// �κ� �ε��̹��� �����ϰ� ���̰� �� �ؽ��ĵ���~?
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
		// �ʱ�ȭ
		g_bLobbyLoadingImageFullSize = FALSE;
		g_arrstrLobbyLoadingImage.clear();

		// ���� �̸��� ���.
		TSTRING strFullPath(GLOGIC::GetPath());
		strFullPath += g_strFileName;

		// �۾�����
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