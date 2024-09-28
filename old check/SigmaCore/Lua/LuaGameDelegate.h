#ifndef _SC_LUA_Delegate_H_
#define _SC_LUA_Delegate_H_

#include <string>
#include "../../=LuaPlus/src/LuaPlus/LuaPlus.h"
#include "../Util/MinMessageBox.h"
#include "../String/StringFormat.h"

extern "C"
{
#include "../../=LuaPlus/src/LuaPlus/src/lualib.h"
}

class LuaDelegate;
class LuaDelegateArgs;


typedef unsigned int        UInt;
typedef size_t              UPInt;

//
// 콜백을 하기 위한 핸들러. 각 기능들은 핸들러를 등록하는 역활을 한다.
// LuaGameDelegate를 통해 콜백된다.
//
class LuaDelegateHandler
{
public:
	virtual ~LuaDelegateHandler() {}

	//
	// 반응을 하기위해서는, 모든 콜벡 메서드는 아래의 구조를 "무조껀" 따라야 한다. 
	// 콜벡을 하기위해서는 인자값을 게임 델리게이터에 넣어야 한다.
	//
	typedef void (*CallbackFn)(const LuaDelegateArgs& params);    

	//
	// 특별한 핸들러 콜백 기호는 모든 메소드를 수신한다.
	//
	typedef void (*FallbackFn)(const LuaDelegateArgs& params, const char* methodName);

	//
	// Interface implemented by callback registrars. The handler should 
	// pass the appropriate parameters to the Visit method.
	//
	class CallbackProcessor
	{
	public:
		virtual ~CallbackProcessor() {}
		virtual void    Process(const std::string& methodName, CallbackFn method) = 0;
	};

	//
	// Callback registrar visitor method
	// Implementations are expected to call the registrar's Process method
	// for all callbacks.
	//
	virtual void        Accept(CallbackProcessor* cbreg) = 0;
};


//////////////////////////////////////////////////////////////////////////


//
// Parameters passed to the callback handler
//
class LuaDelegateArgs
{
public:
	LuaDelegateArgs(LuaDelegateHandler* pthis, LuaValue* pvalue, UInt nargs) 
		: pThis(pthis), pLuaValue(pvalue), NArgs(nargs) 

	{

	}

	LuaDelegateHandler*  GetHandler() const      { return pThis; }
	LuaValue*       GetState() const        { return pLuaValue; }

	const LuaValue&     operator[](UPInt i) const
	{ 
		GASSERT(i >= 0 && i < NArgs);
		return pLuaValue[i]; 
		//GASSERT(i >= 0 && i < NArgs);
	}
	UInt                GetArgCount() const     { return NArgs; }

private:
	LuaDelegateHandler*      pThis;
	LuaValue*           pLuaValue;
	UInt                    NArgs;
};

//////////////////////////////////////////////////////////////////////////

//
// 루아 변수
//
class LuaValue
{

public:

	LuaValue(double v)              : ValueType(LUA_TNUMBER)     { Value.NValue = v; }
	LuaValue(bool v)                : ValueType(LUA_TBOOLEAN)    { Value.BValue = v; }
	LuaValue(const char* ps)        : ValueType(LUA_TSTRING)     { Value.pString = ps; }
	//지원 안함.
	//LuaValue(const wchar_t* ps)     : pObjectInterface(NULL), ValueType(VT_StrngW)    { Value.pStringW = ps; }

	ValueUnion  Value;

	union ValueUnion
	{
		double          NValue;
		bool            BValue;
		const char*     pString;
// 지원 안함.
//		const char**    pStringManaged;
//		const wchar_t*  pStringW;
		void*           pData;
	};

protected:
	// Lua.h basic types 참조
	DWORD ValueType;

};


//////////////////////////////////////////////////////////////////////////


//
// Callback manager that marshals calls from ActionScript 
//
class LuaDelegate 
{
public:
	//
	// Callback target
	//
	struct CallbackDefn
	{
		LuaDelegateHandler*         pThis;
		LuaDelegateHandler::CallbackFn   pCallback;
	};

	// Fallback target
	struct FallbackDefn
	{
		LuaDelegateHandler*         pThis;
		LuaDelegateHandler::FallbackFn   pCallback;
	};

	//
	// Callback hash
	//
	struct CallbackHashFunctor
	{
		UPInt  operator()(const char* data) const
		{
			UPInt  size = G_strlen(data);
			return GString::BernsteinHashFunction(data, size);
		}
	};
	typedef GHash<GString, CallbackDefn, CallbackHashFunctor> CallbackHash;


	//
	// Install and uninstall callbacks
	//
	void            RegisterHandler(LuaDelegateHandler* callback);
	void            UnregisterHandler(LuaDelegateHandler* callback);

	// Install and uninstall fallback handler
	void            RegisterFallbackHandler(LuaDelegateHandler* pthis, LuaDelegateHandler::FallbackFn pcallback)
	{
		Fallback.pThis = pthis;
		Fallback.pCallback = pcallback;
	}
	void            UnregisterFallbackHandler() { Fallback.pThis = NULL; }

	// Clear all callbacks/handlers
	void            ClearAll()
	{
		Callbacks.Clear();
		UnregisterFallbackHandler();
	}

	//
	// ExternalInterface callback entry point
	//
	void            Callback(GFxMovieView* pmovieView, const char* methodName, 
		const GFxValue* args, UInt argCount);

private:
	//
	// Callbacks installed with the game delegate
	//
	CallbackHash    Callbacks;

	//
	// Fallback handler
	//
	FallbackDefn    Fallback;
};


