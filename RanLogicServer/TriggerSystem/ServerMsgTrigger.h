#ifndef CServerMsgTrigger_H
#define CServerMsgTrigger_H

#include "../../RanLogic/TriggerSystem/Trigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	메시지 트리거
	특정 상황에 유저들에게 메시지를 출력 한다.
	
 */
class CServerMsgTrigger	
    : public CServerTrigger<CTrigger>
{
public:
		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType		    GetType() const { return TRIGGER_TYPE_MSG; }

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
		void			    SetMessage(TSTRING msg) { m_msg = msg; }
		const TSTRING	    GetMessage() const { return m_msg; }

protected:
		TSTRING			    m_msg;	//@ 메시지 내용
};

} // end namespace

#endif