#ifndef SServerReturnMessage_H
#define SServerReturnMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	�δ��� ���� �ǰ� ���� �ִ� ������
    ���Ͻ� ȣ�� �ȴ�.
	
 */
struct SServerReturnMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    DWORD dwCharDbNum;  //@ ĳ���� DB ID
    int nWhy;           //@ ��ȯ ����
};

/**
	�ﰢ������ �δ��� ��� �ο����� 
	���� ��Ų��. �̴� Ư���� ��츦 ���� ��� �Ǿ� ����.
	����� Ʈ���Ÿ���Ʈ�� �ٽ� �ε� �Ҷ� ��� �Ǿ� ����.

 */
struct SServerReturnAllMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);
};

} // end namespace

#endif