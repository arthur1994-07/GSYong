#ifndef SClientRebirthMessage_H
#define SClientRebirthMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"
#include "../../RanLogic/Character/GLCharDefine.h"

namespace TriggerSystem
{

/**	
	클라이언트에서 부활이 발생할 때 호출되어 진다.	
	
 */
struct SClientRebirthMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);	
};

} // end namespace

#endif