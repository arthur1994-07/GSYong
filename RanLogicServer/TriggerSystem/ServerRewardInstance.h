#ifndef CServerRewardInstance_H
#define CServerRewardInstance_H

#include "ServerTriggerInstance.h"
#include <map>

namespace TriggerSystem
{

/**
    ���� ����
    ���� ó���� ���� �ʿ��� ��ҵ�
	�츮�� ���� �ʿ� �ش� ĳ���Ͱ� ���ٸ� ������ �������� ������ ���� ����.

 */
struct SRewardInfo
{
        SRewardInfo() : dwDamage(0), dwHeal(0), dwMoney(0), dwExp(0), dwMVP(0) {}

		/**
			ĳ���� ����
			������� �������� ����� ������ ĳ���Ͱ� ������ �ع����� �� ������
			���� �� ���µ� ǥ�ô� ����� �ϹǷ� ������ �ִ´�.

		 */
		DWORD			dwCharDbNum;//@ ĳ������ DB �ѹ�
		std::string		strCharName;//@ ĳ���͸�
		DWORD			dwClass;	//@ ĳ������ Ŭ����

		/**
			���� ����
			����� ���õ� �������� ����

		 */
        DWORD           dwDamage;   //@ ��� ���Ϳ� ��������
        DWORD           dwHeal;     //@ ��� �Ʊ����� ����
        DWORD           dwMoney;    //@ ����
        DWORD           dwExp;      //@ ���� ����ġ(MVP���Ը� �ش�)
        DWORD           dwMVP;      //@ MVP ����(MVP�� ���� ������ �� �ִ�.)
};

typedef std::map<DWORD, SRewardInfo>    CharRewardInfo;
typedef CharRewardInfo::iterator        CharRewardInfoIt;
typedef CharRewardInfo::const_iterator  CharRewardInfoCIt;

/**
	������ ó�� �Ѵ�.
    �������� �� ���� � ���� ������ ���� ���� ������
    ���� �ϰ� ���������� ������ ���� �ϰ� Ŭ�� ������ ���� �Ѵ�.

    ���󿡼� ������ ����(��ȯ) ��Ű�� ó���� �ϰ� ������ ���� �Ѵ�.
    �̴� ���������� ���� �� ��ȯ ��Ŵ���� ���� ó�� �ϰ� �Ѵ�.
	
 */
class CServerRewardInstance
    : public CServerTriggerInstance
{
public:
        CServerRewardInstance();

        /**
            �ν��Ͻ��� ������ �� ȣ�� �ȴ�.
            ���� �ɶ� �츮�� ���� ��� �ο��� ��´�.
            �̷��� ���� �ο��鿡 ���� ������ ������.

         */
        void		    Start();

        /**
        	������ ��� �Ϸ�Ǿ����Ƿ�
            �츮�� �ش� ĳ���͵��� ��� ��ȯ ��Ų��.

         */
        void            Exit();

        /**        	
            ���� ������ ó�� �Ѵ�.
            ���� ��� ������ ���Ӱ� ������ ������ �ִٸ�
            ��� ����� ���� ���� ������ ����� �ش�.

         */
        void            EntryPC(SMessage* msg);

        /**
            ĳ���Ͱ� �������� �� ��� ȣ�� �ȴ�.
            �츮�� ������ ���� ���� �Ѵ�.

        */
        void            Damage(SMessage* msg);

        /**
            ĳ���Ͱ� ���� �� ��� ȣ�� �ȴ�.
            �츮�� �� ���� ���� �Ѵ�.

        */
        void            Heal(SMessage* msg);

        /**
            ��ȯ�ϱ� ����
            ���� ������ ���� ��ҵ�� ������ ó�� �Ѵ�.
            �� ���� �������� ��� �����ϰų�, �ͼ� �ð��� �����ų�, 
            ��� ���������� Ŭ���� �ϸ� ȣ�� �Ǿ� ����.

         */
        void            ReturnPC(SMessage* msg);

		/**
			��� �������� ������ ��� ��ȯ ��Ų��.
			Ư���� ��쿡 ��� �Ǿ� ����.

		 */
		void            ReturnAllPC(SMessage* msg);

        /**
            �������� Ż���� ��û �Ѵ�.
			�츮�� ��� ������ ������ ������ �����⸦ Ŭ���ϸ�
			���� �δ����� Ż���� ��û �ȴ�.

         */
        void            StageExit(SMessage* msg);

        /**
            ������Ʈ�� ���� ���� ������ ���� �Ѵ�.
            ���۵� ���� ������ ���� ���������� ���� ������ �ǹ� �Ѵ�.

         */
        void            RewardAdd(SMessage* msg);

        /**
            ������ ó�� �Ѵ�.
            �������� ����(���� or ��� �������� ����) �� ó�� �ȴ�.

         */
        void            RewardReq(SMessage* msg);

protected:
        /**
            ������ ó�� �Ѵ�.

         */
        void            Reward();

        /**
            ���� ���� ��Ҹ� �������� ���� ����� ��� �Ѵ�.

         */
        void            CalcReward();

        /**
            ���� ������ ������ ���� �Ѵ�.
            MVP �� ����ġ �� �ƴ϶� ���� �ش�.

         */
        void            ApplyServer();

        /**
            ���� ������ Ŭ���̾�Ʈ�� ���� �Ѵ�.
            �츮�� ��Ŷ�� ���� Ŭ�� ������ �˸���.

         */
        void            ApplyClient();

        /**
            �� ���������� ���� �Ǹ� ���� ������ ���󰡹Ƿ�
            ��� �̾��ϱ� ���� ������Ʈ�� ���� ������ ���� �Ѵ�.

         */
        void            RelayRewardInfo();

        /**
            ��� �������� ����� ��ġ�� ��ȯ ��Ų��.

			ReturnAll() : ������Ʈ�� ����� ��ȯ ��Ų��.
			ReturnImmidiately() : ��� ��ȯ ��Ų��.
         */
        void            ReturnAll(int nWhy);
		void            ReturnImmidiatelyAll();
        void            ReturnImmidiately(DWORD dwCharDbNum);

        /**
            ���� ���� ����ġ, MVP�� ��ũ��Ʈ�� ���� ����Ѵ�.
            
         */
        const LONGLONG  RewardMoney(const DWORD dwCharDbNum);
        const int       RewardExp(const DWORD dwCharDbNum);
        const int       RewardMVP(const DWORD dwCharDbNum, const DWORD cDamage, const DWORD cHeal);

        bool            GetChar(const DWORD dwCharDbNum, GLChar*& pChar);

        CharRewardInfo  m_info;     	//@ ĳ���� ������ ���� ���
        float           m_timer;    	//@ ���� ó���� Ÿ�̸�
        bool            m_return;   	//@ ��ȯ ó�� ����
        bool            m_last;     	//@ ������ �������� ����
		bool			m_completed;	//@ ���������� �Ϸ� �ߴ��� ����
		bool			m_immediately;	//@ ��� ��ȯ ����(������Ʈ�� ������ �ʰ� ��ȯ)

        DWORD           m_dwMaxMVP;       //! MVP�� �ɼ��ִ� �ְ� ���� ( ������ ��� �޴´�.)

        bool            m_isReword;
		
};

} // end namespace

#endif