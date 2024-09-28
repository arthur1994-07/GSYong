#ifndef CServerMonsterClearTrigger_H
#define CServerMonsterClearTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	몬스터를 입력된 마리수 만큼 클리어(죽여야)
	성공하는 미션 트리거를 의미 한다.
	
 */
class CServerMonsterClearTrigger	
    : public CServerTrigger<CTrigger>
{
public:
		CServerMonsterClearTrigger();

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

		/**
			바인딩 인터페이스 함수들

		 */
		void		        SetCount(int count) { m_count = count; }
		const int			GetCount() const { return m_count; }

private:
		int			        m_count;	//@ 클리어 마리수
};

} // end namespace

#endif