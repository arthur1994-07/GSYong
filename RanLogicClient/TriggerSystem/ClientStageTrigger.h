#ifndef CClientStageTrigger_H
#define CClientStageTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ClientTriggerDecleare.h"
#include "ClientTrigger.h"

namespace TriggerSystem
{

/**
    스테이지 표시기
    현재 스테이지를 표시 한다.
	
 */
class CClientStageTrigger
	: public CClientTrigger<CTrigger>
{
public:
		CClientStageTrigger();

		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_STAGE; }

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void		        Bind(CTriggerScript* pScript);

        /**
        	트리거와 짝이 되는 인스턴스를 생성 한다.
	        
         */
virtual CTriggerInstance*   CreateInstance() const;

        /**
            마지막 스테이지

         */
        void                SetEndStage(int end) { m_end = end; }
        const int           GetEndStage() const { return m_end; }

        /**
            표시 스테이지 간격

         */
        void                SetRange(int range) { m_range = range; }
        const int           GetRange() const { return m_range; }

protected:
        int                 m_end;      //@ 끝 스테이지
        int                 m_range;    //@ 스테이지 표시 간격
};

} // end namespace

#endif