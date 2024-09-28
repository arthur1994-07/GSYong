#ifndef CTriggerScript_H
#define CTriggerScript_H

#include "../Script/SquirrelMan.h"
#include "sqrat/sqratObject.h"
#include "squirrel.h"

namespace TriggerSystem
{

class CTrigger;

/**
	��ũ��Ʈ�� ���� Ʈ���Ÿ� ����, ���� �Ѵ�.
	���α׷��� ���� �ڵ�� ��ũ��Ʈ�� ���� �ϰ�
	��ũ��Ʈ ó���� ���� �Ѵ�.

 */
class CTriggerScript
	: public SquirrelMan
{
public:
		CTriggerScript();
virtual ~CTriggerScript();

		/**
			��ũ��Ʈ ���� �� ó���� ���� �Ѵ�.
			��ũ��Ʈ���� ���ε� ó���� �����Ѵ�.

		 */
virtual void			Init() = 0;

		/**
			��ũ��Ʈ �� ó���� ���� �Ѵ�.
			����� Ư���� ������ �۾��� ����.

		 */
		void			ToDo() {}

        /**
			��ũ��Ʈ�� ���� ������ Ʈ���Ÿ� ã�´�.
            �Էµ� �̸��� ��ũ��Ʈ ������ �ν��Ͻ����̴�.

		 */
		CTrigger*		Find(const TCHAR* pName);

		/**
			Ŭ������ ���ε� �Ѵ�.
			Ŭ���� ��� �Լ��� ��� ������ ���ε� �ϱ� ���ؼ���
            �ݵ�� Ŭ������ ���������� ���ε� �ؾ� �Ѵ�.
            ���� ��� ���� �� ���� ���� Ŭ������ �����ؾ�
            ���� Ŭ�������� ����Լ��� ������� ���ε� �� �� �ִ�.

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
            Ŭ������ ��� �Լ��� ���ε� �Ѵ�.
            �Լ��� CreateClass �� ���� ���ǵ� Ŭ������ ���ǵ� �Լ����� �Ѵ�.
            �����Լ� �鵵 ��� ��� ���� �ϴ�.

         */
		template<class F>
		CTriggerScript& Func(const SQChar* name, F method)
		{
			BindFunc(name, &method, sizeof(method), Sqrat::SqMemberFunc(method));
			return *this;
		}

        /**
            Ŭ������ ��� ������ ���ε� �Ѵ�.
            ���� ��� �Լ� ���ε� ó�� �ݵ�� CreateClass �� ���� �� Ŭ������
            ��� ���� ���� �Ѵ�. ��� ������ ��� ������ ��� ���� �ϴ�.

            ���� ��� ���� ���ε�� ������ �־��µ� �ֱٿ� ���� �Ǿ� ����� ����� ����������.
            �׷��� ���� ��ټ� ���ε�� �Լ� ������� �Ǿ� �ִ�. ���� ������ ������ �Ѵ�.

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
            �Ʒ� �ζ��� �Լ����� Sqrat::Class �� �Լ����� ���� �Ͽ�
            ��� �ϱ� �����ϰ� ������ ���� �Ͽ���.

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
