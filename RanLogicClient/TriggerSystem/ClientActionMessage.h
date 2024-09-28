#ifndef SClientActionMessage_H
#define SClientActionMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"
#include "../../RanLogic/Character/GLCharDefine.h"

namespace TriggerSystem
{

/**	
	클라이언트의 대상이 액션을 변경하게 되면 호출 된다.
	현재는 아바타 캐릭에 한정 하지만 향후 필요시 확장해 사용해야 한다.
	
 */
struct SClientActionMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

	DWORD dwCharDbNum;			//@ 변경하는 캐릭터 DB Num
	EMACTIONTYPE ePreAction;	//@ 변경전 액션
	EMACTIONTYPE eNextAction;	//@ 변경할 액션
};

} // end namespace

#endif