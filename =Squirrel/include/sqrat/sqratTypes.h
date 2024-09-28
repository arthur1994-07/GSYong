//
// SqratTypes: Type Translators
//

//
// Copyright (c) 2009 Brandon Jones
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
//	1. The origin of this software must not be misrepresented; you must not
//	claim that you wrote the original software. If you use this software
//	in a product, an acknowledgment in the product documentation would be
//	appreciated but is not required.
//
//	2. Altered source versions must be plainly marked as such, and must not be
//	misrepresented as being the original software.
//
//	3. This notice may not be removed or altered from any source
//	distribution.
//

#if !defined(_SCRAT_TYPES_H_)
#define _SCRAT_TYPES_H_

#include <squirrel.h>
#include <string>

#include "sqratClassType.h"

#ifndef SqStdString
#define SqStdString std::basic_string< SQChar >
#endif


namespace Sqrat {



	//
	// Variable Accessors
	//

	// Generic classes
	template<class T>
	struct Var {
		T value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();
			detail::CheckVar<T>(value,vm,idx);	
		}
		static void push(HSQUIRRELVM vm, T value) {
			// 혹시라도 SqObjectType 타입이 아닌넘은 컴파일 타임때 막자
			//static_cast<SqObjectType*>(&value);
			ClassType<T>::PushInstanceCopy(vm, value);
		}
	};

	template<class T>
	struct Var<T&> {
		T value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();
			detail::CheckVar<T>(value,vm,idx);	
		}
		static void push(HSQUIRRELVM vm, T value) {
			// 혹시라도 SqObjectType 타입이 아닌넘은 컴파일 타임때 막자
			static_cast<SqObjectType*>(&value);
			ClassType<T>::PushInstanceCopy(vm, value);
		}
	};

	template<class T>
	struct Var<T*> {
		T* value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();
			detail::CheckVar<T>(value,vm,idx);	
		}
		static void push(HSQUIRRELVM vm, T* value) {
			// 혹시라도 SqObjectType 타입이 아닌넘은 컴파일 타임때 막자
			static_cast<SqObjectType*>(value);
			ClassType<T>::PushInstance(vm, value);
		}
	};

	template<class T>
	struct Var<const T&> {
		T value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();
			detail::CheckVar<T>(value,vm,idx);	
		}
		static void push(HSQUIRRELVM vm, T value) {
			// 혹시라도 SqObjectType 타입이 아닌넘은 컴파일 타임때 막자
			static_cast<SqObjectType*>(&value);
			ClassType<T>::PushInstanceCopy(vm, value);
		}
	};

	template<class T>
	struct Var<const T*> {
		T* value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();
			detail::CheckVar<T>(value,vm,idx);	
		} 
		static void push(HSQUIRRELVM vm, T* value) {
			// 혹시라도 SqObjectType 타입이 아닌넘은 컴파일 타임때 막자
			static_cast<SqObjectType*>(value);
			ClassType<T>::PushInstance(vm, value);
		}
	};

	// Integer Types
	#define SCRAT_INTEGER( type ) \
	template<> \
	struct Var<type> { \
		type value; \
		Var(HSQUIRRELVM vm, SQInteger idx) { \
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();\
			SQInteger sqValue; \
			SQRESULT ret = sq_getinteger(vm, idx, &sqValue); \
			SQRAT_ASSERT_ARG_TYPE(ret == SQ_OK,vm, idx ,"not integer type"); \
			value = static_cast<type>(sqValue); \
		} \
		static void push(HSQUIRRELVM vm, type& value) { \
			sq_pushinteger(vm, static_cast<SQInteger>(value)); \
		} \
	};\
	\
	template<> \
	struct Var<const type> { \
		type value; \
		Var(HSQUIRRELVM vm, SQInteger idx) { \
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();\
			SQInteger sqValue; \
			SQRESULT ret = sq_getinteger(vm, idx, &sqValue); \
			SQRAT_ASSERT_ARG_TYPE(ret == SQ_OK, vm,idx ,"not integer type"); \
			value = static_cast<type>(sqValue); \
		} \
		static void push(HSQUIRRELVM vm, const type& value) { \
			sq_pushinteger(vm, static_cast<SQInteger>(value)); \
		} \
	}; \
	\
	template<> \
	struct Var<const type&> { \
		type value; \
		Var(HSQUIRRELVM vm, SQInteger idx) { \
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();\
			SQInteger sqValue; \
			SQRESULT ret = sq_getinteger(vm, idx, &sqValue); \
			SQRAT_ASSERT_ARG_TYPE(ret == SQ_OK,vm, idx ,"not integer type"); \
			value = static_cast<type>(sqValue); \
		} \
		static void push(HSQUIRRELVM vm, const type& value) { \
			sq_pushinteger(vm, static_cast<SQInteger>(value)); \
		} \
	};

	SCRAT_INTEGER(unsigned int)
	SCRAT_INTEGER(signed int)
	SCRAT_INTEGER(unsigned long)
	SCRAT_INTEGER(signed long)
	SCRAT_INTEGER(unsigned short)
	SCRAT_INTEGER(signed short)
	SCRAT_INTEGER(unsigned __int64)
	SCRAT_INTEGER(signed __int64)

	// Float Types
	#define SCRAT_FLOAT( type ) \
	template<> \
	struct Var<type> { \
		type value; \
		Var(HSQUIRRELVM vm, SQInteger idx) { \
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();\
			SQFloat sqValue; \
			SQRESULT ret = sq_getfloat(vm, idx, &sqValue); \
			SQRAT_ASSERT_ARG_TYPE(ret == SQ_OK, vm,idx ,"not float type"); \
			value = static_cast<type>(sqValue); \
		} \
		static void push(HSQUIRRELVM vm, type& value) { \
			sq_pushfloat(vm, static_cast<SQFloat>(value)); \
		} \
	}; \
	\
	template<> \
	struct Var<const type> { \
		type value; \
		Var(HSQUIRRELVM vm, SQInteger idx) { \
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();\
			SQFloat sqValue; \
			SQRESULT ret = sq_getfloat(vm, idx, &sqValue); \
			SQRAT_ASSERT_ARG_TYPE(ret == SQ_OK, vm,idx ,"not float type"); \
			value = static_cast<type>(sqValue); \
		} \
		static void push(HSQUIRRELVM vm, const type& value) { \
			sq_pushfloat(vm, static_cast<SQFloat>(value)); \
		} \
	}; \
	template<> \
	struct Var<const type&> { \
		type value; \
		Var(HSQUIRRELVM vm, SQInteger idx) { \
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();\
			SQFloat sqValue; \
			SQRESULT ret = sq_getfloat(vm, idx, &sqValue); \
			SQRAT_ASSERT_ARG_TYPE(ret == SQ_OK,vm, idx ,"not float type"); \
			value = static_cast<type>(sqValue); \
		} \
		static void push(HSQUIRRELVM vm, const type& value) { \
			sq_pushfloat(vm, static_cast<SQFloat>(value)); \
		} \
	};

	SCRAT_FLOAT(float)
	SCRAT_FLOAT(double)

	// Boolean Types
	template<>
	struct Var<bool> {
		bool value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();\
			SQBool sqValue;
			SQRESULT ret = sq_getbool(vm, idx, &sqValue);
			SQRAT_ASSERT_ARG_TYPE(ret == SQ_OK,vm, idx ,"not bool type");
			value = (sqValue != 0);
		}
		static void push(HSQUIRRELVM vm, bool& value) {
			sq_pushbool(vm, static_cast<SQBool>(value));
		}
	};

	template<>
	struct Var<const bool> {
		bool value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();\
			SQBool sqValue;
			SQRESULT ret = sq_getbool(vm, idx, &sqValue);
			SQRAT_ASSERT_ARG_TYPE(ret == SQ_OK,vm, idx ,"not bool type");
			value = (sqValue != 0);
		}
		static void push(HSQUIRRELVM vm, const bool& value) {
			sq_pushbool(vm, static_cast<SQBool>(value));
		}
	};

	template<>
	struct Var<const bool&> {
		bool value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();\
			SQBool sqValue;
			SQRESULT ret = sq_getbool(vm, idx, &sqValue);
			SQRAT_ASSERT_ARG_TYPE(ret == SQ_OK,vm, idx ,"not bool type");
			value = (sqValue != 0);
		}
		static void push(HSQUIRRELVM vm, const bool& value) {
			sq_pushbool(vm, static_cast<SQBool>(value));
		}
	};

	// String Types
	typedef SqStdString string;

	template<>
	struct Var<SQChar*> {
		SQChar* value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();\
			sq_tostring(vm, idx);
			SQRESULT ret = sq_getstring(vm, -1, (const SQChar**)&value);
			sq_pop(vm,1);
			SQRAT_ASSERT_ARG_TYPE(ret == SQ_OK,vm, idx ,"not String type");
		}
		static void push(HSQUIRRELVM vm, SQChar* value) {
			sq_pushstring(vm, value, -1);
		}
	};

	template<>
	struct Var<const SQChar*> {
		const SQChar* value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();\
			sq_tostring(vm, idx);
			SQRESULT ret = sq_getstring(vm, -1, &value);
			sq_pop(vm,1);
			SQRAT_ASSERT_ARG_TYPE(ret == SQ_OK,vm, idx ,"not String type");
		}
		static void push(HSQUIRRELVM vm, const SQChar* value) {
			sq_pushstring(vm, value, -1);
		}
	};

	template<>
	struct Var<string> {
		string value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();\
			const SQChar* sqValue;
			sq_tostring(vm, idx);
			SQRESULT ret = sq_getstring(vm, -1, &sqValue);
			value = string(sqValue);
			sq_pop(vm,1);
			SQRAT_ASSERT_ARG_TYPE(ret == SQ_OK,vm, idx ,"not String type");
		}
		static void push(HSQUIRRELVM vm, string value) {
			sq_pushstring(vm, value.c_str(), -1);
		}
	};

	template<>
	struct Var<string&> {
		string value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();\
			const SQChar* sqValue;
			sq_tostring(vm, idx);
			SQRESULT ret = sq_getstring(vm, -1, &sqValue);
			value = string(sqValue);
			sq_pop(vm,1);
			SQRAT_ASSERT_ARG_TYPE(ret == SQ_OK,vm, idx ,"not String type");
		}
		static void push(HSQUIRRELVM vm, string value) {
			sq_pushstring(vm, value.c_str(), -1);
		}
	};

	template<>
	struct Var<const string&> {
		string value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			SQRAT_ASSERT_ARG_COUNT_IF_IS_INVALID_RETURN();\
			const SQChar* sqValue;
			sq_tostring(vm, idx);
			SQRESULT ret = sq_getstring(vm, -1, &sqValue);
			value = string(sqValue);
			sq_pop(vm,1);
			SQRAT_ASSERT_ARG_TYPE(ret == SQ_OK,vm, idx ,"not String type");
		}
		static void push(HSQUIRRELVM vm, string value) {
			sq_pushstring(vm, value.c_str(), -1);
		}
	};

	//
	// Variable Accessors
	//

	// Push
	template<class T>
	inline void PushVar(HSQUIRRELVM vm, T value) {
		Var<T>::push(vm, value);
	}
}

#endif
