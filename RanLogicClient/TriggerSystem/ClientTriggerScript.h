#ifndef CClientTriggerScript_H
#define CClientTriggerScript_H

#include "../../RanLogic/TriggerSystem/TriggerScript.h"

namespace TriggerSystem
{

/**
	클라이언트 트리거에 기반한 트리거 스크립트

 */
class CClientTriggerScript
	: public CTriggerScript
{
public:
	
		/**
			스크립트 실행 전 처리를 수행 한다.
			스크립트와의 바인드 처리를 수행한다.

		 */
virtual void			Init();

};

} // end namespace

#endif
