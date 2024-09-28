#ifndef CTriggerScript_H
#define CTriggerScript_H

#include "../Script/SquirrelMan.h"
#include "sqrat/sqratObject.h"
#include "squirrel.h"

namespace TriggerSystem
{

class CTrigger;

/**
	스크립트로 부터 트리거를 생성, 설정 한다.
	프로그래밍 내부 코드와 스크립트를 연결 하고
	스크립트 처리를 수행 한다.

 */
class CTriggerScript
	: public SquirrelMan
{
public:
		CTriggerScript();
virtual ~CTriggerScript();

		/**
			스크립트 실행 전 처리를 수행 한다.
			스크립트와의 바인드 처리를 수행한다.

		 */
virtual void			Init() = 0;

		/**
			스크립트 후 처리를 수행 한다.
			현재는 특별히 수행할 작업이 없다.

		 */
		void			ToDo() {}

        /**
			스크립트로 부터 생성된 트리거를 찾는다.
            입력된 이름은 스크립트 파일의 인스턴스명이다.

		 */
		CTrigger*		Find(const TCHAR* pName);

		/**
			클래스를 바인드 한다.
			클래스 멤버 함수와 멤버 변수를 바인드 하기 위해서는
            반드시 클래스를 선행적으로 바인드 해야 한다.
            또한 상속 관계 일 때는 하위 클래스로 생성해야
            상위 클래스들의 멤버함수나 멤버들을 바인드 할 수 있다.

		 */
		template<class F>
		void			CreateClass(const TCHAR* pBindName)
		{
			ClassType<F>::Initialized() = false;
			m_obj = Class<F>(m_vm).GetObject();
            m_get = ClassType<F>::GetTable();
            m_set = ClassType<F>::SetTable();
			RootTable(m_vm).Bind(pBindName, Object(m_obj, m_vm));
		}

        /**
            클래스의 멤버 함수를 바인드 한다.
            함수는 CreateClass 에 의해 정의된 클래스에 정의된 함수여야 한다.
            가상함수 들도 모두 사용 가능 하다.

         */
		template<class F>
		CTriggerScript& Func(const SQChar* name, F method)
		{
			BindFunc(name, &method, sizeof(method), Sqrat::SqMemberFunc(method));
			return *this;
		}

        /**
            클래스의 멤버 변수를 바인드 한다.
            위의 멤버 함수 바인드 처럼 반드시 CreateClass 로 정의 한 클래스의
            멤버 변수 여야 한다. 상속 계통의 멤버 변수도 사용 가능 하다.

            원래 멤버 변수 바인드는 문제가 있었는데 최근에 수정 되어 제대로 사용이 가능해졌다.
            그래서 현재 대다수 바인드는 함수 기반으로 되어 있다. 이제 변수도 쓰도록 한다.

         */
        template<class C, class V>
        CTriggerScript& Var(const SQChar* name, V C::* var)
        {
            // Add the getter
            BindAccessor(name, &var, sizeof(var), &Sqrat::sqDefaultGet<C, V>, m_get);

            // Add the setter
            BindAccessor(name, &var, sizeof(var), &Sqrat::sqDefaultSet<C, V>, m_set);

            return *this;
        }

protected:
        /**
            아래 인라인 함수들은 Sqrat::Class 의 함수들을 포팅 하여
            사용 하기 용이하게 적당히 변형 하였다.

         */
        inline void     BindFunc(const SQChar* name, void* method, size_t methodSize, SQFUNCTION func, bool staticVar = false)
        {
            sq_pushobject(m_vm, m_obj);
            sq_pushstring(m_vm, name, -1);

            SQUserPointer methodPtr = sq_newuserdata(m_vm, static_cast<SQUnsignedInteger>(methodSize));
            memcpy(methodPtr, method, methodSize);

            sq_newclosure(m_vm, func, 1);
            sq_newslot(m_vm, -3, staticVar);
            sq_pop(m_vm,1); // pop table
        }

        // Helper function used to bind getters and setters
        inline void     BindAccessor(const SQChar* name, void* var, size_t varSize, SQFUNCTION func, HSQOBJECT table)
        {
            // Push the get or set table
            sq_pushobject(m_vm, table);
            sq_pushstring(m_vm, name, -1);

            // Push the variable offset as a free variable
            SQUserPointer varPtr = sq_newuserdata(m_vm, static_cast<SQUnsignedInteger>(varSize));
            memcpy(varPtr, var, varSize);

            // Create the accessor function
            sq_newclosure(m_vm, func, 1);

            // Add the accessor to the table
            sq_newslot(m_vm, -3, false);

            // Pop get/set table
            sq_pop(m_vm, 1);
        }

		HSQOBJECT		m_obj;
        HSQOBJECT		m_get;
        HSQOBJECT		m_set;
};

} // end namespace

#endif
