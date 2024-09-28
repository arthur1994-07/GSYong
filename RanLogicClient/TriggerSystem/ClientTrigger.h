#ifndef CClientTrigger_H
#define CClientTrigger_H

#include "ClientTriggerInstance.h"

namespace TriggerSystem
{

/**
    Ŭ���̾�Ʈ Ʈ����
    Ŭ���̾�Ʈ���� ����ϴ� ��� Ʈ������ ���� �������̽�
    ��� Ŭ���̾�Ʈ Ʈ���� ���� �������̽� �ӿ� ���� �Ѵ�.
	
 */
template<class T>
class CClientTrigger
	: public T
{
public:
        /**
        	Ŭ���̾�Ʈ Ʈ���Ŵ� �⺻������ Ŭ���̾�Ʈ �ν��Ͻ��� �����ϵ��� �Ѵ�.
	        
         */
virtual CTriggerInstance*   CreateInstance() const { return new CClientTriggerInstance; }
};

} // end namespace

#endif