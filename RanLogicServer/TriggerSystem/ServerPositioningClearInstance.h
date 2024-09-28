#ifndef CServerPositioningClearInstance_H
#define CServerPositioningClearInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
	Ư�� ��ġ�� �ο����� ��ġ�ϸ�
    �Ϸ�Ǵ� ���¸� ó�� �Ѵ�.
    	
 */
class CServerPositioningClearInstance
    : public CServerTriggerInstance
{
public:
        CServerPositioningClearInstance();

        /**
            �ν��Ͻ��� ������ �� ȣ�� �ȴ�.

         */
        void		    Start();

		/**
			������ ó���� ���� �Ѵ�.
			�̵� ����Ʈ�� ��ġ �ϴ��� ���� �Ϸ� ó���� �Ѵ�.
			
		 */
        void		    FrameMove(float fAccTime, float fElapsedTime);

        /**
        	������ �������� ��� ȣ�� �ȴ�.

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
            ���� ���¸� ���� �Ѵ�.
            �츮�� ������ ��ġ�� Ŭ�� �˷� �ش�.

            \param dwCharDbNum ���¸� ������ ĳ���� DB ID, 0 �ϰ�� �� ��ü�� ����.
         */
        void			Update(DWORD dwCharDbNum = 0);

		/**
			Ʈ���ŷ� ���� �ʿ��� ������ ���� �´�.
			�Ź� Ʈ���Ÿ� ����ȯ�ؼ� ������ ������ �����ؼ� �̷��� �̸� �����ͼ� ����.

		 */
		void			Parsing();
    
        /**
            Ŭ��� üũ �Ѵ�.

         */
        bool            Check();

        /**
            ���� ��ġ�� �θƽ��� ���ԵǴ��� ���ɴ�.

         */
        bool            InMinMax(const D3DXVECTOR3& pos);

		D3DXVECTOR2		m_minipos;	//@ Ŭ���̾�Ʈ�� ���� �̴ϸ� ��ǥ
		D3DXVECTOR2		m_min;		//@ ��꿡 ���� AABB �ּ���
		D3DXVECTOR2		m_max;		//@ ��꿡 ���� AABB �ִ���
        int				m_nNow;	    //@ ���� �ο� ��
		int				m_count;	//@ �ּ� ��� �ο���, �� �ο�����ŭ ������ �־�� ��� �ȴ�.

        CHARDBNUM_LIST  m_entrance; //@ ������ �ο� ����Ʈ
        bool            m_completed;//@ �Ϸ� ����
};

} // end namespace

#endif