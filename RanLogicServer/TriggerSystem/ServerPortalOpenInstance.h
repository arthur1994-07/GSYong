#ifndef CServerPortalOpenInstance_H
#define CServerPortalOpenInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

struct SServerPortalMessage;

/**
	��Ż�� ���� �Ѵ�.(Ŭ�����忡���� ���̰�, �������忡���� �۵��ǰ�)
    ������ �ִ� ��Ż�� �۵� �ǵ��� ���� �Ѵ�.
	
 */
class CServerPortalOpenInstance
    : public CServerTriggerInstance
{
public:
        CServerPortalOpenInstance();

        /**
            �ν��Ͻ��� ������ �� ȣ�� �ȴ�.
            �츮�� �ν��Ͻ��� ���۵Ǿ��ٴ� ���� �̼���
            �Ϸ��ߴٴ� ���̹Ƿ� ��Ż ���� ��Ŷ�� ���� �Ѵ�.

         */
        void		    Start();

        /**
        	������ �������� ��� ȣ�� �ȴ�.
            ��Ż�� ���� �� �������� ������ ������ ������
            ���� �� �����Ƿ� ������ ���� �ϸ� ������ ��û �Ѵ�.

         */
        void            EntryPC(SMessage* msg);

        /**
        	PC �� �������� ��� ȣ�� �ȴ�.

         */
        void            ExitPC(SMessage* msg);

		// �÷��̾ ��Ƽ�� Ż������ ��� ȣ�� �ȴ�;
		void			PartyOutPC(SMessage* msg);

        /**
            ��Ż�� ��û �� ��� ȣ�� �ȴ�.
            Ŭ���̾�Ʈ�� ����Ʈ�� ���� ��û �Ǿ� ����.
            �츮�� ���� �̵��� ó���ϰ� ���� ���� �Ѵ�.

        */
        void            DoPortal(SMessage* msg);

protected:
        /**
            Ŭ���̾�Ʈ�� ��Ż�� ������ ��û �Ѵ�.

            \param dwCharDbNum ���¸� ������ ĳ���� DB ID, 0 �ϰ�� �� ��ü�� ����.
         */
        void            Update(DWORD dwCharDbNum = 0);

		/**
			�츮�� ��Ż Ÿ������ ���� �������� ��Ȱ ��Ų��.

		 */
		void			Rebirth();

        /**
            ĳ���͸� �̵� ��Ų��.            

         */
        void            Teleport(const SServerPortalMessage* pPortalMsg);
};

} // end namespace

#endif