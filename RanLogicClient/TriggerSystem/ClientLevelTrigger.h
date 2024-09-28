#ifndef CClientLevelTrigger_H
#define CClientLevelTrigger_H

#include "../../RanLogic/TriggerSystem/LevelTrigger.h"
#include "ClientTriggerDecleare.h"
#include "ClientTrigger.h"

namespace TriggerSystem
{

/**
	클라이언트 레벨 트리거
	난이도를 위해 서브 스크립트를 로딩, 가동 등의 처리를 수행 한다.
	
 */
class CClientLevelTrigger
	: public CClientTrigger<CLevelTrigger>
{
public:
		/**
			트리거를 구분하는 고유 type 를 출력 한다.
			자세한 설명은 CTrigger::GetType() 설명 참조

		 */
virtual TriggerType	GetType() const { return TRIGGER_TYPE_LEVEL; }

		/**
			스크립트와의 바인딩을 처리 한다.
			
		 */
virtual	void		Bind(CTriggerScript* pScript);

		/**	
			레벨 파일은 서브로 스크립트 파일을 가지며
			다시 스크립트를 로딩 하므로써 멀티로 스크립트를 가질 수 있다.

		 */
virtual void		Load();
};

} // end namespace

#endif