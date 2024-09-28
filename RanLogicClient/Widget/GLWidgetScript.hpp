#pragma once

template< typename T >
T GLWidgetScript::LuaCallFunc( const char* szName, const char* fmt, ... )
{
	if ( false == m_bInitialize )
	{
		sc::writeLogError( "Failed LuaCallFunc" );
		return (T) 0;
	}

	if( NULL == m_pLuaState || NULL == szName )
		return (T) 0;

	lua_pushcclosure( m_pLuaState, lua_tinker::on_error, 0 );
	int errfunc = lua_gettop( m_pLuaState );

	lua_pushstring( m_pLuaState, szName );
	lua_gettable( m_pLuaState, LUA_GLOBALSINDEX );
	if( lua_isfunction( m_pLuaState, -1 ) )
	{
		int nArgsCount = 0;

		va_list argsPtr;
		va_start( argsPtr, fmt );

		while( *fmt )
		{
			if( 0 == strncmp( fmt, "-n", 2 ) )
			{
				lua_tinker::push( m_pLuaState, va_arg( argsPtr, int ) );
				fmt += 2;
			}
			else if( 0 == strncmp( fmt, "-dw", 3 ) )
			{
				lua_tinker::push( m_pLuaState, va_arg( argsPtr, DWORD ) );
				fmt += 3;
			}
			else if( 0 == strncmp( fmt, "-l", 2 ) )
			{
				lua_tinker::push( m_pLuaState, va_arg( argsPtr, LONGLONG ) );
				fmt += 2;
			}
			else if( 0 == strncmp( fmt, "-f", 2 ) )
			{
				lua_tinker::push( m_pLuaState, va_arg( argsPtr, double ) );
				fmt += 2;
			}
			else if( 0 == strncmp( fmt, "-s", 2 ) )
			{
				lua_tinker::push( m_pLuaState, va_arg( argsPtr, char* ) );
				fmt += 2;
			}
			else if( 0 == strncmp( fmt, "-b", 2 ) )
			{
                lua_tinker::push( m_pLuaState, bool( va_arg( argsPtr, int ) ? true : false ) );
				fmt += 2;
			}
			else
			{
				fmt += 1;
				continue;
			}

			++nArgsCount;
		}

		va_end( argsPtr );
		
		if( 0 != lua_pcall( m_pLuaState, nArgsCount, 1, errfunc ) )
			lua_pop( m_pLuaState, 1 );

		recordPf(szName);
	}
	else
	{
		lua_tinker::print_error( m_pLuaState, "lua_tinker::call() attempt to call global `%s' (not a function)", szName );
	}

	lua_remove( m_pLuaState, -2 );

	return lua_tinker::pop< T >( m_pLuaState );
}

template< typename T >
T GLWidgetScript::LuaCallFunc( const char* szName, const VEC_WIDGET_CALLFUNCVALUE& vecValue )
{
	if ( false == m_bInitialize )
	{
		sc::writeLogError( "Failed LuaCallFunc" );
		return (T) 0;
	}

	if( NULL == m_pLuaState || NULL == szName )
		return (T) 0;

	lua_pushcclosure( m_pLuaState, lua_tinker::on_error, 0 );
	int errfunc = lua_gettop( m_pLuaState );

	lua_pushstring( m_pLuaState, szName );
	lua_gettable( m_pLuaState, LUA_GLOBALSINDEX );
	if( lua_isfunction( m_pLuaState, -1 ) )
	{
		VEC_WIDGET_CALLFUNCVALUE_CITER iter = vecValue.begin();
		for( ; iter != vecValue.end(); ++iter )
		{
			switch( iter->emType )
			{
			case GLWidgetCallFuncValue::EMTYPE_NULL :
				lua_tinker::push( m_pLuaState, 0 );
				break;

			case GLWidgetCallFuncValue::EMTYPE_INT :
				lua_tinker::push( m_pLuaState, iter->nValue );
				break;

			case GLWidgetCallFuncValue::EMTYPE_UINT :
				lua_tinker::push( m_pLuaState, iter->unValue );
				break;
				
			case GLWidgetCallFuncValue::EMTYPE_DWORD :
				lua_tinker::push( m_pLuaState, iter->dwValue );
				break;

			case GLWidgetCallFuncValue::EMTYPE_DOUBLE :
				lua_tinker::push( m_pLuaState, iter->dValue );
				break;

			case GLWidgetCallFuncValue::EMTYPE_BOOL :
				lua_tinker::push( m_pLuaState, iter->bValue );
				break;

			case GLWidgetCallFuncValue::EMTYPE_STRING :
				lua_tinker::push( m_pLuaState, iter->strValue.c_str() );
				break;

			case GLWidgetCallFuncValue::EMTYPE_TABLE :
				lua_tinker::push( m_pLuaState, *iter->pTbValue );
				break;
			}
		}

		if( 0 != lua_pcall( m_pLuaState, vecValue.size(), 1, errfunc ) )
			lua_pop( m_pLuaState, 1 );

		recordPf(szName);
	}
	else
	{
		lua_tinker::print_error( m_pLuaState, "lua_tinker::call() attempt to call global `%s' (not a function)", szName );
	}

	lua_remove( m_pLuaState, -2 );

	return lua_tinker::pop< T >( m_pLuaState );
}

template< typename T >
T GLWidgetScript::LuaCallFunc( const char* szName, const GLWidgetCallFuncValue* pArgs, int nArgCount )
{
	if ( false == m_bInitialize )
	{
		sc::writeLogError( "Failed LuaCallFunc" );
		return (T) 0;
	}

	if( NULL == m_pLuaState || NULL == szName )
		return (T) 0;

	if( NULL == pArgs )
		return (T) 0;

	lua_pushcclosure( m_pLuaState, lua_tinker::on_error, 0 );
	int errfunc = lua_gettop( m_pLuaState );

	lua_pushstring( m_pLuaState, szName );
	lua_gettable( m_pLuaState, LUA_GLOBALSINDEX );
	if( lua_isfunction( m_pLuaState, -1 ) )
	{
		for( int i = 0; i < nArgCount; ++i )
		{
			switch( pArgs[ i ].emType )
			{
			case GLWidgetCallFuncValue::EMTYPE_NULL :
				//lua_tinker::push( m_pLuaState, 0 );
				//lua_tinker::
				lua_pushnil(m_pLuaState);
				break;

			case GLWidgetCallFuncValue::EMTYPE_INT :
				lua_tinker::push( m_pLuaState, pArgs[ i ].nValue );
				break;

			case GLWidgetCallFuncValue::EMTYPE_UINT :
				lua_tinker::push( m_pLuaState, pArgs[ i ].unValue );
				break;

			case GLWidgetCallFuncValue::EMTYPE_DWORD :
				lua_tinker::push( m_pLuaState, pArgs[ i ].dwValue );
				break;

			case GLWidgetCallFuncValue::EMTYPE_DOUBLE :
				lua_tinker::push( m_pLuaState, pArgs[ i ].dValue );
				break;

			case GLWidgetCallFuncValue::EMTYPE_BOOL :
				lua_tinker::push( m_pLuaState, pArgs[ i ].bValue );
				break;

			case GLWidgetCallFuncValue::EMTYPE_STRING :
				lua_tinker::push( m_pLuaState, pArgs[ i ].strValue.c_str() );
				break;

			case GLWidgetCallFuncValue::EMTYPE_TABLE :
				lua_tinker::push( m_pLuaState, *(pArgs[ i ].pTbValue) );
				break;
			}
		}

		if( 0 != lua_pcall( m_pLuaState, nArgCount, 1, errfunc ) )
			lua_pop( m_pLuaState, 1 );

		recordPf(szName);
	}
	else
	{
		lua_tinker::print_error( m_pLuaState, "lua_tinker::call() attempt to call global `%s' (not a function)", szName );
	}

	lua_remove( m_pLuaState, -2 );

	return lua_tinker::pop< T >( m_pLuaState );
}