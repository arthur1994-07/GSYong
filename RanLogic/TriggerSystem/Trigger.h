#ifndef CTrigger_H
#define CTrigger_H

#include "TriggerSystemType.h"
#include "sqrat/sqratclasstype.h"

namespace TriggerSystem
{

/**
	Ʈ���� ���� �������̽�

	��� Ʈ���Ű� ����� ���� �������̽��� ���� �Ѵ�.
	Ʈ���Ŵ� �����͸� ������� ���� ���� ó���� ��� �Ѵ�.

	��Ƽ �ھ�(������) ������� ���� �ǹǷ� ���� �Լ� ������� �ۼ� �Ǿ�� �Կ� ���� �Ѵ�.
	��Ƽ �ھ�(������) ������� �ν��Ͻ�(TriggerInstance) �� const �� Ʈ���Ÿ� ������ �Ǿ� ������ �� ����.

 */
class CTrigger
	: public Sqrat::SqObjectType
{
public:
		CTrigger();
virtual ~CTrigger() {}

		/**
			Ʈ���Ÿ� �����ϴ� ���� type �� ��� �Ѵ�.
			�� Ʈ���Ű��� ���и� ���� �ϸ� ���� Ʈ���Ű����� ������ �Ұ��� �ϴ�.
			���� Ʈ���� ������ ID�� ���� �Ѵ�. ID �ο��� ��ũ��Ʈ �ۼ��ڰ� ��ũ��Ʈ���� �ο� �Ѵ�.

		 */
virtual TriggerType			GetType() const = 0;
        bool                IsType(const TriggerType type) const { return type == GetType(); }

		/**
			Ʈ���Ÿ� �����ϴ� ���� ID �� ��� �Ѵ�.
			Type �� �� Ʈ���ŵ� ���� Ÿ���� �����ϴ� �뵵�� ���Ǹ�
			ID �� ���� Ʈ���Ŷ� ������ ID �� ���� �� �־� ��� Ʈ���ŵ簣�� ������ ���� �뵵�� ���
			Ư�� ��ũ��Ʈ���� ID �� �ο��Ͽ� Ư�� Ʈ���Ÿ� Ư�� �� ���� �����ϴ� ���� �뵵�� Ȱ��
			
		 */
virtual const TriggerID	    GetID() const { return m_id; }
virtual void		        SetID(TriggerID id) { m_id = id; }

        /**
        	Ʈ���Ŵ� �ڽ��� ����ϴ� �ν��Ͻ��� ���� �� �ش�.
            Ʈ���Ÿ��� ����ϴ� �ν��Ͻ��� �ٸ� �� �ֱ� ������ �̷� �������̽��� �ʿ��ϴ�.
            ���� ������ �Ѱ����� ������ ���� ������Ʈ Ǯ������ Ȯ���ϱ� �����ϴ�.

         */
virtual CTriggerInstance*   CreateInstance() const;

		/**
			��ũ��Ʈ���� ����(Bind)�� ó�� �Ѵ�.
			
		 */
virtual	void		        Bind(CTriggerScript* pScript);

		/**
			�ԷõǴ� Ʈ���� �ν��Ͻ��� ������� ������ ó���� ���� �Ѵ�.
			�ԷµǴ� �ν��Ͻ��� �������� ��ȭ�Ǵ� ������ ������ Ʈ���Ŵ� �̸� �������
			�������� ó���� �����Կ� ���� �Ѵ�.

			\param pInstance Ʈ���� �ν��Ͻ�
		 */
virtual void		        FrameMove(CTriggerInstance* pInstance) const {}

        /**
        	Ʈ���Ű� ���� or �� �� �� ȣ�� �Ǿ� ����.
            ���� ���ٴ� Ŭ���ʿ����� Ʈ������ �ν��Ͻ��� ȿ���� ���� �ʿ䰡 �����Ƿ�
            Ʈ���� ��ü�� �ν��Ͻ��� ������ ������ �� �����ϰ� ��� �ȴ�.

         */
virtual void                Start(CTriggerInstance* pInstance) const {}
virtual void                Exit(CTriggerInstance* pInstance) const {}

        /**
        	�ڽ��� ���� �ð��� ��� �Ѵ�.

	        \return �ڽ��� ���� �ð�, 0 �ϰ�� ���Ѵ� 
         */
        void                SetLifeTime(float lifetime) { m_lifetime = lifetime; }
        const float         GetLifeTime() const { return m_lifetime; }

		/**
			���ķ� ó���� Ʈ���Ÿ� ó�� �Ѵ�.
            �����̶� ���ÿ� ���� ���� �ǹ� �Ѵ�.

		 */
		void		        InsertSibling(CTrigger* pTrigger);
		const CTrigger*	    GetSibling() const { return m_sibling; }

		/**
			������ Ʈ���Ű� �Ҹ�� ���� �Ҹ�ǰ� �ϱ� ���� ���
            �� ������ Ʈ���ſ� ���� ����(sibling) �����̾�� �ϸ�
            ���� ���� sibling �� �ƴ϶� ���� sibling �̾�� �Ѵ�!!!

		 */
        void		        SetAttach(CTrigger* pTrigger) { m_attach = pTrigger; }
        const CTrigger*	    GetAttach() const { return m_attach; }

		/**
			���ķ� ó���� Ʈ���Ÿ� ó�� �Ѵ�.
            �����̶� ���� Ʈ���Ű� ���� �� �� ���� �Ǹ�
            �������� ������� �ǹ� �Ѵ�.

		 */
		void		        InsertChild(CTrigger* pTrigger);
		const CTrigger*	    GetChild() const { return m_child; }

		/**
			Ʈ���ſ� �޽����� ó�� �Ѵ�.
			Ʈ���Ŵ� ��� �δ����� ����Ѵٴ� �Ϳ� ���� �Ѵ�.

		 */
		void		        SendMessage(SSystemMessage* msg);

		/**
			����ȯ ���ǿ� ���ø� �Լ�
            ����ȯ �� return ��(bool��) ó���� �ݵ�� ���� �ؾ� �Ѵ�.

		 */
        template<class T>
        inline bool			CastTrigger(const T*& ref) { return (ref = dynamic_cast<const T*>(this)) != 0; }

protected:
		TriggerID	        m_id;		//@ Ʈ���� ���� ������
		CTrigger*	        m_sibling;	//@ Ʈ���� �ú�, ������ ���� Ʈ����
        CTrigger*	        m_attach;	//@ ���� ���� Ʈ����, �� Ʈ���Ű� �Ҹ�� ���� �Ҹ� �ȴ�.
		CTrigger*	        m_child;	//@ Ʈ���� ���ϵ�, ������ ���� Ʈ����		
        float               m_lifetime; //@ Ʈ���� ���� �ð�
};

} // end namespace

#endif
