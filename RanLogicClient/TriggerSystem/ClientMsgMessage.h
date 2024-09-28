#ifndef SClientMsgMessage_H
#define SClientMsgMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	�޽����� ��� �϶�� �޽���
	�츮�� �� �޽����� ����(Command) �ɶ� �޽����� Ŭ���̾�Ʈ�� ���� �Ѵ�.
	
 */
struct SClientMsgMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    TSTRING strMsg; //@ ����� �޽��� ����(XML Ű����)
    int nTime;      //@ �޽��� ��� �ð�
};

} // end namespace

#endif