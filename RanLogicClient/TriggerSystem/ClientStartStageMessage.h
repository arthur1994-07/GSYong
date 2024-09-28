#ifndef SClientStartStageMessage_H
#define SClientStartStageMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	스테이지가 시작되면 전달 된다.
	
 */
struct SClientStartStageMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);
    
    int nTime;      //@ 스테이지 출력 시간
};

} // end namespace

#endif