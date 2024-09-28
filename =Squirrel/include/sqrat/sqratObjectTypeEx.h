
#if !defined(_SCRAT_OBJECT_TYPE_EX_H_)
#define _SCRAT_OBJECT_TYPE_EX_H_


#include <squirrel.h>

#include "sqrattypes.h"
#include "sqratutil.h"

namespace Sqrat
{
	class Function;
	class Object;
	class Array;
	class Table;
	class Thread;

	inline string GetObjectTypeName(HSQOBJECT obj)
	{
		switch(obj._type)
		{
		case OT_NULL:
			return _SC("NULL");
		case OT_INTEGER:
			return _SC("INTEGER");
		case OT_FLOAT:
			return _SC("FLOAT");
		case OT_USERPOINTER:
			return _SC("USERPOINTER");
		case OT_STRING:
			return _SC("STRING");
		case OT_TABLE:
			return _SC("TABLE");
		case OT_ARRAY:
			return _SC("ARRAY");
		case OT_CLOSURE:
			return _SC("CLOSURE");
		case OT_NATIVECLOSURE:
			return _SC("NATIVECLOSURE");
		case OT_GENERATOR:
			return _SC("GENERATOR");
		case OT_USERDATA:
			return _SC("USERDATA");
		case OT_THREAD:
			return _SC("THREAD");
		case OT_CLASS:
			return _SC("CLASS");
		case OT_INSTANCE:
			{
				sq_pushobject( DefaultVM::Get(), obj);
				SqObjectType* ret = Var<SqObjectType*>( DefaultVM::Get(), -1).value;
				SQRAT_ASSERT(ret);
				string className = SQRAT_GET_CLASS_NAME(*ret);
				sq_pop(DefaultVM::Get(), 1);
				return _SC("INSTANCE_") + className;
			}
		case OT_WEAKREF:
			return _SC("WEAKREF");
		case OT_BOOL:
			return _SC("BOOL");
		}

		return _SC("UNKNOWN");
	}

	template<class T>
	inline string GetObjectTypeName() { return  string(_SC("INSTANCE_")) + SQRAT_GET_CLASS_NAME(T); }
	template<> inline string GetObjectTypeName<unsigned int>()   { return _SC("INTEGER"); }
	template<> inline string GetObjectTypeName<signed int>()     { return _SC("INTEGER"); }
	template<> inline string GetObjectTypeName<unsigned long>()  { return _SC("INTEGER"); }
	template<> inline string GetObjectTypeName<signed long>()    { return _SC("INTEGER"); }
	template<> inline string GetObjectTypeName<unsigned short>() { return _SC("INTEGER"); }
	template<> inline string GetObjectTypeName<signed short>()   { return _SC("INTEGER"); }

	template<> inline string GetObjectTypeName<float>() { return _SC("FLOAT"); }
	template<> inline string GetObjectTypeName<double>() { return _SC("FLOAT"); }


	template<> inline string GetObjectTypeName<bool>() { return _SC("BOOL"); }
	template<> inline string GetObjectTypeName<const bool>()  { return _SC("BOOL"); }
	template<> inline string GetObjectTypeName<const bool&>() { return _SC("BOOL"); }

	template<> inline string GetObjectTypeName<SQChar*>() { return _SC("STRING"); }
	template<> inline string GetObjectTypeName<const SQChar*>() { return _SC("STRING"); }

	template<> inline string GetObjectTypeName<string>()         { return _SC("STRING"); }
	template<> inline string GetObjectTypeName<string&>()        { return _SC("STRING"); }
	template<> inline string GetObjectTypeName<const string&>()  { return _SC("STRING"); }

	template<> inline string GetObjectTypeName<Function>()  { return _SC("CLOSURE"); }

	template<> inline string GetObjectTypeName<Object>()    { SQRAT_ASSERT_MSG(false,"do not use Object"); return _SC("OBJECT"); }
	template<> inline string GetObjectTypeName<Thread>()    { return _SC("GENERATOR"); }
	template<> inline string GetObjectTypeName<Array>()    { return _SC("ARRAY"); }
	template<> inline string GetObjectTypeName<Table>()    { return _SC("TABLE"); }


	// const 버전과 아닌 버전 둘다 등록하진 못한다 , 둘중 하나만 등록할것 , 왜냐하면 스크립트에선
	// 두버전을 구분할수가 없기때문이다

	// Arg Count 0
	template <class C, class R>
	inline string SqMakeOverloadString(R (C::*method)()) {
		return _SC("_EMPTY");
	}

	template <class C, class R>
	inline string SqMakeOverloadString(R (C::*method)() const) {
		return _SC("_EMPTY");
	}

	// Arg Count 1
	template <class C, class R, class A1>
	inline string SqMakeOverloadString(R (C::*method)(A1)) {
		return GetObjectTypeName<A1>();
	}

	template <class C, class R, class A1>
	inline string SqMakeOverloadString(R (C::*method)(A1) const) {
		return GetObjectTypeName<A1>();
	}

	// Arg Count 2
	template <class C, class R, class A1, class A2>
	inline string SqMakeOverloadString(R (C::*method)(A1, A2)) {
		return GetObjectTypeName<A1>() + _SC("_") + GetObjectTypeName<A2>();
	}

	template <class C, class R, class A1, class A2>
	inline string SqMakeOverloadString(R (C::*method)(A1, A2) const) {
		return GetObjectTypeName<A1>() + _SC("_") + GetObjectTypeName<A2>();
	}

	// Arg Count 3
	template <class C, class R, class A1, class A2, class A3>
	inline string SqMakeOverloadString(R (C::*method)(A1, A2, A3)) {
		return GetObjectTypeName<A1>() + _SC("_") + GetObjectTypeName<A2>() 
			+ _SC("_") + GetObjectTypeName<A3>();
	}

	template <class C, class R, class A1, class A2, class A3>
	inline string SqMakeOverloadString(R (C::*method)(A1, A2, A3) const) {
		return GetObjectTypeName<A1>() + _SC("_") + GetObjectTypeName<A2>() 
			+ _SC("_") + GetObjectTypeName<A3>();
	}

	// Arg Count 4
	template <class C, class R, class A1, class A2, class A3, class A4>
	inline string SqMakeOverloadString(R (C::*method)(A1, A2, A3, A4)) {
		return GetObjectTypeName<A1>() + _SC("_") + GetObjectTypeName<A2>() 
			+ _SC("_") + GetObjectTypeName<A3>() + _SC("_") + GetObjectTypeName<A4>();
	}

	template <class C, class R, class A1, class A2, class A3, class A4>
	inline string SqMakeOverloadString(R (C::*method)(A1, A2, A3, A4) const) {
		return GetObjectTypeName<A1>() + _SC("_") + GetObjectTypeName<A2>() 
			+ _SC("_") + GetObjectTypeName<A3>() + _SC("_") + GetObjectTypeName<A4>();
	}

	// Arg Count 5
	template <class C, class R, class A1, class A2, class A3, class A4, class A5>
	inline string SqMakeOverloadString(R (C::*method)(A1, A2, A3, A4, A5)) {
		return GetObjectTypeName<A1>() + _SC("_") + GetObjectTypeName<A2>() 
			+ _SC("_") + GetObjectTypeName<A3>() + _SC("_") + GetObjectTypeName<A4>()
			+ _SC("_") + GetObjectTypeName<A5>();
	}

	template <class C, class R, class A1, class A2, class A3, class A4, class A5>
	inline string SqMakeOverloadString(R (C::*method)(A1, A2, A3, A4, A5) const) {
		return GetObjectTypeName<A1>() + _SC("_") + GetObjectTypeName<A2>() 
			+ _SC("_") + GetObjectTypeName<A3>() + _SC("_") + GetObjectTypeName<A4>()
			+ _SC("_") + GetObjectTypeName<A5>();
	}

	// Arg Count 6
	template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6>
	inline string SqMakeOverloadString(R (C::*method)(A1, A2, A3, A4, A5, A6)) {
		return GetObjectTypeName<A1>() + _SC("_") + GetObjectTypeName<A2>() 
			+ _SC("_") + GetObjectTypeName<A3>() + _SC("_") + GetObjectTypeName<A4>()
			+ _SC("_") + GetObjectTypeName<A5>() + _SC("_") + GetObjectTypeName<A6>();
	}

	template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6>
	inline string SqMakeOverloadString(R (C::*method)(A1, A2, A3, A4, A5, A6) const) {
		return GetObjectTypeName<A1>() + _SC("_") + GetObjectTypeName<A2>() 
			+ _SC("_") + GetObjectTypeName<A3>() + _SC("_") + GetObjectTypeName<A4>()
			+ _SC("_") + GetObjectTypeName<A5>() + _SC("_") + GetObjectTypeName<A6>();
	}

	// Arg Count 7
	template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	inline string SqMakeOverloadString(R (C::*method)(A1, A2, A3, A4, A5, A6, A7)) {
		return GetObjectTypeName<A1>() + _SC("_") + GetObjectTypeName<A2>() 
			+ _SC("_") + GetObjectTypeName<A3>() + _SC("_") + GetObjectTypeName<A4>()
			+ _SC("_") + GetObjectTypeName<A5>() + _SC("_") + GetObjectTypeName<A6>()
			+ _SC("_") + GetObjectTypeName<A7>();
	}

	template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
	inline string SqMakeOverloadString(R (C::*method)(A1, A2, A3, A4, A5, A6, A7) const) {
		return GetObjectTypeName<A1>() + _SC("_") + GetObjectTypeName<A2>() 
			+ _SC("_") + GetObjectTypeName<A3>() + _SC("_") + GetObjectTypeName<A4>()
			+ _SC("_") + GetObjectTypeName<A5>() + _SC("_") + GetObjectTypeName<A6>()
			+ _SC("_") + GetObjectTypeName<A7>();
	}

	// Arg Count 8
	template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	inline string SqMakeOverloadString(R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8)) {
		return GetObjectTypeName<A1>() + _SC("_") + GetObjectTypeName<A2>() 
			+ _SC("_") + GetObjectTypeName<A3>() + _SC("_") + GetObjectTypeName<A4>()
			+ _SC("_") + GetObjectTypeName<A5>() + _SC("_") + GetObjectTypeName<A6>()
			+ _SC("_") + GetObjectTypeName<A7>() + _SC("_") + GetObjectTypeName<A8>();
	}

	template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
	inline string SqMakeOverloadString(R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8) const) {
		return GetObjectTypeName<A1>() + _SC("_") + GetObjectTypeName<A2>() 
			+ _SC("_") + GetObjectTypeName<A3>() + _SC("_") + GetObjectTypeName<A4>()
			+ _SC("_") + GetObjectTypeName<A5>() + _SC("_") + GetObjectTypeName<A6>()
			+ _SC("_") + GetObjectTypeName<A7>() + _SC("_") + GetObjectTypeName<A8>();
	}

	// Arg Count 9
	template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
	inline string SqMakeOverloadString(R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9)) {
		return GetObjectTypeName<A1>() + _SC("_") + GetObjectTypeName<A2>() 
			+ _SC("_") + GetObjectTypeName<A3>() + _SC("_") + GetObjectTypeName<A4>()
			+ _SC("_") + GetObjectTypeName<A5>() + _SC("_") + GetObjectTypeName<A6>()
			+ _SC("_") + GetObjectTypeName<A7>() + _SC("_") + GetObjectTypeName<A8>()
			+ _SC("_") + GetObjectTypeName<A9>();
	}

	template <class C, class R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
	inline string SqMakeOverloadString(R (C::*method)(A1, A2, A3, A4, A5, A6, A7, A8, A9) const) {
		return GetObjectTypeName<A1>() + _SC("_") + GetObjectTypeName<A2>() 
			+ _SC("_") + GetObjectTypeName<A3>() + _SC("_") + GetObjectTypeName<A4>()
			+ _SC("_") + GetObjectTypeName<A5>() + _SC("_") + GetObjectTypeName<A6>()
			+ _SC("_") + GetObjectTypeName<A7>() + _SC("_") + GetObjectTypeName<A8>()
			+ _SC("_") + GetObjectTypeName<A9>();
	}

}


#endif
