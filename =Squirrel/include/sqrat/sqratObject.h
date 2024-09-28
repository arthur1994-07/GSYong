//
// SqratObject: Referenced Squirrel Object Wrapper
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

#if !defined(_SCRAT_OBJECT_H_)
#define _SCRAT_OBJECT_H_

#include <squirrel.h>
#include <string.h>
#include "sqratTypes.h"
#include "sqratOverloadMethods.h"
#include "sqratUtil.h"

namespace Sqrat 
{
	class Object 
	{
	protected:
		HSQUIRRELVM vm;
		HSQOBJECT obj;
		bool release;

		Object(HSQUIRRELVM v, bool releaseOnDestroy = true) : vm(v), release(releaseOnDestroy) {
			sq_resetobject(&obj);
		}

	public:
		// 함수 내부적으로 쓰자 (실수 방지)
		struct AutoStack
		{
			AutoStack(HSQUIRRELVM v = DefaultVM::Get()) 
			{
				vm = v;
				top = sq_gettop(vm); 
			}

			~AutoStack() { sq_pop(vm, sq_gettop(vm) - top ); }

		private:
			HSQUIRRELVM vm;
			SQInteger top;
		};

		//////////////////////////////////////////////////////////////////////////

		Object() : vm(0), release(true) {
			sq_resetobject(&obj);
		}

		Object(const Object& so) : vm(so.vm), obj(so.obj), release(so.release) {
			sq_addref(vm, &obj);
		}

		Object(HSQOBJECT o, HSQUIRRELVM v = DefaultVM::Get()) : vm(v), obj(o), release(true) {
			sq_addref(vm, &obj);
		}

		template<class T>
		Object(T* instance, HSQUIRRELVM v = DefaultVM::Get()) : vm(v), release(true) {
			ClassType<T>::PushInstance(vm, instance);
			sq_getstackobj(vm, -1, &obj);
			sq_addref(vm, &obj);
		}

		virtual ~Object() {
			if(release) {
				Release();
			}
		}

		Object& operator=(const Object& so) {
			if(release) {
				Release();
			}
			vm = so.vm;
			obj = so.obj;
			release = so.release;
			sq_addref(vm, &GetObject());
			return *this;
		}

		HSQUIRRELVM& GetVM() {
			return vm;
		}

		HSQUIRRELVM GetVM() const {
			return vm;
		}

		SQObjectType GetType() const {
			return GetObject()._type;
		}

		bool IsNull() const {
			return sq_isnull(GetObject());
		}

		virtual HSQOBJECT GetObject() const {
			return obj;
		}

		virtual HSQOBJECT& GetObject() {
			return obj;
		}

		operator HSQOBJECT&() {
			return GetObject();
		}

		void Release() {
			sq_release(vm, &obj);
		}

		SQUserPointer GetInstanceUP(SQUserPointer tag = NULL) const {
			SQUserPointer up;
			sq_pushobject(vm, GetObject());
			sq_getinstanceup(vm, -1, &up, tag);
			sq_pop(vm, 1);
			return up;
		}

		Object GetSlot(const SQChar* slot) const {
			HSQOBJECT slotObj;
			sq_pushobject(vm, GetObject());
			sq_pushstring(vm, slot, -1);
			if(SQ_FAILED(sq_get(vm, -2))) {
				sq_pop(vm, 1);
				return Object(vm); // Return a NULL object
			} else {
				sq_getstackobj(vm, -1, &slotObj);
				sq_pop(vm, 2);
				return Object(slotObj, vm);
			}
		}

		void SetSlotNull(const SQChar* name) 
		{
			AutoStack aStack;
			sq_pushobject(vm, GetObject());
			sq_pushstring(vm, name, -1);
			sq_pushnull(vm);
			bool ret = SQ_FAILED(sq_get(vm, -3));
			SQRAT_ASSERT(ret);
		}


		template <class T>
		T Cast() const {
			sq_pushobject(vm, GetObject());
			T ret = Var<T>(vm, -1).value;
			sq_pop(vm, 1);
			return ret;
		}

		template <class T>
		static T Cast(const HSQOBJECT& o, HSQUIRRELVM vm = DefaultVM::Get()) 
		{
			sq_pushobject(vm, o);
			T ret = Var<T>(vm, -1).value;
			sq_pop(vm, 1);
			return ret;
		}

		SQInteger GetSize() const
		{
			AutoStack aStack;
			sq_pushobject(vm, GetObject() );
			SQInteger size = sq_getsize(vm,-1);
			SQRAT_ASSERT_MSG( size >= 0 , "container failed to get size");
			return size;
		}

		//////////////////////////////////////////////////////////////////////////

	protected:
		// Bind a function and it's associated Squirrel closure to the object
		inline void BindFunc(const SQChar* name, void* method, size_t methodSize, SQFUNCTION func, bool staticVar = false) {
			sq_pushobject(vm, GetObject());
			sq_pushstring(vm, name, -1);

			SQUserPointer methodPtr = sq_newuserdata(vm, static_cast<SQUnsignedInteger>(methodSize));
			memcpy(methodPtr, method, methodSize);

			sq_newclosure(vm, func, 1);
			sq_newslot(vm, -3, staticVar);
			sq_pop(vm,1); // pop table
		}

		// Bind a function and it's associated Squirrel closure to the object
		inline void BindOverload(const SQChar* name, void* method, size_t methodSize, 
			SQFUNCTION func, SQFUNCTION overload, int argCount, const string& sqMakeOverloadString,bool staticVar = false) {
			SQRAT_ASSERT(!sqMakeOverloadString.empty());

			sq_pushobject(vm, GetObject());

			// Bind overload handler
			sq_pushstring(vm, name, -1);
			sq_pushstring(vm, name, -1); // function name is passed as a free variable
			sq_newclosure(vm, overload, 1);
			sq_newslot(vm, -3, staticVar);

			string overloadName = string(name) + _SC("_") + sqMakeOverloadString;//SqOverloadName::Get(name, argCount);

			// Bind overloaded function
			sq_pushstring(vm, overloadName.c_str(), -1);
			SQUserPointer methodPtr = sq_newuserdata(vm, static_cast<SQUnsignedInteger>(methodSize));
			memcpy(methodPtr, method, methodSize);
			sq_newclosure(vm, func, 1);
			sq_newslot(vm, -3, staticVar);

			sq_pop(vm,1); // pop table
		}

		// Set the value of a variable on the object. Changes to values set this way are not reciprocated
		template<class V>
		inline void BindValue(const SQChar* name, const V& val, bool staticVar = false) {
			sq_pushobject(vm, GetObject());
			sq_pushstring(vm, name, -1);
			PushVar(vm, val);
			sq_newslot(vm, -3, staticVar);
			sq_pop(vm,1); // pop table
		}

		// Set the value of an instance on the object. Changes to values set this way are reciprocated back to the source instance
		template<class V>
		inline void BindInstance(const SQChar* name, V* val, bool staticVar = false) {
			sq_pushobject(vm, GetObject());
			sq_pushstring(vm, name, -1);
			PushVar(vm, val);
			sq_newslot(vm, -3, staticVar);
			sq_pop(vm,1); // pop table
		}

		
	};

	//
	// Overridden Getter/Setter
	//

	template<>
	struct Var<Object> {
		Object value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			HSQOBJECT sqValue;
			sq_getstackobj(vm, idx, &sqValue);
			value = Object(sqValue, vm);
		}
		static void push(HSQUIRRELVM vm, Object& value) {
			sq_pushobject(vm, value.GetObject());
		}
	};

	template<>
	struct Var<Object&> {
		Object value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			HSQOBJECT sqValue;
			sq_getstackobj(vm, idx, &sqValue);
			value = Object(sqValue, vm);
		}
		static void push(HSQUIRRELVM vm, Object& value) {
			sq_pushobject(vm, value.GetObject());
		}
	};

	template<>
	struct Var<const Object&> {
		Object value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			HSQOBJECT sqValue;
			sq_getstackobj(vm, idx, &sqValue);
			value = Object(sqValue, vm);
		}
		static void push(HSQUIRRELVM vm, Object& value) {
			sq_pushobject(vm, value.GetObject());
		}
	};


	//////////////////////////////////////////////////////////////////////////
	class Thread : public Object
	{
	public:
		Thread(){}
		Thread(HSQOBJECT o,HSQUIRRELVM v) : Object(o,v) {}
	};

	template<>
	struct Var<Thread> {
		Thread value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			HSQOBJECT sqValue;
			sq_getstackobj(vm, idx, &sqValue);
			value = Thread(sqValue, vm);
		}
		static void push(HSQUIRRELVM vm, Object& value) {
			sq_pushobject(vm, value.GetObject());
		}
	};

	template<>
	struct Var<Thread&> {
		Thread value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			HSQOBJECT sqValue;
			sq_getstackobj(vm, idx, &sqValue);
			value = Thread(sqValue, vm);
		}
		static void push(HSQUIRRELVM vm, Object& value) {
			sq_pushobject(vm, value.GetObject());
		}
	};

	template<>
	struct Var<const Thread&> {
		Thread value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			HSQOBJECT sqValue;
			sq_getstackobj(vm, idx, &sqValue);
			value = Thread(sqValue, vm);
		}
		static void push(HSQUIRRELVM vm, Object& value) {
			sq_pushobject(vm, value.GetObject());
		}
	};
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////


	namespace detail
	{
		template<class T>
		void CheckVar(T& value,HSQUIRRELVM vm, SQInteger idx) 
		{
			HSQOBJECT obj;
			sq_getstackobj(vm, idx, &obj);

			SQRAT_ASSERT_ARG_TYPE( !sq_isnull(obj) ,vm,idx ,"object is null");
			SQRAT_ASSERT_ARG_TYPE( sq_isinstance(obj) ,vm, idx , "object is not instance");

			const SqObjectType* pId = ClassType<T>::GetInstance(vm, idx);
			// 반드시 SqTypeID 를 상속한 타입이어야 하므로 컴파일 타임때 static_cast로 확인한다
			static_cast<const T*>(pId);
			const T* pValue = dynamic_cast<const T*>(pId);
			SQRAT_ASSERT_ARG_TYPE( pValue ? true : false ,vm, idx , (_SC("arg must be ") + SQRAT_GET_CLASS_NAME(T)).c_str() );
			if( !pValue )
				return;

			value = *pValue;
		}

		template<class T>
		void CheckVar(T*& value,HSQUIRRELVM vm, SQInteger idx) 
		{
			HSQOBJECT obj;
			sq_getstackobj(vm, idx, &obj);

			SQRAT_ASSERT_ARG_TYPE( !sq_isnull(obj) ,vm, idx , "object is null");
			SQRAT_ASSERT_ARG_TYPE( sq_isinstance(obj) ,vm, idx , "object is not instance");

			value = ClassType<T>::GetInstance(vm, idx);

			SqObjectType* pId = value;
			// 반드시 SqTypeID 를 상속한 타입이어야 하므로 컴파일 타임때 static_cast로 확인한다
			static_cast<const T*>(pId);
			T* pValue = dynamic_cast<T*>(pId);
			SQRAT_ASSERT_ARG_TYPE( pValue ? true : false ,vm, idx , (_SC("arg must be ") + SQRAT_GET_CLASS_NAME(T)).c_str() );
			if( !pValue )
				return;

			value = pValue;
		}
 
	
	
	}

}

#endif
