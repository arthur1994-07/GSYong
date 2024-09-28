#ifndef SServerRetryMessage_H
#define SServerRetryMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	재도전을 요청 한다.
    재도전은 스테이지가 종료 되면 처리되어야 한다.
	
 */
struct SServerRetryReqMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);
};

/**	
	유저가 재도전에 대한 응답 메시지이다.
    우리는 재도전에 모두 응하면 재도전 시키고
    한명이라도 응하지 않으면 탈출 시킨다.
	
 */
struct SServerRetryAnsMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    DWORD dwCharDbNum;  //@ 응답 캐릭터
    bool  bOk;          //@ 응답 결과
};

} // end namespace

#endif