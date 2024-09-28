#ifndef CServerMsgInstance_H
#define CServerMsgInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
	�޽����� ���� �Ѵ�.
    �޽����� XML ���ǵ� Ű�����̾�� �Ѵ�.
    �� �޽����� ���� Ŭ���̾�Ʈ�� ���� ���¿� �°� UI�� ��� �ϰ� �ȴ�.

 */
class CServerMsgInstance
    : public CServerTriggerInstance
{
public:
        /**
            �ν��Ͻ��� ������ �� ȣ�� �ȴ�.

         */
        void		Start();

        /**
            ���� ���¸� ���� �Ѵ�.

            \param dwCharDbNum ���¸� ������ ĳ���� DB ID, 0 �ϰ�� �� ��ü�� ����.
         */
        void        Update(DWORD dwCharDbNum = 0);

        /**
        	������ �������� ��� ȣ�� �ȴ�.

         */
        void        EntryPC(SMessage* msg);
};

} // end namespace

#endif