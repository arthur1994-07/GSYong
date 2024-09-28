#ifndef SClientMsgMessage_H
#define SClientMsgMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	메시지를 출력 하라는 메시지
	우리는 이 메시지가 수행(Command) 될때 메시지를 클라이언트에 전달 한다.
	
 */
struct SClientMsgMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    TSTRING strMsg; //@ 출력할 메시지 내용(XML 키워드)
    int nTime;      //@ 메시지 재생 시간
};

} // end namespace

#endif