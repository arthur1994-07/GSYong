#ifndef SServerExitMessage_H
#define SServerExitMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	맵 퇴출 메시지
	유저가 맵에서 퇴출되었을 때 사용 된다.
	
 */
struct SServerExitMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    DWORD dwCharDbNum; //@ 퇴출된 캐릭터 DB ID
};

} // end namespace

#endif