#ifndef SServerRetryMessage_H
#define SServerRetryMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	�絵���� ��û �Ѵ�.
    �絵���� ���������� ���� �Ǹ� ó���Ǿ�� �Ѵ�.
	
 */
struct SServerRetryReqMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);
};

/**	
	������ �絵���� ���� ���� �޽����̴�.
    �츮�� �絵���� ��� ���ϸ� �絵�� ��Ű��
    �Ѹ��̶� ������ ������ Ż�� ��Ų��.
	
 */
struct SServerRetryAnsMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    DWORD dwCharDbNum;  //@ ���� ĳ����
    bool  bOk;          //@ ���� ���
};

} // end namespace

#endif