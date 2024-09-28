#ifndef SServerSavePosMessage_H
#define SServerSavePosMessage_H

#include "../../RanLogic/TriggerSystem/Message.h"

namespace TriggerSystem
{

/**	
	��ġ�� ���������� ���� �޽���
    �Ϲ������� ���������� ĳ���� ��ġ�� ������� �ʴ´�.    
	
 */
struct SServerSavePosMessage
	: public SMessage
{
    SServerSavePosMessage() : bSave(true) {}

	void Command(CTriggerInstance* pInstance);

    bool bSave; //@ ��ġ�� �������� ����
};

} // end namespace

#endif