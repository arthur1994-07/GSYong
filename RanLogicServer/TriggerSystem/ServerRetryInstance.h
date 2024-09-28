#ifndef CServerRetryInstance_H
#define CServerRetryInstance_H

#include "ServerTriggerInstance.h"

namespace TriggerSystem
{

/**
	�絵���� ���� �Ѵ�.
    �Ϲ� ��Ȱ�� ���� ��ų ��Ȱ�� �ǰ� �Ѵ�.
    ����, ��Ȱ���� ���������� ó���ϸ� �������� ���⵵�� �������Ƿ�
    ����, ��Ȱ�� �̰����� ó�� �Ѵ�.
	
 */
class CServerRetryInstance
    : public CServerTriggerInstance
{
public:
        CServerRetryInstance();

		void			StageClear(SMessage* msg);
		void			StageFail(SMessage* msg);

        /**
        	������ �������� ��� ȣ�� �ȴ�.
            �츮�� �����ϴ� �������� ��� ��Ȱ �Ұ��� �ϰ� ���´�.

         */
        void            EntryPC(SMessage* msg);

        /**
            ������ �ʿ��� ��ȯ �� ��� ȣ�� �ȴ�.
			�츮�� ������ ��ȯ �� �� �絵�� �ð��� ���� �ش�.
			�̰��� ���࿡ �絵�� ���϶� ��ȯ�� �� ��츦 ���ؼ� �̴�.

         */
        void            ReturnPC(SMessage* msg);

        /**
            ������ �ʿ��� �������� ��� ȣ�� �ȴ�.
			�츮�� ������ ���� ���� �� ��Ȱ �Ұ����� Ǯ�� �ش�.

         */
        void            ExitPC(SMessage* msg);

		// �÷��̾ ��Ƽ�� Ż������ ��� ȣ�� �ȴ�;
		void			PartyOutPC(SMessage* msg);

		// �÷��̾ �׾��� ��� ȣ��ȴ�;
        void            DiePC(SMessage* msg);

        /**
        	�絵���� ���� ó���� ���� �Ѵ�.

            RetryAns : ����
            RetryReq : ��û

         */
        void            RetryAns(SMessage* msg);
        void            RetryReq(SMessage* msg);

		/**
			������ ó���� ���� �Ѵ�.
			�츮�� �絵�� �ð� ����� ó�� �Ѵ�.
			
		 */
        void		    FrameMove(float fAccTime, float fElapsedTime);

protected:
        /**
            ĳ������ ��Ȱ ������ ���� �Ѵ�.
            �ߺ� �ڵ� ����

            \param dwCharDbNum ĳ���� DB ID
            \param bEnable ��Ȱ �����ϰ� ���� ����
         */
        void            SetResurrectPVE(DWORD dwCharDbNum, bool bEnable);

        /**
            �絵�� ����� ó���� �Ѵ�.
            
         */
        void            Process(bool bOk, int nWhy = -1 );

        /**
            �絵���� OK ���� ��� �絵���� �ʿ���
            ���� üũ �Ѵ�.

         */
        bool            CheckRetry(DWORD dwCharDbNum);
        bool            AllCheckRetry();

		/**
			�絵���� ���� ����� ��Ƽ���� �����Ѵ�.
			�̰��� �絵���� �������� ��쿡�� ó�� �ȴ�.

		 */
		void			SendResultMessage(int nWhy, DWORD dwChaDbNum=0);

        /**
            �絵�� ����� ó�� �Ѵ�.

         */
        void            CostRetry();

        /**
            Ŭ���̾�Ʈ�� �絵���� ��û �Ѵ�.

         */
        void            ReqRetry();
		void            ReqRetry(DWORD dwCharDbNum);

		/**
			�絵���� ���� ó���� �Ϸ�Ǹ�
			��Ȱ ��Ų �� ���� ó���� ���� �Ѵ�.

		 */
		void			Rebirth();

		/**
			��Ȱ�� �ʿ��� ������ ��� �Ѵ�.

		 */
		const LONGLONG  RetryMoney(GLChar* pChar);

        CHARDBNUM_LIST  m_entrance; //@ ���� �ʿ� ������ �ο� ����Ʈ
		CHARDBNUM_LIST  m_die;		//@ ���� �ʿ� ���� �ο� ����Ʈ
		CHARDBNUM_LIST  m_rem;		//@ ������ ��� ���� �ο� ����Ʈ
		CHARDBNUM_LIST  m_retry;	//@ ������ �ο� ����Ʈ

        float           m_time;     //@ �絵�� Ÿ�̸�
		bool			m_bIsRewardReqest; // �����û�� �Ͽ��°�?

		bool			m_bRetring;
};

} // end namespace

#endif