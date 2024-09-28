#ifndef CServerRetryTrigger_H
#define CServerRetryTrigger_H

#include "../../RanLogic/TriggerSystem/RetryTrigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
    재도전 트리거
    스테이지에서 죽었을 시 스킬로 부활이 가능하지만
    일반 부활은 불가능 하게 처리 한다.
	
 */
class CServerRetryTrigger	
    : public CServerTrigger<CRetryTrigger>
{
public:
		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_RETRY; }

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void		        Bind(CTriggerScript* pScript);

        /**
        	트리거와 짝이 되는 인스턴스를 생성 한다.
	        
         */
virtual CTriggerInstance*   CreateInstance() const;
};

} // end namespace

#endif