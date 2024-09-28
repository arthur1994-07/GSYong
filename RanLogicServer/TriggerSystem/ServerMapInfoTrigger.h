#ifndef CServerMapInfoTrigger_H
#define CServerMapInfoTrigger_H

#include "../../RanLogic/TriggerSystem/MapInfoTrigger.h"
#include "ServerTriggerDecleare.h"
#include "ServerTrigger.h"

namespace TriggerSystem
{

/**
	서버쪽 맵 정보를 가진다.	
	
 */
class CServerMapInfoTrigger
    : public CServerTrigger<CMapInfoTrigger>
{
public:
		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType		GetType() const { return TRIGGER_TYPE_MAPINFO; }

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void			Bind(CTriggerScript* pScript);
};

} // end namespace

#endif