#pragma once

namespace CallbackUtility
{

	/*
	 * CallBackTrigger �� ���� ��ü �����Ϳ� �Լ������͸� memcpy�Ѵ�
	 * (memcopy ������.. OS ���� ������ ũ�Ⱑ �ٸ����ִ�, x64 8byte, x86 4byte ���.. const, virtual,static )
	 * �׸��� Buffer�� ���� �Ѵ�.
	 * Call�� ȣ���ϸ� Static����� �����ĵ� �Լ��� ȣ��
	 * �׸��� ���ۿ� �ִ� �����͸� �̿��Ͽ�.
	 * ��ϵ� �ں� �Լ��� ȣ���Ѵ�.
	 */
	// ParamiterTYPE �ݺ��Ҷ� ���ڷ� �Ѱ��� �Ķ���� Ÿ��
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


		// ��� �Լ� ����
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


	// ������.
	template <typename Callee, typename ParamiterTYPE>
	inline int StateMemberDispatcher(CallBackTrigger<ParamiterTYPE>* pthis,ParamiterTYPE* L)
	{
		typedef int (Callee::*Functor)(ParamiterTYPE*);    // Helper typedef.
		Callee& callee = *(Callee*)pthis->buffer;
		//Functor& f = *(Functor*)(pthis->buffer + sizeof(Callee));
		Functor& f = *(Functor*)(pthis->_function);
		return (callee.*f)(L);
	}


	// Static�Լ� �ݺ� ����ϰ������ �������.
	// ��� ���� ����.
	template <typename ParamiterTYPE>
	class StaticCallBackTrigger
	{
	public :
		void (*fptr)(ParamiterTYPE*);

		// Static�� ����
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