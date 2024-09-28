#ifndef SServerFindTriggerMessage_H
#define SServerFindTriggerMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	스테이지 트리거 파일을 찾는다.
    실행할 트리거를 찾을 때 사용 한다.

 */
struct SServerFindStageTriggerMessage
	: public SSystemMessage
{
	SServerFindStageTriggerMessage() : trigger(0) {}

	void Command(CTrigger* pTrigger);

    DWORD       id;     //@ 찾을 맵 ID
    int         level;  //@ 찾을 맵 레벨
    int         stage;  //@ 찾을 스테이지

    CTrigger*   trigger;//@ 찾은 트리거
};

/**
	포탈 오픈 트리거를 찾는다.
	우리는 GM 명령어를 통해 특정 스테이지로 이동하고
	싶을 때 이를 통해 정보를 추출 한다.

	이 메시지는 SServerFindStageTriggerMessage 를 통해 스테이지 트리거를 찾은 후
	해당 스테이지 트리거에 SendMessage 를 통해 추출해야 함에 주의 한다.

 */
struct SServerFindPortalOpenTriggerMessage
	: public SSystemMessage
{
	SServerFindPortalOpenTriggerMessage() : trigger(0) {}

	void Command(CTrigger* pTrigger);

	CTrigger*   trigger;//@ 찾은 트리거
};

} // end namespace

#endif