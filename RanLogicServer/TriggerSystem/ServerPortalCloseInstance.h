#ifndef CServerPortalCloseInstance_H
#define CServerPortalCloseInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
	��Ż�� �ݴ´�.(Ŭ�����忡���� �Ⱥ��̰�, �������忡���� �۵����� �ʰ�)
    �⺻������ ��Ż�� ���� ���� ���·� �������� ���� �Ƿ� ���� Ŭ���̾�Ʈ��
    ���� �ϸ� �ݾ� �ֵ��� �޽����� ������ �Ѵ�. �̸� ó�� �Ѵ�.
	
 */
class CServerPortalCloseInstance
    : public CServerTriggerInstance
{
public:
        CServerPortalCloseInstance();

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