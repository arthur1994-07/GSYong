#ifndef SClientFindTriggerMessage_H
#define SClientFindTriggerMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"
#include <set>

namespace TriggerSystem
{

/**	
	Ʈ���� ������ ã�´�.
    ������ Ʈ���Ÿ� ã�� �� ��� �Ѵ�.

    ���� �� ID, ����, �������� ������ �������� ã�µ�
    �̰��� ��� �������� �������� �����Ƿ� ���� Ȯ���
    �̰��� �̸��� �������� ���� �����ϴ°� ���� ���̴�.

 */
struct SClientFindTriggerMessage
	: public SSystemMessage
{
	void Command(CTrigger* pTrigger);
    
    DWORD       id;     //@ ã�� �� ID
    int         level;  //@ ã�� �� ����
    int         stage;  //@ ã�� ��������
    
    CTrigger*   trigger;//@ ã�� Ʈ����
};

} // end namespace

#endif