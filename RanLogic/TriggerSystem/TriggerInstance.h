#ifndef CTriggerInstance_H
#define CTriggerInstance_H

#include "TriggerSystemType.h"

class GLLandMan;

namespace TriggerSystem
{

/**
	트리거 인스턴스 처리기
	트리거 처리를 위해 동적으로 변경되는 사항들을 가지고
	트리거 로직을 수행 한다.
	
	작동기가 외부와의 인터페이스 역할을 한다면 트리거는 내부 작동의 역할을
	그리고 인스턴스는 이 둘 사이의 동적인 부분과 메시지 처리를 위한 인터페이스 역할을 수행 한다.

	그러므로 메시지(SMessage)는 인스턴스만으로 내부외부로 접근해 모든 제어를 할 수 있으므로 인스턴스를 인자로 받는다.
	
 */
class CTriggerInstance
{
public:
		CTriggerInstance();
virtual ~CTriggerInstance();

		/**
			인스턴스를 가지는 Finger 를 등록 한다.
			인스턴스는 Finger 와 Trigger 를 가지고 메시지에서 중간자 역할을 수행 한다.

		 */
		void				SetFinger(CTriggerFinger* pFinger) { m_pFinger = pFinger; }
		CTriggerFinger*		GetFinger() { return m_pFinger; }

		/**
			인스턴스와 맞물려 작동 되는 트리거에 대한 처리를 수행 한다.
			트리거가 제거 되면 자동으로 다음 트리거(트리거의 차일드)를 추가 해 준다는 점에 주의 한다.

		 */
		void				SetTrigger(const CTrigger* pTrigger) { m_pTrigger = pTrigger; }
		const CTrigger*		GetTrigger() const { return m_pTrigger; }

		/**
			현재 트리거의 타입을 비교 한다.
			자주 쓰는 함수이므로 편의를 위해 인스턴스에 만든다.

		 */
		bool				IsTriggerType(TriggerType type);

		/**
			프레임 처리를 수행 한다.
			트리거에 인스턴스를 넘겨 줌으로써 트리거에 로직을 수행 한다.
			
		 */
virtual void				FrameMove(float fAccTime, float fElapsedTime);

		/**
			시간 제어 변수들

		 */
		const float			GetTime() const  { return m_fTime; }
        void                SetTime(const float fTime) { m_fTime = fTime; }
		const float			GetElapsedTime() const { return m_fElapsedTime; }
        void				SetElapsedTime(const float fElapsedTime) { m_fElapsedTime = fElapsedTime; }

        /**
            자신의 생명 시간을 출력 한다.
            기본적으로 트리거의 LiftTime 을 사용하지만
            인스턴스마다 특정 상황 일때 생명 시간을 부여한다던가 응용할 수 있다.            

         */
        void                SetLifeTime(const float fTime) { m_fLifeTime = fTime; }
        const float         GetLifeTime() const { return m_fLifeTime; }

        /**
            생명 시간이 끝났는지 살핀다.

         */
        bool                IsTimeOver(const float fTime) const;

        /**
            인스턴스가 생성될 때 호출 된다.

         */
virtual	void		        Start();

        /**
        	인스턴스를 종료 시킨다.
            인스턴스를 종료 시키면 인스턴스가 삭제 되어 지고
            다음 트리거가 있다면 다음 트리거가 발동 된다.

         */
virtual void                Exit();
        bool                IsExit();

		/**
			트리거들은 외부와의 피드백은 통신을 통해 처리 한다.
			통신은 메시지를 통해서 처리되며 메시지는 Send 와 Post 가 존재 한다.
			Send 는 호툴 되는 순간에 바로 전달하며, Post 는 저장 된 후 FrameMove 에서 호출 처리 된다.

		 */
		void				SendMessage(SMessage* msg);
		void				PostMessage(SpMessage msg);

		/**
			형변환 편의용 템플릿 함수
            형변환 시 return 값(bool형) 처리를 반드시 수행 해야 한다.

		 */
        template<class T>
        inline bool			CastFinger(T*& ref) { return (ref = dynamic_cast<T*>(m_pFinger)) != 0; }
        template<class T>
        inline bool			CastTrigger(const T*& ref) { return (ref = dynamic_cast<const T*>(m_pTrigger)) != 0; }
        template<class T>
        inline bool			CastInstance(T*& ref) { return (ref = dynamic_cast<T*>(this)) != 0; }

protected:
        /**
        	진행 시간을 필터링 한다.
            만약 클라가 제접 후 다시 시스템이 가동 되면 서버와의 동기화를 위해
            많은 시간을 진행 시켜야 하는데 이럴 떄 자신이 사용한 시간을 필터하기 위한 용도로 사용 된다.
            TriggerFinger::FrameMove 를 참조 한다.

	        \param fElapsedTime 진행 된 시간
         */
virtual void                ProcessTime(float fElapsedTime);

		/**
			트리거의 상태를 체크 한다.
			인스턴스는 자신이 처리하는 트리거의 상태를 가진다.
			이를 통해 외부에서 적당한 처리를 수행한다.
			현재는 삭제를 위한 용도로 사용 된다.

		 */
        void				SetState(TriggerState state) { m_state = state; }
		bool				IsState(TriggerState state) { return m_state == state; }		

		/**
			외부와의 피드백용 메시지를 처리 한다.

		 */
		void				MessageProc(SMessage* msg);

		CTriggerFinger*		m_pFinger;		//@ 실행 작동기
		const CTrigger*		m_pTrigger;		//@ 실행 트리거
		MessageContainer	m_msgs;			//@ 메시지 저장소

		float				m_fElapsedTime;	//@ 이전 프레임 에서 현재 프레임까지 걸린 시간
		float				m_fTime;		//@ 트리거가 시작된 이후 누적 시간
        float               m_fLifeTime;    //@ 생명 시간, 기본적으로 트리거의 LifeTime 의 값이 다.

		TriggerState		m_state;		//@ 트리거 상태
};

} // end namespace

#endif