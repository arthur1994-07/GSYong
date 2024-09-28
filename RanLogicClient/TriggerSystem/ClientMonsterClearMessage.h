#ifndef SClientMonsterClearMessage_H
#define SClientMonsterClearMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	���� Ŭ���� �޽���
    ���Ͱ� ���� �Ǹ� ���� �ȴ�.
	
 */
struct SClientMonsterClearMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    int nCur;   //@ ���� óġ�� ���� ��
    int nAll;   //@ ��ü óġ �ؾ� �� ���� ��
};

} // end namespace

#endif