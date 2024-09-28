#ifndef CServerPartyTrigger_H
#define CServerPartyTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	파티 관련 처리를 수행 한다.
	현재는 파탈시 인던에서 나가는 처리를 한다.
	파티 관련 처리를 이곳에서 수행하도록 모듈화 한다.
	
 */
class CServerPartyTrigger	
    : public CServerTrigger<CTrigger>
{
public:
		CServerPartyTrigger();

		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType		    GetType() const { return TRIGGER_TYPE_PARTY; }

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void			    Bind(CTriggerScript* pScript);

        /**
        	트리거와 짝이 되는 인스턴스를 생성 한다.
	        
         */
virtual CTriggerInstance*   CreateInstance() const;

		/**
			바인딩 인터페이스 함수들

		 */
		void		        SetAutoOut(bool bAuto) { m_bAutoOut = bAuto; }
		bool				GetAutoOut() const { return m_bAutoOut; }

protected:
		bool				m_bAutoOut;	//@ 파탈시 인던에서 나가게 할지 유무
};

} // end namespace

#endif