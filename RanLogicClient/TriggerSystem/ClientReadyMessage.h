#ifndef SClientReadyMessagee_H
#define SClientReadyMessagee_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	���� ��� ���� �޽���
    ������ ���� ���� ���¸� ���� �޾����� ȣ�� �ȴ�.
	
 */
struct SClientReadyMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    int nCur;   //@ ���� ���� �ο�
    int nAll;   //@ ��ü ���� �ؾ� �� �ο�
    int nTime;  //@ ���� ��� ���� �ð�(sec)
};

} // end namespace

#endif