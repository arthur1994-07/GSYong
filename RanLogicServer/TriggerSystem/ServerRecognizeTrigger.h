#ifndef CServerRecognizeTrigger_H
#define CServerRecognizeTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	서버 인지 시간 트리거
	몬스터가 캐릭터를 적으로 인지하는 시간을 처리 한다.
	
 */
class CServerRecognizeTrigger	
    : public CServerTrigger<CTrigger>
{
public:
		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType			GetType() const { return TRIGGER_TYPE_RECOGNIZE; }

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void				Bind(CTriggerScript* pScript);

        /**
        	트리거와 짝이 되는 인스턴스를 생성 한다.
	        
         */
virtual CTriggerInstance*   CreateInstance() const;

		/**
			바인딩 인터페이스 함수들

		 */
		void				SetRecognizeTime(float time) { m_time = time; }
		const float			GetRecognizeTime() const { return m_time; }

protected:
		float				m_time;	//@ 인지 시간(1 = 1s)
};

} // end namespace

#endif