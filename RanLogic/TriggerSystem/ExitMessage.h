#ifndef SExitMessage_H
#define SExitMessage_H

#include "Message.h"

namespace TriggerSystem
{

/**	
	Ʈ���� ���� �޽���
    �츮�� Ʈ���Ű� ������ �� �� Ʈ���ſ� ������ ���� �ϴ� 
    Ʈ���ŵ��� ���� ���� �� �ش�.
	
 */
struct SExitMessage
	: public SMessage
{
	SExitMessage() : trigger(0) {}

	void Command(CTriggerInstance* pInstance);

    const CTrigger* trigger;  //@ �����Ǵ� Ʈ����
};

} // end namespace

#endif