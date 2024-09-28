#ifndef SExitMessage_H
#define SExitMessage_H

#include "Message.h"

namespace TriggerSystem
{

/**	
	트리거 삭제 메시지
    우리는 트리거가 삭제될 때 이 트리거와 생명을 같이 하는 
    트리거들을 같이 삭제 해 준다.
	
 */
struct SExitMessage
	: public SMessage
{
	SExitMessage() : trigger(0) {}

	void Command(CTriggerInstance* pInstance);

    const CTrigger* trigger;  //@ 삭제되는 트리거
};

} // end namespace

#endif