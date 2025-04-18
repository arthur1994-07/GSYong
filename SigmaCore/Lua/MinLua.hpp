#pragma once

namespace sc
{
	


	

	// this 가 존재한다! 우왕!
	template <typename Callee>
	__forceinline void LuaInstance::registFunctionex(const char* const _name,  const Callee& callee, int (Callee::*func)(lua_State*))
	{
		lua_pushstring(this->m_pLua , _name);
		lua_pushfunctorclosureex(this->m_pLua, callee,func, 0);
		lua_settable(this->m_pLua, LUA_GLOBALSINDEX);

	}

	// 맴버도 된다. 하지만 빠르고 호출이 좋다. 
	// LuaState를 받지 않는다. (자동으로 케스팅 해준다)
	// ex) func -> float Add(float num1, float num2) 등록가능.
	// 단) 인자는 7개 까지만 지원한다.
	template <typename Callee, typename Func>
	__forceinline void LuaInstance::registdirectFunction(const char* const _name, const Callee& callee, Func func)
	{
		lua_pushstring(this->m_pLua, _name);
		lua_pushdirectclosure(this->m_pLua, callee, func, 0);
		lua_settable(this->m_pLua, LUA_GLOBALSINDEX);
	}

	// 전역만 된다.
	// LuaState를 받지 않는다. (자동으로 케스팅 해준다)
	// ex) func -> float Add(float num1, float num2) 등록가능.
	// 단) 인자는 7개 까지만 지원한다.
	template <typename Func>
	__forceinline void LuaInstance::registdirectFunction(const char* const _name,Func func)
	{
		lua_pushstring(this->m_pLua, _name);
		lua_pushdirectclosure(this->m_pLua,func, 0);
		lua_settable(this->m_pLua, LUA_GLOBALSINDEX);
	}

	// 미러링 할때 사용한다.
	// 함수 원형을 넣어라 이상한거 넣지 말고.
	// 맴버 함수 obj.F()	 X
	// 맴버 함수 CClass::F() O
	template <typename Callee>
	__forceinline void LuaInstance::lua_pushObject_MirroringFunction(const char* const _name, int (Callee::*func)(lua_State*))
	{
		
		// 메타 테이블 만들기.
		lua_newtable(this->m_pLua);
		lua_pushstring(this->m_pLua, "__index");
		lua_pushvalue(this->m_pLua, -2);
		lua_settable(this->m_pLua, -3);

		// Print 함수 추가하기.
		lua_pushstring(this->m_pLua, _name);
		lua_pushobjectfunctorclosure(this->m_pLua, func, 0);
		lua_settable(this->m_pLua, -3);

	}


	// 미러링 할때 사용한다.
	// 미러링 할 클레스를 넣자!
	// 주의 : LUA작성자가 잘못 함수를 호출할경우가 생긴다.
	// 그에따른 예외처리 보다는 레퍼런스로 쓰는법을 정확히 할려준다.
	template <typename Callee>
	__forceinline void LuaInstance::lua_pushObject_MirroringClass(const char* const _name, const Callee* callee)
	{
		lua_pushstring(this->m_pLua, _name);
		lua_boxpointer(this->m_pLua, callee);
		lua_pushvalue(this->m_pLua, -3);
		lua_setmetatable(this->m_pLua, -2);
		lua_settable(this->m_pLua, LUA_GLOBALSINDEX);
	}


	




	


	


	

	

	





	/************************************************************************/
	/* Lua Tinker를 사용하는 Script Base Interface;                        */
	/************************************************************************/
	// 전역을 등록한다;
	template < typename T >
	inline void IScriptBase::SetGlobal ( const char* szName, T object )
	{
		lua_tinker::set( m_pLua, szName, object );
	}

	// 전역을 가져온다;
	template < typename T >
	inline T IScriptBase::GetGlobal ( const char* szName )
	{
		return lua_tinker::get<T>( m_pLua, szName );
	}

	// 일반 함수를 등록한다;
	template < typename F >
	inline void IScriptBase::RegFunc ( const char* szFuncName, F func ) 
	{
		lua_tinker::def<F>( m_pLua, szFuncName, func );
	}

	// 클래스를 등록한다;
	template < typename Class >
	inline void IScriptBase::RegClass ( const char *szName )
	{
		lua_tinker::class_add<Class>( m_pLua, szName );
	}

	// 클래스의 멤버함수를 등록한다;
	template < typename Class, typename F >
	inline void IScriptBase::RegClassFunc ( const char* szName, F func )
	{
		lua_tinker::class_def<Class>( m_pLua, szName, func );
	}

	// 클래스의 멤버변수를 등록한다;
	template < typename Class, typename BASE, typename VAR >
	inline void IScriptBase::RegClassVar ( const char* szName, VAR BASE::*val )
	{
		lua_tinker::class_mem<Class>( m_pLua, szName, val );
	}

	// 클래스의 상속 관계를 알린다;
	template < typename Class, typename Super >
	inline void IScriptBase::RegClassInh ()
	{
		lua_tinker::class_inh<Class, Super>( m_pLua );
	}

	// Lua Function Call;
	template < typename R >
	inline R IScriptBase::CallLua ( const char* szFuncName )
	{
		return lua_tinker::call<R>( m_pLua, szFuncName );
	}

	template < typename R, typename T1 >
	inline R IScriptBase::CallLua ( const char* szFuncName, const T1& t1 )
	{
		return lua_tinker::call<R, T1>( m_pLua, szFuncName, t1 );
	}

	template < typename R, typename T1, typename T2 >
	inline R IScriptBase::CallLua ( const char* szFuncName, const T1& t1,  const T2& t2 )
	{
		return lua_tinker::call<R, T1, T2>( m_pLua, szFuncName, t1, t2 );
	}

	template < typename R, typename T1, typename T2, typename T3 >
	inline R IScriptBase::CallLua ( const char* szFuncName, const T1& t1,  const T2& t2, const T3& t3 )
	{
		return lua_tinker::call<R, T1, T2, T3>( m_pLua, szFuncName, t1, t2, t3 );
	}

	template < typename R, typename T1, typename T2, typename T3, typename T4 >
	inline R IScriptBase::CallLua ( const char* szFuncName, const T1& t1,  const T2& t2, const T3& t3, const T4& t4 )
	{
		return lua_tinker::call<R, T1, T2, T3, T4>( m_pLua, szFuncName, t1, t2, t3, t4 );
	}

	// Lua Table Member Function Call;
	template < typename R >
	inline R IScriptBase::CallLuaTableMember ( const char* szTableName, const char* szFuncName )
	{
		return lua_tinker::membercall<R>( m_pLua, szTableName, szFuncName );
	}

	template < typename R, typename T1 >
	inline R IScriptBase::CallLuaTableMember ( const char* szTableName, const char* szFuncName, const T1& t1 )
	{
		return lua_tinker::membercall<R, T1>( m_pLua, szTableName, szFuncName, t1 );
	}

	template < typename R, typename T1, typename T2 >
	inline R IScriptBase::CallLuaTableMember ( const char* szTableName, const char* szFuncName, const T1& t1,  const T2& t2 )
	{
		return lua_tinker::membercall<R, T1, T2>( m_pLua, szTableName, szFuncName, t1, t2 );
	}

	template < typename R, typename T1, typename T2, typename T3 >
	inline R IScriptBase::CallLuaTableMember ( const char* szTableName, const char* szFuncName, const T1& t1,  const T2& t2, const T3& t3 )
	{
		return lua_tinker::membercall<R, T1, T2, T3>( m_pLua, szTableName, szFuncName, t1, t2, t3 );
	}

	template < typename R, typename T1, typename T2, typename T3, typename T4 >
	inline R IScriptBase::CallLuaTableMember ( const char* szTableName, const char* szFuncName, const T1& t1,  const T2& t2, const T3& t3, const T4& t4 )
	{
		return lua_tinker::membercall<R, T1, T2, T3, T4>( m_pLua, szTableName, szFuncName, t1, t2, t3, t4 );
	}

}