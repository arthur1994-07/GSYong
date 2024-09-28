#ifndef SServerHealMessage_H
#define SServerHealMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	캐릭터가 힐을 넣었을 경우 호출 된다.
	
 */
struct SServerHealMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    DWORD dwCharDbNum;  //@ 힐을 시전한 캐릭터 Db ID
    DWORD dwHeal;       //@ 넣은 힐량
};

} // end namespace

#endif