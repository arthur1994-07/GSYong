#ifndef SClientRebirthMessage_H
#define SClientRebirthMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"
#include "../../RanLogic/Character/GLCharDefine.h"

namespace TriggerSystem
{

/**	
	Ŭ���̾�Ʈ���� ��Ȱ�� �߻��� �� ȣ��Ǿ� ����.	
	
 */
struct SClientRebirthMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);	
};

} // end namespace

#endif