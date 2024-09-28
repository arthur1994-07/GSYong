#ifndef CServerStageInstance_H
#define CServerStageInstance_H

#include "ServerTriggerInstance.h"
#include "../../RanLogic/Msg/PVEMsg.h"

namespace TriggerSystem
{

/**
    �⺻���� ���������� ó�� �Ѵ�.

	�ð��� �帣�� �������� �̼��� ������ ������
	�ش� �̼��� �Ϸ�Ǹ� Ŭ���� �Ǵ� ���������� ���� �Ѵ�.
	
 */
class CServerStageInstance
    : public CServerTriggerInstance
{
public:
        CServerStageInstance();

        /**
            ���� ���¸� ���� �Ѵ�.
            ������ Ŭ���̾�Ʈ�� ���� ���¸� �������� �ǹ� �Ѵ�.

        	\param dwCharDbNum ���¸� ������ ĳ���� DB ID, 0 �ϰ�� �� ��ü�� ����.
         */
        void            Update(DWORD dwCharDbNum = 0, int nState = GLMSG::STAGE_STATE_ONGOING);

        /**
            �ν��Ͻ��� ������ �� ȣ�� �ȴ�.
            ���� �ɶ� ���¸� ���� �Ѵ�.

         */
        void		    Start();

        /**
            �ν��Ͻ��� ������ �� ȣ�� �ȴ�.
            �츮�� �̶� ���������� ���¸� ���� �Ѵ�.
            �������� �ð� ���� �޽����� ���� �Ѵ�.

         */
        void            Exit();

        /**
            ������ ���������� ��ȯ �޽����� ����
            �̰��� �ͼ� �ð��� ���� �Ǿ��� ��� �̴�.

         */
        void            ReturnPC(SMessage* msg);

		/**
			������ ó���� ���� �Ѵ�.
			�츮�� 10�ʸ��� �����鿡�� ������ ����ȭ�� �����.
			
		 */
        void		    FrameMove(float fAccTime, float fElapsedTime);

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

		// �÷��̾ �׼��� �������� ��� ȣ�� �ȴ�;
		void			ActionPC(SMessage* msg);

        /**
        	PC �� �׾��� ��� ȣ�� �ȴ�.

         */
        void            DiePC(SMessage* msg);

        /**
            ������������ �ؾ��� �̼��� �Ϸ�Ǿ��� ��� ȣ�� �ȴ�.
            �츮�� �̼Ǽ��� ī��Ʈ�ؼ� ��� �Ϸ�� ���� ó�� �Ѵ�.

        */
        void            MissionCompleted(SMessage* msg);

protected:
        /**
            ���� �ϷḦ ó�� �Ѵ�.
            �츮�� Ŭ���̾�Ʈ�� �̼� �ϷḦ �˸��� ���� ó�� �Ѵ�.

         */
        void            StageCompleted();

        /**
            �������� ���и� ó�� �Ѵ�.
            �ð��� ��� ����Ǿ��� ��� ȣ�� �ȴ�.

         */
        void            StageFailed();

        /**
            �������� ������ ��´�.
            ���� ���������� �÷��� Ÿ���� �̸� ���� ���´�.
            ���� Update �ÿ� �ߺ� ��� �ȴ�.

         */
        bool            GetInfo();

        /**
            �������� ����� ���� ��Ŷ�� ���� �Ѵ�.

         */
        void            SendStageClear(bool bCompleted);

        int             m_stage;    //@ ���� ��������
        int             m_playtime; //@ �������� �� ���� �ð�(sec)
        int             m_mission;  //@ ó�� �ؾ� �� �� �̼Ǽ�
        bool            m_last;     //@ ������ �������� ����
        bool            m_exit;     //@ �������� ���� ����, �ð� ����� �׷��� ���� ��츦 ���� �Ѵ�.

        int             m_completed;//@ �Ϸ��� �̼Ǽ�

        CHARDBNUM_LIST  m_entrance; //@ ���� �ʿ� ������ �ο� ����Ʈ
        CHARDBNUM_LIST  m_die;      //@ ���� �ʿ� ���� �ο� ����Ʈ
};

} // end namespace

#endif