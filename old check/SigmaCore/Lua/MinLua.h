#ifndef _SC_LUA_H_
#define _SC_LUA_H_

#include <string>
#include "../../=LuaPlus/src/LuaPlus/LuaPlus.h"
#include "../Util/MinMessageBox.h"
#include "../Log/LogMan.h"

#include "../String/StringFormat.h"

extern "C"
{
#include "../../=LuaPlus/src/LuaPlus/src/lualib.h"
}

#include "./lua_tinker.h"

namespace sc
{
	/************************************************************************/
	/* 전역 루아 ( 단순 데이터 로딩시에만 사용한다 );                   */
	/* 최종적으로는 제거되고 대체되는것이 좋다;                         */ 
	/************************************************************************/
    extern LuaPlus::LuaState* g_pLua;

    extern std::string lua_strError;

    void lua_init();

    LuaPlus::LuaState* lua_getState();
    LuaPlus::LuaState* lua_getThreadState();
    void lua_destroyState();
    
    LuaPlus::LuaObject lua_getGlobalFromName(const char* szName);

    bool lua_doFile(const std::string& filename, LuaPlus::LuaState* pState=NULL);
    bool lua_doFile(const char* filename, LuaPlus::LuaState* pState=NULL);

    bool lua_doFile(const std::wstring& filename, LuaPlus::LuaState* pState=NULL);
    bool lua_doFile(const wchar_t* filename, LuaPlus::LuaState* pState=NULL);

    bool lua_doBuffer(const char* buff, size_t size, const char* name, LuaPlus::LuaState* pState=NULL);
    std::string lua_getError();


	// Print Lua VStack;
	void PrintVStack ( lua_State* pState );

	// Lua 에서 Error 발생시 호출된다;
	int OnAlert ( lua_State* pState );

	// 값 받아오기;
	const bool GetBooleanByName ( LuaPlus::LuaObject& _luaObject, const char* _szName );
	const int GetIntegerByName ( LuaPlus::LuaObject& _luaObject, const char* _szName );
	const float GetNumberByName ( LuaPlus::LuaObject& _luaObject, const char* _szName );
	const char* GetStringByName ( LuaPlus::LuaObject& _luaObject, const char* _szName );

	const bool GetBooleanByObject ( LuaPlus::LuaObject& _luaObject );
	const int GetIntegerByObject ( LuaPlus::LuaObject& _luaObject );
	const float GetNumberByObject ( LuaPlus::LuaObject& _luaObject );
	const char* GetStringByObject ( LuaPlus::LuaObject& _luaObject );

	// Lua Virtual Stack 의 데이터 변환;
	const bool GetBooleanToLuaState ( lua_State* _pLuaState, int _nStack );
	const int GetIntegerToLuaState ( lua_State* _pLuaState, int _nStack );
	const float GetNumberToLuaState ( lua_State* _pLuaState, int _nStack );
	const char* GetStringToLuaState ( lua_State* _pLuaState, int _nStack );


	/************************************************************************/
	/* Lua Instance 관리를 위한 Lua Class ( LuaPlus );                     */
	/************************************************************************/
	// 인스턴스 던전에서 사용할 루아 객체; 
	// 재사용성에대한 부분은 고려하지 않음;
	// --> 토너먼트에서도 사용한다. 2013 10 15 thkwon
	class LuaInstance
	{
	public:
		LuaInstance ( void );
		~LuaInstance ( void );

	public:
		// lua 객체 복사가 안된다;
		// 자료구조 내지는 객체 단위로 관리 하려 할때는 다 사용 후;
		// 반드시 아래 destroy() 함수를 호출하여야 함;
		// 소멸자에서 파괴하는 경우 객체 복사한쪽에 문제가 생김;
		// 일정에 쫒겨 이렇게 밖에 못만듦;
		void						OnDestroy ( void );

	public:
		const int					DoFile ( const std::string& _fileName );
		const int					DoBuffer ( const char* _pBuffer, const unsigned int _nSize, const char* const _cName );
		const int					DoString ( const char* const _pString ) const;

		/// Print Lua VStack;
		void						PrintVStack ( bool _bDetail = false ) const;

		/// Invoke Function 처리;
		void						RegistFunction ( const char* const _name, lua_CFunction _function ) const;

	public:
		/// Callback Function 처리 - Lua 에 정의되어 있는 Function 을 특정 시점에 호출한다;
		/// 즉, Lua 에서의 Callback 을 의미한다;

		// Global Value 이 존재하는지 확인한다;
		// 여기에는 함수도 속한다;
		const bool					IsExist ( const char* const _name ) const;
		const bool					IsTable ( const char* const _name ) const;
		const bool					IsTableObject ( LuaPlus::LuaObject& _luaObject ) const;

		/************************************************************************/
		/* VStack 을 사용함에 있어서 Push 가 이루어졌다면;                   */
		/* Pop 도 무조건 이루어져야 한다;                                     */
		/* ( 그렇지 않으면 VStack 이 꼬여 Crash 가 일어날 수 있다 );         */
		/* 이를 위해 VStack Count 를 관리하여 꼬이는 일이 없도록 방지한다;  */
		/************************************************************************/

		/// Push VStack;
		// Data 를 VStack 에 올린다;
		void						PushInteger ( const int _nArgv );
		void						PushNumber ( const float _fArgv );
		void						PushBoolean ( const bool _bArgv );

		// Lua 에서 정의된 전역을 가져와서 VStack 에 올린다;
		void						PushFunction ( const char* const _name );
		const bool					PushFunctionEx ( const char* const _name );

		/// Pop VStack;
		const int					PopInteger ( void );
		const float					PopNumber ( void );
		const bool					PopBoolean ( void );

		/************************************************************************/

		/// Call Function;
		const bool					CallFunction ( const unsigned int _nArgv, const unsigned int _nResults );

	public:
		/// Coroutine;
		lua_State* 					CallFunctionCoroutine ( const char* const _name ) const;
		int							CoroutineResume ( lua_State* _pState, int _nArgv ) const;
		int							CoroutineResume ( int _nArgv ) const;
		int							CoroutineYield ( int _nResults ) const;
		int							CoroutineYield ( lua_State* _pState, int _nResults )const;

	public:
		/// Lua 사용;
		const LuaPlus::LuaObject	GetGlobalValue_Lua ( const char* _szName ) const;
		const bool					GetStringGlobalValue_Lua ( const char* _szName, std::string& _refStrValue ) const;
		const bool					GetStringGlobalValueObject_Lua ( LuaPlus::LuaObject& luaObject, std::string& _refStrValue ) const;
		const bool					GetIntegerGlobalValue_Lua ( const char* _szName, int& _refIntValue ) const;
		const bool					GetIntegerGlobalValueObject_Lua ( LuaPlus::LuaObject& luaObject, int& _refIntValue ) const;
		const bool					GetNumberGlobalValue_Lua ( const char* _szName, float& _refFloatValue ) const;
		const bool					GetNumberGlobalValueObject_Lua ( LuaPlus::LuaObject& luaObject, float& _refFloatValue ) const;

		void						SetGlobalValueNumber_Lua ( const char* _szName, const double _dwValue ) const;
		void						SetGlobalValueBoolean_Lua ( const char* _szName, const bool _bValue ) const;
		void						SetGlobalValueString_Lua ( const char* _szName, const char* _szValue ) const;
		
	public:
		//! 멤버 함수를 Callback으로 등록하기;
		template < typename Callee >
		__forceinline void			registFunctionex(const char* const _name, const Callee& callee, int (Callee::*func)(lua_State*));

		//! 함수의 인자값을 자유 자제로 변환해서 등록하기. (맴버 가능);
		template < typename Callee, typename Func >
		__forceinline void			registdirectFunction(const char* const _name,const Callee& callee, Func func);

		//< 함수의 인자값을 자유 자제로 변환해서 등록하기. (전역 가능);
		// 쓸수있는 기본 자료형 정해져있다;
		template < typename Func >
		__forceinline void			registdirectFunction(const char* const _name, Func func);

		//! 미러링에 필요한 함수등록하기 (맴버 함수 원형 등록);
		template < typename Callee >
		__forceinline void			lua_pushObject_MirroringFunction(const char* const _name, int (Callee::*func)(lua_State*));

		//! 미러링에 필요한 클레스 등록하기;
		template < typename Callee >
		__forceinline void			lua_pushObject_MirroringClass(const char* const _name,const Callee* callee);


	public:
		__forceinline lua_State*	GetLuaState ( void ) const { return m_pLua; }

	public:
		const LuaInstance&			operator = ( const LuaInstance& _rhs );

	private:
		DWORD						m_dwVStackCount;

		LuaPlus::LuaState*			m_pLuaPlus;
		lua_State*					m_pLua;

	public:
		static const int INVALID_POP_INTEGER;
		static const float INVAID_POP_FLOAT;
	};






	/************************************************************************/
	/* Lua Tinker를 사용하는 Script Base Interface;                        */
	/************************************************************************/

	/// Interface를 구현하여 사용한다;
	class IScriptBase
	{
	public:
		IScriptBase () : m_pLua( NULL ) { }
		virtual ~IScriptBase ();

	public:
		virtual bool Init ( const char* szFile ) { return true; }
		virtual bool Reload () { return true; }

		virtual void Destroy () { }

	protected:
		bool DefaultInit ( const char* szFile );
		void DefaultDestroy ();

		bool DoFile ( const char* szFile );

	public:
		// 전역을 등록한다;
		template < typename T >
		inline void SetGlobal ( const char* szName, T object );

		// 전역을 가져온다;
		template < typename T >
		inline T GetGlobal ( const char* szName );

		// 일반 함수를 등록한다;
		template < typename F >
		inline void RegFunc ( const char* szFuncName, F func );

		// 클래스를 등록한다;
		template < typename Class >
		inline void RegClass ( const char *szName );

		// 클래스의 멤버함수를 등록한다;
		template < typename Class, typename F >
		inline void RegClassFunc ( const char* szName, F func );

		// 클래스의 멤버변수를 등록한다;
		template < typename Class, typename BASE, typename VAR >
		inline void RegClassVar ( const char* szName, VAR BASE::*val );

		// 클래스의 상속 관계를 알린다;
		template < typename Class, typename Super >
		inline void RegClassInh ();

	public:
		// Lua Function Call;
		template < typename R >
		inline R CallLua ( const char* szFuncName );

		template < typename R, typename T1 >
		inline R CallLua ( const char* szFuncName, const T1& t1 );

		template < typename R, typename T1, typename T2 >
		inline R CallLua ( const char* szFuncName, const T1& t1,  const T2& t2 );

		template < typename R, typename T1, typename T2, typename T3 >
		inline R CallLua ( const char* szFuncName, const T1& t1,  const T2& t2, const T3& t3 );

		template < typename R, typename T1, typename T2, typename T3, typename T4 >
		inline R CallLua ( const char* szFuncName, const T1& t1,  const T2& t2, const T3& t3, const T4& t4 );

		// Parameter를 더 넣고 싶으면 함수를 추가하면 된다;
		// 현재 8개까지 추가 가능하다;
		// 더 추가하고 싶다면 LuaTinker의 Customizing이 필요하다;

		// Lua Table Member Function Call;

		/************************************************************************/
		/* LuaTinker를 Customizing 하여 기능을 추가했으므로 주의가 필요하다;*/
		/* 원래는 LuaTinker에 Table Member Function Call 기능이 없다;         */
		/************************************************************************/
		template < typename R >
		inline R CallLuaTableMember ( const char* szTableName, const char* szFuncName );

		template < typename R, typename T1 >
		inline R CallLuaTableMember ( const char* szTableName, const char* szFuncName, const T1& t1 );

		template < typename R, typename T1, typename T2 >
		inline R CallLuaTableMember ( const char* szTableName, const char* szFuncName, const T1& t1,  const T2& t2 );

		template < typename R, typename T1, typename T2, typename T3 >
		inline R CallLuaTableMember ( const char* szTableName, const char* szFuncName, const T1& t1,  const T2& t2, const T3& t3 );

		template < typename R, typename T1, typename T2, typename T3, typename T4 >
		inline R CallLuaTableMember ( const char* szTableName, const char* szFuncName, const T1& t1,  const T2& t2, const T3& t3, const T4& t4 );

		// Parameter를 더 넣고 싶으면 추가적인 Cusomizing이 필요하다;

	private:
		lua_State* m_pLua;
	};

	typedef lua_tinker::table LuaTable;


	class HitRateScript : public IScriptBase
	{
	public:
		HitRateScript(void);
		virtual ~HitRateScript(void);

	public:
		virtual bool Init( const char* szFile );
	};
}

#include "MinLua.hpp"

#endif // _SC_LUA_H_
