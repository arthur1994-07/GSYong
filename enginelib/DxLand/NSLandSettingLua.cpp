#include "pch.h"

#include <boost/tr1/memory.hpp>

#include "../../=LuaPlus/src/LuaPlus/src/lua.h"
#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/Util/Util.h"
#include "../../SigmaCore/String/StringUtil.h"

#include "../G-Logic/GLogic.h"
#include "../DxTools/DxViewPort.h"

#include "./DxLandMan.h"

#include "NSLandSettingLua.h"

// ----------------------------------------------------------------------------
#include "../../SigmaCore/DebugInclude.h"
// ----------------------------------------------------------------------------

namespace NSLandSettingLua
{
	BOOL ParseCommentLua( LPDIRECT3DDEVICEQ pd3dDevice )
	{
		{
			LuaPlus::LuaObject ob = sc::lua_getGlobalFromName( _T( "GameCameraZoomMax" ) );
			if( ob.IsTable() )
			{
				LuaPlus::LuaTableIterator Iter(ob);
				if ( Iter )
				{
					DxViewPort::GetInstance().SetZoomOutMapSetting( Iter.GetValue().GetFloat() );
				}
			}
		}

		return TRUE;
	}

	BOOL LuaLoad( LPDIRECT3DDEVICEQ pd3dDevice, const std::string& FullPath, const std::string& _FileName )
	{
		std::auto_ptr< sc::BaseStream > pStream
		(
			GLOGIC::openfile_basestream
			(
				FALSE,
				TSTRING(""),
				FullPath.c_str(), 
				_FileName.c_str()
			) 
		);

		if( !pStream.get() )		
		{
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
				MessageBox( NULL, sc::lua_getError().c_str(), FullPath.c_str(), MB_OK );

				return FALSE;
			}

			return ParseCommentLua( pd3dDevice );
		}
		catch( const LuaPlus::LuaException& e )
		{        
			MessageBox( NULL, e.GetErrorMessage(), FullPath.c_str(), MB_OK );

			return FALSE;
		}

		return FALSE;
	}

	void Load( LPDIRECT3DDEVICEQ pd3dDevice, const char* szName )
	{
		std::string strFullName = sc::util::ChangeExtName( szName, _T("set") );

		std::string strPath, strName, strExt;
		sc::string::getFilePathAndName(strFullName, strPath, strName, strExt);

		// 
		LuaLoad( pd3dDevice, strFullName, strName );
	}
};