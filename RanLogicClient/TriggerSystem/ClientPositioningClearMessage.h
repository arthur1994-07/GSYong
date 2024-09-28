#ifndef SClientPositioningClearMessage_H
#define SClientPositioningClearMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	위치 클리어 메시지
    특정 위치에 위치하면 클리어 된다.
	
 */
struct SClientPositioningClearMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    int nX;				//@ 클리어 위치 X
    int nY;				//@ 클리어 위치 Y
    int nNow;           //@ 현재 완료한 사람
    int nNeed;          //@ 미션 완료에 필요한 사람
	bool bCompleted;	//@ 미션 완료 유무
};

} // end namespace

#endif