#ifndef SServerDieMessage_H
#define SServerDieMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	캐릭터가 죽었을 경우 사용 된다.
    이에 맞는 적당한 처리를 수행 한다.
	
 */
struct SServerDieMessage
	: public SMessage
{
    SServerDieMessage()
        : dwCharDbNum(0)
        , bIsRevival(0) {}

	void Command(CTriggerInstance* pInstance);

    DWORD dwCharDbNum;  //@ 죽은 캐릭터 Db ID

    //! 중간에 추가되었다.
    bool  bIsRevival;   //@ 부활할 경우.
};

} // end namespace

#endif