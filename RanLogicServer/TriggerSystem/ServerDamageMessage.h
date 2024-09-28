#ifndef SServerDamageMessage_H
#define SServerDamageMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	���Ϳ� �������� �� ��� ȣ�� �ȴ�.
	
 */
struct SServerDamageMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    DWORD dwCharDbNum;  //@ �������� �� ĳ���� Db ID
    DWORD dwDamage;     //@ ��������
};

} // end namespace

#endif