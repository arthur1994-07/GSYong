#ifndef SServerPortalMessage_H
#define SServerPortalMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	�ý��ۿ� ��Ż(�̵�)�� ��û �Ѵ�.
    Ŭ���̾�Ʈ�� ���� ��Ż ��û�� ���� �� ȣ�� �ȴ�.

    ��Ż�� ���µǾ� �ִٸ� ���� �۵� �ȴ�.
    �̷��� �����ν� ������ ���� ��Ż(��ŷ��)�� ����
    �츮�� ���ϴ� �������� ��Ż�� �ǵ��� ó���� �� �ִ�.
	
 */
struct SServerPortalMessage
	: public SMessage
{
	void Command(CTriggerInstance* pInstance);

    DWORD dwFromMapID;  //@ �̵� ��û �� ID
    DWORD dwFromGateID; //@ �̵� ��û ����Ʈ ID
    DWORD dwToMapID;    //@ �̵� �� �� ID
    DWORD dwToGateID;   //@ �̵� �� ����Ʈ ID
};

} // end namespace

#endif