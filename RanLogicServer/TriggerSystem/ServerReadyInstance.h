#ifndef CServerReadyInstance_H
#define CServerReadyInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
	�δ� ���� ���¸� ó�� �Ѵ�.
    ���� ���� �� �� ��� �ð��� ó�� �Ѵ�.
	
 */
class CServerReadyInstance
    : public CServerTriggerInstance
{
public:
        CServerReadyInstance() : m_bUpdate(false), m_all(0), m_cur(0) {}

		/**
			�ν��Ͻ��� ���۵� �� ȣ�� �ȴ�.
			�������� �ʱ�ȭ�� �����Ѵ�.

		 */
		void			Start();

        /**
            ���� ���¸� ���� �Ѵ�.
            ������ Ŭ���̾�Ʈ�� ���� ���¸� �������� �ǹ� �Ѵ�.

         */
        void            Update();

        /**
            �ν��Ͻ��� ������ �� ȣ�� �ȴ�.
            �츮�� �̶� ���������� ���¸� ���� �Ѵ�.

         */
        void            Exit();

		/**
			������ ó���� ���� �Ѵ�.
			�츮�� m_bUpdate �� ���� ��Ŷ�� ���� �Ѵ�.
            �̷��� �ϴ� ������ ��Ʈ��ũ ���ϸ� ���̱� �����̴�.
            ���� ������ Update() �� ȣ��ȴٸ� ȣ�� �ɋ����� ��Ŷ�� ������� �ϴµ�
            �̷��� �ϸ� �ѹ��� ������¸� ���Ƽ� �����ϹǷ� ȿ�����̴�.
			
		 */
        void		    FrameMove(float fAccTime, float fElapsedTime);

        /**
        	PC �� �������� ��� ȣ�� �ȴ�.
         */
        void            EntryPC(SMessage* msg);

        /**
        	PC �� �������� ��� ȣ�� �ȴ�.
         */
        void            ExitPC(SMessage* msg);

		// �÷��̾ ��Ƽ�� Ż������ ��� ȣ�� �ȴ�;
		void			PartyOutPC(SMessage* msg);

protected:
        /**
            ������ ����� ���� �Ѵ�.

         */
        void            SetCur(DWORD dwChaDbNum);
        void            RemoveCur(DWORD dwChaDbNum);

        /**
            ĳ������ ������ ������ ���� �Ѵ�.
            �ߺ� �ڵ� ����

            \param dwCharDbNum ĳ���� DB ID
            \param bEnable ������ �����ϰ� ���� ����
         */
        void            SetMovement(DWORD dwCharDbNum, bool bEnable);
		
        bool            m_bUpdate;  //@ ��Ŷ ���� ����
        CHARDBNUM_LIST  m_entrance; //@ �̹� ������ �ο� ����Ʈ

		int				m_all;		//@ ��ü �����ؾ��� �ο���
		int				m_cur;		//@ ���� ������ �ο���
};

} // end namespace

#endif