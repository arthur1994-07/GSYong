#ifndef SServerFindTriggerMessage_H
#define SServerFindTriggerMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	�������� Ʈ���� ������ ã�´�.
    ������ Ʈ���Ÿ� ã�� �� ��� �Ѵ�.

 */
struct SServerFindStageTriggerMessage
	: public SSystemMessage
{
	SServerFindStageTriggerMessage() : trigger(0) {}

	void Command(CTrigger* pTrigger);

    DWORD       id;     //@ ã�� �� ID
    int         level;  //@ ã�� �� ����
    int         stage;  //@ ã�� ��������

    CTrigger*   trigger;//@ ã�� Ʈ����
};

/**
	��Ż ���� Ʈ���Ÿ� ã�´�.
	�츮�� GM ��ɾ ���� Ư�� ���������� �̵��ϰ�
	���� �� �̸� ���� ������ ���� �Ѵ�.

	�� �޽����� SServerFindStageTriggerMessage �� ���� �������� Ʈ���Ÿ� ã�� ��
	�ش� �������� Ʈ���ſ� SendMessage �� ���� �����ؾ� �Կ� ���� �Ѵ�.

 */
struct SServerFindPortalOpenTriggerMessage
	: public SSystemMessage
{
	SServerFindPortalOpenTriggerMessage() : trigger(0) {}

	void Command(CTrigger* pTrigger);

	CTrigger*   trigger;//@ ã�� Ʈ����
};

} // end namespace

#endif