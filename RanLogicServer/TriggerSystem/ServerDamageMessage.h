#ifndef SServerDamageMessage_H
#define SServerDamageMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	몬스터에 데미지를 줄 경우 호출 된다.
	
 */
struct SServerDamageMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    DWORD dwCharDbNum;  //@ 데미지를 준 캐릭터 Db ID
    DWORD dwDamage;     //@ 데미지량
};

} // end namespace

#endif