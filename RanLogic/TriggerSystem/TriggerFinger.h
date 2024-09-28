#ifndef CTriggerFinger_H
#define CTriggerFinger_H

#include "TriggerSystemType.h"

namespace TriggerSystem
{

/**
	트리거 작동기
	트리거 동작을 관리 한다.
	외부에서는 이 작동기를 가지고 트리거를 작동 시킨다.
	
 */
class CTriggerFinger
{
public:
		CTriggerFinger();
virtual ~CTriggerFinger();

		/**
			트리거를 작동 시킨다.
          
		 */
virtual	void 						Start();

		/**
			트리거를 정지 시킨다.
			Pause 는 필요성이 없을것 같아 구현하지 않는다.

		 */
virtual void 						Stop();

		/**
			트리거의 프레임을 처리 한다.
			트리거는 Child 와 Sibling 을 가지는데 Sibling 은 동시에 실행 되어야 할 트리거 이므로
			이들 모두 프레임 처리 해 주어야 한다.

			\param fAccTime 전체 누적 시간(sec 단위, 1 -> 1초)
			\param fElapsedTime 프레임 갱신 시간(sec 단위, 1 -> 1초)
		 */
virtual void 						FrameMove(float fAccTime, float fElapsedTime);

		/**
			트리거들은 외부와의 피드백은 통신을 통해 처리 한다.
			통신은 메시지를 통해서 처리되며 메시지는 Send 와 Post 가 존재 한다.
			Send 는 호툴 되는 순간에 바로 전달하며, Post 는 저장 된 후 FrameMove 에서 호출 처리 된다.

			Send 는 즉각적으로 처리되기 때문에 포인터로 넘기고
			Post 는 프레임에 처리되므로 메모리 할당한 포인터를 넘겨야 한다.
		 */
virtual void						SendMessage(SMessage* msg);
virtual void						PostMessage(SpMessage msg);

		/**
			트리거를 추가 한다.
            우리 메인 Trigger 에 붙은 Sibling 을 계속 적으로 
            현재 실행 Trigger 로 추가 해 준다.
			
		 */
		void						AddTrigger(const CTrigger* pTrigger);

protected:
        /**
        	트리거 작동 시킨다.

        	\param start 작동할 시작 트리거
         */
        void 						Start(CTrigger* start);

		TriggerInstanceContainer	m_cur;      //@ 현재 실행중인 트리거인스턴스들
        TriggerInstanceContainer	m_new;      //@ 새로 생성한 트리거인스턴스들        
        float                       m_fTime;    //@ 진행 시간
};

} // end namespace

#endif