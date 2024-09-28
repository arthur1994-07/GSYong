#ifndef CServerPartyInstance_H
#define CServerPartyInstance_H

#include "ServerTriggerInstance.h"
#include "../../RanLogic/Msg/PVEMsg.h"

namespace TriggerSystem
{

/**
	��Ƽ�� ���õ� ó���� ���� �Ѵ�.

    �츮�� ���� ��Ƽ���� Ż���ϸ� �δ� ������ ������.
    ���� ��Ƽ ���� �ʿ��� ������ �ִٸ� �̰����� ó���� �ָ� �ȴ�.
	���� �̸� ������ �� ���� ������ ���� �ϰ� �ʹٸ� ��ƼƮ���Ÿ�
	�ش� Ʈ���Ÿ� ���� ����� ���� �ȴ�.

 */
class CServerPartyInstance
    : public CServerTriggerInstance
{
public:
        CServerPartyInstance();

		/**
            �ν��Ͻ��� ������ �� ȣ�� �ȴ�.
            ���� �ɶ� ���¸� ���� �Ѵ�.

         */
        void		    Start();

        /**
        	������ �������� ��� ȣ�� �ȴ�.
            �츮�� �ش� �������Ը� ���¸� ���� �Ѵ�.

         */
        void            EntryPC(SMessage* msg);

        /**
        	PC �� �������� ��� ȣ�� �ȴ�.

         */
        void            ExitPC(SMessage* msg);

		// �÷��̾ ��Ƽ�� Ż������ ��� ȣ�� �ȴ�;
		void			PartyOutPC(SMessage* msg);
		
		/**
			������ ó���� ���� �Ѵ�.
			��Ƽ���� Ż���� ������ ���� ������.
			
		 */
        void		    FrameMove(float fAccTime, float fElapsedTime);

protected:
		// ���� �ο� ����;
		const bool		ExitPartyMember ( const DWORD _dwChaNum );

		CHARDBNUM_LIST  m_entrance; //@ ���� �ʿ� ������ �ο� ����Ʈ
		bool			m_bAutoOut;	//@ �ĵy�� �ڵ� �ƿ� ����
};

} // end namespace

#endif