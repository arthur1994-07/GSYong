#ifndef SClientStartStageMessage_H
#define SClientStartStageMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	���������� ���۵Ǹ� ���� �ȴ�.
	
 */
struct SClientStartStageMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);
    
    int nTime;      //@ �������� ��� �ð�
};

} // end namespace

#endif