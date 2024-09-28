#ifndef CClientPortalCloseTrigger_H
#define CClientPortalCloseTrigger_H

#include "../../RanLogic/TriggerSystem/PortalCloseTrigger.h"
#include "ClientTriggerDecleare.h"
#include "ClientTrigger.h"

namespace TriggerSystem
{

/**
	클라이언트 포탈 닫기 트리거
	스테이지 시작시 포탈을 닫을 때 사용 한다.
	
 */
class CClientPortalCloseTrigger
	: public CClientTrigger<CPortalCloseTrigger>
{
public:
		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType	GetType() const { return TRIGGER_TYPE_PORTALCLOSE; }

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void		Bind(CTriggerScript* pScript);

		/**
			트리거가 시작되면 포탈을 닫아 준다.

		 */
		void        Start(CTriggerInstance* pInstance) const;
};

} // end namespace

#endif