#ifndef SServerHealMessage_H
#define SServerHealMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	ĳ���Ͱ� ���� �־��� ��� ȣ�� �ȴ�.
	
 */
struct SServerHealMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    DWORD dwCharDbNum;  //@ ���� ������ ĳ���� Db ID
    DWORD dwHeal;       //@ ���� ����
};

} // end namespace

#endif