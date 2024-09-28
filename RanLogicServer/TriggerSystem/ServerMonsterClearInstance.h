#ifndef CServerMonsterClearInstance_H
#define CServerMonsterClearInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
	���͸� Ŭ���� �ؾ� �ϴ� ���¸� ��Ÿ����.
    �츮�� ���Ͱ� ���� �� ������ �����鿡�� ���� ���¸� ���� �Ѵ�.
    ���� ���� �ð����� ���� �ð��� ���� �Ѵ�.
	
 */
class CServerMonsterClearInstance
    : public CServerTriggerInstance
{
public:
        CServerMonsterClearInstance();

        /**
            Ŭ���� ���͸� ī��Ʈ �Ѵ�.

         */
        void        IncCur();
        int         GetCur();

        /**
            �ν��Ͻ��� ������ �� ȣ�� �ȴ�.

         */
        void		Start();

		/**
			������ ó���� ���� �Ѵ�.
			���� ��Ŷ�� �ִٸ� �����ִ� ������ �Ѵ�.
			
		 */
        void		FrameMove(float fAccTime, float fElapsedTime);

        /**
            ���� ���¸� ���� �Ѵ�.

            \param dwCharDbNum ���¸� ������ ĳ���� DB ID, 0 �ϰ�� �� ��ü�� ����.
         */
        void        Update(DWORD dwCharDbNum = 0);

        /**
        	������ �������� ��� ȣ�� �ȴ�.

         */
        void        EntryPC(SMessage* msg);

        /**
            �츮�� ���Ͱ� �ʿ��� ���� �Ǹ� �̸� ī��Ʈ �Ѵ�.
            �׷��� ���� ���ôٹ������� ���� �������� �����Ƿ�
            m_bUpdate ������ �ξ� �����ӿ� ���¸� �����ϴ� ������ ó���ؾ�
            ��Ʈ��ũ ���ϰ� ���� ����.

        */
        void        DieCrow(SMessage* msg);

protected:
        bool        m_update;   //@ ���� �� ���� ����
        int         m_count;    //@ ���� ���� ��
};

} // end namespace

#endif