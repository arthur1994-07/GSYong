#include "pch.h"

#include "../../SigmaCore/Lua/MinLua.h"
#include "../../SigmaCore/Memory/SmartPtr.h"
#include "../../SigmaCore/Log/LogMan.h"
#include "../../SigmaCore/String/StringFormat.h"
#include "../../SigmaCore/String/StringUtil.h"
#include "../../SigmaCore/File/BaseStream.h"
#include "../../SigmaCore/Util/MinMessageBox.h"
#include "../../SigmaCore/Math/Random.h"

#include "../../EngineLib/G-Logic/GLOGIC.h"

#include "../GLogicFile.h"
#include "../GLogicDataMan.h"
#include "GLTexasHoldemSetting.h"

namespace TexasHoldem
{

	GLTexasHoldemSetting::GLTexasHoldemSetting(void)
	: m_strFilePath("")
	, m_wRequireLevel(0)
	, m_wPointTransferItemMid( 501 )
	, m_wPointTransferItemSid( 1 )
	, m_strAddress( "http://casinox_admin.mincoms.com/RanMiniHoldem/index.htm" )
	, m_fCommision( 0.0f )
	, m_bUseChipPurchaseInGameMoney( true )
	, m_bUseChipPurchaseInPoint( true )
	{}

	GLTexasHoldemSetting::~GLTexasHoldemSetting(void)
	{}

	GLTexasHoldemSetting* const GLTexasHoldemSetting::getInstance(void)
	{
		static GLTexasHoldemSetting instance;
		return &instance;
	}

	namespace GLTexasHoldem
	{
		const bool LoadFile( const std::string FileName )
		{
			GLTexasHoldemSetting* pTexasHoldemSetting = GLTexasHoldemSetting::getInstance();
			if( !pTexasHoldemSetting )
				return false;

			pTexasHoldemSetting->m_strFilePath = FileName;

			sc::lua_init();
			if ( !sc::lua_doFile( FileName ) )
			{
				std::string strErrMsg(
					sc::string::format(
					"%1% file missing.(error 5045)",
					FileName));
				sc::ErrorMessageBox( strErrMsg, false );
				return false;
			}

			pTexasHoldemSetting->load();

			return true;
		}
	}

	const bool GLTexasHoldemSetting::load()
	{
		LuaPlus::LuaObject MiniGame_TexasHoldem = sc::lua_getGlobalFromName( "MiniGame_TexasHoldem" );
		for (LuaPlus::LuaTableIterator Iter( MiniGame_TexasHoldem ); Iter; Iter.Next() )
		{
			std::string strKey( ( const char* ) Iter.GetKey().GetString() );

			if ( strKey == "nRequire_Level" )
			{
				m_wRequireLevel = Iter.GetValue().GetInteger();
			}
			else if ( strKey == "strAddress" )
			{
				std::wstring wstrAddress( ( const wchar_t* ) Iter.GetValue().GetWString() );
				m_strAddress = sc::string::unicodeToAnsi( wstrAddress );
			}
			else if ( strKey == "fCommission" )
			{
				m_fCommision = Iter.GetValue().GetFloat();
			}
			else if ( strKey == "bUsePurchaseChipInGameMoney" )
			{
				m_bUseChipPurchaseInGameMoney = Iter.GetValue().GetBoolean();
			}
			else if ( strKey == "bUsePurchaseChipInPoint" )
			{
				m_bUseChipPurchaseInPoint = Iter.GetValue().GetBoolean();
			}
		}
		return true;
	}

}
