#ifndef SServerEntryMessage_H
#define SServerEntryMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	맵 진입 메시지
	유저가 맵에 진입했을 때 사용 된다.
    제접할 때도 쓰이므로 뭔가 유저 진입에 따른 피드백 처리등에 활용할 수 있다.
	
 */
struct SServerEntryMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    DWORD dwCharDbNum; //@ 입장한 캐릭터 DB ID
};

} // end namespace

#endif