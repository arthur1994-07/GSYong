#ifndef CServerStartStageTrigger_H
#define CServerStartStageTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	스테이지 시작 트리거
    스테이지 시작에 관련된 데이터를 가진다.
	
 */
class CServerStartStageTrigger	
    : public CServerTrigger<CTrigger>
{
public:
		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType		    GetType()  const { return TRIGGER_TYPE_STARTSTAGE; }

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void			    Bind(CTriggerScript* pScript);

        /**
        	트리거와 짝이 되는 인스턴스를 생성 한다.
	        
         */
virtual CTriggerInstance*   CreateInstance() const;
};

} // end namespace

#endif