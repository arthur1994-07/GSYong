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
	/* ���� ��� ( �ܼ� ������ �ε��ÿ��� ����Ѵ� );                   */
	/* ���������δ� ���ŵǰ� ��ü�Ǵ°��� ����;                         */ 
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

	// Lua ���� Error �߻��� ȣ��ȴ�;
	int OnAlert ( lua_State* pState );

	// �� �޾ƿ���;
	const bool GetBooleanByName ( LuaPlus::LuaObject& _luaObject, const char* _szName );
	const int GetIntegerByName ( LuaPlus::LuaObject& _luaObject, const char* _szName );
	const float GetNumberByName ( LuaPlus::LuaObject& _luaObject, const char* _szName );
	const char* GetStringByName ( LuaPlus::LuaObject& _luaObject, const char* _szName );

	const bool GetBooleanByObject ( LuaPlus::LuaObject& _luaObject );
	const int GetIntegerByObject ( LuaPlus::LuaObject& _luaObject );
	const float GetNumberByObject ( LuaPlus::LuaObject& _luaObject );
	const char* GetStringByObject ( LuaPlus::LuaObject& _luaObject );

	// Lua Virtual Stack �� ������ ��ȯ;
	const bool GetBooleanToLuaState ( lua_State* _pLuaState, int _nStack );
	const int GetIntegerToLuaState ( lua_State* _pLuaState, int _nStack );
	const float GetNumberToLuaState ( lua_State* _pLuaState, int _nStack );
	const char* GetStringToLuaState ( lua_State* _pLuaState, int _nStack );


	/************************************************************************/
	/* Lua Instance ������ ���� Lua Class ( LuaPlus );                     */
	/************************************************************************/
	// �ν��Ͻ� �������� ����� ��� ��ü; 
	// ���뼺������ �κ��� ������� ����;
	// --> ��ʸ�Ʈ������ ����Ѵ�. 2013 10 15 thkwon
	class LuaInstance
	{
	public:
		LuaInstance ( void );
		~LuaInstance ( void );

	public:
		// lua ��ü ���簡 �ȵȴ�;
		// �ڷᱸ�� ������ ��ü ������ ���� �Ϸ� �Ҷ��� �� ��� ��;
		// �ݵ�� �Ʒ� destroy() �Լ��� ȣ���Ͽ��� ��;
		// �Ҹ��ڿ��� �ı��ϴ� ��� ��ü �������ʿ� ������ ����;
		// ������ �i�� �̷��� �ۿ� ������;
		void						OnDestroy ( void );

	public:
		const int					DoFile ( const std::string& _fileName );
		const int					DoBuffer ( const char* _pBuffer, const unsigned int _nSize, const char* const _cName );
		const int					DoString ( const char* const _pString ) const;

		/// Print Lua VStack;
		void						PrintVStack ( bool _bDetail = false ) const;

		/// Invoke Function ó��;
		void						RegistFunction ( const char* const _name, lua_CFunction _function ) const;

	public:
		/// Callback Function ó�� - Lua �� ���ǵǾ� �ִ� Function �� Ư�� ������ ȣ���Ѵ�;
		/// ��, Lua ������ Callback �� �ǹ��Ѵ�;

		// Global Value �� �����ϴ��� Ȯ���Ѵ�;
		// ���⿡�� �Լ��� ���Ѵ�;
		const bool					IsExist ( const char* const _name ) const;
		const bool					IsTable ( const char* const _name ) const;
		const bool					IsTableObject ( LuaPlus::LuaObject& _luaObject ) const;

		/************************************************************************/
		/* VStack �� ����Կ� �־ Push �� �̷�����ٸ�;                   */
		/* Pop �� ������ �̷������ �Ѵ�;                                     */
		/* ( �׷��� ������ VStack �� ���� Crash �� �Ͼ �� �ִ� );         */
		/* �̸� ���� VStack Count �� �����Ͽ� ���̴� ���� ������ �����Ѵ�;  */
		/************************************************************************/

		/// Push VStack;
		// Data �� VStack �� �ø���;
		void						PushInteger ( const int _nArgv );
		void						PushNumber ( const float _fArgv );
		void						PushBoolean ( const bool _bArgv );

		// Lua ���� ���ǵ� ������ �����ͼ� VStack �� �ø���;
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
		/// Lua ���;
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
		//! ��� �Լ��� Callback���� ����ϱ�;
		template < typename Callee >
		__forceinline void			registFunctionex(const char* const _name, const Callee& callee, int (Callee::*func)(lua_State*));

		//! �Լ��� ���ڰ��� ���� ������ ��ȯ�ؼ� ����ϱ�. (�ɹ� ����);
		template < typename Callee, typename Func >
		__forceinline void			registdirectFunction(const char* const _name,const Callee& callee, Func func);

		//< �Լ��� ���ڰ��� ���� ������ ��ȯ�ؼ� ����ϱ�. (���� ����);
		// �����ִ� �⺻ �ڷ��� �������ִ�;
		template < typename Func >
		__forceinline void			registdirectFunction(const char* const _name, Func func);

		//! �̷����� �ʿ��� �Լ�����ϱ� (�ɹ� �Լ� ���� ���);
		template < typename Callee >
		__forceinline void			lua_pushObject_MirroringFunction(const char* const _name, int (Callee::*func)(lua_State*));

		//! �̷����� �ʿ��� Ŭ���� ����ϱ�;
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
	/* Lua Tinker�� ����ϴ� Script Base Interface;                        */
	/************************************************************************/

	/// Interface�� �����Ͽ� ����Ѵ�;
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
		// ������ ����Ѵ�;
		template < typename T >
		inline void SetGlobal ( const char* szName, T object );

		// ������ �����´�;
		template < typename T >
		inline T GetGlobal ( const char* szName );

		// �Ϲ� �Լ��� ����Ѵ�;
		template < typename F >
		inline void RegFunc ( const char* szFuncName, F func );

		// Ŭ������ ����Ѵ�;
		template < typename Class >
		inline void RegClass ( const char *szName );

		// Ŭ������ ����Լ��� ����Ѵ�;
		template < typename Class, typename F >
		inline void RegClassFunc ( const char* szName, F func );

		// Ŭ������ ��������� ����Ѵ�;
		template < typename Class, typename BASE, typename VAR >
		inline void RegClassVar ( const char* szName, VAR BASE::*val );

		// Ŭ������ ��� ���踦 �˸���;
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

		// Parameter�� �� �ְ� ������ �Լ��� �߰��ϸ� �ȴ�;
		// ���� 8������ �߰� �����ϴ�;
		// �� �߰��ϰ� �ʹٸ� LuaTinker�� Customizing�� �ʿ��ϴ�;

		// Lua Table Member Function Call;

		/************************************************************************/
		/* LuaTinker�� Customizing �Ͽ� ����� �߰������Ƿ� ���ǰ� �ʿ��ϴ�;*/
		/* ������ LuaTinker�� Table Member Function Call ����� ����;         */
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

		// Parameter�� �� �ְ� ������ �߰����� Cusomizing�� �ʿ��ϴ�;

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
