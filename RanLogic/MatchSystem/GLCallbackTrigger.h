#pragma once

namespace CallbackUtility
{

	/*
	 * CallBackTrigger 을 통해 객체 포인터와 함수포인터를 memcpy한다
	 * (memcopy 이유는.. OS 별로 포인터 크기가 다를수있다, x64 8byte, x86 4byte 등등.. const, virtual,static )
	 * 그리고 Buffer에 적제 한다.
	 * Call을 호출하면 Static멤버에 디스페쳐된 함수를 호출
	 * 그리고 버퍼에 있는 포인터를 이용하여.
	 * 등록된 코벡 함수를 호출한다.
	 */
	// ParamiterTYPE 콜벡할때 인자로 넘겨질 파라미터 타입
	template <typename ParamiterTYPE>
	class CallBackTrigger
	{
	
	public:
		CallBackTrigger(DWORD id) : buffer(NULL),_function(NULL),IdentityName(id)
		{

		}
	
		virtual ~CallBackTrigger()
		{
			buffer = NULL;
			free(_function);
			_function = NULL;
		}

		DWORD IdentityName;
		unsigned char* buffer;
		unsigned char* _function;

		int (*fptr)(CallBackTrigger*,ParamiterTYPE*);
	public :


		// 멤버 함수 가능
		template <typename Callee>
		inline void RegistCallback( Callee& callee, int (Callee::*func)(ParamiterTYPE*))
		{
			buffer = (unsigned char*)&callee;
			if(_function!=NULL)
			{
				free(_function);
				_function = NULL;
			}
			_function = (unsigned char*)malloc(sizeof(func));
			memcpy(_function , &func, sizeof(func));
			fptr = CallbackUtility::StateMemberDispatcher<Callee,ParamiterTYPE>;
		}

		void Call(ParamiterTYPE* L)
		{
			if(buffer == NULL)
				return;
			if(_function == NULL)
				return;
			fptr(this,L);
			buffer = NULL;
			free(_function);
			_function = NULL;
		}				
	};


	// 디스페쳐.
	template <typename Callee, typename ParamiterTYPE>
	inline int StateMemberDispatcher(CallBackTrigger<ParamiterTYPE>* pthis,ParamiterTYPE* L)
	{
		typedef int (Callee::*Functor)(ParamiterTYPE*);    // Helper typedef.
		Callee& callee = *(Callee*)pthis->buffer;
		//Functor& f = *(Functor*)(pthis->buffer + sizeof(Callee));
		Functor& f = *(Functor*)(pthis->_function);
		return (callee.*f)(L);
	}


	// Static함수 콜벡 등록하고싶을때 사용하자.
	// 사실 별거 없다.
	template <typename ParamiterTYPE>
	class StaticCallBackTrigger
	{
	public :
		void (*fptr)(ParamiterTYPE*);

		// Static만 가능
		inline void RegistStaticCallback( int (*func)(ParamiterTYPE*))
		{
			fptr = func;
		}

		void Call(ParamiterTYPE* L)
		{
			fptr(L);
		}
	};

}