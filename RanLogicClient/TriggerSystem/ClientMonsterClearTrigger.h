#ifndef CClientMonsterClearTrigger_H
#define CClientMonsterClearTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ClientTriggerDecleare.h"
#include "ClientTrigger.h"

namespace TriggerSystem
{

/**
	몬스터 클리어 체크 표시기
    현재 잡은 몬스터, 잡아야 하는 몬스터를 표시 한다.
	
 */
class CClientMonsterClearTrigger
	: public CClientTrigger<CTrigger>
{
public:
		CClientMonsterClearTrigger();

		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_MONSTERCLEAR; }

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