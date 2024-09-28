#ifndef CServerWaitTrigger_H
#define CServerWaitTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	서버 시간 대기 트리거
	일정 시간 동안 트리거의 진행을 대기 시킨다.
	예를들어 스테이지 클리어 후 다음 진행을 위해 몇초간 대기를 시킬 수 있다.
	
 */
class CServerWaitTrigger	
    : public CServerTrigger<CTrigger>
{
public:
		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType	GetType() const { return TRIGGER_TYPE_WAIT; }

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void		Bind(CTriggerScript* pScript);

		/**
			바인딩 인터페이스 함수들

		 */
		void		SetTime(float time) { SetLifeTime(time); }
};

} // end namespace

#endif