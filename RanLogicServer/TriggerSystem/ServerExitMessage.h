#ifndef SServerExitMessage_H
#define SServerExitMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	�� ���� �޽���
	������ �ʿ��� ����Ǿ��� �� ��� �ȴ�.
	
 */
struct SServerExitMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    DWORD dwCharDbNum; //@ ����� ĳ���� DB ID
};

} // end namespace

#endif