#ifndef CServerTrigger_H
#define CServerTrigger_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
    ���� Ʈ����
    �������� ����ϴ� ��� Ʈ������ ���� �������̽�
    ��� ���� Ʈ���� ���� �������̽� �ӿ� ���� �Ѵ�.
	
 */
template<class T>
class CServerTrigger
	: public T
{
public:
        /**
        	���� Ʈ���Ŵ� �⺻������ ���� �ν��Ͻ��� �����ϵ��� �Ѵ�.
	        
         */
virtual CTriggerInstance*   CreateInstance() const { return new CServerTriggerInstance; }
};

} // end namespace

#endif