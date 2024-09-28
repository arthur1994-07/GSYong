#ifndef SServerDieMessage_H
#define SServerDieMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	ĳ���Ͱ� �׾��� ��� ��� �ȴ�.
    �̿� �´� ������ ó���� ���� �Ѵ�.
	
 */
struct SServerDieMessage
	: public SMessage
{
    SServerDieMessage()
        : dwCharDbNum(0)
        , bIsRevival(0) {}

	void Command(CTriggerInstance* pInstance);

    DWORD dwCharDbNum;  //@ ���� ĳ���� Db ID

    //! �߰��� �߰��Ǿ���.
    bool  bIsRevival;   //@ ��Ȱ�� ���.
};

} // end namespace

#endif