#ifndef CTriggerInstance_H
#define CTriggerInstance_H

#include "TriggerSystemType.h"

class GLLandMan;

namespace TriggerSystem
{

/**
	Ʈ���� �ν��Ͻ� ó����
	Ʈ���� ó���� ���� �������� ����Ǵ� ���׵��� ������
	Ʈ���� ������ ���� �Ѵ�.
	
	�۵��Ⱑ �ܺο��� �������̽� ������ �Ѵٸ� Ʈ���Ŵ� ���� �۵��� ������
	�׸��� �ν��Ͻ��� �� �� ������ ������ �κа� �޽��� ó���� ���� �������̽� ������ ���� �Ѵ�.

	�׷��Ƿ� �޽���(SMessage)�� �ν��Ͻ������� ���οܺη� ������ ��� ��� �� �� �����Ƿ� �ν��Ͻ��� ���ڷ� �޴´�.
	
 */
class CTriggerInstance
{
public:
		CTriggerInstance();
virtual ~CTriggerInstance();

		/**
			�ν��Ͻ��� ������ Finger �� ��� �Ѵ�.
			�ν��Ͻ��� Finger �� Trigger �� ������ �޽������� �߰��� ������ ���� �Ѵ�.

		 */
		void				SetFinger(CTriggerFinger* pFinger) { m_pFinger = pFinger; }
		CTriggerFinger*		GetFinger() { return m_pFinger; }

		/**
			�ν��Ͻ��� �¹��� �۵� �Ǵ� Ʈ���ſ� ���� ó���� ���� �Ѵ�.
			Ʈ���Ű� ���� �Ǹ� �ڵ����� ���� Ʈ����(Ʈ������ ���ϵ�)�� �߰� �� �شٴ� ���� ���� �Ѵ�.

		 */
		void				SetTrigger(const CTrigger* pTrigger) { m_pTrigger = pTrigger; }
		const CTrigger*		GetTrigger() const { return m_pTrigger; }

		/**
			���� Ʈ������ Ÿ���� �� �Ѵ�.
			���� ���� �Լ��̹Ƿ� ���Ǹ� ���� �ν��Ͻ��� �����.

		 */
		bool				IsTriggerType(TriggerType type);

		/**
			������ ó���� ���� �Ѵ�.
			Ʈ���ſ� �ν��Ͻ��� �Ѱ� �����ν� Ʈ���ſ� ������ ���� �Ѵ�.
			
		 */
virtual void				FrameMove(float fAccTime, float fElapsedTime);

		/**
			�ð� ���� ������

		 */
		const float			GetTime() const  { return m_fTime; }
        void                SetTime(const float fTime) { m_fTime = fTime; }
		const float			GetElapsedTime() const { return m_fElapsedTime; }
        void				SetElapsedTime(const float fElapsedTime) { m_fElapsedTime = fElapsedTime; }

        /**
            �ڽ��� ���� �ð��� ��� �Ѵ�.
            �⺻������ Ʈ������ LiftTime �� ���������
            �ν��Ͻ����� Ư�� ��Ȳ �϶� ���� �ð��� �ο��Ѵٴ��� ������ �� �ִ�.            

         */
        void                SetLifeTime(const float fTime) { m_fLifeTime = fTime; }
        const float         GetLifeTime() const { return m_fLifeTime; }

        /**
            ���� �ð��� �������� ���ɴ�.

         */
        bool                IsTimeOver(const float fTime) const;

        /**
            �ν��Ͻ��� ������ �� ȣ�� �ȴ�.

         */
virtual	void		        Start();

        /**
        	�ν��Ͻ��� ���� ��Ų��.
            �ν��Ͻ��� ���� ��Ű�� �ν��Ͻ��� ���� �Ǿ� ����
            ���� Ʈ���Ű� �ִٸ� ���� Ʈ���Ű� �ߵ� �ȴ�.

         */
virtual void                Exit();
        bool                IsExit();

		/**
			Ʈ���ŵ��� �ܺο��� �ǵ���� ����� ���� ó�� �Ѵ�.
			����� �޽����� ���ؼ� ó���Ǹ� �޽����� Send �� Post �� ���� �Ѵ�.
			Send �� ȣ�� �Ǵ� ������ �ٷ� �����ϸ�, Post �� ���� �� �� FrameMove ���� ȣ�� ó�� �ȴ�.

		 */
		void				SendMessage(SMessage* msg);
		void				PostMessage(SpMessage msg);

		/**
			����ȯ ���ǿ� ���ø� �Լ�
            ����ȯ �� return ��(bool��) ó���� �ݵ�� ���� �ؾ� �Ѵ�.

		 */
        template<class T>
        inline bool			CastFinger(T*& ref) { return (ref = dynamic_cast<T*>(m_pFinger)) != 0; }
        template<class T>
        inline bool			CastTrigger(const T*& ref) { return (ref = dynamic_cast<const T*>(m_pTrigger)) != 0; }
        template<class T>
        inline bool			CastInstance(T*& ref) { return (ref = dynamic_cast<T*>(this)) != 0; }

protected:
        /**
        	���� �ð��� ���͸� �Ѵ�.
            ���� Ŭ�� ���� �� �ٽ� �ý����� ���� �Ǹ� �������� ����ȭ�� ����
            ���� �ð��� ���� ���Ѿ� �ϴµ� �̷� �� �ڽ��� ����� �ð��� �����ϱ� ���� �뵵�� ��� �ȴ�.
            TriggerFinger::FrameMove �� ���� �Ѵ�.

	        \param fElapsedTime ���� �� �ð�
         */
virtual void                ProcessTime(float fElapsedTime);

		/**
			Ʈ������ ���¸� üũ �Ѵ�.
			�ν��Ͻ��� �ڽ��� ó���ϴ� Ʈ������ ���¸� ������.
			�̸� ���� �ܺο��� ������ ó���� �����Ѵ�.
			����� ������ ���� �뵵�� ��� �ȴ�.

		 */
        void				SetState(TriggerState state) { m_state = state; }
		bool				IsState(TriggerState state) { return m_state == state; }		

		/**
			�ܺο��� �ǵ��� �޽����� ó�� �Ѵ�.

		 */
		void				MessageProc(SMessage* msg);

		CTriggerFinger*		m_pFinger;		//@ ���� �۵���
		const CTrigger*		m_pTrigger;		//@ ���� Ʈ����
		MessageContainer	m_msgs;			//@ �޽��� �����

		float				m_fElapsedTime;	//@ ���� ������ ���� ���� �����ӱ��� �ɸ� �ð�
		float				m_fTime;		//@ Ʈ���Ű� ���۵� ���� ���� �ð�
        float               m_fLifeTime;    //@ ���� �ð�, �⺻������ Ʈ������ LifeTime �� ���� ��.

		TriggerState		m_state;		//@ Ʈ���� ����
};

} // end namespace

#endif