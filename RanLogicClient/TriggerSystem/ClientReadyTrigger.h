#ifndef CClientReadyTrigger_H
#define CClientReadyTrigger_H

#include "ClientTriggerDecleare.h"
#include "ClientTrigger.h"

namespace TriggerSystem
{

/**
	클라이언트 입장 대기 트리거
	설명은 CReadyTrigger 참조
	
 */
class CClientReadyTrigger
	: public CClientTrigger<CTrigger>
{
public:
		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType	        GetType() const { return TRIGGER_TYPE_READY; }

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void		        Bind(CTriggerScript* pScript);

        /**
            트리거와 짝이 되는 인스턴스를 생성 한다..

         */
virtual CTriggerInstance*   CreateInstance() const;
};

} // end namespace

#endif