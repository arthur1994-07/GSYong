#ifndef SServerEntryMessage_H
#define SServerEntryMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	�� ���� �޽���
	������ �ʿ� �������� �� ��� �ȴ�.
    ������ ���� ���̹Ƿ� ���� ���� ���Կ� ���� �ǵ�� ó��� Ȱ���� �� �ִ�.
	
 */
struct SServerEntryMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    DWORD dwCharDbNum; //@ ������ ĳ���� DB ID
};

} // end namespace

#endif