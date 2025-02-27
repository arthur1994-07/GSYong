// lua_tinker.h
//
// LuaTinker - Simple and light C++ wrapper for Lua.
//
// Copyright (c) 2005-2007 Kwon-il Lee (zupet@hitel.net)
// 
// please check Licence.txt file for licence and legal issues. 

#if !defined(_LUA_TINKER_H_)
#define _LUA_TINKER_H_

#pragma warning(disable:4996)

#include <new>
#include <string>
#include "../../=LuaPlus/src/LuaPlus/src/lua.h"

namespace lua_tinker
{
	// init LuaTinker
	void	init(lua_State *L);

	void	init_s64(lua_State *L);
	void	init_u64(lua_State *L);

	// excution
	bool	dofile(lua_State *L, const char *filename);
	bool	dofile(lua_State *L, const wchar_t *filename);

	void	dostring(lua_State *L, const char* buff);

	bool	dobuffer(lua_State *L, const char* buff, size_t sz);

	// debug helpers
	void	enum_stack(lua_State *L);
	int		on_error(lua_State *L);
	void	print_error(lua_State *L, const char* fmt, ...);

	// dynamic type extention
	struct lua_value
	{
		virtual void to_lua(lua_State *L) = 0;
	};

	// type trait
	template<typename T> struct class_name;
	struct table;

template<bool C, typename A, typename B> struct if_ {};
template<typename A, typename B>		struct if_<true, A, B> { typedef A type; };
template<typename A, typename B>		struct if_<false, A, B> { typedef B type; };

	template<typename A>
	struct is_ptr { static const bool value = false; };
	template<typename A>
	struct is_ptr<A*> { static const bool value = true; };

	template<typename A>
	struct is_ref { static const bool value = false; };
	template<typename A>
	struct is_ref<A&> { static const bool value = true; };

	template<typename A>
	struct remove_const { typedef A type; };
	template<typename A>
	struct remove_const<const A> { typedef A type; };

	template<typename A>
	struct base_type { typedef A type; };
	template<typename A>
	struct base_type<A*> { typedef A type; };
	template<typename A>
	struct base_type<A&> { typedef A type; };

	template<typename A>
	struct class_type { typedef typename remove_const<typename base_type<A>::type>::type type; };
	
	/////////////////////////////////
	enum { no = 1, yes = 2 }; 
	typedef char (& no_type )[no]; 
	typedef char (& yes_type)[yes]; 

	struct int_conv_type { int_conv_type(int); }; 

	no_type int_conv_tester (...); 
	yes_type int_conv_tester (int_conv_type); 

	no_type vfnd_ptr_tester (const volatile char *); 
	no_type vfnd_ptr_tester (const volatile short *); 
	no_type vfnd_ptr_tester (const volatile int *); 
	no_type vfnd_ptr_tester (const volatile long *); 
	no_type vfnd_ptr_tester (const volatile double *); 
	no_type vfnd_ptr_tester (const volatile float *); 
	no_type vfnd_ptr_tester (const volatile bool *); 
	yes_type vfnd_ptr_tester (const volatile void *); 

	template <typename T> T* add_ptr(T&); 

	template <bool C> struct bool_to_yesno { typedef no_type type; }; 
	template <> struct bool_to_yesno<true> { typedef yes_type type; }; 

	template <typename T> 
	struct is_enum 
	{ 
		static T arg; 
		static const bool value = ( (sizeof(int_conv_tester(arg)) == sizeof(yes_type)) && (sizeof(vfnd_ptr_tester(add_ptr(arg))) == sizeof(yes_type)) ); 
	}; 
	/////////////////////////////////

	// from lua
	template<typename T>
	struct void2val { static T invoke(void* input){ return *(T*)input; } };
	template<typename T>
	struct void2ptr { static T* invoke(void* input){ return (T*)input; } };
	template<typename T>
	struct void2ref { static T& invoke(void* input){ return *(T*)input; } };

	template<typename T>  
	struct void2type
	{
		static T invoke(void* ptr)
		{
			return	if_<is_ptr<T>::value
						,void2ptr<base_type<T>::type>
						,if_<is_ref<T>::value
							,void2ref<base_type<T>::type>
							,void2val<base_type<T>::type>
						>::type
					>::type::invoke(ptr);
		}
	};

	template<typename T>  
	struct user2type { static T invoke(lua_State *L, int index) { return void2type<T>::invoke(lua_touserdata(L, index)); } };

	template<typename T>
	struct lua2enum { static T invoke(lua_State *L, int index) { return (T)(int)lua_tonumber(L, index); } };

	template<typename T>
	struct lua2object
	{ 
		static T invoke(lua_State *L, int index) 
		{ 
			if(!lua_isuserdata(L,index))
			{
				lua_pushstring(L, "no class at first argument. (forgot ':' expression ?)");
				lua_error(L);
			}
			return void2type<T>::invoke(user2type<user*>::invoke(L,index)->m_p); 
		} 
	};

	template<typename T>
	T lua2type(lua_State *L, int index)
	{
		return	if_<is_enum<T>::value
					,lua2enum<T>
					,lua2object<T> 
				>::type::invoke(L, index);
	}

	struct user
	{
		user(void* p) : m_p(p) {}
		virtual ~user() {}
		void* m_p;
	};

	template<typename T>
	struct val2user : user
	{
		val2user() : user(new T) {}

		template<typename T1>
		val2user(T1 t1) : user(new T(t1)) {}

		template<typename T1, typename T2>
		val2user(T1 t1, T2 t2) : user(new T(t1, t2)) {}

		template<typename T1, typename T2, typename T3>
		val2user(T1 t1, T2 t2, T3 t3) : user(new T(t1, t2, t3)) {}

		~val2user() { delete ((T*)m_p); }
	};

	template<typename T>
	struct ptr2user : user
	{
		ptr2user(T* t) : user((void*)t) {}
	};

	template<typename T>
	struct ref2user : user
	{
		ref2user(T& t) : user(&t) {}
	};

	// to lua
	template<typename T>
	struct val2lua { static void invoke(lua_State *L, T& input){ new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(input); } };
	template<typename T>
	struct ptr2lua { static void invoke(lua_State *L, T* input){ if(input) new(lua_newuserdata(L, sizeof(ptr2user<T>))) ptr2user<T>(input); else lua_pushnil(L); } };
	template<typename T>
	struct ref2lua { static void invoke(lua_State *L, T& input){ new(lua_newuserdata(L, sizeof(ref2user<T>))) ref2user<T>(input); } };

	template<typename T>
	struct enum2lua { static void invoke(lua_State *L, T val) { lua_pushnumber(L, (int)val); } };

	template<typename T>
	struct object2lua 
	{ 
		static void invoke(lua_State *L, T val) 
		{ 
			if_<is_ptr<T>::value
				,ptr2lua<base_type<T>::type>
				,if_<is_ref<T>::value
					,ref2lua<base_type<T>::type>
					,val2lua<base_type<T>::type>
				>::type
			>::type::invoke(L, val);

			meta_push(L, class_name<class_type<T>::type>::name());
			lua_setmetatable(L, -2);
		} 
	};

	template<typename T>
	void type2lua(lua_State *L, T val)
	{
		if_<is_enum<T>::value
			,enum2lua<T>
			,object2lua<T>
		>::type::invoke(L, val);
	}

	// get value from cclosure
	template<typename T>  
	T upvalue_(lua_State *L)
	{
		return user2type<T>::invoke(L, lua_upvalueindex(1));
	}

	// read a value from lua stack 
	template<typename T>  
	T read(lua_State *L, int index)				{ return lua2type<T>(L, index); }

	template<>	char*				read(lua_State *L, int index);
	template<>	const char*			read(lua_State *L, int index);
	template<>	char				read(lua_State *L, int index);
	template<>	unsigned char		read(lua_State *L, int index);
	template<>	short				read(lua_State *L, int index);
	template<>	unsigned short		read(lua_State *L, int index);
	template<>	long				read(lua_State *L, int index);
	template<>	unsigned long		read(lua_State *L, int index);
	template<>	int					read(lua_State *L, int index);
	template<>	unsigned int		read(lua_State *L, int index);
	template<>	float				read(lua_State *L, int index);
	template<>	double				read(lua_State *L, int index);
	template<>	bool				read(lua_State *L, int index);
	template<>	void				read(lua_State *L, int index);
	template<>	__int64				read(lua_State *L, int index);
	template<>	unsigned __int64	read(lua_State *L, int index);
	template<>	table				read(lua_State *L, int index);

	// push a value to lua stack 
	template<typename T>  
	void push(lua_State *L, T ret)					{ type2lua<T>(L, ret); }
	
	template<>	void push(lua_State *L, char ret);
	template<>	void push(lua_State *L, unsigned char ret);
	template<>	void push(lua_State *L, short ret);
	template<>	void push(lua_State *L, unsigned short ret);
	template<>	void push(lua_State *L, long ret);
	template<>	void push(lua_State *L, unsigned long ret);
	template<>	void push(lua_State *L, int ret);
	template<>	void push(lua_State *L, unsigned int ret);
	template<>	void push(lua_State *L, float ret);
	template<>	void push(lua_State *L, double ret);
	template<>	void push(lua_State *L, char* ret);
	template<>	void push(lua_State *L, const char* ret);
	template<>	void push(lua_State *L, std::string ret);
	template<>	void push(lua_State *L, bool ret);
	template<>	void push(lua_State *L, lua_value* ret);
	template<>	void push(lua_State *L, __int64 ret);
	template<>	void push(lua_State *L, unsigned __int64 ret);
	template<>	void push(lua_State *L, table ret);

	// pop a value from lua stack
	template<typename T>  
	T pop(lua_State *L) { T t = read<T>(L, -1); lua_pop(L, 1); return t; }
	
	template<>	void	pop(lua_State *L);
	template<>	table	pop(lua_State *L);

	// functor
	template<typename T1=void, typename T2=void, typename T3=void, typename T4=void, typename T5=void, typename T6=void, typename T7=void, typename T8=void>
	struct functor
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6,T7,T8)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7),read<T8>(L,8))); return 1; }
		template<>
		static int invoke<void>(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5,T6,T7,T8)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7),read<T8>(L,8)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	struct functor<T1,T2,T3,T4,T5,T6,T7>
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6,T7)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7))); return 1; }
		template<>
		static int invoke<void>(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5,T6,T7)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	struct functor<T1,T2,T3,T4,T5,T6>
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6))); return 1; }
		template<>
		static int invoke<void>(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5,T6)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	struct functor<T1,T2,T3,T4,T5>
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5))); return 1; }
		template<>
		static int invoke<void>(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4>
	struct functor<T1,T2,T3,T4> 
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4))); return 1; }
		template<>
		static int invoke<void>(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4)); return 0; }
	};

	template<typename T1, typename T2, typename T3>
	struct functor<T1,T2,T3> 
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3))); return 1; }
		template<>
		static int invoke<void>(lua_State *L) { upvalue_<void(*)(T1,T2,T3)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3)); return 0; }
	};

	template<typename T1, typename T2>
	struct functor<T1,T2> 
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2)>(L)(read<T1>(L,1),read<T2>(L,2))); return 1; }
		template<>
		static int invoke<void>(lua_State *L) { upvalue_<void(*)(T1,T2)>(L)(read<T1>(L,1),read<T2>(L,2)); return 0; }
	};

	template<typename T1>
	struct functor<T1> 
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1)>(L)(read<T1>(L,1))); return 1; }
		template<>
		static int invoke<void>(lua_State *L) { upvalue_<void(*)(T1)>(L)(read<T1>(L,1)); return 0; }
	};

	template<>
	struct functor<>
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)()>(L)()); return 1; }
		template<>
		static int invoke<void>(lua_State *L) { upvalue_<void(*)()>(L)(); return 0; }
	};

	// push_functor
	template<typename RVal> 
	void push_functor(lua_State *L, RVal (*func)())
	{
		lua_pushcclosure(L, functor<>::invoke<RVal>, 1);
	}

	template<typename RVal, typename T1> 
	void push_functor(lua_State *L, RVal (*func)(T1))
	{ 
		lua_pushcclosure(L, functor<T1>::invoke<RVal>, 1);
	}

	template<typename RVal, typename T1, typename T2> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2))
	{ 
		lua_pushcclosure(L, functor<T1,T2>::invoke<RVal>, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3))
	{ 
		lua_pushcclosure(L, functor<T1,T2,T3>::invoke<RVal>, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4))
	{ 
		lua_pushcclosure(L, functor<T1,T2,T3,T4>::invoke<RVal>, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5))
	{ 
		lua_pushcclosure(L, functor<T1,T2,T3,T4,T5>::invoke<RVal>, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> 
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5,T6))
	{ 
		lua_pushcclosure(L, functor<T1,T2,T3,T4,T5,T6>::invoke<RVal>, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5,T6,T7))
	{ 
		lua_pushcclosure(L, functor<T1,T2,T3,T4,T5,T6,T7>::invoke<RVal>, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5,T6,T7,T8))
	{ 
		lua_pushcclosure(L, functor<T1,T2,T3,T4,T5,T6,T7,T8>::invoke<RVal>, 1);
	}

	// member variable
	struct var_base
	{
		virtual void get(lua_State *L) = 0;
		virtual void set(lua_State *L) = 0;
	};

	template<typename T, typename V>
	struct mem_var : var_base
	{
		V T::*_var;
		mem_var(V T::*val) : _var(val) {}
		void get(lua_State *L)	{ push(L, read<T*>(L,1)->*(_var));		}
		void set(lua_State *L)	{ read<T*>(L,1)->*(_var) = read<V>(L, 3);	}
	};

	// member function
	template<typename T, typename T1=void, typename T2=void, typename T3=void, typename T4=void, typename T5=void, typename T6=void, typename T7=void, typename T8=void>
	struct mem_functor
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6,T7,T8)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8),read<T8>(L,9)));; return 1; }
		template<>
		static int invoke<void>(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6,T7,T8)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8),read<T8>(L,9)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	struct mem_functor<T,T1,T2,T3,T4,T5,T6,T7>
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6,T7)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8)));; return 1; }
		template<>
		static int invoke<void>(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6,T7)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	struct mem_functor<T,T1,T2,T3,T4,T5,T6>
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7)));; return 1; }
		template<>
		static int invoke<void>(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
	struct mem_functor<T,T1,T2,T3,T4,T5>
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6)));; return 1; }
		template<>
		static int invoke<void>(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4> 
	struct mem_functor<T,T1,T2,T3,T4>
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5))); return 1; }
		template<>
		static int invoke<void>(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3> 
	struct mem_functor<T,T1,T2,T3>
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4))); return 1; }
		template<>
		static int invoke<void>(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4)); return 0; }
	};

	template<typename T, typename T1, typename T2> 
	struct mem_functor<T,T1, T2>
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2)>(L))(read<T1>(L,2),read<T2>(L,3))); return 1; }
		template<>
		static int invoke<void>(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2)>(L))(read<T1>(L,2),read<T2>(L,3)); return 0; }
	};

	template<typename T, typename T1> 
	struct mem_functor<T,T1>
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1)>(L))(read<T1>(L,2))); return 1; }
		template<>
		static int invoke<void>(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1)>(L))(read<T1>(L,2)); return 0; }
	};

	template<typename T> 
	struct mem_functor<T>
	{
		template<typename RVal>
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)()>(L))()); return 1; }
		template<>
		static int invoke<void>(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)()>(L))(); return 0; }
	};
	
	// push_functor
	template<typename RVal, typename T>
	void push_functor(lua_State *L, RVal (T::*func)()) 
	{ 
		lua_pushcclosure(L, mem_functor<T>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T>
	void push_functor(lua_State *L, RVal (T::*func)() const) 
	{ 
		lua_pushcclosure(L, mem_functor<T>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1>
	void push_functor(lua_State *L, RVal (T::*func)(T1)) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1>
	void push_functor(lua_State *L, RVal (T::*func)(T1) const) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2)) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1,T2>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2) const) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1,T2>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3)) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1,T2,T3>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3) const) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1,T2,T3>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4)) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1,T2,T3,T4>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4) const) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1,T2,T3,T4>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5)) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1,T2,T3,T4,T5>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5) const) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1,T2,T3,T4,T5>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6)) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1,T2,T3,T4,T5,T6>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6) const) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1,T2,T3,T4,T5,T6>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7)) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1,T2,T3,T4,T5,T6,T7>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7) const) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1,T2,T3,T4,T5,T6,T7>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8)) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1,T2,T3,T4,T5,T6,T7,T8>::invoke<RVal>, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8) const) 
	{ 
		lua_pushcclosure(L, mem_functor<T,T1,T2,T3,T4,T5,T6,T7,T8>::invoke<RVal>, 1); 
	}

	// constructor
	template<typename T1=void, typename T2=void, typename T3=void, typename T4=void>
	struct constructor {};

	template<typename T1, typename T2, typename T3>
	struct constructor<T1,T2,T3>
	{
		template<typename T>
		static void invoke(lua_State *L)
		{
			new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4));
		}
	};


	template<typename T1, typename T2>
	struct constructor<T1,T2>
	{
		template<typename T>
		static void invoke(lua_State *L)
		{
			new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3));
		}
	};

	template<typename T1>
	struct constructor<T1>
	{
		template<typename T>
		static void invoke(lua_State *L)
		{
			new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2));
		}
	};

	template<>
	struct constructor<void>
	{ 
		template<typename T>
		static void invoke(lua_State *L) 
		{ 
			new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>();
		} 
	};

	template<typename T>
	struct creator
	{
		template<typename CONSTRUCTOR>
		static int invoke(lua_State *L) 
		{ 
			CONSTRUCTOR::invoke<T>(L);
			meta_push(L, class_name<class_type<T>::type>::name());
			lua_setmetatable(L, -2);

			return 1; 
		}
	};

	// destroyer
	template<typename T>
	int destroyer(lua_State *L) 
	{ 
		((user*)lua_touserdata(L, 1))->~user();
		return 0;
	}

	// global function
	template<typename F> 
	void def(lua_State* L, const char* name, F func)
	{ 
		lua_pushstring(L, name);
		lua_pushlightuserdata(L, func);
		push_functor(L, func);
		lua_settable(L, LUA_GLOBALSINDEX);
	}

	// global variable
	template<typename T>
	void set(lua_State* L, const char* name, T object)
	{
		lua_pushstring(L, name);
		push(L, object);
		lua_settable(L, LUA_GLOBALSINDEX);
	}

	template<typename T>
	T get(lua_State* L, const char* name)
	{
		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		return pop<T>(L);
	}

	template<typename T>
	void decl(lua_State* L, const char* name, T object)
	{
		set(L, name, object);
	}

	// table member call ( Customizing )
	template<typename RVal>
	RVal membercall(lua_State* L, const char* tn, const char* fn)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, tn);
		lua_gettable(L, LUA_GLOBALSINDEX);
		lua_getfield(L, -1, fn);
		if(lua_isfunction(L,-1))
		{
			if(lua_pcall(L, 0, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::membercall() attempt to call global `%s' `%s' (not a function)", tn, fn);
		}

		lua_remove(L, -3);
		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1>
	RVal membercall(lua_State* L, const char* tn, const char* fn, T1 arg)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, tn);
		lua_gettable(L, LUA_GLOBALSINDEX);
		lua_getfield(L, -1, fn);
		if(lua_isfunction(L,-1))
		{
			push(L, arg);
			if(lua_pcall(L, 1, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::membercall() attempt to call global `%s' `%s' (not a function)", tn, fn);
		}

		lua_remove(L, -3);
		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2>
	RVal membercall(lua_State* L, const char* tn, const char* fn, T1 arg1, T2 arg2)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, tn);
		lua_gettable(L, LUA_GLOBALSINDEX);
		lua_getfield(L, -1, fn);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			if(lua_pcall(L, 2, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::membercall() attempt to call global `%s' `%s' (not a function)", tn, fn);
		}

		lua_remove(L, -3);
		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3>
	RVal membercall(lua_State* L, const char* tn, const char* fn, T1 arg1, T2 arg2, T3 arg3)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, tn);
		lua_gettable(L, LUA_GLOBALSINDEX);
		lua_getfield(L, -1, fn);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			if(lua_pcall(L, 3, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::membercall() attempt to call global `%s' `%s' (not a function)", tn, fn);
		}

		lua_remove(L, -3);
		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4>
	RVal membercall(lua_State* L, const char* tn, const char* fn, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, tn);
		lua_gettable(L, LUA_GLOBALSINDEX);
		lua_getfield(L, -1, fn);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			push(L, arg4);
			if(lua_pcall(L, 4, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::membercall() attempt to call global `%s' `%s' (not a function)", tn, fn);
		}

		lua_remove(L, -3);
		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	// call
	template<typename RVal>
	RVal call(lua_State* L, const char* name)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			if(lua_pcall(L, 0, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1>
	RVal call(lua_State* L, const char* name, T1 arg)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg);
			if(lua_pcall(L, 1, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			if(lua_pcall(L, 2, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			if(lua_pcall(L, 3, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			push(L, arg4);
			if(lua_pcall(L, 4, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			push(L, arg4);
			push(L, arg5);
			if(lua_pcall(L, 5, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			push(L, arg4);
			push(L, arg5);
			push(L, arg6);
			if(lua_pcall(L, 6, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			push(L, arg4);
			push(L, arg5);
			push(L, arg6);
			push(L, arg7);
			if(lua_pcall(L, 7, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	RVal call(lua_State* L, const char* name, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8)
	{
		lua_pushcclosure(L, on_error, 0);
		int errfunc = lua_gettop(L);

		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isfunction(L,-1))
		{
			push(L, arg1);
			push(L, arg2);
			push(L, arg3);
			push(L, arg4);
			push(L, arg5);
			push(L, arg6);
			push(L, arg7);
			push(L, arg8);
			if(lua_pcall(L, 8, 1, errfunc) != 0)
			{
				lua_pop(L, 1);
			}
		}
		else
		{
			print_error(L, "lua_tinker::call() attempt to call global `%s' (not a function)", name);
		}

		lua_remove(L, -2);
		return pop<RVal>(L);
	}

	// class helper
	int meta_get(lua_State *L);
	int meta_set(lua_State *L);
	void meta_push(lua_State *L, const char* name);

	// class init
	template<typename T>
	void class_add(lua_State* L, const char* name) 
	{ 
		class_name<T>::name(name);

		lua_pushstring(L, name);
		lua_newtable(L);

		lua_pushstring(L, "__name");
		lua_pushstring(L, name);
		lua_rawset(L, -3);

		lua_pushstring(L, "__index");
		lua_pushcclosure(L, meta_get, 0);
		lua_rawset(L, -3);

		lua_pushstring(L, "__newindex");
		lua_pushcclosure(L, meta_set, 0);
		lua_rawset(L, -3);

		lua_pushstring(L, "__gc");
		lua_pushcclosure(L, destroyer<T>, 0);
		lua_rawset(L, -3);

		lua_settable(L, LUA_GLOBALSINDEX);
	}

	// Tinker Class Inheritence
	template<typename T, typename P>
	void class_inh(lua_State* L)
	{
		meta_push(L, class_name<T>::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, "__parent");
			meta_push(L, class_name<P>::name());
			lua_rawset(L, -3);
		}
		lua_pop(L, 1);
	}

	// Tinker Class Constructor
	template<typename T, typename CONSTRUCTOR>
	void class_con(lua_State* L, CONSTRUCTOR)
	{
		meta_push(L, class_name<T>::name());
		if(lua_istable(L, -1))
		{
			lua_newtable(L);
			lua_pushstring(L, "__call");
			lua_pushcclosure(L, creator<T>::invoke<CONSTRUCTOR>, 0);
			lua_rawset(L, -3);
			lua_setmetatable(L, -2);
		}
		lua_pop(L, 1);
	}

	// Tinker Class Functions
	template<typename T, typename F>
	void class_def(lua_State* L, const char* name, F func) 
	{ 
		meta_push(L, class_name<T>::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, name);
			new(lua_newuserdata(L,sizeof(F))) F(func);
			push_functor(L, func);
			lua_rawset(L, -3);
		}
		lua_pop(L, 1);
	}

	// Tinker Class Variables
	template<typename T, typename BASE, typename VAR>
	void class_mem(lua_State* L, const char* name, VAR BASE::*val) 
	{ 
		meta_push(L, class_name<T>::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, name);
			new(lua_newuserdata(L,sizeof(mem_var<BASE,VAR>))) mem_var<BASE,VAR>(val);
			lua_rawset(L, -3);
		}
		lua_pop(L, 1);
	}

	template<typename T>
	struct class_name
	{
		// global name
		static const char* name(const char* name = NULL)
		{
			static char temp[256] = "";
			if(name) strcpy(temp, name);
			return temp;
		}
	};

	// Table Object on Stack
	struct table_obj
	{
		table_obj(lua_State* L, int index);
		~table_obj();

		void inc_ref();
		void dec_ref();

		bool validate();

		template<typename T>
		void set(const char* name, T object)
		{
			if(validate())
			{
				lua_pushstring(m_L, name);
				push(m_L, object);
				lua_settable(m_L, m_index);
			}
		}

		template<typename T>
		T get(const char* name)
		{
			if(validate())
			{
				lua_pushstring(m_L, name);
				lua_gettable(m_L, m_index);
			}
			else
			{
				lua_pushnil(m_L);
			}

			return pop<T>(m_L);
		}

		template<typename T>
		void set( int idx, T object)
		{
			if(validate())
			{
				lua_pushnumber(m_L, idx);
				push(m_L, object);
				lua_settable(m_L, m_index);
			}
		}

		template<typename T>
		T get( int idx )
		{
			if(validate())
			{
				lua_pushnumber(m_L, idx );
				lua_gettable(m_L, m_index);
			}
			else
			{
				lua_pushnil(m_L);
			}
			return pop<T>(m_L);
		}

		int TableLen()
		{
			return lua_objlen( m_L, m_index );
		}


		lua_State*		m_L;
		int				m_index;
		const void*		m_pointer;
		int				m_ref;
	};

	// Table Object Holder
	struct table
	{
		table(lua_State* L);
		table(lua_State* L, int index);
		table(lua_State* L, const char* name);
		table(const table& input);
		~table();

		template<typename T>
		void set(const char* name, T object)
		{
			m_obj->set(name, object);
		}

		template<typename T>
		T get(const char* name)
		{
			return m_obj->get<T>(name);
		}

		template<typename T>
		void set(int idx, T object)
		{
			m_obj->set(idx, object);
		}

		template<typename T>
		T get(int idx)
		{
			return m_obj->get<T>(idx);
		}

		int TableLen()
		{
			return m_obj->TableLen();
		}


		table_obj*		m_obj;
	};

} // namespace lua_tinker

#endif //_LUA_TINKER_H_