#ifndef SClientMonsterClearMessage_H
#define SClientMonsterClearMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	몬스터 클리어 메시지
    몬스터가 제거 되면 전달 된다.
	
 */
struct SClientMonsterClearMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    int nCur;   //@ 현재 처치한 몬스터 수
    int nAll;   //@ 전체 처치 해야 할 몬스터 수
};

} // end namespace

#endif