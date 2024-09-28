#ifndef SClientFindTriggerMessage_H
#define SClientFindTriggerMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"
#include <set>

namespace TriggerSystem
{

/**	
	트리거 파일을 찾는다.
    실행할 트리거를 찾을 때 사용 한다.

    현재 맵 ID, 레벨, 스테이지 정보를 바탕으로 찾는데
    이것은 몇몇 컨텐츠에 응집력이 있으므로 향후 확장시
    이것의 이름을 컨텐츠명에 맞춰 변경하는게 좋을 것이다.

 */
struct SClientFindTriggerMessage
	: public SSystemMessage
{
	void Command(CTrigger* pTrigger);
    
    DWORD       id;     //@ 찾을 맵 ID
    int         level;  //@ 찾을 맵 레벨
    int         stage;  //@ 찾을 스테이지
    
    CTrigger*   trigger;//@ 찾은 트리거
};

} // end namespace

#endif