#ifndef SClientActionMessage_H
#define SClientActionMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"
#include "../../RanLogic/Character/GLCharDefine.h"

namespace TriggerSystem
{

/**	
	Ŭ���̾�Ʈ�� ����� �׼��� �����ϰ� �Ǹ� ȣ�� �ȴ�.
	����� �ƹ�Ÿ ĳ���� ���� ������ ���� �ʿ�� Ȯ���� ����ؾ� �Ѵ�.
	
 */
struct SClientActionMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

	DWORD dwCharDbNum;			//@ �����ϴ� ĳ���� DB Num
	EMACTIONTYPE ePreAction;	//@ ������ �׼�
	EMACTIONTYPE eNextAction;	//@ ������ �׼�
};

} // end namespace

#endif