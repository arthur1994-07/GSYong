#include "stdafx.h"
#include "../String/StringUtil.h"
#include "./MinLua.h"

// ----------------------------------------------------------------------------
#include "../debuginclude.h"
// ----------------------------------------------------------------------------

namespace sc
{

//////////////////////////////////////////////////////////////////////////////////////
// 전역 루아는 최종적으로 제거되는것이 좋다;

    LuaPlus::LuaState* g_pLua = NULL;
	std::string lua_strError = "";

	void lua_init()
	{
		// 값이 있다면 미리 삭제해 줘야 한다.
		// 원래는 사용자가 lua_init 후 lua_destroyState 를 호출 잘 해줘야 하는데,
		// 누락된 부분이 많은거 같아서, 이렇게 강제로 해버림.
		if (g_pLua)
		{
			lua_destroyState();
		}

		if (!g_pLua)
		{
			g_pLua = LuaPlus::LuaState::Create();
			lua_State* L = g_pLua->GetCState();
			// Lua 기본 함수들을 로드한다.			
			luaopen_base(L);
			luaopen_table(L);
			//luaopen_io(L);
			luaopen_os(L);
			luaopen_string(L);
			luaopen_math(L);
			luaopen_debug(L);
			//luaopen_package(L);
			//luaL_openlibs(L);
		}
	}

	LuaPlus::LuaState* lua_getState()
	{
		if (!g_pLua)
			lua_init();
		return g_pLua;
	}

	LuaPlus::LuaState* lua_getThreadState()
	{		
		LuaPlus::LuaObject LO = LuaPlus::LuaState::CreateThread( lua_getState() );
		return LO.GetState();
	}

	void lua_destroyState()
	{
		if (g_pLua)
		{
			LuaPlus::LuaState::Destroy(g_pLua);
			g_pLua = NULL;
		}
	}

	// 가비지 컬렉션으로 자동 처리된다
	/*
	void lua_destroyThreadState( LuaPlus::LuaState*& pState )
	{
		if ( NULL != pState )
		{
			LuaPlus::LuaState::Destroy( pState );
			pState = NULL;
		}
	}
	*/

	LuaPlus::LuaObject lua_getGlobalFromName( const char* szName )
	{
		return g_pLua->GetGlobals()[szName];
	}

    bool lua_doFile(const std::wstring& filename, LuaPlus::LuaState* pState)
    {
        return lua_doFile(filename.c_str(), pState);
    }

	bool lua_doFile(const wchar_t *filename, LuaPlus::LuaState* pState)
	{
        std::string strFileName = sc::string::unicodeToAnsi( filename, (int) wcslen(filename) );
		return lua_doFile( strFileName.c_str(), pState );
	}

    bool lua_doFile(const std::string& filename, LuaPlus::LuaState* pState)
    {
        return lua_doFile(filename.c_str(), pState);
    }

	bool lua_doFile(const char *filename, LuaPlus::LuaState* pState)
	{
		if (!pState)
			pState = g_pLua;

		if (0 != pState->DoFile(filename))
		{
			lua_strError = LuaPlus::LuaStackObject(pState, -1).GetString();
			return false;
		}
		else
		{
			return true;
		}
	}

	bool lua_doBuffer(const char* buff, size_t size, const char* name, LuaPlus::LuaState* pState)
	{
		if (!pState)
			pState = g_pLua;

		if (0 != pState->DoBuffer(buff, size, name))
		{
			lua_strError = LuaPlus::LuaStackObject(pState, -1).GetString();
			return false;
		}
		else
		{
			return true;
		}
	}

	std::string lua_getError()
	{
		return lua_strError;
	}


	void PrintVStack ( lua_State* pState )
	{
		sc::writeLogInfo( "----- Lua VStack ----- " );

		int nTop = lua_gettop( pState );
		for ( int i=nTop; i>=1; --i )
		{
			switch ( lua_type( pState, i ) )
			{
			case LUA_TNIL:
				sc::writeLogInfo( sc::string::format(
					"%02d : nil", i ) );
				break;

			case LUA_TBOOLEAN:
				sc::writeLogInfo( sc::string::format(
					"%02d : boolean | %d",
					i, static_cast< int >( lua_toboolean( pState, i ) ) ) );
				break;

			case LUA_TNUMBER:
				sc::writeLogInfo( sc::string::format(
					"%02d : number | %f",
					i, lua_tonumber( pState, i ) ) );
				break;

			case LUA_TSTRING:
				sc::writeLogInfo( sc::string::format(
					"%02d : string | %s",
					i, sc::GetStringToLuaState( pState, i ) ) );
				break;

			case LUA_TTABLE:
				sc::writeLogInfo( sc::string::format(
					"%02d : table | 0x%08x",
					i, ( int )lua_topointer( pState, i ) ) );
				break;

			case LUA_TFUNCTION:
				sc::writeLogInfo( sc::string::format(
					"%02d : function | 0x%08x",
					i, ( int )lua_topointer( pState, i ) ) );
				break;

			case LUA_TUSERDATA:
				sc::writeLogInfo( sc::string::format(
					"%02d : userdata | 0x%08x",
					i, ( int )lua_topointer( pState, i ) ) );
				break;

			case LUA_TTHREAD:
				sc::writeLogInfo( sc::string::format(
					"%02d : thread | 0x%08x",
					i, ( int )lua_topointer( pState, i ) ) );
				break;

			default:
				sc::writeLogInfo( sc::string::format(
					"%02d : unknown", i ) );
				break;
			}
		}

		sc::writeLogInfo( "----------------------- " );
	}


	int OnAlert ( lua_State* pState )
	{
		//  [8/30/2016 khkim] GS 서버 crash 관련 수정 lua_tostring 가 null 이기 때문에 sc::string::format 에서 서버가 죽음
		std::string strErrorMessage = sc::GetStringToLuaState( pState, -1 );

		sc::writeLogError ( sc::string::format( "[ Lua ] [ %1% ]", strErrorMessage ) );

		PrintVStack( pState );

		return 0;
	}


	const bool GetBooleanByName ( LuaPlus::LuaObject& _luaObject, const char* _szName )
	{
		if ( _luaObject.IsNil() )
		{
			GASSERT( 0 && "Lua Global Object 가 없습니다." );
		}

		LuaPlus::LuaObject luaObjectMember = _luaObject.GetByName( _szName );
		if ( luaObjectMember.IsNil() )
		{
			GASSERT( 0 && "Lua Member 가 없습니다." );

			return false;
		}
		else if ( luaObjectMember.IsBoolean() )
		{
			return luaObjectMember.GetBoolean();
		}

		return false;
	}

	const int GetIntegerByName ( LuaPlus::LuaObject& _luaObject, const char* _szName )
	{
		if ( _luaObject.IsNil() )
		{
			GASSERT( 0 && "Lua Global Object 가 없습니다." );
		}

		LuaPlus::LuaObject luaObjectMember = _luaObject.GetByName( _szName );
		if ( luaObjectMember.IsNil() )
		{
			GASSERT( 0 && "Lua Member 가 없습니다." );

			return 0;
		}
		else if ( luaObjectMember.IsInteger() )
		{
			return luaObjectMember.GetInteger();
		}

		return 0;
	}

	const float GetNumberByName ( LuaPlus::LuaObject& _luaObject, const char* _szName )
	{
		if ( _luaObject.IsNil() )
		{
			GASSERT( 0 && "Lua Global Object 가 없습니다." );
		}

		LuaPlus::LuaObject luaObjectMember = _luaObject.GetByName( _szName );
		if ( luaObjectMember.IsNil() )
		{
			GASSERT( 0 && "Lua Member 가 없습니다." );

			return 0.f;
		}
		else if ( luaObjectMember.IsNumber() )
		{
			return luaObjectMember.GetNumber();
		}

		return 0.f;
	}

	const char* GetStringByName ( LuaPlus::LuaObject& _luaObject, const char* _szName )
	{
		static std::string strDefault;

		if ( _luaObject.IsNil() )
		{
			GASSERT( 0 && "Lua Global Object 가 없습니다." );
		}

		LuaPlus::LuaObject luaObjectMember = _luaObject.GetByName( _szName );
		if ( luaObjectMember.IsNil() )
		{
			GASSERT( 0 && "Lua Member 가 없습니다." );

			return strDefault.c_str();
		}
		else if ( luaObjectMember.IsString() )
		{
			return luaObjectMember.GetString();
		}

		return strDefault.c_str();
	}


	const bool GetBooleanByObject ( LuaPlus::LuaObject& _luaObject )
	{
		if ( _luaObject.IsNil() )
		{
			GASSERT( 0 && "Lua Global Object 가 없습니다." );

			return false;
		}
		else if ( _luaObject.IsBoolean() )
		{
			return _luaObject.GetBoolean();
		}

		return false;
	}

	const int GetIntegerByObject ( LuaPlus::LuaObject& _luaObject )
	{
		if ( _luaObject.IsNil() )
		{
			GASSERT( 0 && "Lua Global Object 가 없습니다." );

			return 0;
		}
		else if ( _luaObject.IsInteger() )
		{
			return _luaObject.GetInteger();
		}

		return 0;
	}

	const float GetNumberByObject ( LuaPlus::LuaObject& _luaObject )
	{
		if ( _luaObject.IsNil() )
		{
			GASSERT( 0 && "Lua Global Object 가 없습니다." );

			return 0.f;
		}
		else if ( _luaObject.IsNumber() )
		{
			return _luaObject.GetNumber();
		}

		return 0.f;
	}

	const char* GetStringByObject ( LuaPlus::LuaObject& _luaObject )
	{
		static std::string strDefault;

		if ( _luaObject.IsNil() )
		{
			GASSERT( 0 && "Lua Global Object 가 없습니다." );

			return strDefault.c_str();
		}
		else if ( _luaObject.IsString() )
		{
			return _luaObject.GetString();
		}

		return strDefault.c_str();
	}


	const bool GetBooleanToLuaState ( lua_State* _pLuaState, int _nStack )
	{
		if ( NULL == _pLuaState )
		{
			sc::writeLogError( "[ Lua Log ] [ Failed GetBooleanToLuaState, The LuaState is NULL. ]" );
			return false;
		}

		if ( false == lua_isboolean( _pLuaState, _nStack ) )
		{
			sc::writeLogError( "[ Lua Log ] [ Failed GetBooleanToLuaState, The LuaState's Stack Value is not Boolean Data. ]" );
			return false;
		}

		return lua_toboolean( _pLuaState, _nStack );
	}

	const int GetIntegerToLuaState ( lua_State* _pLuaState, int _nStack )
	{
		if ( NULL == _pLuaState )
		{
			sc::writeLogError( "[ Lua Log ] [ Failed GetIntegerToLuaState, The LuaState is NULL. ]" );
			return 0;
		}

		if ( false == lua_isnumber( _pLuaState, _nStack ) )
		{
			sc::writeLogError( "[ Lua Log ] [ Failed GetIntegerToLuaState, The LuaState's Stack Value is not Integer Data. ]" );
			return 0;
		}

		return lua_tointeger( _pLuaState, _nStack );
	}

	const float GetNumberToLuaState ( lua_State* _pLuaState, int _nStack )
	{
		if ( NULL == _pLuaState )
		{
			sc::writeLogError( "[ Lua Log ] [ Failed GetNumberToLuaState, The LuaState is NULL. ]" );
			return 0.f;
		}

		if ( false == lua_isnumber( _pLuaState, _nStack ) )
		{
			sc::writeLogError( "[ Lua Log ] [ Failed GetNumberToLuaState, The LuaState's Stack Value is not Number Data. ]" );
			return 0.f;
		}

		return lua_tonumber( _pLuaState, _nStack );
	}

	const char* GetStringToLuaState ( lua_State* _pLuaState, int _nStack )
	{
		static std::string strDefault;

		if ( NULL == _pLuaState )
		{
			sc::writeLogError( "[ Lua Log ] [ Failed GetStringToLuaState, The LuaState is NULL. ]" );
			return strDefault.c_str();
		}

		if ( false == lua_isstring( _pLuaState, _nStack ) )
		{
			sc::writeLogError( "[ Lua Log ] [ Failed GetStringToLuaState, The LuaState's Stack Value is not String Data. ]" );
			return strDefault.c_str();
		}

		return lua_tostring( _pLuaState, _nStack );
	}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////


	const int LuaInstance::INVALID_POP_INTEGER = 0xffffffff;
	const float LuaInstance::INVAID_POP_FLOAT = 0xffffffff;

	LuaInstance::LuaInstance ( void )
		: m_pLuaPlus ( LuaPlus::LuaState::Create() )
		, m_dwVStackCount ( 0 )
	{
		m_pLua = m_pLuaPlus->GetCState();

		luaopen_base( m_pLua );
		luaopen_table( m_pLua );
		luaopen_os( m_pLua );
		luaopen_string( m_pLua );
		luaopen_math( m_pLua );
		luaopen_debug( m_pLua );

		lua_register( m_pLua, "_ALERT", OnAlert );
	}

	LuaInstance::~LuaInstance ( void )
	{		
	}

	void LuaInstance::OnDestroy ( void )
	{
		if ( NULL == m_pLuaPlus )
			return;

		PrintVStack();

		LuaPlus::LuaState::Destroy( m_pLuaPlus );
		m_pLuaPlus = NULL;
		m_pLua = NULL;

		m_dwVStackCount = 0;
	}

	const int LuaInstance::DoFile ( const std::string& _fileName )
	{
		if ( 0 == m_pLuaPlus->DoFile( _fileName.c_str() ) )
			return 0;

		std::string strErrorMessage = 
			LuaPlus::LuaStackObject( m_pLuaPlus, -1 ).GetString();
		sc::ErrorMessageBox( strErrorMessage, false );

		return -1;
	}

	const int LuaInstance::DoBuffer (
		const char* _pBuffer,
		const unsigned int _nSize,
		const char* const _cName )
	{
		if ( 0 == m_pLuaPlus->DoBuffer( _pBuffer, _nSize, _cName ) )
			return 0;

		std::string strErrorMessage = 
			LuaPlus::LuaStackObject( m_pLuaPlus, -1 ).GetString();
		sc::ErrorMessageBox( strErrorMessage, false );

		return -1;
	}

	const int LuaInstance::DoString ( const char* const _pString ) const
	{
		if ( 0 == m_pLuaPlus->DoString( _pString ) )
			return 0;

		std::string strErrorMessage = 
			LuaPlus::LuaStackObject( m_pLuaPlus, -1 ).GetString();
		sc::ErrorMessageBox( strErrorMessage, false );

		return -1;
	}

	void LuaInstance::PrintVStack ( bool _bDetail ) const
	{
		if ( _bDetail )
			sc::writeLogInfo( "----- Lua VStack ----- " );

		int nTop = lua_gettop( m_pLua );

		sc::writeLogInfo( sc::string::format(
			"[ Lua Log ] [ VStack Size : %1% / VStack Real Size : %2% ]", nTop, m_dwVStackCount ) );

		/// 더 자세히 보고 싶을 때 호출;
		if ( _bDetail )
		{
			for ( int i=nTop; i>=1; --i )
			{
				switch ( lua_type( m_pLua, i ) )
				{
				case LUA_TNIL:
					sc::writeLogInfo( sc::string::format(
						"%02d : nil", i ) );
					break;

				case LUA_TBOOLEAN:
					sc::writeLogInfo( sc::string::format(
						"%02d : boolean | %d",
						i, static_cast< int >( lua_toboolean( m_pLua, i ) ) ) );
					break;

				case LUA_TNUMBER:
					sc::writeLogInfo( sc::string::format(
						"%02d : number | %f",
						i, lua_tonumber( m_pLua, i ) ) );
					break;

				case LUA_TSTRING:
					sc::writeLogInfo( sc::string::format(
						"%02d : string | %s",
						i, sc::GetStringToLuaState( m_pLua, i ) ) );
					break;

				case LUA_TTABLE:
					sc::writeLogInfo( sc::string::format(
						"%02d : table | 0x%08x",
						i, ( int )lua_topointer( m_pLua, i ) ) );
					break;

				case LUA_TFUNCTION:
					sc::writeLogInfo( sc::string::format(
						"%02d : function | 0x%08x",
						i, ( int )lua_topointer( m_pLua, i ) ) );
					break;

				case LUA_TUSERDATA:
					sc::writeLogInfo( sc::string::format(
						"%02d : userdata | 0x%08x",
						i, ( int )lua_topointer( m_pLua, i ) ) );
					break;

				case LUA_TTHREAD:
					sc::writeLogInfo( sc::string::format(
						"%02d : thread | 0x%08x",
						i, ( int )lua_topointer( m_pLua, i ) ) );
					break;

				default:
					sc::writeLogInfo( sc::string::format(
						"%02d : unknown", i ) );
					break;
				}
			}

			sc::writeLogInfo( "----------------------- " );
		}
	}

	void LuaInstance::RegistFunction (
		const char* const _name,
		lua_CFunction _function ) const
	{
		lua_register( m_pLua, _name, _function );
	}

	/* 
	 * 루아 쪽 함수를 호출하기전 해당 함수가 정의 되어 있는지 검사;
	 * 정의 되어 있지 않은 함수 호출의 경우 0을 반환;
	 * 다중 반환의 경우 스택이 꼬일 수 있으므로;
	 * 반드시 함수 호출 전 해당 함수가 정의 되어 있는지 확인;
	 */
	const bool LuaInstance::IsExist ( const char* const _name ) const
	{
		const LuaPlus::LuaObject luaObject( m_pLuaPlus->GetGlobals()[ _name ] );
		if ( luaObject.IsNil() == true )
			return false;

		return true;
	}

	const bool LuaInstance::IsTable ( const char* const _name ) const
	{
		const LuaPlus::LuaObject luaObject( m_pLuaPlus->GetGlobals()[ _name ] );
		if ( luaObject.IsNil() == true )
			return false;

		if ( luaObject.IsTable() )
			return true;

		return false;
	}

	const bool LuaInstance::IsTableObject ( LuaPlus::LuaObject& _luaObject ) const
	{
		if ( _luaObject.IsNil() == true )
			return false;

		if ( _luaObject.IsTable() )
			return true;

		return false;
	}

	void LuaInstance::PushInteger ( const int _nArgv )
	{
		lua_pushinteger( m_pLua, _nArgv );

		m_dwVStackCount++;
	}

	void LuaInstance::PushNumber ( const float _fArgv )
	{
		lua_pushnumber( m_pLua, _fArgv );

		m_dwVStackCount++;
	}

	void LuaInstance::PushBoolean ( const bool _bArgv )
	{
		lua_pushboolean( m_pLua, _bArgv ? 1 : 0 );

		m_dwVStackCount++;
	}

	void LuaInstance::PushFunction ( const char* const _name )
	{
		lua_getglobal( m_pLua, _name );

		m_dwVStackCount++;
	}

	const bool LuaInstance::PushFunctionEx ( const char* const _name )
	{
		if ( false == IsExist( _name ) )
			return false;

		PushFunction( _name );

		return true;
	}


	const int LuaInstance::PopInteger ( void )
	{
		// VStack 이 비어있을 경우 실패한다;
		if ( m_dwVStackCount <= 0 )
		{
			sc::writeLogDebug( "Failed PopInteger!!" );

			return INVALID_POP_INTEGER;
		}

		m_dwVStackCount--;

		// 유효성 검사;
		if ( !lua_isnumber( m_pLua, -1 ) )
		{
			lua_pop( m_pLua, 1 );

			return INVALID_POP_INTEGER;
		}

		// Pop;
		const int nResult = lua_tointeger( m_pLua, -1 );
		lua_pop( m_pLua, 1 );

		return nResult;
	}

	const float LuaInstance::PopNumber ( void )
	{
		// VStack 이 비어있을 경우 실패한다;
		if ( m_dwVStackCount <= 0 )
		{
			sc::writeLogDebug( "Failed PopNumber!!" );

			return INVALID_POP_INTEGER;
		}

		m_dwVStackCount--;

		// 유효성 검사;
		if ( !lua_isnumber( m_pLua, -1 ) )
		{
			lua_pop( m_pLua, 1 );

			return INVAID_POP_FLOAT;
		}

		// 루아 내부의 모든 값은 double로 처리 되고 있다;
		const float fResult = static_cast< float >( lua_tonumber( m_pLua, -1 ) );
		lua_pop( m_pLua, 1 );

		return fResult;
	}

	const bool LuaInstance::PopBoolean ( void )
	{
		// VStack 이 비어있을 경우 실패한다;
		if ( m_dwVStackCount <= 0 )
		{
			sc::writeLogDebug( "Failed PopBoolean!!" );

			return INVALID_POP_INTEGER;
		}

		m_dwVStackCount--;

		// 유효성 검사;
		if ( !lua_isboolean( m_pLua, -1 ) )
		{
			lua_pop( m_pLua, 1 );

			return false;
		}

		const bool bResult = lua_toboolean( m_pLua, -1 ) ? true : false;
		lua_pop( m_pLua, 1 );

		return bResult;
	}

	const bool LuaInstance::CallFunction (
		const unsigned int _nArgv,
		const unsigned int _nResults )
	{
		if ( _nArgv > m_dwVStackCount )
		{
			sc::writeLogError( "CallFunctionError : Invalid VStack State." );

			return false;
		}

		// VStack 이 _nArgv 에 1을 더한 것 만큼 감소한다;
		// 즉, _nArgv + 1 만큼 감소시킨다 ( +1 은 함수이름도 VStack 에 쌓이기 때문이다 );
		// 이는 lua_pcall 이 실패하더라도 유효하다;
		m_dwVStackCount -= ( _nArgv + 1 );

		int nError = lua_pcall( m_pLua, _nArgv, _nResults, 0 );
		if ( 0 != nError )
		{
			//  [8/30/2016 khkim] GS 서버 crash 관련 수정 lua_tostring 가 null 이기 때문에 sc::string::format 에서 서버가 죽음
			sc::writeLogError( sc::string::format( "CallFunctionError : %1%", sc::GetStringToLuaState( m_pLua, -1 ) ) );

			// 생성된 Error Message 를 제거한다;
			lua_pop( m_pLua, 1 );

			// Call Function Error 발생시의 Virtual Stack 을 확인한다;
			PrintVStack();

			return false;
		}

		// VStack 에 무조건 ( null 이라도 ) _nResults 만큼의 값이 쌓이게 된다;
		// 즉, _nResults 만큼의 VStackCount 가 증가한다;
		m_dwVStackCount += _nResults;

		return true;
	}


	lua_State* LuaInstance::CallFunctionCoroutine (
		const char* const _name ) const
	{
		if ( false == IsExist( _name ) )
			return NULL;

		lua_State* pState = lua_newthread( m_pLua );
		lua_getglobal( pState, _name );

		return pState;
	}

	int LuaInstance::CoroutineResume ( int _nArgv ) const
	{
		int nResult = lua_resume( m_pLua, _nArgv );
		if ( nResult )
		{
			sc::writeLogError( sc::GetStringToLuaState( m_pLua,-1 ) );
			lua_pop( m_pLua, 1 );
		}

		return nResult;
	}

	int LuaInstance::CoroutineResume ( lua_State* _pState, int _nArgv ) const
	{
		int nResult = lua_resume( _pState, _nArgv );
		if ( LUA_YIELD != nResult )
		{
			if ( LUA_ERRRUN == nResult )
			{
				sc::writeLogError( sc::GetStringToLuaState( _pState, -1 ) );
				lua_pop( _pState, 1 );
			}
		}

		return nResult;
	}

	int LuaInstance::CoroutineYield ( int _nResults ) const
	{
		return lua_yield( m_pLua, _nResults );
	}

	int LuaInstance::CoroutineYield ( lua_State* _pState, int _nResults ) const
	{
		return lua_yield( _pState, _nResults );
	}


	const LuaPlus::LuaObject LuaInstance::GetGlobalValue_Lua ( const char* _szName ) const
	{
		return m_pLuaPlus->GetGlobals()[ _szName ];
	}


	const bool LuaInstance::GetStringGlobalValue_Lua ( const char* _szName, std::string& _refStrValue ) const
	{
		LuaPlus::LuaObject luaObject = GetGlobalValue_Lua( _szName );

		if ( false == GetStringGlobalValueObject_Lua( luaObject, _refStrValue ) )
		{
			sc::writeLogError ( sc::string::format( 
				"'%1%' Global Value's Type is Invalid.",
				_szName ) );

			return false;
		}
		
		return true;
	}

	const bool LuaInstance::GetStringGlobalValueObject_Lua ( LuaPlus::LuaObject& luaObject, std::string& _refStrValue ) const
	{
		if ( luaObject.IsNil() )
		{
			return false;
		}

		if ( luaObject.IsWString() )
		{
			const std::wstring wStrData(
				( const wchar_t* )( luaObject.GetWString() ) );
			_refStrValue = sc::string::unicodeToAnsi( wStrData );

			return true;
		}

		if ( luaObject.IsString() )
		{
			_refStrValue = luaObject.GetString();

			return true;
		}

		sc::writeLogError ( "Global Value's Type is Invalid." );

		return false;
	}

	const bool LuaInstance::GetIntegerGlobalValue_Lua ( const char* _szName, int& _refIntValue ) const
	{
		LuaPlus::LuaObject luaObject = GetGlobalValue_Lua( _szName );

		if ( false == GetIntegerGlobalValueObject_Lua( luaObject, _refIntValue ) )
		{
			sc::writeLogError ( sc::string::format( 
				"'%1%' Global Value's Type is Invalid.",
				_szName ) );

			return false;
		}

		return true;
	}

	const bool LuaInstance::GetIntegerGlobalValueObject_Lua ( LuaPlus::LuaObject& luaObject, int& _refIntValue ) const
	{
		if ( luaObject.IsNil() )
		{
			return false;
		}

		if ( luaObject.IsInteger() )
		{
			_refIntValue = luaObject.GetInteger();

			return true;
		}

		sc::writeLogError ( "Global Value's Type is Invalid." );

		return false;
	}

	const bool LuaInstance::GetNumberGlobalValue_Lua ( const char* _szName, float& _refFloatValue ) const
	{
		LuaPlus::LuaObject luaObject = GetGlobalValue_Lua( _szName );

		if ( false == GetNumberGlobalValueObject_Lua( luaObject, _refFloatValue ) )
		{
			sc::writeLogError ( sc::string::format( 
				"'%1%' Global Value's Type is Invalid.",
				_szName ) );

			return false;
		}

		return true;
	}

	const bool LuaInstance::GetNumberGlobalValueObject_Lua ( LuaPlus::LuaObject& luaObject, float& _refFloatValue ) const
	{
		if ( luaObject.IsNil() )
		{
			return false;
		}

		if ( luaObject.IsNumber() )
		{
			_refFloatValue = static_cast< float >( luaObject.GetNumber() );

			return true;
		}

		sc::writeLogError ( "Global Value's Type is Invalid." );

		return false;
	}

	void LuaInstance::SetGlobalValueNumber_Lua ( const char* _szName, const double _dwValue ) const
	{
		// Lua 에서 사용할 수 있는 전역을 등록한다;
		// Push Data, Global Value in VStack;
		lua_pushnumber( m_pLua, _dwValue );
		lua_setglobal( m_pLua, _szName );
	}

	void LuaInstance::SetGlobalValueBoolean_Lua ( const char* _szName, const bool _bValue ) const
	{
		LuaPlus::LuaObject objInfo = GetGlobalValue_Lua( _szName );
		if ( objInfo.IsNil() == true )
		{
			// Lua 에서 사용할 수 있는 전역을 등록한다;
			// Push Data, Global Value in VStack;
			lua_pushboolean( m_pLua, _bValue );
			lua_setglobal( m_pLua, _szName );
		}
		else
		{
			objInfo.SetBoolean(_szName,_bValue);
		}
	}

	void LuaInstance::SetGlobalValueString_Lua ( const char* _szName, const char* _szValue ) const
	{
		LuaPlus::LuaObject ObjInfo = GetGlobalValue_Lua(_szName);
		if ( ObjInfo.IsNil() == true )
		{	
			// Lua 에서 사용할 수 있는 전역을 등록한다;
			// Push Data, Global Value in VStack;
			lua_pushstring( m_pLua, _szValue );
			lua_setglobal( m_pLua, _szName );
		}
		else
		{
			ObjInfo.SetString(_szName,_szValue);
		}
	}


	const LuaInstance& LuaInstance::operator = ( const LuaInstance& _rhs )
	{
		// 깊은 복사 안됨;
		m_pLuaPlus = _rhs.m_pLuaPlus;
		m_pLua = _rhs.m_pLua;

		return *this;
	}
	


	/************************************************************************/
	/* Lua Tinker를 사용하는 Script Base Interface;                        */
	/************************************************************************/
	IScriptBase::~IScriptBase ()
	{
		DefaultDestroy();
	}

	bool IScriptBase::DefaultInit ( const char* szFile )
	{
		m_pLua = luaL_newstate();
		if ( !m_pLua )
		{
			sc::writeLogError( "IScriptBase::DefaultInit - Failed Initialize lua_state." );
			return false;
		}

		luaL_openlibs( m_pLua );
		lua_tinker::init( m_pLua );

		lua_register( m_pLua, "_ALERT", OnAlert );

		return DoFile( szFile );
	}

	void IScriptBase::DefaultDestroy ()
	{
		if ( m_pLua )
		{
			lua_close( m_pLua );
			m_pLua = NULL;
		}
	}

	bool IScriptBase::DoFile ( const char* szFile )
	{
		if ( !m_pLua )
		{
			sc::writeLogError( "IScriptBase::DoFile - Lua_state is not initialized." );
			return false;
		}

		return lua_tinker::dofile( m_pLua, szFile );
	}

	HitRateScript::HitRateScript(void)
	{
	}
	HitRateScript::~HitRateScript(void)
	{
	}
	bool HitRateScript::Init( const char* szFile )
	{
		DefaultDestroy();
		return this->DefaultInit(szFile);
	}
}