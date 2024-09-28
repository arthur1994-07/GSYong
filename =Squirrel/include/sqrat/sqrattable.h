//
// SqratTable: Table Binding
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

#if !defined(_SCRAT_TABLE_H_)
#define _SCRAT_TABLE_H_

#include <squirrel.h>
#include <string.h>

#include "sqratObject.h"
#include "sqratFunction.h"
#include "sqratGlobalMethods.h"

namespace Sqrat {


	//////////////////////////////////////////////////////////////////////////
	template<class KEY,class VALUE>
	class ForEachFunction
	{
	public:
		virtual ~ForEachFunction(){}
		virtual void operator()(const KEY& key,const VALUE& value ) = 0;
	};


	template<class KEY,class VALUE>
	void ForEach(const Object& obj,ForEachFunction<KEY,VALUE>& func)
	{
		HSQUIRRELVM vm = obj.GetVM();
		SQRAT_ASSERT_MSG( OT_TABLE == obj.GetType() || OT_ARRAY == obj.GetType() , "only table or array can ForEach" );
		sq_pushobject(vm, obj.GetObject() );
		sq_pushnull(vm);  //null iterator
		while(SQ_SUCCEEDED(sq_next(vm,-2)))
		{
			HSQOBJECT key,value;
			//here -1 is the value and -2 is the key
			sq_getstackobj(vm, -1, &value);
			sq_getstackobj(vm, -2, &key);

			func( Object::Cast<KEY>(key, vm), Object::Cast<VALUE>(value, vm) );
			sq_pop(vm,2); //pops key and val before the nex iteration
		}

		sq_pop(vm,2); //pops the null iterator , pops the container
	}




	class TableBase : public Object {
	public:
		TableBase(Object& obj) : Object(obj) { SQRAT_ASSERT_MSG( GetType() == OT_TABLE, "object is not table"); }

		TableBase(HSQUIRRELVM v = DefaultVM::Get()) : Object(v, true) {
		}
		// Bind a Table or Class to the Table (Can be used to facilitate Namespaces)
		// Note: Bind cannot be called "inline" like other functions because it introduces order-of-initialization bugs.
		void Bind(const SQChar* name, Object& obj) {
			sq_pushobject(vm, GetObject());
			sq_pushstring(vm, name, -1);
			sq_pushobject(vm, obj.GetObject());
			sq_newslot(vm, -3, false);
			sq_pop(vm,1); // pop table
		}
		
		// Bind a raw Squirrel closure to the Table
		TableBase& SquirrelFunc(const SQChar* name, SQFUNCTION func) {
			sq_pushobject(vm, GetObject());
			sq_pushstring(vm, name, -1);
			sq_newclosure(vm, func, 0);
			sq_newslot(vm, -3, false);
			sq_pop(vm,1); // pop table

			return *this;
		}


		//////////////////////////////////////////////////////////////////////////

		

		//
		// Variable Binding
		//
		
		template<class V>
		TableBase& SetValue(const SQChar* name, const V& val) {
			BindValue<V>(name, val, false);
			return *this;
		}

		template<class V>
		TableBase& SetInstance(const SQChar* name, V* val) {
			BindInstance<V>(name, val, false);
			return *this;
		}

		template<class F>
		TableBase& Func(const SQChar* name, F method) {
			BindFunc(name, &method, sizeof(method), SqGlobalFunc(method));
			return *this;
		}

		template<class F>
		TableBase& Overload(const SQChar* name, F method) {
//			BindOverload(name, &method, sizeof(method), SqGlobalFunc(method), SqOverloadFunc(method), SqGetArgCount(method));
			BindOverload(name, &method, sizeof(method), SqMemberFunc(method), 
				SqOverloadFunc(method), SqGetArgCount(method), SqMakeOverloadString(method) );

			return *this;
		}

		//
		// Function Calls
		//

		Function GetFunction(const SQChar* name) {
			HSQOBJECT funcObj;
			sq_pushobject(vm, GetObject());
			sq_pushstring(vm, name, -1);
			if(SQ_FAILED(sq_get(vm, -2))) {
				sq_pushnull(vm);
			}
			sq_getstackobj(vm, -1, &funcObj);
			sq_pop(vm, 2);

			return Function(vm, GetObject(), funcObj);
		}

	};

	class Table : public TableBase {
	public:
		Table(Object& obj) : TableBase(obj) {}
		Table(HSQUIRRELVM v = DefaultVM::Get()) : TableBase(v) {
			sq_newtable(vm);
			sq_getstackobj(vm,-1,&obj);
			sq_addref(vm, &obj);
			sq_pop(vm,1);
		}
	};


	//
	// Root Table
	//

	class RootTable : public TableBase {
	public:
		RootTable(HSQUIRRELVM v = DefaultVM::Get()) : TableBase(v) {
			sq_pushroottable(vm);
			sq_getstackobj(vm,-1,&obj);
			sq_addref(vm, &obj);
			sq_pop(v,1); // pop root table
		}
	};


	//////////////////////////////////////////////////////////////////////////

	class Array : public Object 
	{
	public:
		Array() {}
		Array(HSQOBJECT o,HSQUIRRELVM v) : Object(o,v) 
        {
            SQRAT_ASSERT_MSG( GetType() == OT_ARRAY, "object is not array");
        } 

		Array(Object& obj) : Object(obj) 
		{
			SQRAT_ASSERT_MSG( GetType() == OT_ARRAY, "object is not array");
		}

		void Clear() 
		{
			sq_pushobject(vm, GetObject());
			SQRESULT ret = sq_arrayresize(vm,-1,0);
			sq_pop(vm,-1);
			SQRAT_ASSERT_MSG(ret == SQ_OK,"check that object is array" );
		}

		void Resize(SQInteger size)
		{
			sq_pushobject(vm, GetObject());
			SQRESULT ret = sq_arrayresize(vm,-1,size);
			sq_pop(vm,-1);
			SQRAT_ASSERT_MSG(ret == SQ_OK,"check that object is array" );
		}

	protected:
		template<class V>
		inline void BindInstance(SQInteger index, V* val, bool staticVar = false) {
			sq_pushobject(vm, GetObject());
			sq_pushinteger(vm, index);
			PushVar(vm, val);
			sq_newslot(vm, -3, staticVar);
			sq_pop(vm,1); // pop table
		}

		template<class V>
		inline void BindValue(SQInteger index, const V& val, bool staticVar = false) {
			sq_pushobject(vm, GetObject());
			sq_pushinteger(vm, index);
			PushVar(vm, val);
			sq_newslot(vm, -3, staticVar);
			sq_pop(vm,1); // pop table
		}
	};


	template<>
	struct Var<Array> {
		Array value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			HSQOBJECT sqValue;
			sq_getstackobj(vm, idx, &sqValue);
			value = Array(sqValue, vm);
		}
		static void push(HSQUIRRELVM vm, Object& value) {
			sq_pushobject(vm, value.GetObject());
		}
	};

	template<>
	struct Var<Array&> {
		Array value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			HSQOBJECT sqValue;
			sq_getstackobj(vm, idx, &sqValue);
			value = Array(sqValue, vm);
		}
		static void push(HSQUIRRELVM vm, Object& value) {
			sq_pushobject(vm, value.GetObject());
		}
	};

	template<>
	struct Var<const Array&> {
		Array value;
		Var(HSQUIRRELVM vm, SQInteger idx) {
			HSQOBJECT sqValue;
			sq_getstackobj(vm, idx, &sqValue);
			value = Array(sqValue, vm);
		}
		static void push(HSQUIRRELVM vm, Object& value) {
			sq_pushobject(vm, value.GetObject());
		}
	};



	namespace detail
	{
		template<class T>
		struct ForEachToVector : public ForEachFunction<int,T> 
		{
			ForEachToVector() { pVector = NULL; }
			SqStdVector<T>* pVector;

			virtual void operator()(const int& ,const T& value ) { pVector->push_back(value); }
		};

		template<class KEY,class VALUE>
		struct ForEachToMap : public ForEachFunction<KEY,VALUE> 
		{
			ForEachToMap() { pMap = NULL; }
			SqStdMap<KEY,VALUE>* pMap;

			virtual void operator()(const KEY& key,const VALUE& value ) 
			{
				pMap->insert(std::pair<KEY,VALUE>(key,value)); 
			}
		};

	}

	template<class T>
	void GetVectorFromArray(const Array& obj,SqStdVector<T>* pVector)
	{
		pVector->clear();
		pVector->reserve( obj.GetSize() );
		detail::ForEachToVector<T> foreachToVector;
		foreachToVector.pVector = pVector;
		ForEach(obj,foreachToVector);
	}

	template<class KEY,class VALUE>
	void GetMapFromTable(const Table& obj,SqStdMap<KEY,VALUE>* pMap)
	{
		pMap->clear();
		detail::ForEachToMap<KEY,VALUE> foreachToMap;
		foreachToMap.pMap = pMap;
		ForEach(obj,foreachToMap);
	}

}

#endif
