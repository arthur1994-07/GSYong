#ifndef SClientStageMessage_H
#define SClientStageMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	�������� ���� �޽���
    ���� ���������� ���� �ð����� ���� �ȴ�.
	
 */
struct SClientStageMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    int nStage; //@ ���� ��������
    int nTime;  //@ ���� �ð�(sec)
    int nState; //@ ���� ����
};

} // end namespace

#endif