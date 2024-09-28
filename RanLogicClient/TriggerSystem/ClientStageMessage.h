#ifndef SClientStageMessage_H
#define SClientStageMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	스테이지 상태 메시지
    현재 스테이지와 남은 시간등을 전송 된다.
	
 */
struct SClientStageMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    int nStage; //@ 현재 스테이지
    int nTime;  //@ 남은 시간(sec)
    int nState; //@ 현재 상태
};

} // end namespace

#endif