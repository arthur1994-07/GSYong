#ifndef CClientPositioningClearTrigger_H
#define CClientPositioningClearTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ClientTriggerDecleare.h"
#include "ClientTrigger.h"

namespace TriggerSystem
{

/**
	위치 클리어 체크 표시기
    특정 위치에 위치하면 클리어 된다.
	
 */
class CClientPositioningClearTrigger
	: public CClientTrigger<CTrigger>
{
public:
		CClientPositioningClearTrigger();

		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_POSITIONINGCLEAR; }

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