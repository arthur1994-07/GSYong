#ifndef SClientReadyMessagee_H
#define SClientReadyMessagee_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	입장 대기 상태 메시지
    서버로 부터 입장 상태를 전달 받았을시 호출 된다.
	
 */
struct SClientReadyMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    int nCur;   //@ 현재 입장 인원
    int nAll;   //@ 전체 입장 해야 할 인원
    int nTime;  //@ 입장 대기 남은 시간(sec)
};

} // end namespace

#endif