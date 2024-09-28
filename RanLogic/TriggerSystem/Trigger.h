#ifndef CTrigger_H
#define CTrigger_H

#include "TriggerSystemType.h"
#include "sqrat/sqratclasstype.h"

namespace TriggerSystem
{

/**
	트리거 공용 인터페이스

	모든 트리거가 사용할 공용 인터페이스를 정의 한다.
	트리거는 데이터를 기반으로 순수 로직 처리만 담당 한다.

	멀티 코어(스레딩) 기반으로 수행 되므로 순수 함수 기반으로 작성 되어야 함에 주의 한다.
	멀티 코어(스레딩) 기반으로 인스턴스(TriggerInstance) 는 const 로 트리거를 가지게 되어 조작할 수 없다.

 */
class CTrigger
	: public Sqrat::SqObjectType
{
public:
		CTrigger();
virtual ~CTrigger() {}

		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			각 트리거간에 구분만 가능 하며 같은 트리거간에는 구분이 불가능 하다.
			같은 트리거 구분은 ID를 통해 한다. ID 부여는 스크립트 작성자가 스크립트에서 부여 한다.

		 */
virtual TriggerType			GetType() const = 0;
        bool                IsType(const TriggerType type) const { return type == GetType(); }

		/**
			트리거를 구분하는 고유 ID 를 출력 한다.
			Type 은 각 트리거들 간에 타입을 구분하는 용도로 사용되며
			ID 는 같은 트리거라도 고유한 ID 를 가질 수 있어 모든 트리거든간에 구분을 위한 용도로 사용
			특히 스크립트에서 ID 를 부여하여 특정 트리거를 특정 할 때에 삭제하는 등의 용도로 활용
			
		 */
virtual const TriggerID	    GetID() const { return m_id; }
virtual void		        SetID(TriggerID id) { m_id = id; }

        /**
        	트리거는 자신을 사용하는 인스턴스를 생성 해 준다.
            트리거마다 사용하는 인스턴스가 다를 수 있기 때문에 이런 인터페이스가 필요하다.
            또한 생성을 한곳으로 모으면 향후 오브젝트 풀등으로 확장하기 용이하다.

         */
virtual CTriggerInstance*   CreateInstance() const;

		/**
			스크립트와의 연결(Bind)을 처리 한다.
			
		 */
virtual	void		        Bind(CTriggerScript* pScript);

		/**
			입련되는 트리거 인스턴스를 기반으로 고유한 처리를 수행 한다.
			입력되는 인스턴스가 여러가지 변화되는 정보를 가지고 트리거는 이를 기반으로
			로직적인 처리를 수행함에 주위 한다.

			\param pInstance 트리거 인스턴스
		 */
virtual void		        FrameMove(CTriggerInstance* pInstance) const {}

        /**
        	트리거가 시작 or 끝 날 떄 호출 되어 진다.
            서버 보다는 클라쪽에서는 트리거의 인스턴스의 효용은 거의 필요가 없으므로
            트리거 자체가 인스턴스의 역할을 수행할 때 유용하게 사용 된다.

         */
virtual void                Start(CTriggerInstance* pInstance) const {}
virtual void                Exit(CTriggerInstance* pInstance) const {}

        /**
        	자신의 생명 시간을 출력 한다.

	        \return 자신의 생명 시간, 0 일경우 무한대 
         */
        void                SetLifeTime(float lifetime) { m_lifetime = lifetime; }
        const float         GetLifeTime() const { return m_lifetime; }

		/**
			병렬로 처리할 트리거를 처리 한다.
            병렬이란 동시에 실행 됨을 의미 한다.

		 */
		void		        InsertSibling(CTrigger* pTrigger);
		const CTrigger*	    GetSibling() const { return m_sibling; }

		/**
			임의의 트리거가 소멸시 같이 소멸되게 하기 위한 기능
            단 임의의 트리거와 동일 라인(sibling) 관계이어야 하며
            또한 상위 sibling 이 아니라 하위 sibling 이어야 한다!!!

		 */
        void		        SetAttach(CTrigger* pTrigger) { m_attach = pTrigger; }
        const CTrigger*	    GetAttach() const { return m_attach; }

		/**
			직렬로 처리할 트리거를 처리 한다.
            직렬이란 현재 트리거가 실행 된 후 종료 되면
            다음으로 실행됨을 의미 한다.

		 */
		void		        InsertChild(CTrigger* pTrigger);
		const CTrigger*	    GetChild() const { return m_child; }

		/**
			트리거에 메시지를 처리 한다.
			트리거는 모든 인던에서 사용한다는 것에 주의 한다.

		 */
		void		        SendMessage(SSystemMessage* msg);

		/**
			형변환 편의용 템플릿 함수
            형변환 시 return 값(bool형) 처리를 반드시 수행 해야 한다.

		 */
        template<class T>
        inline bool			CastTrigger(const T*& ref) { return (ref = dynamic_cast<const T*>(this)) != 0; }

protected:
		TriggerID	        m_id;		//@ 트리거 고유 구분자
		CTrigger*	        m_sibling;	//@ 트리거 시블링, 병렬적 수행 트리거
        CTrigger*	        m_attach;	//@ 내가 붙은 트리거, 이 트리거가 소멸시 같이 소멸 된다.
		CTrigger*	        m_child;	//@ 트리거 차일드, 직렬적 수행 트리거		
        float               m_lifetime; //@ 트리거 생명 시간
};

} // end namespace

#endif
