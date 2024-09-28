#ifndef CServerPostStageTrigger_H
#define CServerPostStageTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
    스테이지 후 처리
    우리는 스테이지 종료(성공 or 실패) 가 되면 종료 상태 UI 를 임의의 시간 동안 출력 후
    실제 스테이지 종료를 처리 한다. 이 딜레이를 위해 만들었으며
    만약 스테이지에 이 모든 처리가 있으면 복잡도가 높아 짐으로 분리하면 관리도 편해서 만듬
	
 */
class CServerPostStageTrigger	
    : public CServerTrigger<CTrigger>
{
public:
		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_POSTSTAGE; }

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
        void		        SetTime(int time) { m_time = time; }
        const int	        GetTime() const { return m_time; }

protected:
        int			        m_time;	//@ 스테이지 종료 후 대기 시간
};

} // end namespace

#endif