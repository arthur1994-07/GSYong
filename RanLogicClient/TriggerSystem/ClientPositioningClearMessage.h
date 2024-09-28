#ifndef SClientPositioningClearMessage_H
#define SClientPositioningClearMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	��ġ Ŭ���� �޽���
    Ư�� ��ġ�� ��ġ�ϸ� Ŭ���� �ȴ�.
	
 */
struct SClientPositioningClearMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    int nX;				//@ Ŭ���� ��ġ X
    int nY;				//@ Ŭ���� ��ġ Y
    int nNow;           //@ ���� �Ϸ��� ���
    int nNeed;          //@ �̼� �Ϸῡ �ʿ��� ���
	bool bCompleted;	//@ �̼� �Ϸ� ����
};

} // end namespace

#endif